#include "hmi_cards.h"
#include "paint_helpers.h"
#include <QPainterPath>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QIcon>
using namespace Hmi;
namespace {
QIcon glyph(const QString &name) {
    QPixmap pix(48,48); pix.fill(Qt::transparent); QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing); icon(p,name,QRectF(4,4,40,40),Text);
    return QIcon(pix);
}
void title(QPainter &p, const QString &name, const QString &label) {
    icon(p,name,QRectF(13,12,19,19)); text(p,QRectF(39,9,210,24),label,14);
}
void batteryOutline(QPainter &p, QRectF r, const QColor &c) {
    p.setPen(QPen(c,1.4));p.setBrush(Qt::NoBrush);p.drawRoundedRect(r,2,2);
    p.drawLine(QPointF(r.right()+2,r.top()+r.height()*.3),QPointF(r.right()+2,r.bottom()-r.height()*.3));
    p.fillRect(r.adjusted(3,3,-7,-3),c);
}
void syncSlider(QSlider *slider, int value) { bool blocked=slider->blockSignals(true);slider->setValue(value);slider->blockSignals(blocked); }
}
HmiCard::HmiCard(VehicleDataCenter *model, const QSize &design, QWidget *parent)
    : QWidget(parent),m(model),base(design) {
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(m,&VehicleDataCenter::changed,this,[this](){update();});
}
void HmiCard::setup(QPainter &p, bool purple) {
    p.setRenderHint(QPainter::Antialiasing);p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.scale(width()/qreal(base.width()),height()/qreal(base.height()));panel(p,QRectF(QPointF(0,0),base),purple);
}
void HmiCard::place(QWidget *w, const QRect &r) { placements.append(Placement{w,r});w->setGeometry(r); }
QPushButton *HmiCard::button(const QString &id, const QString &label, const QRect &r, bool checkable) {
    QPushButton *b=new QPushButton(label,this);b->setObjectName(id);b->setAccessibleName(label.isEmpty()?id:label);
    b->setCheckable(checkable);b->setCursor(Qt::PointingHandCursor);place(b,r);return b;
}
QSlider *HmiCard::slider(const QString &id,const QRect &r,int maximum) {
    QSlider *s=new QSlider(Qt::Horizontal,this);s->setObjectName(id);s->setAccessibleName(id);s->setRange(0,maximum);place(s,r);return s;
}
void HmiCard::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    const qreal sx=width()/qreal(base.width()),sy=height()/qreal(base.height());
    for(int i=0;i<placements.size();++i) { const Placement &a=placements[i];
        a.widget->setGeometry(qRound(a.rect.x()*sx),qRound(a.rect.y()*sy),qRound(a.rect.width()*sx),qRound(a.rect.height()*sy));
        QFont f=a.widget->font();f.setPixelSize(qRound(12*qMin(sx,sy)));a.widget->setFont(f);
        QPushButton *b=qobject_cast<QPushButton *>(a.widget);if(b)b->setIconSize(QSize(qRound(20*sx),qRound(20*sy)));
    }
}
DashboardCard::DashboardCard(VehicleDataCenter *model, Kind k, QWidget *parent)
    : HmiCard(model,k==Speed?QSize(230,198):k==Battery?QSize(422,198):k==Energy?QSize(244,198):k==Trip?QSize(356,118):k==Service?QSize(334,118):QSize(206,118),parent),kind(k) {}
