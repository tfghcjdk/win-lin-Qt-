#include "system_manager.h"

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>
#include <QTextStream>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QStringList>
#include <QtGlobal>

#ifdef Q_OS_LINUX
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <netdb.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

namespace {
#ifdef Q_OS_LINUX
const char kDefaultConfigDirectory[] = "/Kd1234/config";
const char kStateName[] = "rtc_time_sync.state";
const char kLegacyMarkerName[] = ".rtc_time_synced_once";
const char kLogName[] = "time_sync.log";
const char kWifiLogName[] = "wifi_startup.log";
const qint64 kSyncIntervalSeconds=6*60*60;
const int kRetryCheckMilliseconds=10*60*1000;
bool syncRunning=false;

QString configDirectory() {
    const QByteArray overridePath=qgetenv("NEV_TIME_CONFIG_DIR");
    return overridePath.isEmpty()?QString::fromLatin1(kDefaultConfigDirectory):QString::fromLocal8Bit(overridePath);
}

QString statePath() { return QDir(configDirectory()).filePath(QString::fromLatin1(kStateName)); }
QString legacyMarkerPath() { return QDir(configDirectory()).filePath(QString::fromLatin1(kLegacyMarkerName)); }
QString logPath() { return QDir(configDirectory()).filePath(QString::fromLatin1(kLogName)); }
QString wifiLogPath() { return QDir(configDirectory()).filePath(QString::fromLatin1(kWifiLogName)); }

void appendLog(const QString &message) {
    QFile file(logPath());
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))return;
    QTextStream out(&file);
    out<<QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyy-MM-ddTHH:mm:ssZ"))<<" "<<message<<"\n";
}

void appendWifiLog(const QString &message) {
    QFile file(wifiLogPath());
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))return;
    QTextStream out(&file);
    out<<QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyy-MM-ddTHH:mm:ssZ"))<<" "<<message<<"\n";
}

struct CommandResult {
    int exitCode;
    QByteArray output;
    bool started;
};

CommandResult runCommand(const QString &program,const QStringList &arguments,int timeoutMilliseconds) {
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(program,arguments);
    if(!process.waitForStarted(3000))return CommandResult{-1,process.errorString().toLocal8Bit(),false};
    if(!process.waitForFinished(timeoutMilliseconds)) {
        process.kill();
        process.waitForFinished(1000);
        return CommandResult{-1,process.readAll(),true};
    }
    return CommandResult{process.exitCode(),process.readAll(),true};
}

QString compactOutput(const QByteArray &value) {
    QString text=QString::fromLocal8Bit(value).trimmed();
    text.replace(QLatin1Char('\n'),QLatin1Char(' '));
    text.replace(QLatin1Char('\r'),QLatin1Char(' '));
    return text.left(300);
}

bool hasIpv4Address(const QString &ifconfig,const QString &interfaceName) {
    const CommandResult result=runCommand(ifconfig,QStringList()<<interfaceName,5000);
    return result.started&&result.exitCode==0&&
           (result.output.contains("inet addr:")||result.output.contains("inet "));
}

