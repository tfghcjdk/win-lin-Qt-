@echo off
setlocal EnableExtensions DisableDelayedExpansion
for /f "tokens=2 delims=:" %%C in ('chcp') do set "NEV_ORIGINAL_CODEPAGE=%%C"
chcp 65001 >nul
set "NEV_AUTOMATION_SELF=%~f0"
set "NEV_AUTOMATION_MODE=%~1"
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -Command "$s=[IO.File]::ReadAllText($env:NEV_AUTOMATION_SELF,[Text.Encoding]::UTF8); & ([scriptblock]::Create(($s -split '(?m)^# POWERSHELL_PAYLOAD\r?$',2)[1]))"
set "NEV_EXIT_CODE=%ERRORLEVEL%"
if "%NEV_EXIT_CODE%"=="0" goto close_after_delay
if "%NEV_EXIT_CODE%"=="10" goto close_after_delay
echo.
echo [失败] 操作未完成，请查看上方错误。按任意键后退出。
pause
goto finish

:close_after_delay
echo [提示] 窗口将在 3 秒后自动关闭。
timeout /t 3 /nobreak >nul 2>&1
if errorlevel 1 powershell.exe -NoLogo -NoProfile -Command "Start-Sleep -Seconds 3"
set "NEV_EXIT_CODE=0"

:finish
if defined NEV_ORIGINAL_CODEPAGE chcp %NEV_ORIGINAL_CODEPAGE% >nul
exit /b %NEV_EXIT_CODE%

# POWERSHELL_PAYLOAD
# This payload stays inside the .bat file; no external .ps1 file is needed.
$ErrorActionPreference = 'Stop'
$utf8 = New-Object System.Text.UTF8Encoding($false)
[Console]::InputEncoding = $utf8
[Console]::OutputEncoding = $utf8
$OutputEncoding = $utf8

$projectPath = 'D:\QTXM\Qt164\Qt1604'
$expectedRemote = 'git@github.com:tfghcjdk/win-lin-Qt-.git'
$privateKey = 'D:\QTXM\Qt164\Githupmm\.ssh\id_ed25519'
$messageFile = $null

function Read-GitResult {
    param([string[]]$Arguments)
    $savedPreference = $ErrorActionPreference
    try {
        $ErrorActionPreference = 'Continue'
        $lines = @(& $script:gitExe @Arguments 2>&1 | ForEach-Object { $_.ToString() })
        $code = $LASTEXITCODE
    }
    finally { $ErrorActionPreference = $savedPreference }
    return [pscustomobject]@{ ExitCode = $code; Lines = $lines }
}

function Get-GitOutput {
    param([string[]]$Arguments)
    $result = Read-GitResult -Arguments $Arguments
    if ($result.ExitCode -ne 0) {
        $result.Lines | ForEach-Object { Write-Host $_ }
        throw ('Git 检查失败，退出码：' + $result.ExitCode)
    }
    return $result.Lines
}

function Invoke-Git {
    param([string[]]$Arguments)
    $savedPreference = $ErrorActionPreference
    try {
        $ErrorActionPreference = 'Continue'
        & $script:gitExe @Arguments
        $code = $LASTEXITCODE
    }
    finally { $ErrorActionPreference = $savedPreference }
    if ($code -ne 0) { throw ('Git 命令失败，退出码：' + $code) }
}