void DashboardCard::paintEvent(QPaintEvent *) {
    QPainter p(this);setup(p);
    const bool valid=m->dataAvailable;
    if(kind==Speed) {
        p.save();QPainterPath clip;clip.addRoundedRect(QRectF(1,1,228,196),8,8);p.setClipPath(clip);
        road(p,QRectF(1,26,228,171));p.restore();
        text(p,QRectF(15,10,60,22),valid?"READY":"OFFLINE",12,valid?Green:Amber);
        text(p,QRectF(78,10,120,22),valid?QStringLiteral("经济模式"):QStringLiteral("数据不可用"),12,Muted);
        text(p,QRectF(0,37,230,94),valid?"60":"--",76,Text,true,Qt::AlignCenter);
        text(p,QRectF(0,124,230,23),"km/h",15,Muted,false,Qt::AlignCenter);
        const QStringList gears=QStringList()<<"P"<<"R"<<"N"<<"D";
        for(int i=0;i<4;++i) {const QRectF r(51+i*36,158,27,29);if(i==3&&valid){p.setPen(QPen(Cyan,1));p.setBrush(QColor("#0c3b5a"));p.drawRoundedRect(r,5,5);}text(p,r,gears[i],16,i==3&&valid?Text:Muted,false,Qt::AlignCenter);}
    } else if(kind==Battery) {
        text(p,QRectF(16,21,125,22),QStringLiteral("剩余续航"),12,Muted);
        text(p,QRectF(16,42,100,39),valid?"428":"--",31,Text,true);
        text(p,QRectF(94,51,40,24),"km",14,Muted);
        batteryOutline(p,QRectF(17,99,33,16),valid?Green:Muted);
        text(p,QRectF(62,89,65,36),valid?"80":"--",25,Text);text(p,QRectF(101,96,27,25),"%",13,Muted);
        p.setPen(Qt::NoPen);p.setBrush(QColor("#153248"));p.drawRoundedRect(QRectF(17,125,80,9),4,4);
        p.setBrush(valid?Green:Muted);p.drawRoundedRect(QRectF(17,125,valid?64:0,9),4,4);
        text(p,QRectF(16,145,135,24),valid?QStringLiteral("预计可行驶 428 km"):QStringLiteral("等待车辆数据"),11,Muted);
        photo(p,"car",QRectF(147,39,238,132));
        const QStringList positions=QStringList()<<QStringLiteral("左前")<<QStringLiteral("右前")<<QStringLiteral("左后")<<QStringLiteral("右后");
        for(int i=0;i<4;++i) {
            qreal x=(i%2==0)?147:354,y=(i<2)?8:145;
            const bool warn=valid&&m->lowPressure&&i==0;
            text(p,QRectF(x,y,59,17),positions[i],10,Muted);
            text(p,QRectF(x,y+16,66,19),!valid?"-- bar":warn?"1.8 bar":i<2?"2.5 bar":"2.4 bar",12,warn?QColor("#ff7379"):Text,true);
        }
        p.setPen(QPen(valid&&m->lowPressure?QColor("#a64a54"):QColor("#2a5d7b"),1));p.setBrush(QColor("#12304a"));p.drawRoundedRect(QRectF(204,171,142,22),11,11);
        icon(p,"check",QRectF(211,174,15,15),!valid?Muted:m->lowPressure?QColor("#ff7379"):Green);
        text(p,QRectF(233,171,110,22),!valid?QStringLiteral("数据已断开"):m->lowPressure?QStringLiteral("左前胎压偏低"):QStringLiteral("车辆状态正常"),11);
    } else if(kind==Energy) {
        text(p,QRectF(15,10,180,25),QStringLiteral("能量流"),14);
        QLinearGradient car(22,0,103,0);car.setColorAt(0,QColor("#073049"));car.setColorAt(.5,QColor("#154b69"));car.setColorAt(1,QColor("#082b41"));
        p.setPen(QPen(QColor("#3ca6d5"),1.2));p.setBrush(car);p.drawRoundedRect(QRectF(27,43,63,140),26,26);
        p.setBrush(QColor("#0a1d30"));p.drawRoundedRect(QRectF(34,53,49,32),11,11);
        for(int x=22;x<=88;x+=66)for(int y=70;y<=147;y+=77){p.setBrush(QColor("#163f59"));p.drawRoundedRect(QRectF(x,y,8,23),3,3);}
        const QColor flow=valid?Cyan:Muted;
        batteryOutline(p,QRectF(47,112,23,36),flow);
        p.setPen(QPen(flow,2));
        for(int i=0;i<3;++i){qreal y=90+i*31+(valid?(m->position%4)*1.5:0);if(y>110&&y<151)continue;p.drawLine(QPointF(58,y+12),QPointF(58,y));p.drawLine(QPointF(54,y+5),QPointF(58,y));p.drawLine(QPointF(62,y+5),QPointF(58,y));}
        text(p,QRectF(126,40,104,18),QStringLiteral("电机功率"),11,Muted);
        text(p,QRectF(126,59,56,30),valid?"35":"--",23,Cyan,true);text(p,QRectF(166,64,40,25),"kW",12,Muted);
        text(p,QRectF(126,94,104,18),QStringLiteral("瞬时电耗"),11,Muted);
        text(p,QRectF(126,113,104,24),valid?"12.3":"--",21,Cyan,true);
        text(p,QRectF(177,116,62,22),"kWh/100km",9,Muted);
        text(p,QRectF(126,146,104,18),QStringLiteral("累计里程"),11,Muted);
        text(p,QRectF(126,164,104,27),valid?"5236 km":"-- km",18,Text);
    } else if(kind==Trip) {
        title(p,"trip",QStringLiteral("行车信息"));
        const QStringList labels=QStringList()<<QStringLiteral("平均电耗")<<QStringLiteral("本次里程")<<QStringLiteral("行驶时长")<<QStringLiteral("平均车速");
        const QStringList values=QStringList()<<"13.2"<<"125.6"<<"2h 18"<<"54";
        const QStringList units=QStringList()<<"kWh/100km"<<"km"<<"min"<<"km/h";
        for(int i=0;i<4;++i){int x=18+i*85;text(p,QRectF(x,43,83,20),labels[i],11,Muted);text(p,QRectF(x,66,83,26),valid?values[i]:"--",20,Text,true);text(p,QRectF(x,94,83,17),units[i],10,Muted);}
    } else if(kind==Service) {
        title(p,"bell",QStringLiteral("提醒与维护"));
        icon(p,"check",QRectF(13,63,22,22),valid&&!m->lowPressure?Green:Amber);
        text(p,QRectF(41,61,119,26),!valid?QStringLiteral("等待车辆数据"):m->lowPressure?QStringLiteral("请检查左前胎压"):QStringLiteral("当前无故障"),11);
        p.setPen(QColor("#1b3449"));p.drawLine(159,37,159,103);
        const QStringList rows=QStringList()<<(valid?QStringLiteral("下次保养  3280 km"):QStringLiteral("保养信息  --"))<<(valid?QStringLiteral("动力电池  状态正常"):QStringLiteral("动力电池  未知"))<<(!valid?QStringLiteral("胎压  未知"):m->lowPressure?QStringLiteral("胎压  低压警报"):QStringLiteral("胎压  正常"));
        const QStringList icons=QStringList()<<"wrench"<<"battery"<<"car";
        for(int i=0;i<3;++i){icon(p,icons[i],QRectF(172,39+i*25,16,16),i?Green:Muted);text(p,QRectF(195,36+i*25,130,21),rows[i],10,i==2&&m->lowPressure?Amber:Muted);}
    } else {
        road(p,QRectF(1,1,204,116));
        text(p,QRectF(27,25,165,30),QStringLiteral("绿色出行"),24,Text,false,Qt::AlignCenter);
        text(p,QRectF(18,57,180,29),QStringLiteral("让未来更美好"),19,Text,false,Qt::AlignCenter);
        p.setPen(QPen(Cyan,2));p.drawLine(86,105,119,105);
    }
}
ClimateCard::ClimateCard(VehicleDataCenter *model,QWidget *parent):HmiCard(model,QSize(262,190),parent) {
    minus=button("temperatureDown","−",QRect(22,48,40,40));plus=button("temperatureUp","+",QRect(202,48,40,40));
    autoButton=button("climateAuto",QStringLiteral("自动"),QRect(193,94,56,29),true);
    acButton=button("climateAC","A/C",QRect(13,135,43,34),true);
    front=button("frontDefrost",QString(),QRect(69,132,45,37),true);front->setIcon(glyph("defrost"));front->setAccessibleName(QStringLiteral("前挡除雾"));
    rear=button("rearDefrost",QString(),QRect(131,132,45,37),true);rear->setIcon(glyph("rear"));rear->setAccessibleName(QStringLiteral("后挡除雾"));
    recirc=button("recirculation",QString(),QRect(193,132,45,37),true);recirc->setIcon(glyph("recirc"));recirc->setAccessibleName(QStringLiteral("内循环"));
    temperatureSlider=slider("temperatureSlider",QRect(48,97,129,24),60);
    temperatureSlider->setRange(32,60);
    temperatureSlider->setSingleStep(1);
    temperatureSlider->setPageStep(2);
    temperatureSlider->setAccessibleName(QStringLiteral("空调设定温度"));
    connect(minus,&QPushButton::clicked,this,[this](){m->setTemperature(m->temperature-.5);});
    connect(plus,&QPushButton::clicked,this,[this](){m->setTemperature(m->temperature+.5);});
    connect(autoButton,&QPushButton::clicked,this,[this](bool v){m->automatic=v;m->notify();});
    connect(acButton,&QPushButton::clicked,this,[this](bool v){m->ac=v;m->notify();});
    connect(front,&QPushButton::clicked,this,[this](bool v){m->frontDefrost=v;m->notify();});
    connect(rear,&QPushButton::clicked,this,[this](bool v){m->rearDefrost=v;m->notify();});
    connect(recirc,&QPushButton::clicked,this,[this](bool v){m->recirculation=v;m->notify();});
    connect(temperatureSlider,&QSlider::valueChanged,this,[this](int value){m->setTemperature(value/2.0);});
    connect(m,&VehicleDataCenter::changed,this,[this](){refresh();});refresh();
}
void ClimateCard::refresh(){minus->setEnabled(m->temperature>16);plus->setEnabled(m->temperature<30);autoButton->setChecked(m->automatic);acButton->setChecked(m->ac);front->setChecked(m->frontDefrost);rear->setChecked(m->rearDefrost);recirc->setChecked(m->recirculation);syncSlider(temperatureSlider,qRound(m->temperature*2.0));}
void ClimateCard::paintEvent(QPaintEvent *) {QPainter p(this);setup(p);title(p,"snow",QStringLiteral("空调控制"));
    text(p,QRectF(65,45,116,44),QString::number(m->temperature,'f',1),31,Text,true,Qt::AlignCenter);text(p,QRectF(172,63,24,22),QStringLiteral("℃"),14);
    icon(p,"snow",QRectF(20,100,20,20),Text);
    text(p,QRectF(67,170,53,17),QStringLiteral("前挡除雾"),9,Muted,false,Qt::AlignCenter);text(p,QRectF(129,170,53,17),QStringLiteral("后挡除雾"),9,Muted,false,Qt::AlignCenter);text(p,QRectF(193,170,46,17),QStringLiteral("内循环"),9,Muted,false,Qt::AlignCenter);
}
NavigationCard::NavigationCard(VehicleDataCenter *model,QWidget *parent):HmiCard(model,QSize(248,190),parent) {
    QPushButton *b=button("openNavigation",QString(),QRect(4,34,240,151));b->setStyleSheet("background:transparent;border:0;");b->setAccessibleName(QStringLiteral("打开导航详情"));b->setToolTip(QStringLiteral("查看路线"));connect(b,&QPushButton::clicked,this,&NavigationCard::openRequested);
}
void NavigationCard::paintEvent(QPaintEvent *) {
    QPainter p(this);setup(p);title(p,"nav",QStringLiteral("导航"));
    p.save();p.setClipRect(QRectF(8,35,232,146));p.fillRect(QRectF(8,35,232,146),QColor("#091b2d"));
    for(int i=0;i<7;++i){p.setPen(QPen(QColor("#163049"),7));p.drawLine(QPointF(-90+i*66,157),QPointF(45+i*32,48));p.setPen(QPen(QColor("#21425b"),1));p.drawLine(QPointF(-90+i*66,157),QPointF(45+i*32,48));}
    for(int i=0;i<5;++i){p.setPen(QPen(QColor("#163049"),7));p.drawLine(QPointF(0,75+i*24),QPointF(255,47+i*24));p.setPen(QPen(QColor("#23445d"),1));p.drawLine(QPointF(0,75+i*24),QPointF(255,47+i*24));}
    if(m->routeActive){QPainterPath route;route.moveTo(122,152);route.lineTo(130,113);route.lineTo(123,76);route.lineTo(143,58);p.setPen(QPen(QColor("#075661"),10));p.drawPath(route);p.setPen(QPen(Green,4));p.drawPath(route);}
    QPolygonF arrow;arrow<<QPointF(124,125)<<QPointF(114,143)<<QPointF(124,139)<<QPointF(134,143);p.setPen(QPen(Text,1));p.setBrush(Cyan);p.drawPolygon(arrow);
    p.setPen(QPen(QColor("#f45363"),3));p.setBrush(Qt::white);p.drawEllipse(QRectF(205,119,28,28));text(p,QRectF(205,119,28,28),"60",13,QColor("#102338"),true,Qt::AlignCenter);
    p.fillRect(QRectF(8,35,232,33),QColor(4,18,34,235));icon(p,"arrow",QRectF(16,40,24,24),Text);
    text(p,QRectF(52,35,176,18),m->routeActive?QStringLiteral("前方 800 米"):QStringLiteral("暂无进行中的路线"),12,Text,true);
    text(p,QRectF(52,52,176,15),m->routeActive?QStringLiteral("进入 科技大道"):QStringLiteral("点击查看路线详情"),10,Muted);
    p.fillRect(QRectF(8,155,232,27),QColor("#071828"));text(p,QRectF(15,157,127,22),m->routeActive?QStringLiteral("12 公里 · 28 分钟"):QStringLiteral("导航已结束"),11);text(p,QRectF(150,157,81,22),m->routeActive?QStringLiteral("预计 15:04"):"--:--",10,Muted);p.restore();
}
CameraCard::CameraCard(VehicleDataCenter *model,QWidget *parent):HmiCard(model,QSize(172,190),parent) {
    QPushButton *b=button("openCamera",QStringLiteral("点击进入  ›"),QRect(13,145,146,34));b->setProperty("quiet",true);connect(b,&QPushButton::clicked,this,&CameraCard::openRequested);
}
void CameraCard::paintEvent(QPaintEvent *) {QPainter p(this);setup(p);title(p,"camera",QStringLiteral("倒车影像"));photo(p,"camera",QRectF(10,39,152,99));p.fillRect(QRectF(10,119,152,19),QColor(5,16,28,195));text(p,QRectF(16,119,146,19),QStringLiteral("进入时自动连接摄像头"),9,Muted);}
MediaCard::MediaCard(VehicleDataCenter *model,QWidget *parent):HmiCard(model,QSize(206,190),parent) {
    QPushButton *prev=button("previousTrack",QString(),QRect(23,143,36,35));prev->setIcon(glyph("previous"));prev->setProperty("quiet",true);prev->setAccessibleName(QStringLiteral("上一曲"));
    play=button("playPause",QString(),QRect(86,138,37,40));play->setAccessibleName(QStringLiteral("播放暂停"));
    QPushButton *next=button("nextTrack",QString(),QRect(151,143,36,35));next->setIcon(glyph("next"));next->setProperty("quiet",true);next->setAccessibleName(QStringLiteral("下一曲"));
    progress=slider("mediaProgress",QRect(12,115,182,19),m->duration());progress->setAccessibleName(QStringLiteral("媒体播放进度"));
    connect(prev,&QPushButton::clicked,this,[this](){m->changeTrack(-1);});connect(next,&QPushButton::clicked,this,[this](){m->changeTrack(1);});
    connect(play,&QPushButton::clicked,this,[this](){m->playing=!m->playing;m->notify();});connect(progress,&QSlider::valueChanged,m,&VehicleDataCenter::setPosition);
    connect(m,&VehicleDataCenter::changed,this,[this](){refresh();});refresh();
}
void MediaCard::refresh(){play->setIcon(glyph(m->playing?"pause":"play"));const bool blocked=progress->blockSignals(true);progress->setMaximum(m->duration());progress->setValue(m->position);progress->blockSignals(blocked);}
void MediaCard::paintEvent(QPaintEvent *) {QPainter p(this);setup(p,true);title(p,"music",QStringLiteral("音乐"));photo(p,"cover",QRectF(12,41,68,66));
    text(p,QRectF(90,40,110,23),m->trackTitle(),11,Text,true);text(p,QRectF(90,65,108,20),m->artist(),10,Muted);text(p,QRectF(90,87,108,20),QStringLiteral("让每次出发都有意义"),9,Muted);
    text(p,QRectF(12,132,60,17),QStringLiteral("播放状态"),9,Amber);text(p,QRectF(118,132,78,17),timeText(m->position)+" / "+timeText(m->duration()),9,Muted,false,Qt::AlignRight|Qt::AlignVCenter);
}
