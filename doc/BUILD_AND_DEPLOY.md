# 构建、运行与部署

## 当前状态

`NEV-SmartHMI.pro` 是当前唯一 Qt 应用工程，CMake 同样提供 C++11 / Qt Widgets 应用目标。当前 UI 和模拟交互已实现，真实硬件集成尚未实现。早期遗留的 `Qt1604` 工程已经清理。

## Windows / Qt Creator

1. 打开 `D:\QTXM\Qt164\Qt1604\NEV-SmartHMI.pro`。
2. 选择实际安装的 **Qt 5.14.2 MinGW 32-bit** Kit。
3. 原来已打开辅助工程时，先执行“构建 → 执行 qmake”，再构建和运行；必要时重新打开项目。
4. 应用为 1024 × 600 无边框窗口。Esc 返回 / 退出，设置页也提供退出按钮。

需求基线中的 MSVC 2017 32-bit 并未在本次验证；项目为 MSVC 配置了 `/utf-8`，后续使用该 Kit 时还需实际编译验证。

## 本机命令行构建

以下为 PowerShell 命令，Qt 和编译器路径仅用于本机示例：

```powershell
Set-Location -LiteralPath 'D:\QTXM\Qt164\Qt1604'
$env:PATH = 'D:\QT\Tools\mingw730_32\bin;D:\QT\5.14.2\mingw73_32\bin;' + $env:PATH
New-Item -ItemType Directory -Path build/ui -Force | Out-Null
qmake -o build/ui/Makefile NEV-SmartHMI.pro CONFIG+=release
mingw32-make -C build/ui -j4
```

输出为 `build/ui/release/NEV-SmartHMI.exe`。在 Qt Creator 中运行会使用 Kit 提供的运行环境；直接双击程序前需部署运行库：

```powershell
windeployqt --release --no-translations build/ui/release/NEV-SmartHMI.exe
```

程序资源已经由 `assets/hmi.qrc` 嵌入，不需要桌面原型图片的原始路径。

## 自动化交互测试与截图

```powershell
New-Item -ItemType Directory -Path build/ui-tests -Force | Out-Null
qmake -o build/ui-tests/Makefile tests/ui_tests.pro CONFIG+=release
mingw32-make -C build/ui-tests -j4
$env:HMI_SCREENSHOT_DIR = 'D:\QTXM\Qt164\Qt1604\build\ui-validation'
.\build\ui-tests\release\hmi-ui-tests.exe
```

测试验证真实按钮点击、北京时间跨日换算、温度按钮与滑条双向同步、温度边界、跨页状态、媒体进度、路线切换、相机返回、断连与低压场景，并保存各页截图。无显示设备时可设置 `QT_QPA_PLATFORM=offscreen`；Windows 离屏测试需要 `QT_QPA_FONTDIR=C:\Windows\Fonts`。普通桌面运行不要设置离屏变量。

## CMake 入口

需要 CMake 3.5+、Qt 5.4.1+ Core / Gui / Widgets 和匹配的 C++ 编译器。`CMakeLists.txt` 已启用 AUTOMOC / AUTORCC、C++11 和资源嵌入；使用与 Qt 安装匹配的生成器及 `CMAKE_PREFIX_PATH`。本机未找到 CMake，未实际执行此入口，不能把 qmake 验证等同于 CMake 验证。

## ARM 尚需验证

目标仍为 X6818、Linux 3.4.39、Qt 5.4.1。必须确认 SDK、armv7l ABI、浮点约定、sysroot、Qt 安装、显示/触控插件、中文字体、设备权限和部署路径。代码没有引入 Qt 5.6+ API，但尚未进行真实 Qt 5.4.1 ARM 交叉编译与板端运行。

`build_arm.sh`、`deploy_to_board.sh`、硬件驱动文件和 CAN 仿真脚本仍为占位，不能用它们完成板端构建或部署。

## OpenCV 3.4.16

Linux构建会启用 `HMI_WITH_OPENCV`，链接 `opencv_core`、`opencv_imgproc` 和 `opencv_calib3d`。qmake依次使用显式的 `OPENCV_ROOT`、环境变量 `OPENCV_ROOT`、Qt Kit的 `QMAKE_SYSROOT/usr` 或交叉编译器 `-print-sysroot` 返回目录下的 `usr`。找不到 `opencv2/calib3d.hpp` 时会在qmake阶段停止，防止交叉编译错误引用Ubuntu主机的 `/usr/include`。

X6818 Buildroot推荐使用：

```sh
cd ~/x6818_linux_180209/buildroot
find output -path '*/opencv2/calib3d.hpp' -print
find output -name 'libopencv_calib3d.so*' -print
```

如果两个文件都位于 `output/staging/usr`，在Qt Creator的qmake附加参数中填写：

```text
OPENCV_ROOT=/home/xdedu/x6818_linux_180209/buildroot/output/staging/usr
```

然后清理构建目录、重新执行qmake并重新构建。也可以在命令行使用：

```sh
qmake NEV-SmartHMI.pro OPENCV_ROOT=/home/xdedu/x6818_linux_180209/buildroot/output/staging/usr
make -j4
```

只有运行库位于开发板 `/usr/lib` 还不够：编译端必须同时具有同一ARM ABI的OpenCV 3.4.16头文件、ARM链接库和无版本 `.so` 开发链接。若 `output/staging` 中没有这些文件，需要用原来构建板端OpenCV 3.4.16的交叉编译配置执行安装到staging，不能使用Ubuntu的x86-64 OpenCV库。相机标定与 `/etc/nev-smarthmi/camera_calibration.ini` 配置见 [MONOCULAR_DISTANCE.md](MONOCULAR_DISTANCE.md)。

## Qt HTTPS 独立验证

`tools/navigation_https_probe` 是不依赖OpenCV和界面的ARM命令行程序，用于验证QtNetwork能否直接加载板端OpenSSL和 `/etc/ssl/certs/ca-certificates.crt` 并访问高德HTTPS接口。它读取 `/Kd1234/config/navigation.ini`，只打印Key长度，不输出Key内容。

使用X6818 Qt Kit单独打开 `tools/navigation_https_probe/navigation_https_probe.pro`，构建后将 `navigation-https-probe` 上传至 `/Kd1234`，执行：

```sh
chmod +x /Kd1234/navigation-https-probe
/Kd1234/navigation-https-probe
echo $?
```

成功输出包含 `Qt SSL support: yes`、非零CA证书数量和 `RESULT: HTTPS and AMap API OK`，退出码为0。该程序不调用BusyBox `wget`，因此不要求安装 `/usr/bin/openssl` 命令。