class WifiStartupThread : public QThread {
public:
    explicit WifiStartupThread(QObject *parent):QThread(parent){}
protected:
    void run() {
        QSettings settings(QDir(configDirectory()).filePath(QStringLiteral("wifi.ini")),QSettings::IniFormat);
        settings.beginGroup(QStringLiteral("wifi"));
        const bool enabled=settings.value(QStringLiteral("enabled"),true).toBool();
        const QString interfaceName=settings.value(QStringLiteral("interface"),QStringLiteral("wlan0")).toString().trimmed();
        const QString driver=settings.value(QStringLiteral("driver"),QStringLiteral("nl80211")).toString().trimmed();
        const QString driverModule=settings.value(QStringLiteral("driver_module"),QStringLiteral("/lib/modules/wlan.ko")).toString().trimmed();
        const QString supplicant=settings.value(QStringLiteral("supplicant"),QStringLiteral("/Kd1234/wifi_new/wpa_supplicant")).toString().trimmed();
        const QString cli=settings.value(QStringLiteral("cli"),QStringLiteral("/Kd1234/wifi_new/wpa_cli")).toString().trimmed();
        QString supplicantConfig=settings.value(QStringLiteral("supplicant_config"),QStringLiteral("/Kd1234/wpa_supplicant.conf")).toString().trimmed();
        const QString controlDirectory=settings.value(QStringLiteral("control_directory"),QStringLiteral("/var/run/wpa_supplicant")).toString().trimmed();
        const QString ifconfig=settings.value(QStringLiteral("ifconfig"),QStringLiteral("ifconfig")).toString().trimmed();
        const QString dhcp=settings.value(QStringLiteral("dhcp"),QStringLiteral("udhcpc")).toString().trimmed();
        settings.endGroup();

        if(!enabled){appendWifiLog(QStringLiteral("automatic WLAN startup disabled"));return;}
        if(interfaceName.isEmpty()||driver.isEmpty()) {appendWifiLog(QStringLiteral("invalid WLAN interface or driver setting"));return;}
        if(!QFileInfo(supplicant).isExecutable()) {appendWifiLog(QStringLiteral("wpa_supplicant is not executable: %1").arg(supplicant));return;}
        if(!QFileInfo(cli).isExecutable()) {appendWifiLog(QStringLiteral("wpa_cli is not executable: %1").arg(cli));return;}
        if(!QFileInfo(supplicantConfig).isReadable()) {
            const QString fallback=QStringLiteral("/etc/wpa_supplicant.conf");
            if(QFileInfo(fallback).isReadable()) {
                appendWifiLog(QStringLiteral("WLAN configuration %1 not readable; using %2").arg(supplicantConfig,fallback));
                supplicantConfig=fallback;
            } else {
                appendWifiLog(QStringLiteral("WLAN configuration is not readable: %1").arg(supplicantConfig));
                return;
            }
        }

        const QString sysInterfacePath=QStringLiteral("/sys/class/net/%1").arg(interfaceName);
        if(!QFileInfo(sysInterfacePath).exists()&&!driverModule.isEmpty()) {
            if(!QFileInfo(driverModule).isFile()) {
                appendWifiLog(QStringLiteral("WLAN driver module is missing: %1").arg(driverModule));
                return;
            }
            CommandResult module=runCommand(QStringLiteral("/sbin/insmod"),QStringList()<<driverModule,10000);
            if(!module.started||module.exitCode!=0)
                module=runCommand(QStringLiteral("insmod"),QStringList()<<driverModule,10000);
            if(!module.started||module.exitCode!=0) {
                appendWifiLog(QStringLiteral("failed to load %1: %2").arg(driverModule,compactOutput(module.output)));
                return;
            }
            for(int attempt=0;attempt<10&&!QFileInfo(sysInterfacePath).exists();++attempt)sleep(1);
            if(!QFileInfo(sysInterfacePath).exists()) {
                appendWifiLog(QStringLiteral("%1 loaded but %2 did not appear").arg(driverModule,interfaceName));
                return;
            }
        }

        QDir().mkpath(controlDirectory);
        CommandResult result=runCommand(ifconfig,QStringList()<<interfaceName<<QStringLiteral("up"),5000);
        if(!result.started||result.exitCode!=0) {
            appendWifiLog(QStringLiteral("failed to bring %1 up: %2").arg(interfaceName,compactOutput(result.output)));
            return;
        }

        const QStringList cliBase=QStringList()<<QStringLiteral("-p")<<controlDirectory<<QStringLiteral("-i")<<interfaceName;
        result=runCommand(cli,cliBase+QStringList()<<QStringLiteral("ping"),5000);
        if(result.output.contains("PONG")&&hasIpv4Address(ifconfig,interfaceName)) {
            appendWifiLog(QStringLiteral("WLAN already ready on %1").arg(interfaceName));
            return;
        }
        if(!result.output.contains("PONG")) {
            const QStringList arguments=QStringList()<<QStringLiteral("-B")<<QStringLiteral("-D")<<driver
                    <<QStringLiteral("-i")<<interfaceName<<QStringLiteral("-c")<<supplicantConfig
                    <<QStringLiteral("-f")<<QStringLiteral("/tmp/wpa_supplicant.log");
            result=runCommand(supplicant,arguments,10000);
            if(!result.started||result.exitCode!=0) {
                appendWifiLog(QStringLiteral("wpa_supplicant failed: %1").arg(compactOutput(result.output)));
                return;
            }
        } else {
            runCommand(cli,cliBase+QStringList()<<QStringLiteral("reconnect"),5000);
        }

        bool associated=false;
        for(int attempt=0;attempt<30&&!associated;++attempt) {
            result=runCommand(cli,cliBase+QStringList()<<QStringLiteral("status"),5000);
            associated=result.output.contains("wpa_state=COMPLETED");
            if(!associated)sleep(1);
        }
        if(!associated) {
            appendWifiLog(QStringLiteral("%1 did not associate within 30 seconds; see /tmp/wpa_supplicant.log").arg(interfaceName));
            return;
        }

        if(!hasIpv4Address(ifconfig,interfaceName)) {
            result=runCommand(dhcp,QStringList()<<QStringLiteral("-i")<<interfaceName<<QStringLiteral("-n")
                              <<QStringLiteral("-q")<<QStringLiteral("-t")<<QStringLiteral("5"),45000);
            if(!result.started||result.exitCode!=0) {
                appendWifiLog(QStringLiteral("DHCP failed on %1: %2").arg(interfaceName,compactOutput(result.output)));
                return;
            }
        }
        if(hasIpv4Address(ifconfig,interfaceName))appendWifiLog(QStringLiteral("WLAN ready on %1").arg(interfaceName));
        else appendWifiLog(QStringLiteral("WLAN associated on %1 but no IPv4 address was assigned").arg(interfaceName));
    }
};

