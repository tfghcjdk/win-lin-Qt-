# OpenCV 3.4.16 单目地面测距

## 板端依赖

目标板运行时需要在 `/usr/lib` 提供 ARM 版本的 `libopencv_core.so.3.4`、`libopencv_imgproc.so.3.4`、`libopencv_calib3d.so.3.4` 及其依赖，并提供可供链接的 `.so` 符号链接。开发头文件需要位于 `/usr/include/opencv2`；交叉编译时则放入工具链 sysroot 对应的 `usr/include` 和 `usr/lib`。

qmake 在板端原生构建时使用 `/usr`。交叉编译时自动使用 `QMAKE_SYSROOT/usr`；若 OpenCV 位于独立 staging 目录，可在 qmake 时传入 `OPENCV_ROOT=/path/to/opencv-arm-root`。生成的程序运行时搜索路径包含 `/usr/lib`。

## 标定文件

复制 `config/camera_calibration.ini.example` 到开发板：

```sh
mkdir -p /etc/nev-smarthmi
cp camera_calibration.ini.example /etc/nev-smarthmi/camera_calibration.ini
```

必须对最终安装位置的 `/dev/video9` 摄像头进行棋盘格标定，并填写：

- `image_width`、`image_height`：执行标定时的图像尺寸，当前建议 1280 × 720。
- `fx`、`fy`、`cx`、`cy`：OpenCV 相机内参矩阵。
- `k1`、`k2`、`p1`、`p2`、`k3`：镜头畸变参数。
- `height_cm`：摄像头光学中心到平整地面的垂直高度，单位厘米。
- `pitch_degrees`：摄像头光轴相对水平线向下的角度，必须为正值。

所有参数测量并复核后才能把 `enabled` 改为 `true`。参数缺失、范围无效或文件不存在时，程序继续显示摄像头画面，但不会输出OpenCV自动测距或自动障碍物警告。

## 算法与警告

摄像头线程每三帧处理一次图像：显示仍使用原始1280×720画面，测距分析默认缩小到640像素宽以控制X6818负载；去畸变后截取下部地面区域，执行灰度、模糊、Canny边缘、形态学闭合及轮廓筛选，以候选轮廓底部中心作为地面接触点，再根据针孔模型、相机高度和俯角计算前向距离。距离使用指数平滑；连续三次未检出后清除结果。`analysis_width` 和 `process_every_frames` 可在板端性能测试后调整。

当前警告条件为有效距离不超过 80 cm。设置页的55 cm入口只用于检查警告视觉。轮廓法可能把路面纹理、阴影和标线识别为障碍物，也可能漏检悬空、低对比度或没有明显边缘的物体，因此必须通过不同距离、坡道、昼夜和雨天录像验证。实车安全应用应增加超声波雷达等独立距离来源。