try {
    Write-Host '========================================'
    Write-Host ' NEV-SmartHMI Git 自动提交与推送'
    Write-Host '========================================'
    $mode = [string]$env:NEV_AUTOMATION_MODE
    if ($mode -notin @('', '--push-only')) {
        throw '不支持的参数。正常提交请直接运行；重试推送请使用 --push-only。'
    }
    $pushOnly = $mode -eq '--push-only'
    $gitCommand = Get-Command git.exe -CommandType Application -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($null -eq $gitCommand) { throw '未找到 Git，请安装 Git for Windows 并将 Git 加入 PATH。' }
    $script:gitExe = $gitCommand.Source
    if (-not [IO.Directory]::Exists($projectPath)) { throw ('项目目录不存在：' + $projectPath) }
    Set-Location -LiteralPath $projectPath
    Write-Host ('[目录] ' + $projectPath)

    # Test only file existence. Never read or copy the private key.
    if (-not [IO.File]::Exists($privateKey)) { throw ('找不到 SSH 私钥：' + $privateKey) }
    $keyForSsh = $privateKey.Replace('\', '/')
    $env:GIT_SSH_COMMAND = 'ssh -i "' + $keyForSsh + '" -o IdentitiesOnly=yes -o ConnectTimeout=20 -o ConnectionAttempts=1'
    $env:GIT_SSH_VARIANT = 'ssh'

    $repository = Read-GitResult -Arguments @('rev-parse', '--show-toplevel')
    if ($repository.ExitCode -ne 0) {
        if (Test-Path -LiteralPath (Join-Path $projectPath '.git')) {
            $repository.Lines | ForEach-Object { Write-Host $_ }
            throw '已有 .git，但无法读取仓库。请修复仓库或目录权限后重试。'
        }
        if ($pushOnly) { throw '项目尚未初始化，没有可以重推的本地提交。请先正常运行脚本。' }
        Write-Host '[初始化] 当前目录不是 Git 仓库，正在创建 main 分支。'
        Invoke-Git -Arguments @('init', '--initial-branch=main')
    }
    else {
        $actualRoot = [IO.Path]::GetFullPath([string]$repository.Lines[0]).TrimEnd('\', '/')
        $expectedRoot = [IO.Path]::GetFullPath($projectPath).TrimEnd('\', '/')
        if (-not [string]::Equals($actualRoot, $expectedRoot, [StringComparison]::OrdinalIgnoreCase)) {
            throw ('当前目录属于上级仓库，已停止以避免提交其他项目：' + $actualRoot)
        }
    }

    $branch = Read-GitResult -Arguments @('symbolic-ref', '--quiet', '--short', 'HEAD')
    if ($branch.ExitCode -ne 0 -or $branch.Lines.Count -ne 1 -or $branch.Lines[0] -cne 'main') {
        throw '当前分支不是 main，或处于 detached HEAD 状态。请先检查 git status 并手动切换到 main。'
    }
    foreach ($state in @('MERGE_HEAD', 'CHERRY_PICK_HEAD', 'REVERT_HEAD', 'rebase-merge', 'rebase-apply', 'sequencer')) {
        $statePath = [string](Get-GitOutput -Arguments @('rev-parse', '--git-path', $state))
        if (Test-Path -LiteralPath $statePath) { throw ('存在未完成的 Git 操作：' + $state + '。请先处理完成。') }
    }

    $remotes = @(Get-GitOutput -Arguments @('remote'))
    if ($remotes -notcontains 'origin') {
        Write-Host '[远程] 添加指定的 origin。'
        Invoke-Git -Arguments @('remote', 'add', 'origin', $expectedRemote)
    }
    $fetchUrls = @(Get-GitOutput -Arguments @('remote', 'get-url', '--all', 'origin'))
    $pushUrls = @(Get-GitOutput -Arguments @('remote', 'get-url', '--push', '--all', 'origin'))
    if ($fetchUrls.Count -ne 1 -or $pushUrls.Count -ne 1 -or $fetchUrls[0] -cne $expectedRemote -or $pushUrls[0] -cne $expectedRemote) {
        Write-Host ('[预期] ' + $expectedRemote)
        Write-Host ('[当前 fetch] ' + ($fetchUrls -join ', '))
        Write-Host ('[当前 push] ' + ($pushUrls -join ', '))
        throw 'origin 地址或推送地址不符合配置，已停止。请按 doc/GIT_WORKFLOW.md 核对后手动调整。'
    }

    if (-not $pushOnly) {
        $changes = @(Get-GitOutput -Arguments @('status', '--porcelain=v1', '--untracked-files=all'))
        if ($changes.Count -eq 0) {
            Write-Host '[无变动] 工作区和暂存区没有需要提交的变动，本次未执行提交和推送。'
            Write-Host '[提示] 这不表示远端已同步。若上次推送失败，请运行 git_commit_push.bat --push-only。'
            exit 10
        }
        Write-Host '[暂存] 正在执行 git add .'
        Invoke-Git -Arguments @('add', '.')
        $staged = Read-GitResult -Arguments @('diff', '--cached', '--quiet', '--exit-code')
        if ($staged.ExitCode -eq 0) {
            Write-Host '[无变动] 暂存后没有可提交的差异，本次未执行提交和推送。'
            Write-Host '[提示] 若上次推送失败，请运行 git_commit_push.bat --push-only。'
            exit 10
        }
        if ($staged.ExitCode -ne 1) {
            $staged.Lines | ForEach-Object { Write-Host $_ }
            throw '无法检查暂存区，请运行 git status 查看原因。'
        }
        Write-Host '请输入 Commit Message（直接回车使用带当前时间的默认日志）：' -NoNewline
        $message = [Console]::ReadLine()
        if ($null -eq $message) { throw '未读取到输入，请在可交互的终端中运行脚本。' }
        if ([string]::IsNullOrWhiteSpace($message)) {
            $timestamp = & $env:ComSpec /d /c 'echo %DATE% %TIME%'
            if ($LASTEXITCODE -ne 0) { throw '无法获取当前日期和时间。' }
            $message = 'Auto-commit: ' + [string]$timestamp
        }
        # Pass a UTF-8 message file to Git, never interpolate user text into cmd.
        $messageFile = [IO.Path]::GetTempFileName()
        [IO.File]::WriteAllText($messageFile, $message + [Environment]::NewLine, $utf8)
        Write-Host '[提交] 正在创建本地提交。'
        Invoke-Git -Arguments @('-c', 'i18n.commitEncoding=utf-8', 'commit', '--cleanup=verbatim', '--file', $messageFile)
        Write-Host '[提交] 本地提交成功。'
    }
    else {
        $head = Read-GitResult -Arguments @('rev-parse', '--verify', 'HEAD')
        if ($head.ExitCode -ne 0) { throw 'main 尚无本地提交，无法重试推送。' }
        Write-Host '[仅推送] 本次直接推送 main 已有提交，不创建新提交。'
    }

    Write-Host '[推送] 正在执行 git push origin main，请等待。'
    Invoke-Git -Arguments @('push', 'origin', 'main')
    Write-Host '[成功] main 已成功推送至指定的 GitHub 仓库。'
    exit 0
}
catch {
    Write-Host ('[错误] ' + $_.Exception.Message) -ForegroundColor Red
    Write-Host '[提示] 已生成的本地提交会保留；排除问题后可用 --push-only 重试推送。'
    Write-Host '[提示] 身份设置、SSH、远端已有提交等处理方法请参阅 doc/GIT_WORKFLOW.md。'
    exit 1
}
finally {
    if ($messageFile -and [IO.File]::Exists($messageFile)) {
        Remove-Item -LiteralPath $messageFile -Force -ErrorAction SilentlyContinue
    }
}
