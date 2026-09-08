# 构建与部署说明

## 当前工程入口

项目处于框架阶段，C++ 文件仅有注释，没有 `main()` 或窗口实现，不能启动 HMI 程序。

| 入口 | 当前行为 |
| --- | --- |
| `CMakeLists.txt` | `LANGUAGES NONE`；提供 `NEV-SmartHMI-Skeleton` 源文件浏览目标，不检测 Qt 或编译器 |
| `NEV-SmartHMI.pro` | `TEMPLATE = aux`；供 Qt Creator 浏览文件，不生成应用 |
| `scripts/build_arm.sh` | 注释占位，无构建行为 |
| `scripts/deploy_to_board.sh` | 注释占位，无设备传输或启动行为 |

在 Qt Creator 中选择“打开文件或项目”，打开根目录 `NEV-SmartHMI.pro`。框架阶段没有可执行文件，因此不应配置启动程序或将“运行”失败作为产品缺陷。

## 可选：检查 CMake 框架入口

本项目框架入口声明 CMake 3.5 及以上。若本机 CMake、Ninja 已在 PATH 中，可在 PowerShell 执行：

```powershell
Set-Location -LiteralPath 'D:\QTXM\Qt164\Qt1604'
cmake -S . -B build/skeleton -G Ninja
cmake --build build/skeleton --target NEV-SmartHMI-Skeleton
```

上面的 `-S / -B` 调用形式需 CMake 3.13 及以上；使用更早版本时在单独构建目录调用 `cmake <源目录>`。没有 Ninja 时选择本机已安装的生成器。目标完成仅说明框架入口有效，不验证 Qt、MSVC、ARM 编译或产品功能。

## 开始功能开发后接入应用目标

1. 明确 ARM 工具链和 sysroot，确认其 ABI 与板端 Qt 库一致；不能仅凭 Cortex-A53 推断使用 aarch64。
2. 将 CMake 项目语言设为 CXX，明确要求 C++11，并添加真实源文件和 Qt 5.4.1 兼容的 Qt 模块。
3. 将 qmake 的 `TEMPLATE` 从 `aux` 调整为 `app`，接入 C++11、真实 `SOURCES` / `HEADERS`、资源文件和所需 Qt 模块。
4. 实现最小程序入口与窗口后，再生成可执行程序。按平台选择模拟实现或 Linux 硬件实现，避免 Windows 编译直接包含 Linux 设备头文件。
5. 在 Windows Qt 5.14.2 / MSVC 2017 32-bit 和 ARM Qt 5.4.1 两端验证构建与启动。

上述操作属于后续实施计划，本次没有加入功能代码或虚构可运行目标。

## ARM 构建和部署参数清单

| 参数 | 当前状态 |
| --- | --- |
| 交叉编译器可执行路径、版本和目标 ABI | 待提供 |
| Qt 5.4.1 的 ARM 安装路径 / qmake / mkspec | 待提供 |
| 板端 sysroot、libc 与依赖库 | 待提供 |
| 屏幕和触控平台插件、字体目录 | 待板端核实 |
| CAN / UART / V4L2 设备节点、驱动和权限 | 待板端核实 |
| 音频后端和 Qt Multimedia 可用性 | 待板端核实 |
| 板端地址、登录账户、部署路径和启动命令 | 待提供 |

这些参数齐备后再实现 `build_arm.sh` 和 `deploy_to_board.sh`。部署时应记录二进制和依赖版本、校验结果、启动日志及回退办法；本阶段没有执行网络部署。

## 本阶段验证记录（2026-09-08）

- 使用本机 Qt 5.14.2 MinGW 32-bit 的 qmake 成功生成辅助工程 Makefile，并使用对应 `mingw32-make` 完成框架目标检查。输出符合 `TEMPLATE = aux` 的预期：没有需要编译的应用目标。
- 检查确认所有 CMake 显式引用的文件存在，目录 README 已被 qmake 工程收录，项目 Markdown 相对链接均能解析到现有文件。
- 检查确认 C++、预留 Python 与 shell 文件仅包含注释，未写入产品功能实现。
- 本机已检查的位置和 PATH 未发现 CMake，未实际运行 CMake 配置；本次只核对其文件引用和框架配置。
- 本次使用 MinGW 仅验证辅助工程入口，没有验证需求中的 MSVC 2017 32-bit、Qt 5.4.1 ARM 编译或真实开发板运行。