bool readEpochFile(const QString &path,qint64 *epoch) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))return false;
    while(!file.atEnd()) {
        const QByteArray line=file.readLine().trimmed();
        if(!line.startsWith("epoch="))continue;
        bool ok=false;
        const qint64 value=line.mid(6).toLongLong(&ok);
        if(ok&&value>0){*epoch=value;return true;}
    }
    return false;
}

bool readLastSyncEpoch(qint64 *epoch) {
    if(readEpochFile(statePath(),epoch))return true;
    return readEpochFile(legacyMarkerPath(),epoch);
}

QStringList rtcDevices() {
    return QStringList()<<QStringLiteral("/dev/rtc0")<<QStringLiteral("/dev/rtc1")<<QStringLiteral("/dev/rtc");
}

bool readRtc(qint64 *epoch,QString *deviceUsed) {
    const QStringList devices=rtcDevices();
    for(int i=0;i<devices.size();++i) {
        const QByteArray path=QFile::encodeName(devices[i]);
        const int fd=::open(path.constData(),O_RDONLY);
        if(fd<0)continue;
        struct rtc_time value;
        memset(&value,0,sizeof(value));
        const bool readOk=(::ioctl(fd,RTC_RD_TIME,&value)==0);
        ::close(fd);
        if(!readOk)continue;
        const QDate date(value.tm_year+1900,value.tm_mon+1,value.tm_mday);
        const QTime time(value.tm_hour,value.tm_min,value.tm_sec);
        if(!date.isValid()||!time.isValid())continue;
        const QDateTime utc(date,time,Qt::UTC);
        *epoch=static_cast<qint64>(utc.toTime_t());
        if(deviceUsed)*deviceUsed=devices[i];
        return true;
    }
    return false;
}

bool setSystemTime(qint64 seconds,qint64 microseconds) {
    struct timeval value;
    value.tv_sec=static_cast<time_t>(seconds);
    value.tv_usec=static_cast<suseconds_t>(microseconds);
    return ::settimeofday(&value,0)==0;
}

bool writeRtc(qint64 epoch,QString *deviceUsed) {
    const time_t seconds=static_cast<time_t>(epoch);
    struct tm utc;
    memset(&utc,0,sizeof(utc));
    if(!::gmtime_r(&seconds,&utc))return false;

    struct rtc_time value;
    memset(&value,0,sizeof(value));
    value.tm_sec=utc.tm_sec;value.tm_min=utc.tm_min;value.tm_hour=utc.tm_hour;
    value.tm_mday=utc.tm_mday;value.tm_mon=utc.tm_mon;value.tm_year=utc.tm_year;
    value.tm_wday=utc.tm_wday;value.tm_yday=utc.tm_yday;value.tm_isdst=0;

    const QStringList devices=rtcDevices();
    for(int i=0;i<devices.size();++i) {
        const QByteArray path=QFile::encodeName(devices[i]);
        const int fd=::open(path.constData(),O_RDWR);
        if(fd<0)continue;
        const bool writeOk=(::ioctl(fd,RTC_SET_TIME,&value)==0);
        ::close(fd);
        if(!writeOk)continue;
        if(deviceUsed)*deviceUsed=devices[i];
        return true;
    }
    return false;
}

