# 项目目录与文件职责

本项目直接位于 `D:\QTXM\Qt164\Qt1604`，不额外嵌套同名工程目录。目录按根目录 `.md` 的 NEV-SmartHMI 规划落地。

```text
Qt1604/
├── .md                               原始需求资料，保留原文
├── .gitignore                        构建产物、本机配置和常见凭据忽略规则
├── .gitattributes                    文本换行规则，bat 使用 CRLF
├── CMakeLists.txt                    Qt Widgets / C++11 应用构建入口
├── NEV-SmartHMI.pro                  当前唯一 Qt Creator 应用入口
├── hmi_sources.pri                   qmake 共用源码清单
├── README.md                        项目导航与状态
├── PRD.md                           十个功能模块的需求与验收
├── TECH_SPEC.md                     技术架构与接口约束
├── BUG_TRACKER.md                   缺陷追踪表
├── CHANGELOG.md                     实际变更记录
├── git_commit_push.bat              Windows 自动提交与推送工具
├── doc/
│   ├── README.md                    文档索引
│   ├── NEV_Smart_HMI_Project_Spec.md 项目启动说明（Markdown）
│   ├── PROJECT_STRUCTURE.md         本目录说明
│   ├── DEVELOPMENT_GUIDE.md         开发任务模板与协作规程
│   ├── BUILD_AND_DEPLOY.md          构建与部署阶段说明
│   └── GIT_WORKFLOW.md              Git 脚本用法与恢复说明
├── scripts/
│   ├── README.md
│   ├── git_commit.py                Python 日志联动工具占位
│   ├── build_arm.sh                 ARM 构建占位
│   └── deploy_to_board.sh           板端部署占位
├── src/
│   ├── README.md
│   ├── main.cpp                     程序入口、字体和 QSS 加载
│   ├── core/
│   │   ├── README.md
│   │   ├── vehicle_data_center.h    UI 演示状态接口
│   │   ├── vehicle_data_center.cpp  UI 演示状态实现
│   │   ├── system_manager.h
│   │   └── system_manager.cpp
│   ├── hardware/
│   │   ├── README.md
│   │   ├── can_bus.h
│   │   ├── can_bus.cpp
│   │   ├── serial_port.h
│   │   ├── serial_port.cpp
│   │   ├── camera_v4l2.h           V4L2 摄像头采集线程接口
│   │   ├── camera_v4l2.cpp         MJPEG/YUYV、MMAP 与安全释放实现
│   │   ├── monocular_distance.h    单目地面测距接口
│   │   └── monocular_distance.cpp  OpenCV 3.4.16 轮廓检测与距离换算
│   ├── ui/
│   │   ├── README.md
│   │   ├── main_window.h
│   │   ├── main_window.cpp          六页导航和倒车覆盖层
│   │   ├── hmi_cards.h/.cpp         可复用功能卡片
│   │   ├── paint_helpers.h/.cpp     图标、地图和素材绘制
│   │   ├── header_bar/README.md      F01 顶部状态
│   │   ├── side_nav/README.md        F02 左侧导航
│   │   ├── dashboard/README.md       F03 仪表 / F04 续航胎压 / F05 能量
│   │   ├── hvac/README.md            F06 空调
│   │   ├── navigation/README.md      F07 导航提示
│   │   ├── reverse_camera/README.md  F08 倒车影像
│   │   ├── media/README.md           F09 多媒体
│   │   └── trip_info/README.md       F10 行车与保养
│   └── common/
│       ├── README.md
│       ├── theme_manager.h
│       ├── theme_manager.cpp
│       └── vehicle_types.h
├── assets/
│   ├── README.md
│   ├── hmi.qrc                     Qt 嵌入资源清单
│   ├── images/README.md             图片与图标预留
│   ├── images/prototype.jpg          用户提供的原型静态素材
│   ├── fonts/README.md              中文字体预留
│   └── qss/
│       ├── README.md
│       └── dark_theme.qss           已实现暗色控件样式
└── tests/
    ├── README.md
    ├── ui_tests.cpp                 UI 交互和边界测试
    ├── ui_tests.pro                 Qt Test 工程入口
    └── mock_can_publisher.py        CAN 数据仿真占位
```

## 占位约定

- UI、演示状态和 QSS 已实现；硬件驱动、系统管理、主题管理、预留 `.py` 和 `.sh` 仍为注释占位。
- 原文的 `name.h/.cpp` 在实际目录中拆为独立的 `.h` 与 `.cpp` 文件。
- 原文 UI 子目录保留模块职责 README；当前共享实现集中在 `main_window.*`、`hmi_cards.*` 和 `paint_helpers.*`。
- 目录内 README 同时使预留目录可被 Git 跟踪，无需额外 `.gitkeep`。
- 当前按用户明确指定的 Windows `.bat` 流程实现 Git 工具，Python 日志联动暂不实现。
- 原始资料的 `doc/*.pdf` 与 `doc/*.html` 为未来可选导出产物，本次以 Markdown 项目启动说明替代，不创建空 PDF / HTML。

## 层次关系

`src/ui` 负责展示与交互，`src/core` 管理业务数据和状态，`src/hardware` 负责设备适配与通信，`src/common` 存放共享类型和主题定义。后续 Windows 模拟和 ARM 真实硬件接入应遵循同一业务接口；具体约束见根目录 TECH_SPEC。

## 旧工程清理

关联远端时取回的 `Qt1604.pro`、旧 Qt Creator 用户配置、根目录 `main.cpp` / `mainwindow.*`、`resources.qrc`、旧 `images/` 和 `.build-ui/` 均来自早期提交 `ec828ec`。这些文件不被当前应用引用，已在 0.2.1 清理。当前目录只保留 `NEV-SmartHMI` 工程、其文档以及可重新生成的本机 `build/` 验证产物。

## 0.2.0-ui 实现增量

原框架树作为初始化记录保留。当前 `NEV-SmartHMI.pro` / `CMakeLists.txt` 已可构建应用，`src/main.cpp`、`src/core/vehicle_data_center.*`、`src/ui/main_window.*` 和 QSS 已实现；新增 `hmi_sources.pri`、`src/ui/hmi_cards.*`、`src/ui/paint_helpers.*`、`assets/hmi.qrc`、`assets/images/prototype.jpg`、`tests/ui_tests.cpp/.pro` 与 `doc/UI_IMPLEMENTATION.md`。不再适用初始“所有源文件仅注释”说明。
