# NEV-Locator — 手机定位推送 App

配合 NEV-SmartHMI 车机（X6818 开发板）使用的 Android 端定位推送工具。

## 功能

- **定位推送**：前台服务读取 GPS，每秒向开发板 UDP 45454 发送一条 JSON
  （WGS-84 坐标，由板端纠偏为 GCJ-02）。
- **中文目的地**：输入中文地名 → 调高德地理编码（返回 GCJ-02）→
  发送 `type=destination` 报文，板端收到后自动重算路线并显示地名。
- 开发板 IP、高德 Web 服务 Key 在界面上填写，自动保存。

## UDP 协议（与板端 PhoneLocationReceiver 对应，version=1）

定位报文（每秒 1 条）：
```json
{"version":1,"seq":42,"time":1757822400000,"lat":39.988345,"lng":116.474918,"accuracy":10,"speed":0,"bearing":0}
```
目的地报文（用户触发，GCJ-02 坐标）：
```json
{"version":1,"type":"destination","name":"中关村广场","lat":39.983,"lng":116.3076}
```

## 方式一：Android Studio（推荐，省事）

1. 下载安装 Android Studio（https://developer.android.com/studio，约 1.1 GB，
   安装时保持默认选项，会自动装 Android SDK 和 Platform-Tools）。
2. 启动后 **Open** → 选择本目录（`android/`，即包含 `settings.gradle` 的文件夹），
   弹窗选 **Trust**。
3. 首次 Gradle Sync 会自动补齐 wrapper、按 `gradle-wrapper.properties`
   从腾讯云镜像下载 Gradle 8.7，依赖走阿里云镜像（已配好），耐心等 5~15 分钟。
4. 同步完成后菜单 **Build → Build App Bundle(s)/APK(s) → Build APK(s)**，
   产物在 `app\build\outputs\apk\debug\app-debug.apk`。
5. 安装：手机开开发者选项 + USB 调试后 `adb install`，或把 APK 发给手机直接安装
   （允许"安装未知来源应用"）。

## 方式二：命令行编译 APK（Windows，无需 Android Studio）

1. **JDK 17**：安装 Temurin 17（https://adoptium.net），确认
   `java -version` 输出 17.x。
2. **Android SDK 命令行工具**：
   - 下载 commandlinetools-win-*.zip（https://developer.android.com/studio#command-tools）
   - 解压到如 `D:\Android\Sdk\cmdline-tools\latest\`
   - 设置环境变量 `ANDROID_HOME=D:\Android\Sdk`，PATH 加 `%ANDROID_HOME%\cmdline-tools\latest\bin` 和 `%ANDROID_HOME%\platform-tools`
   - 安装组件：
     ```
     sdkmanager --licenses        （全部输 y）
     sdkmanager "platform-tools" "platforms;android-34" "build-tools;34.0.0"
     ```
3. **Gradle 8.7**：下载 gradle-8.7-bin.zip（https://gradle.org/releases），
   解压后把 `bin` 加入 PATH，确认 `gradle -v`。
4. **编译**：
   ```
   cd android
   gradle assembleDebug
   ```
   产物：`app\build\outputs\apk\debug\app-debug.apk`
5. **安装到手机**（iQOO 12 Pro 开开发者选项 + USB 调试）：
   ```
   adb install app\build\outputs\apk\debug\app-debug.apk
   ```
   或直接把 APK 拷到手机上点开安装（需允许"安装未知来源应用"）。

> 也可以直接用 Android Studio 打开本目录，点 Run 即可，省去上面 1-3 步（见方式一）。

国内网络提示：若 `sdkmanager` 或 Gradle 依赖下载极慢，可给 sdkmanager 加
`--proxy` 参数，或改用 Android Studio 内置 SDK Manager 安装（走它自己的加速通道）。

## 使用流程

1. 手机开热点（车机连的就是这个热点），查看车机 IP（`ifconfig wlan0`）填到 App 里。
2. 填高德 Web 服务 Key（与板端 `/Kd1234/config/navigation.ini` 里的一致）。
3. 点"开始推送定位"→ 授予定位权限（选"使用 App 期间允许"）→ 状态区显示坐标。
4. 输中文目的地 → "发送目的地到车机"→ 车机屏幕显示"前往 XX"并重算路线。

## 注意

- 手机与开发板必须在同一热点网段；UDP 不保证送达，定位包丢了板端会等下一包，
  目的地包如果 3 秒内车机没反应就再发一次。
- 后台运行请允许 App 后台活动/忽略电池优化（vivo 系统：设置 → 电池 → 后台耗电管理）。
- GPS 室内无信号时 App 会用网络定位兜底，精度差（>50m）的包会被板端拒绝，属正常。