double monotonicSeconds() {
    struct timespec value;
    if(::clock_gettime(CLOCK_MONOTONIC,&value)!=0)return 0.0;
    return static_cast<double>(value.tv_sec)+static_cast<double>(value.tv_nsec)/1000000000.0;
}

bool queryNtp(const char *host,qint64 *seconds,qint64 *microseconds) {
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;hints.ai_socktype=SOCK_DGRAM;hints.ai_protocol=IPPROTO_UDP;
    struct addrinfo *addresses=0;
    if(::getaddrinfo(host,"123",&hints,&addresses)!=0)return false;

    bool success=false;
    for(struct addrinfo *address=addresses;address&&!success;address=address->ai_next) {
        const int fd=::socket(address->ai_family,address->ai_socktype,address->ai_protocol);
        if(fd<0)continue;
        struct timeval timeout;timeout.tv_sec=3;timeout.tv_usec=0;
        ::setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

        unsigned char request[48];
        memset(request,0,sizeof(request));
        request[0]=0x23;
        const double sentAt=monotonicSeconds();
        const ssize_t sent=::sendto(fd,request,sizeof(request),0,address->ai_addr,address->ai_addrlen);
        unsigned char response[48];
        const ssize_t received=sent==static_cast<ssize_t>(sizeof(request))?::recvfrom(fd,response,sizeof(response),0,0,0):-1;
        const double receivedAt=monotonicSeconds();
        ::close(fd);
        if(received<48)continue;

        const unsigned char leap=(response[0]>>6)&0x03;
        const unsigned char mode=response[0]&0x07;
        const unsigned char stratum=response[1];
        if(leap==3||(mode!=4&&mode!=5)||stratum==0||stratum>15)continue;

        uint32_t networkSeconds=0,networkFraction=0;
        memcpy(&networkSeconds,response+40,sizeof(networkSeconds));
        memcpy(&networkFraction,response+44,sizeof(networkFraction));
        const quint64 ntpSeconds=ntohl(networkSeconds);
        if(ntpSeconds<=2208988800ULL)continue;
        const double fraction=static_cast<double>(ntohl(networkFraction))/4294967296.0;
        const double halfRoundTrip=qMax(0.0,(receivedAt-sentAt)/2.0);
        const double unixTime=static_cast<double>(ntpSeconds-2208988800ULL)+fraction+halfRoundTrip;
        const qint64 whole=static_cast<qint64>(unixTime);
        const QDateTime utc=QDateTime::fromTime_t(static_cast<uint>(whole),Qt::UTC);
        if(utc.date().year()<2025||utc.date().year()>2035)continue;
        *seconds=whole;
        *microseconds=static_cast<qint64>((unixTime-static_cast<double>(whole))*1000000.0);
        success=true;
    }
    ::freeaddrinfo(addresses);
    return success;
}

bool writeSyncState(qint64 epoch,const QString &server,const QString &rtcDevice) {
    QFile file(statePath());
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text))return false;
    QTextStream out(&file);
    out<<"epoch="<<epoch<<"\n";
    out<<"utc="<<QDateTime::fromTime_t(static_cast<uint>(epoch),Qt::UTC).toString(Qt::ISODate)<<"\n";
    out<<"server="<<server<<"\n";
    out<<"rtc="<<rtcDevice<<"\n";
    file.close();
    QFile::setPermissions(statePath(),QFile::ReadOwner|QFile::WriteOwner);
    return file.error()==QFile::NoError;
}

