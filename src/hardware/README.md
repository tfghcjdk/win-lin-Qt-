# 硬件抽象与通信

SocketCAN 与 UART / 串口仍为预留接口。V4L2 摄像头已实现 MJPEG/YUYV、MMAP 缓冲、采集线程、断连/超时处理和安全释放；设备节点默认 `/dev/video0`，可通过 `HMI_CAMERA_DEVICE` 环境变量覆盖。Windows 后续通过平台摄像头后端接入。

当前状态：仅建立框架，尚未实现相关功能。
