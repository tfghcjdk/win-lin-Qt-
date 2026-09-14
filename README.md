# NEV-SmartHMI 新能源汽车智能座舱

当前版本：**0.2.9-ui**。已实现 1024 × 600 Qt Widgets 页面与交互，可编译运行。

## 打开与运行

在 Qt Creator 中打开根目录 **`NEV-SmartHMI.pro`**，选择本机 **Desktop Qt 5.14.2 MinGW 32-bit**，执行 qmake 后构建、运行。该工程已由 `TEMPLATE = aux` 改为 `app`。若 Qt Creator 仍显示旧的辅助工程状态，请重新运行 qmake 或关闭并重新打开本工程。

本机本次构建的程序位于 `build/ui/release/NEV-SmartHMI.exe`。完成运行库部署后可直接双击。按 **Esc** 关闭倒车预览、从详情返回首页；在首页按 Esc 退出，也可在设置页点击“退出应用”。

## 已实现的页面

- **首页**：顶部每秒更新的实时北京时间和连接状态、左侧导航、车速与档位、续航与四轮胎压、能量流、空调、导航、倒车预览、媒体、行车统计和维护提醒。
- **车辆状态**：放大的续航 / 胎压、能量与行程卡片；支持正常、数据断开及左前低胎压演示场景。
- **导航**：离线示意地图与路线卡片，可开始 / 结束演示导航。
- **空调**：0.5 ℃ 步进、16–30 ℃ 设置范围、中间滑条直接调节温度、Auto、A/C、前后除雾与内循环；首页和详情共享状态。
- **媒体**：三首示意曲目的播放 / 暂停、切歌、进度拖动和演示音量；页面共享状态。
- **设置**：版本和运行环境、演示数据断连、低胎压场景及退出操作。
- **倒车影像**：进入页面时自动打开 Linux `/dev/video9`，优先请求 1280 × 720 / 30 FPS；画面水平镜像，以 Qt 平滑算法按比例显示并叠加带深色描边的绿、黄、红平滑曲线辅助线。Linux 使用 OpenCV 3.4.16 单目地面测距，80 cm 内触发后方障碍物警告；返回或按 Esc 自动停止并释放设备。

车辆数值和媒体计时仍为模拟数据。Linux 已接入 V4L2 摄像头、WLAN 后台启动、定期 NTP / RTC 校时和高德驾车路径规划（`NavigationService`，固定起终点模式，读取 `/Kd1234/config/navigation.ini`；路线失败时自动回退离线示意内容并重试）；尚未接入 CAN / 串口、实时地图渲染（当前仅文字摘要与转向指令）、真实定位和音频输出。单目测距必须安装有效的 `/etc/nev-smarthmi/camera_calibration.ini` 后才启用；温度范围、胎压场景、测距阈值和辅助线均未按实车标定。WLAN 默认复用 `/Kd1234/wifi_new` 和 `/Kd1234/wpa_supplicant.conf`，详情见 [WLAN 自动启动说明](doc/WIFI_STARTUP.md)。RTC 使用方法和断电保持条件见 [时间同步与 RTC 部署说明](doc/TIME_SYNC_RTC.md)。

## 开发板凭据存放位置

下表说明运行时凭据和参数在开发板上的最终位置。仓库内只放模板文件，**真实凭据一律在板端手工填入**；`.gitignore` 已屏蔽下列文件避免误提交。

| 内容 | 开发板位置 | 仓库模板 | 权限 |
| --- | --- | --- | --- |
| 高德导航 API Key | `/Kd1234/config/navigation.ini` | `config/navigation.ini.example` | `600` |
| WiFi 名称和密码 | `/Kd1234/wpa_supplicant.conf`（不可读时回退 `/etc/wpa_supplicant.conf`） | — | `600` |
| WLAN 启动参数 | `/Kd1234/config/wifi.ini`，可选 | `config/wifi.ini.example` | `600` |

