#include "main_window.h"
#include "hmi_cards.h"
#include "paint_helpers.h"
#include "hardware/camera_v4l2.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QDateTime>
#include <QKeyEvent>
#include <QSlider>
#include <QApplication>
#include <QLinearGradient>
#include <QShowEvent>
#include <QHideEvent>
using namespace Hmi;
namespace {
class NavButton : public QPushButton {
public:
    NavButton(const QString &label,const QString &symbol,QWidget *parent):QPushButton(parent),label(label),symbol(symbol){setCheckable(true);setCursor(Qt::PointingHandCursor);setAccessibleName(label);setFocusPolicy(Qt::StrongFocus);}
protected:
    void paintEvent(QPaintEvent *) {
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
        if(isChecked()) {QLinearGradient g(0,0,width(),0);g.setColorAt(0,QColor("#087fca"));g.setColorAt(1,QColor("#083461"));p.setBrush(g);p.setPen(QColor("#0d5a8e"));p.drawRoundedRect(rect().adjusted(1,1,-1,-1),5,5);p.fillRect(QRectF(0,8,3,height()-16),Cyan);}
        else if(underMouse())p.fillRect(rect(),QColor("#122b43"));
        Hmi::icon(p,symbol,QRectF((width()-25)/2.,14,25,25),isChecked()?Text:Muted);
        Hmi::text(p,QRectF(0,43,width(),25),label,13,isChecked()?Text:Muted,false,Qt::AlignCenter);
        if(hasFocus()){p.setPen(QPen(Cyan,1,Qt::DotLine));p.setBrush(Qt::NoBrush);p.drawRoundedRect(rect().adjusted(3,3,-3,-3),4,4);}
    }
private:QString label,symbol;
};
class PageShell : public QWidget {
public:
    PageShell(const QString &heading,const QString &caption,QWidget *parent=0):QWidget(parent),heading(heading),caption(caption){}
protected:
    void paintEvent(QPaintEvent *){QPainter p(this);p.setRenderHint(QPainter::Antialiasing);Hmi::text(p,QRectF(4,0,850,32),heading,23,Text,true);Hmi::text(p,QRectF(5,35,850,20),caption,12,Muted);}
private:QString heading,caption;
};
class CameraOverlay : public QWidget {
public:
    explicit CameraOverlay(QWidget *parent):QWidget(parent),camera(this),status(QStringLiteral("摄像头准备中")) {
        setObjectName("cameraOverlay");setGeometry(0,0,1024,600);
        const QByteArray configured=qgetenv("HMI_CAMERA_DEVICE");
        if(!configured.isEmpty())camera.device=QString::fromLocal8Bit(configured);
        QPushButton *close=new QPushButton(QStringLiteral("返回  ·  Esc"),this);close->setObjectName("closeCamera");close->setGeometry(869,18,135,36);connect(close,&QPushButton::clicked,this,[this](){hide();});
        connect(&camera,&CameraV4l2::frameReady,this,[this](const QImage &image){frame=image.mirrored(true,false);camera.acknowledge();update();});
        connect(&camera,&CameraV4l2::message,this,[this](const QString &text){status=text;update();});
    }
    ~CameraOverlay(){stopCamera();}
protected:
    void showEvent(QShowEvent *event){QWidget::showEvent(event);startCamera();}
    void hideEvent(QHideEvent *event){stopCamera();QWidget::hideEvent(event);}
    void paintEvent(QPaintEvent *) {
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);p.setRenderHint(QPainter::SmoothPixmapTransform);p.fillRect(rect(),QColor("#061322"));
        Hmi::icon(p,"camera",QRectF(24,22,24,24));Hmi::text(p,QRectF(60,16,700,35),QStringLiteral("倒车影像"),21);
        const QRectF target(88,72,848,454);p.setPen(QPen(QColor("#24465e"),1));p.setBrush(QColor("#02070c"));p.drawRoundedRect(target,8,8);
        if(!frame.isNull()) {
            QRectF source(frame.rect());const qreal targetRatio=target.width()/target.height(),sourceRatio=source.width()/source.height();
            if(sourceRatio>targetRatio){const qreal width=source.height()*targetRatio;source.setLeft((source.width()-width)/2);source.setWidth(width);}
            else{const qreal height=source.width()/targetRatio;source.setTop((source.height()-height)/2);source.setHeight(height);}
            QPainterPath clip;clip.addRoundedRect(target,8,8);p.save();p.setClipPath(clip);p.drawImage(target,frame,source);p.restore();
        } else Hmi::text(p,target,QStringLiteral("正在等待摄像头画面"),19,Muted,false,Qt::AlignCenter);
        drawGuides(p,target);
        Hmi::text(p,QRectF(25,548,974,25),status+QStringLiteral("  ·  返回后自动关闭摄像头  ·  辅助线未经车辆标定"),13,frame.isNull()?Amber:Muted,false,Qt::AlignCenter);
    }
private:
    void startCamera(){if(camera.isRunning())return;frame=QImage();status=QStringLiteral("正在连接 %1").arg(camera.device);camera.start();update();}
    void stopCamera(){if(!camera.isRunning())return;camera.stop();camera.wait();frame=QImage();}
    static void line(QPainter &p,const QLineF &segment,const QColor &color){p.setPen(QPen(QColor(0,0,0,150),8,Qt::SolidLine,Qt::RoundCap));p.drawLine(segment);p.setPen(QPen(color,4,Qt::SolidLine,Qt::RoundCap));p.drawLine(segment);}
    static void drawGuides(QPainter &p,const QRectF &r){
        const QPointF lt(r.x()+r.width()*.43,r.y()+r.height()*.50),lg(r.x()+r.width()*.36,r.y()+r.height()*.65),ly(r.x()+r.width()*.27,r.y()+r.height()*.80),lb(r.x()+r.width()*.17,r.y()+r.height()*.95);
        const QPointF rt(r.x()+r.width()*.57,r.y()+r.height()*.50),rg(r.x()+r.width()*.64,r.y()+r.height()*.65),ry(r.x()+r.width()*.73,r.y()+r.height()*.80),rb(r.x()+r.width()*.83,r.y()+r.height()*.95);
        const QColor green("#35e86f"),yellow("#ffd43b"),red("#ff3b3b");line(p,QLineF(lt,lg),green);line(p,QLineF(rt,rg),green);line(p,QLineF(lg,ly),yellow);line(p,QLineF(rg,ry),yellow);line(p,QLineF(ly,lb),red);line(p,QLineF(ry,rb),red);line(p,QLineF(lg,rg),green);line(p,QLineF(ly,ry),yellow);line(p,QLineF(lb,rb),red);
    }
    CameraV4l2 camera;QImage frame;QString status;
};
QLabel *info(QWidget *parent,const QRect &rect,const QString &heading,const QString &body) {
    QLabel *label=new QLabel(parent);label->setGeometry(rect);label->setWordWrap(true);label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    label->setText(QStringLiteral("<div style='font-size:17px;color:#e0f1ff;'>%1</div><br/><div style='font-size:13px;color:#a1b9ce;'>%2</div>").arg(heading,body));
    label->setStyleSheet("QLabel { background:#0e263d;border:1px solid #1c3c55;border-radius:9px;padding:18px; }");return label;
}
QPushButton *action(QWidget *parent,const QString &name,const QString &label,const QRect &r) {QPushButton *b=new QPushButton(label,parent);b->setObjectName(name);b->setGeometry(r);b->setCursor(Qt::PointingHandCursor);return b;}
}
MainWindow::MainWindow(QWidget *parent):QWidget(parent),state(new VehicleDataCenter(this)),stack(new QStackedWidget(this)),cameraOverlay(0) {
    setObjectName("hmiWindow");setWindowTitle(QStringLiteral("NEV-SmartHMI · 智能座舱"));setWindowFlags(Qt::Window|Qt::FramelessWindowHint);setFixedSize(1024,600);
    stack->setObjectName("pageStack");stack->setGeometry(100,52,912,522);
    stack->addWidget(homePage());stack->addWidget(vehiclePage());stack->addWidget(navigationPage());stack->addWidget(climatePage());stack->addWidget(mediaPage());stack->addWidget(settingsPage());
    const QStringList names=QStringList()<<QStringLiteral("首页")<<QStringLiteral("车辆状态")<<QStringLiteral("导航")<<QStringLiteral("空调")<<QStringLiteral("媒体")<<QStringLiteral("设置");
    const QStringList icons=QStringList()<<"home"<<"car"<<"nav"<<"fan"<<"music"<<"settings";
    for(int i=0;i<6;++i){NavButton *b=new NavButton(names[i],icons[i],this);b->setObjectName(QString("nav%1").arg(i));b->setGeometry(12,52+i*87,78,82);navButtons.append(b);connect(b,&QPushButton::clicked,this,[this,i](){setPage(i);});}
    cameraOverlay=new CameraOverlay(this);cameraOverlay->hide();setPage(0);
    QTimer *timer=new QTimer(this);timer->setInterval(1000);connect(timer,&QTimer::timeout,state,&VehicleDataCenter::tick);timer->start();
    connect(state,&VehicleDataCenter::changed,this,[this](){update();});
}
void MainWindow::setPage(int index){if(index<0||index>=stack->count())return;stack->setCurrentIndex(index);for(int i=0;i<navButtons.size();++i)navButtons[i]->setChecked(i==index);}
bool MainWindow::cameraIsOpen() const{return cameraOverlay->isVisible();}
void MainWindow::showCamera(){cameraOverlay->show();cameraOverlay->raise();}
QString MainWindow::beijingClockText(const QDateTime &utcNow) {
    const QDateTime beijing=utcNow.toUTC().addSecs(8*60*60);
    const QStringList week=QStringList()<<QStringLiteral("星期一")<<QStringLiteral("星期二")<<QStringLiteral("星期三")<<QStringLiteral("星期四")<<QStringLiteral("星期五")<<QStringLiteral("星期六")<<QStringLiteral("星期日");
    return beijing.toString(QStringLiteral("yyyy年M月d日"))+"   "+week[beijing.date().dayOfWeek()-1]+"   "+beijing.toString("HH:mm");
}
void MainWindow::keyPressEvent(QKeyEvent *event){if(event->key()==Qt::Key_Escape){if(cameraIsOpen())cameraOverlay->hide();else if(currentPage()!=0)setPage(0);else close();event->accept();return;}QWidget::keyPressEvent(event);}
void MainWindow::paintEvent(QPaintEvent *) {
    QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
    QLinearGradient bg(0,0,1024,600);bg.setColorAt(0,QColor("#061522"));bg.setColorAt(.6,QColor("#071a2b"));bg.setColorAt(1,QColor("#040f1c"));p.fillRect(rect(),bg);
    p.setPen(QColor("#17354d"));p.drawLine(12,43,1012,43);p.fillRect(QRectF(12,52,78,522),QColor("#081d2e"));
    Hmi::text(p,QRectF(25,9,307,26),beijingClockText(QDateTime::currentDateTimeUtc()),14);
    p.setBrush(QColor("#153247"));p.setPen(QColor("#2b617d"));p.drawRoundedRect(QRectF(348,11,72,22),11,11);Hmi::text(p,QRectF(348,11,72,22),QStringLiteral("北京时间"),10,Cyan,false,Qt::AlignCenter);
    Hmi::text(p,QRectF(445,10,25,25),"4G",11,Muted);Hmi::icon(p,"signal",QRectF(468,14,17,17),Muted);Hmi::icon(p,"wifi",QRectF(499,12,22,22),Muted);Hmi::icon(p,"bluetooth",QRectF(533,13,19,19),Muted);Hmi::icon(p,"pin",QRectF(565,12,20,20),Muted);Hmi::text(p,QRectF(588,10,32,25),"GPS",10,Muted);
    Hmi::icon(p,"battery",QRectF(635,10,29,25),state->dataAvailable?Green:Muted);Hmi::text(p,QRectF(672,10,127,25),state->dataAvailable?QStringLiteral("正在充电 80%"):QStringLiteral("车辆数据已断开"),11,Text);
    p.setPen(QColor("#304355"));p.drawLine(803,14,803,30);Hmi::text(p,QRectF(817,10,191,25),"C++ / Qt  |  1024×600",11,Muted);
    Hmi::text(p,QRectF(15,578,540,20),QStringLiteral("探索电动出行的更多可能   /   New Energy New Life"),9,Muted);
    Hmi::text(p,QRectF(654,578,354,20),QStringLiteral("未连接车辆  |  Esc 返回 / 退出"),9,Muted,false,Qt::AlignRight|Qt::AlignVCenter);
}
QWidget *MainWindow::homePage() {
    QWidget *page=new QWidget;page->setObjectName("homePage");
    DashboardCard *speed=new DashboardCard(state,DashboardCard::Speed,page);speed->setGeometry(0,0,230,198);
    DashboardCard *battery=new DashboardCard(state,DashboardCard::Battery,page);battery->setGeometry(238,0,422,198);
    DashboardCard *energy=new DashboardCard(state,DashboardCard::Energy,page);energy->setGeometry(668,0,244,198);
    ClimateCard *climate=new ClimateCard(state,page);climate->setObjectName("homeClimate");climate->setGeometry(0,206,262,190);
    NavigationCard *nav=new NavigationCard(state,page);nav->setGeometry(270,206,248,190);connect(nav,&NavigationCard::openRequested,this,[this](){setPage(2);});
    CameraCard *camera=new CameraCard(state,page);camera->setGeometry(526,206,172,190);connect(camera,&CameraCard::openRequested,this,&MainWindow::showCamera);
    MediaCard *media=new MediaCard(state,page);media->setObjectName("homeMedia");media->setGeometry(706,206,206,190);
    DashboardCard *trip=new DashboardCard(state,DashboardCard::Trip,page);trip->setGeometry(0,404,356,118);
    DashboardCard *service=new DashboardCard(state,DashboardCard::Service,page);service->setGeometry(364,404,334,118);
    DashboardCard *slogan=new DashboardCard(state,DashboardCard::Slogan,page);slogan->setGeometry(706,404,206,118);return page;
}
QWidget *MainWindow::vehiclePage() {
    QWidget *page=new PageShell(QStringLiteral("车辆状态"),QStringLiteral("能量、续航与胎压一目了然 · 当前使用本地模拟数据"));page->setObjectName("vehiclePage");
    DashboardCard *battery=new DashboardCard(state,DashboardCard::Battery,page);battery->setGeometry(0,67,560,263);
    DashboardCard *energy=new DashboardCard(state,DashboardCard::Energy,page);energy->setGeometry(570,67,342,263);
    DashboardCard *trip=new DashboardCard(state,DashboardCard::Trip,page);trip->setGeometry(0,342,469,156);
    DashboardCard *service=new DashboardCard(state,DashboardCard::Service,page);service->setGeometry(480,342,432,156);return page;
}
QWidget *MainWindow::navigationPage() {
    QWidget *page=new PageShell(QStringLiteral("导航"),QStringLiteral("探索前方的每一段旅程 · 地图与路线为离线示意，无实时定位"));page->setObjectName("navigationPage");
    NavigationCard *nav=new NavigationCard(state,page);nav->setGeometry(0,68,570,437);
    info(page,QRect(582,68,330,240),QStringLiteral("前往 · 科技园"),QStringLiteral("下一路口右转进入科技大道<br/><br/>剩余距离 &nbsp; 12 公里<br/><br/>预计用时 &nbsp; 28 分钟<br/><br/>限速参考 &nbsp; 60 km/h"));
    QLabel *status=info(page,QRect(582,320,330,114),QStringLiteral("路线状态"),QStringLiteral("导航路线进行中"));
    QPushButton *toggle=action(page,"toggleRoute",QStringLiteral("结束导航"),QRect(599,449,296,44));
    auto refresh=[this,toggle,status](){toggle->setText(state->routeActive?QStringLiteral("结束导航"):QStringLiteral("开始导航"));status->setText(state->routeActive?QStringLiteral("路线进行中\n原型路径 · 无联网地图服务"):QStringLiteral("路线已结束\n点击下方按钮重新开始"));};
    connect(toggle,&QPushButton::clicked,this,[this](){state->routeActive=!state->routeActive;state->notify();});connect(state,&VehicleDataCenter::changed,page,refresh);refresh();return page;
}
QWidget *MainWindow::climatePage() {
    QWidget *page=new PageShell(QStringLiteral("舒适座舱"),QStringLiteral("调节温度与模式，让每一次出发都恰到好处"));page->setObjectName("climatePage");
    ClimateCard *climate=new ClimateCard(state,page);climate->setObjectName("detailClimate");climate->setGeometry(0,75,562,408);
    info(page,QRect(576,75,336,171),QStringLiteral("空调设置"),QStringLiteral("温度每次调节 0.5 ℃<br/><br/>设定范围：16–30 ℃<br/><br/>中间滑条直接调节设定温度"));
    info(page,QRect(576,258,336,225),QStringLiteral("空调控制说明"),QStringLiteral("A/C、自动模式、前后除雾与内循环均可切换。<br/><br/>页面共享同一份状态，返回首页后设置仍会保留。<br/><br/>此处尚未向车辆发送控制指令；范围与模式规则需按实际协议确定。"));return page;
}
QWidget *MainWindow::mediaPage() {
    QWidget *page=new PageShell(QStringLiteral("音乐 · 伴你同行"),QStringLiteral("每段旅程，都有属于它的旋律"));page->setObjectName("mediaPage");
    MediaCard *media=new MediaCard(state,page);media->setObjectName("detailMedia");media->setGeometry(0,69,454,418);
    info(page,QRect(468,69,444,168),QStringLiteral("播放列表"),QStringLiteral("01 &nbsp; 向着更远的远方 — 追光者<br/><br/>02 &nbsp; 沿途的风景 — 旅行日记<br/><br/>03 &nbsp; 城市之外 — 夜航计划"));
    info(page,QRect(468,249,444,151),QStringLiteral("媒体控制"),QStringLiteral("支持播放 / 暂停、切歌与拖动进度。<br/><br/>未提供音频文件，当前仅显示播放状态与计时，不输出声音。"));
    QLabel *volume=new QLabel(page);volume->setGeometry(484,418,405,22);
    QSlider *slider=new QSlider(Qt::Horizontal,page);slider->setObjectName("mediaVolume");slider->setAccessibleName(QStringLiteral("音量"));slider->setRange(0,100);slider->setValue(state->volume);slider->setGeometry(486,449,403,26);
    connect(slider,&QSlider::valueChanged,state,&VehicleDataCenter::setVolume);auto refresh=[this,volume](){volume->setText(QStringLiteral("音量设置  %1%（暂不控制系统音量）").arg(state->volume));};connect(state,&VehicleDataCenter::changed,page,refresh);refresh();return page;
}
QWidget *MainWindow::settingsPage() {
    QWidget *page=new PageShell(QStringLiteral("设置"),QStringLiteral("系统信息与测试场景"));page->setObjectName("settingsPage");
    info(page,QRect(0,72,447,218),QStringLiteral("NEV-SmartHMI"),QStringLiteral("界面版本 &nbsp; 0.2.4-ui<br/><br/>设计分辨率 &nbsp; 1024 × 600<br/><br/>兼容目标 &nbsp; Qt 5.4.1 / C++11<br/><br/>当前运行 Qt &nbsp; %1").arg(QString::fromLatin1(qVersion())));
    info(page,QRect(461,72,451,218),QStringLiteral("连接说明"),QStringLiteral("车辆、胎压和能量数据为模拟来源。<br/><br/>Linux 已接入 V4L2 倒车摄像头；CAN / 串口、GPS、网络状态和音频后端尚未接入。<br/><br/>北京时间按本机 UTC+8 计算，不执行 NTP / RTC 写入。"));
    info(page,QRect(0,303,912,116),QStringLiteral("检查不同的数据状态"),QStringLiteral("切换模拟数据或低胎压场景后，可返回首页 / 车辆状态页查看反馈。<br/><br/>低胎压场景只用于验证警报视觉，不定义真实车辆的报警阈值。"));
    QPushButton *connection=action(page,"toggleData",QString(),QRect(17,437,266,46));
    QPushButton *pressure=action(page,"togglePressure",QString(),QRect(302,437,285,46));
    QPushButton *quit=action(page,"quitApplication",QStringLiteral("退出应用"),QRect(606,437,289,46));
    connect(connection,&QPushButton::clicked,this,[this](){state->setDataAvailable(!state->dataAvailable);});
    connect(pressure,&QPushButton::clicked,this,[this](){state->setLowPressure(!state->lowPressure);});
    connect(quit,&QPushButton::clicked,this,&QWidget::close);
    auto refresh=[this,connection,pressure](){connection->setText(state->dataAvailable?QStringLiteral("模拟车辆数据断开"):QStringLiteral("恢复车辆数据"));pressure->setText(state->lowPressure?QStringLiteral("恢复正常胎压场景"):QStringLiteral("预览左前低胎压警报"));};connect(state,&VehicleDataCenter::changed,page,refresh);refresh();return page;
}
