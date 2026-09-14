# WLAN 自动启动

Linux 主程序启动后由 `SystemManager::initializeWifi()` 创建后台线程，依次加载无线驱动、启动 `wlan0`、检查已有 `wpa_supplicant` 控制连接、等待 `wpa_state=COMPLETED`，并在没有 IPv4 地址时执行 `udhcpc`。界面线程不会等待联网过程。

默认值与板端已经验证过的命令一致：

- 驱动模块：`/lib/modules/wlan.ko`（仅当 `/sys/class/net/wlan0` 不存在时自动 `insmod`）
- 接口：`wlan0`
- 驱动：`nl80211`
- 程序：`/Kd1234/wifi_new/wpa_supplicant`、`/Kd1234/wifi_new/wpa_cli`
- 热点配置：`/Kd1234/wpa_supplicant.conf`，不可读时自动回退 `/etc/wpa_supplicant.conf`
- 控制目录：`/var/run/wpa_supplicant`
- DHCP：`udhcpc`

热点 SSID 和密码只写入 `wpa_supplicant.conf`（板端 `/etc/wpa_supplicant.conf` 或 `/Kd1234/wpa_supplicant.conf` 均可），建议权限设为 `600`。程序不把热点密码编译进二进制文件。

WLAN 就绪或失败后，后台会立即触发一次 NTP 校时检查；校时成功后顶部北京时间为 NTP 实时时间，并由 RTC 在断电后保持。未联网时时间来源为 RTC 恢复值（需至少成功校时一次）。

如需覆盖默认值，将 `config/wifi.ini.example` 复制为板端 `/Kd1234/config/wifi.ini`。设置 `enabled=false` 可停用程序内自动启动，交给系统启动脚本管理；`driver_module` 留空可禁用程序内驱动加载。

主程序需要以 root 身份运行，或具备配置网络接口和执行 DHCP 所需权限。启动结果写入 `/Kd1234/config/wifi_startup.log`，`wpa_supplicant` 的详细输出写入 `/tmp/wpa_supplicant.log`。板端验证：

```sh
chmod 600 /etc/wpa_supplicant.conf
chmod +x /Kd1234/wifi_new/wpa_supplicant /Kd1234/wifi_new/wpa_cli
rm -f /Kd1234/config/wifi_startup.log
/Kd1234/NEV-SmartHMI
cat /Kd1234/config/wifi_startup.log
/Kd1234/wifi_new/wpa_cli -p /var/run/wpa_supplicant -i wlan0 status
ifconfig wlan0
```

成功日志为 `WLAN ready on wlan0`；如果程序启动前已经联网，则为 `WLAN already ready on wlan0`。重启开发板后无需手动 `insmod`，程序会检测接口缺失并自动加载驱动。

时间同步的详细机制（RTC 恢复、6 小时周期、断电保持条件）见 [时间同步与 RTC 部署说明](TIME_SYNC_RTC.md)。
