# NEV-SmartHMI 新能源汽车智能仪表与车载终端

项目根目录：`D:\QTXM\Qt164\Qt1604`。项目需求来源为根目录原始文件 [`.md`](.md)。

**当前阶段：项目框架与文档初始化。尚未实现任何 HMI 功能，也不生成可运行程序。** C++、QSS、ARM 构建、部署与仿真文件仅作占位；Windows Git 提交推送工具是本阶段唯一可执行的工作流功能。

## 目标环境

| 项目 | 约束 |
| --- | --- |
| 开发板 | X6818 / S5P6818，ARM Cortex-A53；附件报告的目标用户态为 `armv7l` |
| 目标系统 | Linux `3.4.39-9tripod`；实际 ABI、驱动与 sysroot 待板端核实 |
| 目标 Qt | Qt 5.4.1，作为所有 Qt API 的兼容下限 |
| Windows 模拟环境 | Qt 5.14.2，MSVC 2017 32-bit |
| 语言与界面 | C++11，Qt Widgets / QPainter，1024 × 600 触控布局 |

## 文档入口

| 文档 | 用途 |
| --- | --- |
| [PRD.md](PRD.md) | 10 个功能模块、交互需求与后续验收标准 |
| [TECH_SPEC.md](TECH_SPEC.md) | 分层架构、模块边界、线程与兼容约束 |
| [BUG_TRACKER.md](BUG_TRACKER.md) | 缺陷登记和验证记录 |
| [CHANGELOG.md](CHANGELOG.md) | 实际完成的版本变更 |
| [项目启动说明](doc/NEV_Smart_HMI_Project_Spec.md) | 阶段范围、实施顺序与待确认事项 |
| [目录说明](doc/PROJECT_STRUCTURE.md) | 文件树、模块职责和占位约定 |
| [开发协作规程](doc/DEVELOPMENT_GUIDE.md) | 功能开发任务模板和文档维护方法 |
| [构建与部署说明](doc/BUILD_AND_DEPLOY.md) | 当前工程入口及后续工具链接入条件 |
| [Git 操作说明](doc/GIT_WORKFLOW.md) | 自动提交、SSH、失败恢复与注意事项 |

## 使用项目框架

在 Qt Creator 中打开 `NEV-SmartHMI.pro`，可查看目录和文件。它当前采用 `TEMPLATE = aux`，不编译应用。`CMakeLists.txt` 同样仅提供源文件浏览目标；配置它不需要安装 Qt，也不能用于验证 Qt 或 ARM 工具链兼容性。

后续明确开始功能开发后，再接入 `QApplication`、窗口实现、Qt 模块和应用构建目标。具体步骤见[构建与部署说明](doc/BUILD_AND_DEPLOY.md)。

## 自动提交与推送

双击根目录 [`git_commit_push.bat`](git_commit_push.bat)，按中文提示输入提交说明。直接回车使用 `Auto-commit: %DATE% %TIME%`。脚本使用指定的 SSH 私钥，将本项目提交推送至 [tfghcjdk/win-lin-Qt-](https://github.com/tfghcjdk/win-lin-Qt-) 的 `main` 分支。

私钥保留在 `D:\QTXM\Qt164\Githupmm\.ssh\id_ed25519`，不应复制到项目内。运行细节与失败后的恢复办法见 [Git 操作说明](doc/GIT_WORKFLOW.md)。框架创建阶段已完成脚本隔离验证；随后按用户要求将本地 `main` 关联到该仓库的 `origin/main`，使用指定私钥连接。

提交前手动更新 `CHANGELOG.md`，修复缺陷时同步更新 `BUG_TRACKER.md`。当前批处理按用户指定的流程提交，不自动推断提交类型或改写这两份文档；原始需求中的 Python 日志联动工具留待后续开发。

## 范围与状态

- 已建立：项目目录、Markdown 需求与技术文档、源码占位、构建工程入口、Git 批处理。
- 待实现：10 个 HMI 模块、硬件通信、仿真、资源加载、ARM 构建与部署自动化。
- 待提供：实际原型图与素材、CAN / UART 协议、报警阈值、ARM 工具链与 sysroot、板端设备和部署参数。
- 原始 `.md` 内的 Python 代码及“已生成 PDF / HTML”文字作为输入资料保留；本阶段没有执行该代码，也没有生成或声称存在 PDF / HTML 文件。

## 远端已有工程的保留

连接 GitHub 时发现远端已有 `Qt1604.pro`、根目录 `main.cpp` / `mainwindow.*`、`resources.qrc`、`images/` 以及历史构建文件。本次保留这些已有文件和提交历史，再追加 NEV-SmartHMI 框架。查看新框架请打开 `NEV-SmartHMI.pro`；根目录 `Qt1604.pro` 属于远端原有工程，其功能未在本次修改或验证。

`.gitignore` 排除本机新增的构建产物与 Qt Creator 用户设置；远端历史中已经跟踪的同类文件仍保留，本次未进行历史清理。
