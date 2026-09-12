#include <QCoreApplication>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslError>
#include <QSslSocket>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

namespace {
QTextStream output(stdout,QIODevice::WriteOnly);

void printLine(const QString &text) {
    output<<text<<"\n";
    output.flush();
}
}

int main(int argc,char *argv[]) {
    QCoreApplication app(argc,argv);
    const QString configPath=argc>1?QString::fromLocal8Bit(argv[1]):QStringLiteral("/Kd1234/config/navigation.ini");
    if(!QFileInfo(configPath).isReadable()) {
        printLine(QStringLiteral("ERROR: cannot read %1").arg(configPath));
        return 2;
    }

    QSettings settings(configPath,QSettings::IniFormat);
    const QString key=settings.value(QStringLiteral("amap/web_service_key")).toString().trimmed();
    if(key.isEmpty()) {
        printLine(QStringLiteral("ERROR: [amap] web_service_key is missing"));
        return 3;
    }
    printLine(QStringLiteral("AMap key loaded: %1 characters").arg(key.size()));
    printLine(QStringLiteral("Qt runtime: %1").arg(QString::fromLatin1(qVersion())));
    printLine(QStringLiteral("Qt SSL support: %1").arg(QSslSocket::supportsSsl()?QStringLiteral("yes"):QStringLiteral("no")));
    printLine(QStringLiteral("OpenSSL runtime: %1").arg(QSslSocket::sslLibraryVersionString()));
    if(!QSslSocket::supportsSsl())return 4;

    const QString caPath=QStringLiteral("/etc/ssl/certs/ca-certificates.crt");
    const QList<QSslCertificate> certificates=QSslCertificate::fromPath(caPath,QSsl::Pem);
    if(certificates.isEmpty()) {
        printLine(QStringLiteral("ERROR: no CA certificates loaded from %1").arg(caPath));
        return 5;
    }
    QSslConfiguration ssl=QSslConfiguration::defaultConfiguration();
    ssl.setCaCertificates(certificates);
    QSslConfiguration::setDefaultConfiguration(ssl);
    printLine(QStringLiteral("CA certificates loaded: %1").arg(certificates.size()));

    const QString endpoint=QStringLiteral("https://restapi.amap.com/v5/direction/driving");
    QUrl url(endpoint);
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("origin"),QStringLiteral("116.481028,39.989643"));
    query.addQueryItem(QStringLiteral("destination"),QStringLiteral("116.434446,39.908160"));
    query.addQueryItem(QStringLiteral("key"),key);
    url.setQuery(query);

    QNetworkAccessManager manager;
    QNetworkReply *reply=manager.get(QNetworkRequest(url));
    QObject::connect(reply,&QNetworkReply::sslErrors,[reply](const QList<QSslError> &errors){
        for(int i=0;i<errors.size();++i)printLine(QStringLiteral("SSL ERROR: %1").arg(errors[i].errorString()));
        Q_UNUSED(reply);
    });
    QObject::connect(reply,&QNetworkReply::finished,[&app,reply](){
        const QByteArray body=reply->readAll();
        if(reply->error()!=QNetworkReply::NoError) {
            printLine(QStringLiteral("REQUEST ERROR: %1").arg(reply->errorString()));
            reply->deleteLater();
            app.exit(6);
            return;
        }
        const bool apiOk=body.contains("\"status\":\"1\"")&&body.contains("\"infocode\":\"10000\"");
        printLine(QStringLiteral("HTTPS response bytes: %1").arg(body.size()));
        printLine(apiOk?QStringLiteral("RESULT: HTTPS and AMap API OK"):QStringLiteral("RESULT: HTTPS worked, but AMap API returned an error"));
        if(!apiOk)printLine(QString::fromUtf8(body.left(300)));
        reply->deleteLater();
        app.exit(apiOk?0:7);
    });

    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout,&QTimer::timeout,[&app,reply](){
        printLine(QStringLiteral("REQUEST ERROR: timeout"));
        reply->abort();
        app.exit(8);
    });
    timeout.start(15000);
    return app.exec();
}
