# Git 提交与推送工作流

当前可用入口为项目根目录的 [`git_commit_push.bat`](../git_commit_push.bat)。它是单文件 Windows 批处理脚本，内嵌 Windows 自带的 PowerShell，用于安全读取中文和包含命令行特殊字符的提交说明，不需要额外的 `.ps1` 或 Python 文件。

## 固定配置与前提

| 项目 | 配置 |
| --- | --- |
| 项目目录 | `D:\QTXM\Qt164\Qt1604` |
| GitHub 仓库 | [tfghcjdk/win-lin-Qt-](https://github.com/tfghcjdk/win-lin-Qt-) |
| origin 地址 | `git@github.com:tfghcjdk/win-lin-Qt-.git` |
| 本地和远程分支 | `main` |
| SSH 私钥 | `D:\QTXM\Qt164\Githupmm\.ssh\id_ed25519` |
| 运行环境 | Windows 10/11、Windows PowerShell 5.1、Git for Windows 2.28 或更新版本，`git.exe` 已加入 PATH |
| 文件格式 | `git_commit_push.bat` 使用 UTF-8 无 BOM、CRLF 换行；脚本运行时自动切换并在退出前恢复终端代码页 |

脚本仅检查私钥文件是否存在；私钥由 SSH 客户端在连接时使用。应将对应公钥配置到拥有该 GitHub 仓库写权限的账户中。私钥保留在上述仓库外路径，不要复制进项目，也不要粘贴到 Markdown 或提交说明中。首次 SSH 连接可能要求确认主机指纹；加密的私钥可能要求输入口令。

首次提交前，如尚未配置 Git 身份，可在项目已经初始化后设置当前仓库的身份：

```bat
cd /d D:\QTXM\Qt164\Qt1604
git config user.name "你的姓名"
git config user.email "你的提交邮箱"
```

如果目录还不是 Git 仓库，首次运行脚本会创建 `main` 分支并添加 `origin`；因此也可以先运行一次，再根据提示补齐身份。脚本不会创建 GitHub 远程仓库，远程仓库需已存在且账户具有写权限。

## 正常提交

双击根目录的 `git_commit_push.bat`，或在命令提示符中执行：

```bat
"D:\QTXM\Qt164\Qt1604\git_commit_push.bat"
```

1. 自动切换到固定项目目录，检查 Git、SSH 私钥、仓库根目录、当前分支和远程地址。不存在本地仓库时自动初始化；没有 `origin` 时自动添加。
2. 当前分支必须为 `main`；已有的 `origin` 获取地址和推送地址必须都唯一且与固定 SSH 地址一致。未完成的合并、变基等操作会使脚本停止并提示处理。
3. 检查工作区与暂存区。没有变动时提示并退出，既不创建空提交，也不访问远端。
4. 有变动时执行 `git add .`，包括新增、修改和删除的文件，遵循 `.gitignore`。
5. 输入一行 Commit Message。直接回车或仅输入空白时，使用 `Auto-commit: %DATE% %TIME%`，日期时间取输入完成时 Windows 命令提示符的当前值。
6. 使用 UTF-8 临时文件传递提交说明并执行 `git commit`，然后执行 `git push origin main`。临时文件位于系统临时目录，结束时清理。
7. 推送成功后显示中文成功提示，等待 3 秒并退出。遇到错误时输出原始 Git 错误和中文提示，执行 `pause` 等待按键。

提交说明可包含中文、`&`、双引号、单引号、`%`、`!`、`|`、`<`、`>` 和括号。它们作为文本传入 Git，不作为批处理命令执行。输入示例：

```text
docs: 完成 "Qt 5.4.1" 框架说明 & 校验 100% !
```

无变动时也保留 3 秒用于查看提示。双击运行时窗口随后关闭；从已有终端运行时只返回该终端，不关闭用户原来的终端。发生错误时，只有按键结束 `pause` 后脚本才会退出。

脚本的 SSH 参数只作用于此次运行的子进程，不写入全局 Git 配置。它指定该私钥、`IdentitiesOnly=yes`、20 秒 SSH 建连超时和一次建连尝试；主机身份校验保持 SSH 默认行为。

## 推送失败后的恢复

推送失败不会删除已经成功创建的本地提交。修复网络、SSH 权限或远端分支问题后，可以只重试推送：

```bat
"D:\QTXM\Qt164\Qt1604\git_commit_push.bat" --push-only
```

`--push-only` 复用同样的目录、分支、远程与 SSH 校验，直接执行 `git push origin main`，不会运行 `git add .` 或创建提交；本地未提交的修改不会随此次重推上传。普通模式看到“无变动”仅表示工作区和暂存区干净，并不代表上次的提交已到达 GitHub，因此失败后应使用这个参数。

| 提示或错误 | 处理方式 |
| --- | --- |
| 未找到 Git | 安装 Git for Windows 并加入 PATH，重新打开终端后重试。 |
| 找不到 SSH 私钥 | 核实固定路径中的文件名；脚本不会生成、迁移或覆盖密钥。 |
| `Author identity unknown` | 按前面的命令配置当前仓库 `user.name` 和 `user.email`，再正常运行脚本。 |
| 当前分支不是 `main` / detached HEAD | 先运行 `git status` 和 `git branch` 检查，处理现有修改后手动切换或创建 `main`，再运行脚本。脚本不会自动重命名已有分支。 |
| `origin` 地址不符合配置 | 运行 `git remote -v` 检查获取和推送地址，核实仓库后再手动调整，见下文。 |
| `Permission denied (publickey)` | 核实该私钥对应公钥已登记到正确 GitHub 账户，以及该账户具有仓库写权限。 |
| 网络断开、连接超时 | 修复连接后运行 `--push-only`。 |
| `non-fast-forward` / `fetch first` | 远端 `main` 含本地没有的提交。先获取并检查历史，完成合并或变基，再使用 `--push-only`。脚本不会强制推送或自动覆盖远端。 |
| Git 操作未完成 / 合并冲突 | 按 `git status` 的提示完成或主动中止现有操作，确认状态后重试。 |

确认 `origin` 应指向本项目后，可手动设置获取地址；如果曾额外配置了 `pushurl`，清除它使推送地址重新继承获取地址：

```bat
git remote -v
git remote set-url origin git@github.com:tfghcjdk/win-lin-Qt-.git
git config --get-all remote.origin.pushurl
git config --unset-all remote.origin.pushurl
git remote -v
```

没有配置 `pushurl` 时，最后的 `--unset-all` 返回非零表示不存在该设置；无需因此创建 `pushurl`。若存在多个获取地址或 URL 重写规则，应先用 `git config --get-regexp url`、`git config --get-all remote.origin.url` 核对并手动整理，脚本不会静默覆盖现有配置。

目录首次初始化但远端已有历史时，首次推送可能被拒绝。这时在项目目录的同一个命令提示符中获取远端并检查差异：

```bat
set "GIT_SSH_COMMAND=ssh -i D:/QTXM/Qt164/Githupmm/.ssh/id_ed25519 -o IdentitiesOnly=yes"
git fetch origin
git log --oneline --graph --decorate --all
git status
```

根据历史关系决定合并或变基并解决冲突；如果本地与远端分别初始化、历史互不关联，应先核对远端内容，再选择有意保留双方历史的合并方案。不要通过强制推送来跳过检查。这里手工设置的 `GIT_SSH_COMMAND` 只在当前命令提示符内有效，关闭该终端即可清除。

## 文档记录约定

本次按用户要求提供可用的 Windows `.bat` 提交入口。原始 `.md` 中 `scripts/git_commit.py` 的 `feat` / `fix` 自动修改 `CHANGELOG.md` 和 `BUG_TRACKER.md` 属于后续工作流设计，本脚本不推断提交类型或自动写入这些文件。

功能迭代与问题修复完成后，先人工更新根目录的 `CHANGELOG.md`，涉及缺陷时同步更新 `BUG_TRACKER.md`，再运行本脚本一并提交。项目功能代码、ARM 构建和部署自动化仍以项目框架中的阶段说明为准。

## 隔离验证记录（2026-09-08）

在系统临时目录创建脚本副本，仅替换副本中的项目路径、远程地址和测试密钥路径，使用真实 Git 与本地 bare 仓库完成以下 11 项检查：

1. 无仓库时初始化 `main`，回车生成带日期时间的默认提交并推送成功。
2. 无变更时提示退出，不生成提交或推送，保留查看提示的等待时间。
3. 中文、引号、`&`、`%`、`!`、管道、重定向符与括号提交说明按原文写入 Git。
4. 本地远端拒绝推送时返回错误，显示原始原因，并保留本地提交。
5. 推送失败后正常重跑显示无变更和重推提示，不误报远端已经同步。
6. `--push-only` 成功重推已有提交，不创建重复提交。
7. 错误分支时停止，不执行暂存。
8. 错误远端时停止，不覆盖原远端配置。
9. 配置多个推送地址时停止。
10. 暂存遇到 `index.lock` 错误时停止，不继续提交。
11. 错误发生后，保持输入流打开时进程等待按键；按键后返回非零退出码。

以上检查全部通过，批处理文件也已确认使用 UTF-8 无 BOM 和 CRLF。本次没有初始化真实项目的 Git 仓库、创建真实项目提交或向 GitHub 推送；指定私钥只检查了文件存在性，实际 SSH 认证和网络连通性留待用户正式运行时验证。

## 后续实际仓库关联（2026-09-08）

初次隔离验证完成后，按用户上传要求初始化了本地仓库，关联 `origin/main`，使用指定私钥成功读取远端并获取 `ec828ec` 历史。远端已有 Qt1604 工程，已保留其全部文件和提交历史，当前框架在此基础上追加。上节的“没有初始化真实项目或向 GitHub 推送”描述的是隔离测试阶段，不代表本地仓库的后续状态。
