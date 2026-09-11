#include "monocular_distance.h"
#include <QFileInfo>
#include <QSettings>
#include <QtGlobal>
#include <cmath>

#if defined(Q_OS_LINUX) && defined(HMI_WITH_OPENCV)
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#endif

MonocularDistanceEstimator::MonocularDistanceEstimator(const QString &configPath)
    : ready(false),calibrationWidth(0),calibrationHeight(0),analysisMaxWidth(640),processEveryFrames(3),frameCounter(0),lastDistance(-1),missCount(0),
      fx(0),fy(0),cx(0),cy(0),k1(0),k2(0),p1(0),p2(0),k3(0),cameraHeightCm(0),pitchDegrees(0),roiTopRatio(.42),minContourArea(900) {
#if defined(Q_OS_LINUX) && defined(HMI_WITH_OPENCV)
    if (!QFileInfo::exists(configPath)) { error=QStringLiteral("缺少单目测距标定文件：%1").arg(configPath);return; }
    QSettings settings(configPath,QSettings::IniFormat);
    if (!settings.value(QStringLiteral("calibration/enabled"),false).toBool()) { error=QStringLiteral("单目测距标定尚未启用");return; }
    calibrationWidth=settings.value(QStringLiteral("calibration/image_width")).toInt();
    calibrationHeight=settings.value(QStringLiteral("calibration/image_height")).toInt();
    fx=settings.value(QStringLiteral("camera/fx")).toDouble();fy=settings.value(QStringLiteral("camera/fy")).toDouble();
    cx=settings.value(QStringLiteral("camera/cx")).toDouble();cy=settings.value(QStringLiteral("camera/cy")).toDouble();
    k1=settings.value(QStringLiteral("camera/k1"),0).toDouble();k2=settings.value(QStringLiteral("camera/k2"),0).toDouble();
    p1=settings.value(QStringLiteral("camera/p1"),0).toDouble();p2=settings.value(QStringLiteral("camera/p2"),0).toDouble();k3=settings.value(QStringLiteral("camera/k3"),0).toDouble();
    cameraHeightCm=settings.value(QStringLiteral("mounting/height_cm")).toDouble();pitchDegrees=settings.value(QStringLiteral("mounting/pitch_degrees")).toDouble();
    roiTopRatio=settings.value(QStringLiteral("detection/roi_top_ratio"),.42).toDouble();minContourArea=settings.value(QStringLiteral("detection/min_contour_area"),900).toDouble();
    analysisMaxWidth=qBound(320,settings.value(QStringLiteral("detection/analysis_width"),640).toInt(),1280);
    processEveryFrames=qBound(1,settings.value(QStringLiteral("detection/process_every_frames"),3).toInt(),10);
    ready=calibrationWidth>0 && calibrationHeight>0 && fx>0 && fy>0 && cameraHeightCm>0 && pitchDegrees>0 && pitchDegrees<80 && roiTopRatio>.1 && roiTopRatio<.9 && minContourArea>0;
    if (!ready) error=QStringLiteral("单目测距标定参数无效");
#else
    Q_UNUSED(configPath);error=QStringLiteral("当前构建未启用 Linux OpenCV 单目测距");
#endif
}

double MonocularDistanceEstimator::groundDistanceCm(double pixelY,double focalY,double centerY,double heightCm,double pitchDegrees) {
    if (focalY<=0 || heightCm<=0 || pitchDegrees<=0 || pitchDegrees>=80) return -1;
    const double pitch=pitchDegrees*3.14159265358979323846/180.;
    const double normalizedY=(pixelY-centerY)/focalY;
    const double down=std::sin(pitch)+normalizedY*std::cos(pitch);
    if (down<=.01) return -1;
    const double forward=heightCm*(std::cos(pitch)-normalizedY*std::sin(pitch))/down;
    return forward>0?forward:-1;
}

MonocularDistanceEstimator::Measurement MonocularDistanceEstimator::measure(const QImage &image) {
    Measurement result;
    if (!ready || image.isNull()) return result;
    if (++frameCounter<processEveryFrames) return result;
    frameCounter=0;result.updated=true;
#if defined(Q_OS_LINUX) && defined(HMI_WITH_OPENCV)
    const QImage rgb=image.convertToFormat(QImage::Format_RGB888);
    cv::Mat rgbView(rgb.height(),rgb.width(),CV_8UC3,const_cast<uchar *>(rgb.constBits()),rgb.bytesPerLine());
    cv::Mat fullBgr,bgr;cv::cvtColor(rgbView,fullBgr,cv::COLOR_RGB2BGR);
    if (fullBgr.cols>analysisMaxWidth) cv::resize(fullBgr,bgr,cv::Size(analysisMaxWidth,qRound(double(fullBgr.rows)*analysisMaxWidth/fullBgr.cols)),0,0,cv::INTER_AREA);
    else bgr=fullBgr;
    const double sx=double(bgr.cols)/calibrationWidth,sy=double(bgr.rows)/calibrationHeight;
    const cv::Mat camera=(cv::Mat_<double>(3,3)<<fx*sx,0,cx*sx,0,fy*sy,cy*sy,0,0,1);
    const cv::Mat distortion=(cv::Mat_<double>(1,5)<<k1,k2,p1,p2,k3);
    cv::Mat corrected;cv::undistort(bgr,corrected,camera,distortion);
    const int roiTop=qBound(0,int(corrected.rows*roiTopRatio),corrected.rows-1);
    cv::Mat gray,edges;cv::cvtColor(corrected(cv::Rect(0,roiTop,corrected.cols,corrected.rows-roiTop)),gray,cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray,gray,cv::Size(5,5),0);cv::Canny(gray,edges,60,160);
    const cv::Mat kernel=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,kernel);
    std::vector<std::vector<cv::Point> > contours;cv::findContours(edges,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    int nearest=-1;cv::Rect nearestRect;
    const double scaledMinArea=minContourArea*sx*sy;
    for (size_t i=0;i<contours.size();++i) {
        if (cv::contourArea(contours[i])<scaledMinArea) continue;
        cv::Rect box=cv::boundingRect(contours[i]);box.y+=roiTop;
        if (box.width<15 || box.height<20 || box.x<corrected.cols*.05 || box.x+box.width>corrected.cols*.95) continue;
        const double forward=groundDistanceCm(box.y+box.height,camera.at<double>(1,1),camera.at<double>(1,2),cameraHeightCm,pitchDegrees);
        if (forward<20 || forward>500) continue;
        const int distance=qRound(forward);
        if (nearest<0 || distance<nearest) { nearest=distance;nearestRect=box; }
    }
    if (nearest>=0) {
        missCount=0;lastDistance=lastDistance<0?nearest:qRound(lastDistance*.65+nearest*.35);
        result.valid=true;result.distanceCm=lastDistance;result.obstacleArea=QRect(nearestRect.x,nearestRect.y,nearestRect.width,nearestRect.height);
    } else if (++missCount>=3) { lastDistance=-1;result.valid=false;result.distanceCm=-1; }
    else if (lastDistance>=0) { result.valid=true;result.distanceCm=lastDistance; }
#endif
    return result;
}
