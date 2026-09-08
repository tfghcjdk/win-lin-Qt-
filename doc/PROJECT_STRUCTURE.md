# 项目目录与文件职责

本项目直接位于 `D:\QTXM\Qt164\Qt1604`，不额外嵌套同名工程目录。目录按根目录 `.md` 的 NEV-SmartHMI 规划落地。

```text
Qt1604/
├── .md                               原始需求资料，保留原文
├── .gitignore                        构建产物、本机配置和常见凭据忽略规则
├── .gitattributes                    文本换行规则，bat 使用 CRLF
├── CMakeLists.txt                    框架浏览目标，当前无应用编译目标
├── NEV-SmartHMI.pro                  Qt Creator 辅助工程入口
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
│   ├── main.cpp                     程序入口占位
│   ├── core/
│   │   ├── README.md
│   │   ├── vehicle_data_center.h
│   │   ├── vehicle_data_center.cpp
│   │   ├── system_manager.h
│   │   └── system_manager.cpp
│   ├── hardware/
│   │   ├── README.md
│   │   ├── can_bus.h
│   │   ├── can_bus.cpp
│   │   ├── serial_port.h
│   │   ├── serial_port.cpp
│   │   ├── camera_v4l2.h
│   │   └── camera_v4l2.cpp
│   ├── ui/
│   │   ├── README.md
│   │   ├── main_window.h
│   │   ├── main_window.cpp
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
│   ├── images/README.md             图片与图标预留
│   ├── fonts/README.md              中文字体预留
│   └── qss/
│       ├── README.md
│       └── dark_theme.qss           样式占位
└── tests/
    ├── README.md
    └── mock_can_publisher.py        CAN 数据仿真占位
```

## 占位约定

- `.h`、`.cpp`、`.qss` 以及预留 `.py`、`.sh` 当前均只有注释和职责说明，没有功能实现。
- 原文的 `name.h/.cpp` 在实际目录中拆为独立的 `.h` 与 `.cpp` 文件。
- 原文 UI 子目录未指定具体组件文件名，当前用 README 说明职责；开始对应模块开发时再建立文件。
- 目录内 README 同时使预留目录可被 Git 跟踪，无需额外 `.gitkeep`。
- 当前按用户明确指定的 Windows `.bat` 流程实现 Git 工具，Python 日志联动暂不实现。
- 原始资料的 `doc/*.pdf` 与 `doc/*.html` 为未来可选导出产物，本次以 Markdown 项目启动说明替代，不创建空 PDF / HTML。

## 层次关系

`src/ui` 负责展示与交互，`src/core` 管理业务数据和状态，`src/hardware` 负责设备适配与通信，`src/common` 存放共享类型和主题定义。后续 Windows 模拟和 ARM 真实硬件接入应遵循同一业务接口；具体约束见根目录 TECH_SPEC。

## 关联 GitHub 后保留的原有文件

上述树描述本次创建的 NEV-SmartHMI 框架。后续关联远端 `main` 时，已同时取回并保留远端原有的 `Qt1604.pro`、`Qt1604.pro.user`、根目录 `main.cpp` / `mainwindow.*`、`resources.qrc`、`images/` 和 `.build-ui/`。它们来自已有提交 `ec828ec`，不属于本次新写的功能代码，也未纳入 NEV-SmartHMI 辅助工程的编译目标。