void restoreFromRtcWhenNeeded() {
    qint64 markerEpoch=0;
    if(!readLastSyncEpoch(&markerEpoch))return;
    qint64 rtcEpoch=0;QString rtcDevice;
    if(!readRtc(&rtcEpoch,&rtcDevice)) {
        appendLog(QStringLiteral("RTC restore failed: no readable RTC device"));
        return;
    }
    if(rtcEpoch<markerEpoch-24*60*60) {
        appendLog(QStringLiteral("RTC restore refused: RTC value is older than the saved synchronization"));
        return;
    }
    if(setSystemTime(rtcEpoch,0))appendLog(QStringLiteral("system clock restored from %1 (UTC RTC, RTC was not written)").arg(rtcDevice));
    else appendLog(QStringLiteral("RTC restore failed: settimeofday requires root or CAP_SYS_TIME"));
}

class TimeSyncThread : public QThread {
public:
    explicit TimeSyncThread(QObject *parent):QThread(parent){}
protected:
    void run() {
        const char *servers[]={"ntp.aliyun.com","ntp.tencent.com","cn.pool.ntp.org"};
        for(int round=0;round<3;++round) {
            for(unsigned int i=0;i<sizeof(servers)/sizeof(servers[0]);++i) {
                qint64 seconds=0,microseconds=0;
                if(!queryNtp(servers[i],&seconds,&microseconds))continue;
                if(!setSystemTime(seconds,microseconds)) {
                    appendLog(QStringLiteral("NTP received but settimeofday failed; run the application as root"));
                    return;
                }
                QString rtcDevice;
                if(!writeRtc(seconds,&rtcDevice)) {
                    appendLog(QStringLiteral("system clock synchronized, but RTC write failed; check /dev/rtc* and permissions"));
                    return;
                }
                if(!writeSyncState(seconds,QString::fromLatin1(servers[i]),rtcDevice)) {
                    appendLog(QStringLiteral("RTC was written, but the synchronization state could not be saved"));
                    return;
                }
                appendLog(QStringLiteral("periodic NTP synchronization completed; UTC written to %1 via %2").arg(rtcDevice,QString::fromLatin1(servers[i])));
                return;
            }
            sleep(5);
        }
        appendLog(QStringLiteral("periodic NTP synchronization failed; current system time and RTC were left unchanged"));
    }
};

bool synchronizationDue() {
    qint64 lastSync=0;
    if(!readLastSyncEpoch(&lastSync))return true;
    const qint64 now=static_cast<qint64>(::time(0));
    return now<lastSync||now-lastSync>=kSyncIntervalSeconds;
}

void startSynchronizationIfDue() {
    if(syncRunning||!synchronizationDue())return;
    syncRunning=true;
    TimeSyncThread *thread=new TimeSyncThread(QCoreApplication::instance());
    QObject::connect(thread,&QThread::finished,thread,[](){syncRunning=false;});
    QObject::connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    thread->start();
}
#endif
}

void SystemManager::initializeWifi() {
#ifdef Q_OS_LINUX
    if(!QFileInfo(configDirectory()).isDir())return;
    WifiStartupThread *thread=new WifiStartupThread(QCoreApplication::instance());
    QObject::connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    // The WLAN startup thread finishes as soon as the link is ready (or fails);
    // retry the periodic clock check at that point instead of waiting for the
    // fixed retry window, so the header shows verified NTP time sooner.
    QObject::connect(thread,&QThread::finished,thread,[](){startSynchronizationIfDue();});
    thread->start();
#endif
}

void SystemManager::initializeClock() {
#ifdef Q_OS_LINUX
    if(!QFileInfo(configDirectory()).isDir())return;
    qint64 markerEpoch=0;
    if(readLastSyncEpoch(&markerEpoch))restoreFromRtcWhenNeeded();
    else if(QFileInfo(statePath()).exists()||QFileInfo(legacyMarkerPath()).exists())appendLog(QStringLiteral("invalid synchronization state ignored; NTP will be retried"));
    QTimer *timer=new QTimer(QCoreApplication::instance());
    timer->setInterval(kRetryCheckMilliseconds);
    QObject::connect(timer,&QTimer::timeout,[](){startSynchronizationIfDue();});
    timer->start();
    startSynchronizationIfDue();
    // The first NTP attempt can precede WLAN association, so retry once after
    // the background WLAN startup window instead of waiting ten minutes.
    QTimer::singleShot(45000,[](){startSynchronizationIfDue();});
#endif
}
