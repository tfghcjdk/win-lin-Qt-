# 开发板时间同步与 RTC 部署说明

## 实现行为

应用在 Linux 开发板上检测 `/Kd1234/config`：

1. 启动时读取硬件 RTC，将其 UTC 时间恢复到 Linux 系统时钟。
2. 没有同步记录或距离上次成功同步达到 6 小时时，后台依次请求 `ntp.aliyun.com`、`ntp.tencent.com` 和 `cn.pool.ntp.org`。
3. NTP 响应通过层级、模式和日期范围校验后，应用使用 `settimeofday()` 设置系统时间。
4. 系统时间设置成功后，通过 `RTC_SET_TIME` 将 UTC 写入第一个可用的 `/dev/rtc0`、`/dev/rtc1` 或 `/dev/rtc`。
5. 系统时间和 RTC 都更新成功后，写入 `/Kd1234/config/rtc_time_sync.state`。
6. 断网或同步失败时不修改系统时间和 RTC；程序每 10 分钟检查一次，网络恢复且同步到期后自动重试。

NTP 查询在工作线程执行，不阻塞界面。顶部界面按 UTC+8 每秒显示北京时间，运行日志位于 `/Kd1234/config/time_sync.log`。

旧版本的 `/Kd1234/config/.rtc_time_synced_once` 仍可读取。升级后不需要手工删除它；第一次到达 6 小时间隔并成功同步后，程序会创建新的状态文件。

## 运行前检查

```sh
ls -l /dev/rtc*
cat /proc/driver/rtc
hwclock -r -u
ls -ld /Kd1234/config
```

应用必须以 root 身份运行，或者具备设置系统时间和写 RTC 的能力。当前开发板以 root 登录时可直接运行。

## 首次联网校时验证

保持 iQOO 热点和 WLAN 已连接，然后启动新版本应用。首次没有状态文件时会立即尝试同步。等待约 15 秒后检查：

```sh
cat /Kd1234/config/time_sync.log
cat /Kd1234/config/rtc_time_sync.state
date -u
date
hwclock -r -u
```

成功日志包含：

```text
periodic NTP synchronization completed
```

状态文件记录成功时间、服务器和 RTC 设备。程序运行期间每 10 分钟检查一次是否到达 6 小时周期；未到期不会访问NTP。

如果日志显示 UDP 请求失败，需要确认手机热点没有拦截 UDP 123。若显示权限错误，需要以 root 启动；若显示 RTC 写入失败，需要检查 RTC 设备和内核驱动。

## 无 WiFi 验证

至少成功联网校时一次后关闭 WiFi，再正常重启开发板：

```sh
reboot
```

启动后检查：

```sh
date -u
date
hwclock -r -u
cat /Kd1234/config/time_sync.log
```

界面时钟应继续逐秒变化，日期和北京时间应正确。无网络时NTP失败不会清空状态，也不会把错误时间写入RTC；网络恢复后会按检查周期自动同步。

## 硬件限制

离线及断电后持续走时必须满足以下条件：

- 开发板具有可工作的硬件 RTC。
- RTC 驱动生成 `/dev/rtc*`。
- RTC 后备电池已经安装且电量正常。
- RTC 使用 UTC 保存时间。

如果开发板完全断电后 `hwclock -r -u` 回到旧日期，需要安装或更换 RTC 后备电池。仅依靠 eMMC 文件只能保存上次校时记录，不能计算断电期间经过的真实时间。定期NTP校时可以纠正RTC长期运行产生的漂移。
