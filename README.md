# NEV-SmartHMI 新能源汽车智能座舱

当前版本：**0.2.1-ui**。已实现 1024 × 600 Qt Widgets 页面与交互演示，可编译运行。

## 打开与运行

在 Qt Creator 中打开根目录 **`NEV-SmartHMI.pro`**，选择本机 **Desktop Qt 5.14.2 MinGW 32-bit**，执行 qmake 后构建、运行。该工程已由 `TEMPLATE = aux` 改为 `app`。若 Qt Creator 仍显示旧的辅助工程状态，请重新运行 qmake 或关闭并重新打开本工程。

本机本次构建的程序位于 `build/ui/release/NEV-SmartHMI.exe`。完成运行库部署后可直接双击。按 **Esc** 关闭倒车预览、从详情返回首页；在首页按 Esc 退出，也可在设置页点击“退出应用”。

## 已实现的页面

- **首页**：顶部时间和演示连接状态、左侧导航、车速与档位、续航与四轮胎压、能量流、空调、导航、倒车预览、媒体、行车统计和维护提醒。
- **车辆状态**：放大的续航 / 胎压、能量与行程卡片；支持正常、数据断开及左前低胎压演示场景。
- **导航**：离线示意地图与路线卡片，可开始 / 结束演示导航。
- **空调**：0.5 ℃ 步进、演示范围 16–30 ℃、独立 0–8 档风量、Auto、A/C、前后除雾与内循环；首页和详情共享状态。
- **媒体**：三首示意曲目的播放 / 暂停、切歌、进度拖动和演示音量；页面共享状态。
- **设置**：版本和运行环境、演示数据断连、低胎压场景及退出操作。
- **倒车预览**：静态图片全屏查看，支持按钮与 Esc 返回。

所有车辆数值、地图、媒体计时和摄像头画面均为**演示**。尚未接入 CAN / 串口、实时地图、V4L2、音频输出、NTP / RTC 同步和持久化。演示温度范围、风量档位及胎压场景不是实车标定参数。

## 设计与实现

依据用户提供的原型 JPG、根目录 [`.md`](.md) 和 `NEV_Smart_HMI_Project_Spec.pdf` 实现。采用 Qt Core / Gui / Widgets 与 C++11，不依赖 Qt Quick、新版控件或外部在线资源；API 兼容目标是 Qt 5.4.1。

文字、导航、图标、地图和状态由 Qt 实时绘制，按钮与滑块为真实控件。车辆、专辑封面和倒车画面取自用户提供原型的静态素材区域，由 Qt 资源系统打包，无桌面绝对路径依赖。素材说明见 [assets/images/README.md](assets/images/README.md)。

`VehicleDataCenter` 统一管理演示状态；`HmiCard` 组织可复用卡片；`MainWindow` 以 `QStackedWidget` 管理六页和倒车覆盖层。具体接口和测试见 [UI 实现说明](doc/UI_IMPLEMENTATION.md)。

## 文档

| 文档 | 内容 |
| --- | --- |
| [PRD.md](PRD.md) | 十模块需求与阶段边界 |
| [TECH_SPEC.md](TECH_SPEC.md) | 架构和 Qt 5.4.1 兼容约束 |
| [UI_IMPLEMENTATION.md](doc/UI_IMPLEMENTATION.md) | 当前页面、状态模型、验证结果和限制 |
| [BUILD_AND_DEPLOY.md](doc/BUILD_AND_DEPLOY.md) | Windows 构建、运行、测试及 ARM 待办 |
| [CHANGELOG.md](CHANGELOG.md) | 实际变更日志 |
| [BUG_TRACKER.md](BUG_TRACKER.md) | 缺陷与修复记录 |
| [GIT_WORKFLOW.md](doc/GIT_WORKFLOW.md) | Git 提交推送脚本说明 |
| [PROJECT_STRUCTURE.md](doc/PROJECT_STRUCTURE.md) | 框架目录与当前实现增量 |

## Git 仓库

远端为 [tfghcjdk/win-lin-Qt-](https://github.com/tfghcjdk/win-lin-Qt-) 的 `main`，本地已关联 `origin/main`。根目录 `git_commit_push.bat` 可提交并推送后续修改，私钥保留在仓库之外。

本机 `build/`、`.build-ui/` 和 Qt Creator 用户配置由 `.gitignore` 排除。仓库中早期遗留的 `Qt1604.pro`、根目录 `mainwindow.*`、旧资源文件和 `.build-ui/` 已在 0.2.1 清理；当前唯一应用入口为 `NEV-SmartHMI.pro`。