模板文件包含所有允许的键名和默认值说明，部署时复制到板端对应路径后改写真实值。`navigation.ini` 与 `wifi.ini` 仅保存非敏感参数；Wi-Fi 密码和高德 Web 服务 Key 分别存放在 `wpa_supplicant.conf` 与 `navigation.ini` 中并设置 `600`，不在程序源码或仓库里留底。详见 [WLAN 自动启动说明](doc/WIFI_STARTUP.md) 与 [构建与部署说明](doc/BUILD_AND_DEPLOY.md)。

手机定位推送 App（NEV-Locator，Android）：每秒向板端 UDP 45454 推送 GPS 定位，并支持中文地名设置目的地。工程与命令行编译指南见 [android/README.md](android/README.md)；无手机时的板端自测工具为 `scripts/udp_location_simulator.py`。

## 设计与实现

依据用户提供的原型 JPG、根目录 [`.md`](.md) 和 `NEV_Smart_HMI_Project_Spec.pdf` 实现。采用 Qt Core / Gui / Widgets 与 C++11，不依赖 Qt Quick、新版控件或外部在线资源；API 兼容目标是 Qt 5.4.1。

文字、导航、图标、地图和状态由 Qt 实时绘制，按钮与滑块为真实控件。车辆和专辑封面由 Qt 资源系统打包；倒车全屏页在 Linux 使用实时 V4L2 画面。素材说明见 [assets/images/README.md](assets/images/README.md)。

`VehicleDataCenter` 统一管理演示状态；`HmiCard` 组织可复用卡片；`MainWindow` 以 `QStackedWidget` 管理六页和倒车覆盖层。具体接口和测试见 [UI 实现说明](doc/UI_IMPLEMENTATION.md)。

Qt Designer 表单位于 `src/ui/main_window.ui`，已加入 `NEV-SmartHMI.pro` 的 `FORMS`。运行界面的动态状态和自绘卡片逻辑由同目录 C++ 文件实现。

## 文档

| 文档 | 内容 |
| --- | --- |
| [PRD.md](PRD.md) | 十模块需求与阶段边界 |
| [TECH_SPEC.md](TECH_SPEC.md) | 架构和 Qt 5.4.1 兼容约束 |
| [UI_IMPLEMENTATION.md](doc/UI_IMPLEMENTATION.md) | 当前页面、状态模型、验证结果和限制 |
| [MONOCULAR_DISTANCE.md](doc/MONOCULAR_DISTANCE.md) | OpenCV 3.4.16 板端依赖、标定配置、算法与验证限制 |
| [TIME_SYNC_RTC.md](doc/TIME_SYNC_RTC.md) | 定期 NTP 校时、RTC 写入和离线时间验证 |
| [WIFI_STARTUP.md](doc/WIFI_STARTUP.md) | 程序启动 WLAN、配置、日志与板端验证 |
| [BUILD_AND_DEPLOY.md](doc/BUILD_AND_DEPLOY.md) | Windows 构建、运行、测试及 ARM 待办 |
| [CHANGELOG.md](CHANGELOG.md) | 实际变更日志 |
| [BUG_TRACKER.md](BUG_TRACKER.md) | 缺陷与修复记录 |
| [GIT_WORKFLOW.md](doc/GIT_WORKFLOW.md) | Git 提交推送脚本说明 |
| [PROJECT_STRUCTURE.md](doc/PROJECT_STRUCTURE.md) | 框架目录与当前实现增量 |

## Git 仓库

远端为 [tfghcjdk/win-lin-Qt-](https://github.com/tfghcjdk/win-lin-Qt-) 的 `main`，本地已关联 `origin/main`。根目录 `git_commit_push.bat` 可提交并推送后续修改，私钥保留在仓库之外。

本机 `build/`、`.build-ui/` 和 Qt Creator 用户配置由 `.gitignore` 排除。仓库中早期遗留的 `Qt1604.pro`、根目录 `mainwindow.*`、旧资源文件和 `.build-ui/` 已在 0.2.1 清理；当前唯一应用入口为 `NEV-SmartHMI.pro`。
