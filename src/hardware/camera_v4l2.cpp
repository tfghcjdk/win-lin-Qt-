#include "camera_v4l2.h"
#include <QElapsedTimer>
#include <QFile>
#include <QImageReader>
#include <QtGlobal>

#ifdef Q_OS_LINUX
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <vector>

namespace {
int control(int fd,unsigned long command,void *value) {
    int result;
    do { result=ioctl(fd,command,value); } while (result<0 && errno==EINTR);
    return result;
}

int clip(int value) { return qBound(0,value,255); }

QImage yuyvImage(const uchar *data,int width,int height,unsigned stride) {
    QImage image(width,height,QImage::Format_RGB32);
    if (image.isNull()) return image;
    for (int y=0;y<height;++y) {
        const uchar *src=data+y*stride;
        QRgb *dst=reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x=0;x<width;x+=2,src+=4) {
            const int u=src[1]-128,v=src[3]-128;
            for (int k=0;k<2;++k) {
                const int yy=qMax(0,int(src[k*2])-16)*298;
                dst[x+k]=qRgb(clip((yy+409*v+128)>>8),
                              clip((yy-100*u-208*v+128)>>8),
                              clip((yy+516*u+128)>>8));
            }
        }
    }
    return image;
}
}
#endif

void CameraV4l2::run() {
#ifndef Q_OS_LINUX
    emit message(QStringLiteral("当前平台不支持 V4L2，摄像头未启动"));
#else
    pending.storeRelease(0);
    if (device.trimmed().isEmpty()) {
        emit message(QStringLiteral("摄像头设备节点不能为空"));
        return;
    }

    const QByteArray path=QFile::encodeName(device);
    const int fd=::open(path.constData(),O_RDWR|O_NONBLOCK);
    if (fd<0) {
        emit message(QStringLiteral("无法打开 %1：%2").arg(device,QString::fromLocal8Bit(strerror(errno))));
        return;
    }

    struct Buffer { void *data; size_t length; };
    std::vector<Buffer> buffers;
    bool streaming=false;
    const auto error=[this](const char *step) {
        emit message(QString::fromLatin1(step)+QStringLiteral("：")+QString::fromLocal8Bit(strerror(errno)));
    };

    const auto capture=[&]() {
        v4l2_capability cap={};
        if (control(fd,VIDIOC_QUERYCAP,&cap)<0) { error("VIDIOC_QUERYCAP"); return; }
        unsigned capabilities=cap.capabilities;
#ifdef V4L2_CAP_DEVICE_CAPS
        if (capabilities&V4L2_CAP_DEVICE_CAPS) capabilities=cap.device_caps;
#endif
        if (!(capabilities&V4L2_CAP_VIDEO_CAPTURE) || !(capabilities&V4L2_CAP_STREAMING)) {
            emit message(QStringLiteral("设备不支持单平面 VIDEO_CAPTURE/STREAMING"));
            return;
        }

        const bool jpeg=QImageReader::supportedImageFormats().contains("jpeg")
                     || QImageReader::supportedImageFormats().contains("jpg");
        const unsigned formats[]={V4L2_PIX_FMT_MJPEG,V4L2_PIX_FMT_YUYV};
        v4l2_format format={};
        bool supported=false;
        for (int i=0;i<2;++i) {
            if (i==0 && !jpeg) continue;
            format.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
            format.fmt.pix.width=qBound(320,requestedSize.width(),1920);
            format.fmt.pix.height=qBound(240,requestedSize.height(),1080);
            format.fmt.pix.pixelformat=formats[i];
            format.fmt.pix.field=V4L2_FIELD_ANY;
            if (control(fd,VIDIOC_S_FMT,&format)<0) continue;
            supported=format.fmt.pix.pixelformat==V4L2_PIX_FMT_YUYV
                   || (jpeg && format.fmt.pix.pixelformat==V4L2_PIX_FMT_MJPEG);
            if (supported) break;
        }

        const unsigned width=format.fmt.pix.width,height=format.fmt.pix.height;
        if (!supported || !width || !height || width>1920 || height>1080 || (format.fmt.pix.pixelformat==V4L2_PIX_FMT_YUYV && width%2)) {
            emit message(QStringLiteral("摄像头格式不可用，需要 MJPEG/YUYV，最大 1920x1080"));
            return;
        }

        v4l2_streamparm rate={};
        rate.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        rate.parm.capture.timeperframe.numerator=1;
        rate.parm.capture.timeperframe.denominator=qBound(1,requestedFps,60);
        control(fd,VIDIOC_S_PARM,&rate);

        v4l2_requestbuffers request={};
        request.count=4;
        request.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        request.memory=V4L2_MEMORY_MMAP;
        if (control(fd,VIDIOC_REQBUFS,&request)<0) { error("VIDIOC_REQBUFS"); return; }
        if (request.count<2 || request.count>32) {
            emit message(QStringLiteral("摄像头缓冲区数量无效"));
            return;
        }

        for (unsigned i=0;i<request.count;++i) {
            v4l2_buffer buffer={};
            buffer.type=request.type;
            buffer.memory=request.memory;
            buffer.index=i;
            if (control(fd,VIDIOC_QUERYBUF,&buffer)<0) { error("VIDIOC_QUERYBUF"); return; }
            void *data=mmap(0,buffer.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,buffer.m.offset);
            if (data==MAP_FAILED) { error("mmap"); return; }
            buffers.push_back(Buffer{data,buffer.length});
            if (control(fd,VIDIOC_QBUF,&buffer)<0) { error("VIDIOC_QBUF"); return; }
        }

        v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (control(fd,VIDIOC_STREAMON,&type)<0) { error("VIDIOC_STREAMON"); return; }
        streaming=true;
        emit message(QStringLiteral("摄像头已连接：%1，%2x%3，%4").arg(device).arg(width).arg(height)
                     .arg(format.fmt.pix.pixelformat==V4L2_PIX_FMT_MJPEG?"MJPEG":"YUYV"));

        QElapsedTimer noFrame;
        noFrame.start();
        while (!isInterruptionRequested()) {
            pollfd descriptor={fd,POLLIN,0};
            const int ready=poll(&descriptor,1,200);
            if (ready<0 && errno==EINTR) continue;
            if (ready<0 || (descriptor.revents&(POLLERR|POLLHUP|POLLNVAL))) {
                emit message(QStringLiteral("摄像头已断开或读取失败"));
                return;
            }
            if (!ready) {
                if (noFrame.elapsed()>5000) {
                    emit message(QStringLiteral("摄像头连续 5 秒没有返回画面"));
                    return;
                }
                continue;
            }

            v4l2_buffer buffer={};
            buffer.type=type;
            buffer.memory=V4L2_MEMORY_MMAP;
            if (control(fd,VIDIOC_DQBUF,&buffer)<0) {
                if (errno==EAGAIN) continue;
                error("VIDIOC_DQBUF");
                return;
            }
            if (buffer.index>=buffers.size() || buffer.bytesused>buffers[buffer.index].length) {
                emit message(QStringLiteral("摄像头返回了无效缓冲区"));
                return;
            }

            noFrame.restart();
            if (!pending.loadAcquire() && !(buffer.flags&V4L2_BUF_FLAG_ERROR)) {
                const uchar *data=static_cast<const uchar *>(buffers[buffer.index].data);
                QImage image;
                if (format.fmt.pix.pixelformat==V4L2_PIX_FMT_MJPEG) {
                    image=QImage::fromData(data,buffer.bytesused,"JPG");
                } else {
                    const unsigned stride=qMax(format.fmt.pix.bytesperline,width*2);
                    if (quint64(buffer.bytesused)<quint64(stride)*(height-1)+width*2) {
                        emit message(QStringLiteral("YUYV 帧长度或步长无效"));
                        return;
                    }
                    image=yuyvImage(data,width,height,stride);
                }
                if (image.isNull()) {
                    emit message(QStringLiteral("画面解码失败，请检查 Qt JPEG 插件"));
                    return;
                }
                pending.storeRelease(1);
                emit frameReady(image);
            }
            if (control(fd,VIDIOC_QBUF,&buffer)<0) { error("VIDIOC_QBUF"); return; }
        }
        emit message(QStringLiteral("倒车摄像头已停止"));
    };

    capture();
    if (streaming) {
        v4l2_buf_type type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        control(fd,VIDIOC_STREAMOFF,&type);
    }
    for (size_t i=0;i<buffers.size();++i) munmap(buffers[i].data,buffers[i].length);
    ::close(fd);
#endif
}
