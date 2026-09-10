#include "paint_helpers.h"
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFont>
namespace Hmi {
const QColor Text("#e4f2ff"), Muted("#93aec7"), Cyan("#27c5f4"), Green("#0bd4ad"), Amber("#e7ba71");
void text(QPainter &p, const QRectF &r, const QString &s, int pixels, const QColor &color, bool bold, int align) {
    p.save(); QFont f = p.font(); f.setPixelSize(pixels); f.setBold(bold);
    p.setFont(f); p.setPen(color); p.drawText(r, align, s); p.restore();
}
void panel(QPainter &p, const QRectF &r, bool purple) {
    QLinearGradient g(r.topLeft(), r.bottomRight());
    g.setColorAt(0, purple ? QColor("#1c294b") : QColor("#102b45"));
    g.setColorAt(1, QColor("#091d30"));
    p.setPen(QPen(QColor("#17374d"), 1)); p.setBrush(g); p.drawRoundedRect(r.adjusted(.5,.5,-.5,-.5), 9,9);
}
QString timeText(int v) { return QStringLiteral("%1:%2").arg(v/60,2,10,QChar('0')).arg(v%60,2,10,QChar('0')); }
void photo(QPainter &p, const QString &name, const QRectF &target) {
    // Reuse only illustration areas of the supplied prototype. All UI text/controls are native Qt.
    // This asset is a static design reference, never presented as a live camera or map.
    static const QImage source(QStringLiteral(":/hmi/prototype.jpg"));
    QRectF crop;
    if (name == "car") crop = QRectF(796,163,310,183);
    else if (name == "camera") crop = QRectF(989,475,227,142);
    else if (name == "cover") crop = QRectF(1259,477,103,99);
    else crop = QRectF(1248,721,318,153);
    p.save(); QPainterPath path; path.addRoundedRect(target,5,5); p.setClipPath(path);
    if (name == "car") {
        // Clip to the vehicle silhouette, excluding all text around the reference photo.
        QPainterPath silhouette;
        silhouette.moveTo(805,278);
        silhouette.cubicTo(806,250,821,231,851,222);
        silhouette.cubicTo(878,187,900,177,945,174);
        silhouette.cubicTo(986,169,1027,175,1044,188);
        silhouette.cubicTo(1067,201,1092,222,1098,244);
        silhouette.lineTo(1097,293);
        silhouette.cubicTo(1092,319,1080,334,1066,331);
        silhouette.cubicTo(1051,331,1048,319,1045,306);
        silhouette.lineTo(927,316);
        silhouette.cubicTo(916,341,896,344,882,332);
        silhouette.lineTo(866,308);
        silhouette.cubicTo(832,307,807,302,805,278);
        silhouette.closeSubpath();
        QTransform transform;
        transform.translate(target.x(),target.y());
        transform.scale(target.width()/crop.width(),target.height()/crop.height());
        transform.translate(-crop.x(),-crop.y());
        p.setClipPath(transform.map(silhouette),Qt::IntersectClip);
    }
    if (!source.isNull()) p.drawImage(target, source, crop);
    else p.fillRect(target,QColor("#17364e"));
    p.restore();
}
void road(QPainter &p, const QRectF &r) {
    p.save(); p.setClipRect(r); p.translate(r.topLeft());
    const qreal w=r.width(), h=r.height();
    QLinearGradient sky(0,0,0,h); sky.setColorAt(0,QColor("#07192c")); sky.setColorAt(.58,QColor("#14375b")); sky.setColorAt(1,QColor("#071626"));
    p.fillRect(QRectF(0,0,w,h),sky);
    QPainterPath distant; distant.moveTo(0,h*.56);
    distant.cubicTo(w*.11,h*.27,w*.2,h*.60,w*.32,h*.45);
    distant.cubicTo(w*.40,h*.32,w*.48,h*.60,w*.58,h*.52);
    distant.cubicTo(w*.72,h*.31,w*.79,h*.41,w,h*.36);
    distant.lineTo(w,h);distant.lineTo(0,h);distant.closeSubpath();p.fillPath(distant,QColor("#102940"));
    QPainterPath mountain; mountain.moveTo(0,h*.54);
    mountain.cubicTo(w*.09,h*.47,w*.15,h*.62,w*.25,h*.53);
    mountain.cubicTo(w*.35,h*.44,w*.39,h*.65,w*.52,h*.58);
    mountain.cubicTo(w*.66,h*.50,w*.75,h*.64,w*.84,h*.52);
    mountain.cubicTo(w*.93,h*.43,w*.96,h*.52,w,h*.46);
    mountain.lineTo(w,h); mountain.lineTo(0,h); mountain.closeSubpath();
    p.fillPath(mountain,QColor("#071727"));
    QPolygonF road; road << QPointF(w*.47,h*.58) << QPointF(w*.56,h*.58) << QPointF(w*1.2,h) << QPointF(-w*.2,h);
    p.setBrush(QColor("#10263d")); p.setPen(Qt::NoPen); p.drawPolygon(road);
    p.setPen(QPen(QColor("#507da6"),1.3)); p.drawLine(QPointF(w*.44,h*.60),QPointF(0,h*.9)); p.drawLine(QPointF(w*.6,h*.60),QPointF(w,h*.9));
    p.setPen(QPen(QColor("#92b3cd"),1.4));
    for(int i=0;i<4;++i) { qreal y=h*(.63+i*.09); p.drawLine(QPointF(w*(.5-i*.055),y),QPointF(w*(.485-i*.058),y+h*.037)); }
    QLinearGradient fade(0,h*.4,0,h); fade.setColorAt(0,QColor(5,18,32,0)); fade.setColorAt(1,QColor(5,18,32,200)); p.fillRect(QRectF(0,0,w,h),fade); p.restore();
}
void icon(QPainter &p, const QString &name, const QRectF &r, const QColor &color) {
    p.save(); p.translate(r.topLeft()); p.scale(r.width()/24.,r.height()/24.);
    p.setPen(QPen(color,1.7,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin)); p.setBrush(Qt::NoBrush);
    QPainterPath a;
    if(name=="home") { a.moveTo(2,11); a.lineTo(12,3); a.lineTo(22,11); a.moveTo(5,10); a.lineTo(5,21); a.lineTo(10,21); a.lineTo(10,15); a.lineTo(14,15); a.lineTo(14,21); a.lineTo(19,21); a.lineTo(19,10); p.drawPath(a); }
    else if(name=="car") { p.drawRoundedRect(QRectF(3,10,18,9),2,2); a.moveTo(5,10);a.lineTo(7,5);a.lineTo(17,5);a.lineTo(19,10);p.drawPath(a); p.drawLine(6,19,6,22);p.drawLine(18,19,18,22);p.drawLine(6,13,8,13);p.drawLine(16,13,18,13); }
    else if(name=="nav") { QPolygonF q; q<<QPointF(3,10)<<QPointF(21,3)<<QPointF(14,21)<<QPointF(11,13);p.setBrush(color);p.drawPolygon(q); }
    else if(name=="fan") { for(int i=0;i<4;++i) {p.save();p.translate(12,12);p.rotate(i*90);QPainterPath blade;blade.moveTo(0,0);blade.cubicTo(-8,-3,-4,-14,1,-10);blade.cubicTo(5,-7,4,-3,0,0);p.fillPath(blade,color);p.restore();}p.setBrush(color);p.drawEllipse(QPointF(12,12),2,2); }
    else if(name=="music") { a.moveTo(8,18);a.lineTo(8,5);a.lineTo(20,3);a.lineTo(20,16);a.moveTo(8,9);a.lineTo(20,7);p.drawPath(a);p.setBrush(color);p.drawEllipse(QRectF(2,16,6,4));p.drawEllipse(QRectF(14,14,6,4)); }
    else if(name=="settings") { p.drawEllipse(QRectF(6,6,12,12));p.drawEllipse(QRectF(9,9,6,6)); for(int i=0;i<8;++i){p.save();p.translate(12,12);p.rotate(i*45);p.drawLine(0,-8,0,-11);p.restore();} }
    else if(name=="snow") { for(int i=0;i<6;++i){p.save();p.translate(12,12);p.rotate(i*60);p.drawLine(0,0,0,-10);p.drawLine(0,-6,-3,-8);p.drawLine(0,-6,3,-8);p.restore();} }
    else if(name=="camera") { p.drawRoundedRect(QRectF(2,6,14,12),2,2);QPolygonF q;q<<QPointF(16,10)<<QPointF(22,6)<<QPointF(22,18)<<QPointF(16,14);p.drawPolygon(q);p.drawEllipse(QPointF(9,12),2.5,2.5); }
    else if(name=="trip" || name=="signal") {p.setPen(Qt::NoPen);p.setBrush(color);for(int i=0;i<4;++i)p.drawRoundedRect(QRectF(2+i*5,18-i*4,3,4+i*4),.7,.7);}
    else if(name=="bell") { a.moveTo(5,17);a.lineTo(7,14);a.lineTo(7,9);a.cubicTo(7,2,17,2,17,9);a.lineTo(17,14);a.lineTo(19,17);a.closeSubpath();p.drawPath(a);p.drawArc(QRectF(9,17,6,5),180*16,180*16); }
    else if(name=="wifi") {p.drawArc(QRectF(1,4,22,21),45*16,90*16);p.drawArc(QRectF(5,9,14,13),45*16,90*16);p.drawArc(QRectF(9,14,6,6),45*16,90*16);p.setBrush(color);p.drawEllipse(QPointF(12,20),1,1);}
    else if(name=="bluetooth") {a.moveTo(11,2);a.lineTo(18,7);a.lineTo(6,17);a.moveTo(6,7);a.lineTo(18,17);a.lineTo(11,22);a.lineTo(11,2);p.drawPath(a);}
    else if(name=="pin") {p.drawEllipse(QRectF(5,2,14,14));a.moveTo(6,12);a.lineTo(12,22);a.lineTo(18,12);p.drawPath(a);p.drawEllipse(QRectF(9,6,6,6));}
    else if(name=="battery") {p.drawRoundedRect(QRectF(1,6,20,12),2,2);p.drawLine(23,10,23,14);p.fillRect(QRectF(4,9,13,6),color);}
    else if(name=="check") {p.setPen(Qt::NoPen);p.setBrush(color);p.drawEllipse(QRectF(2,2,20,20));p.setPen(QPen(QColor("#073a40"),2));a.moveTo(7,12);a.lineTo(10,15);a.lineTo(17,8);p.drawPath(a);}
    else if(name=="wrench") {a.moveTo(15,3);a.cubicTo(8,0,5,8,10,12);a.lineTo(3,19);a.lineTo(6,22);a.lineTo(14,14);a.cubicTo(21,17,25,8,21,5);a.lineTo(17,9);a.lineTo(14,6);a.closeSubpath();p.drawPath(a);}
    else if(name=="defrost" || name=="rear") {if(name=="rear")p.drawRect(QRectF(3,5,18,14));else {a.moveTo(5,19);a.lineTo(2,7);a.quadTo(12,1,22,7);a.lineTo(19,19);a.closeSubpath();p.drawPath(a);}for(int i=0;i<3;++i){int x=7+i*5;p.drawLine(x,16,x,8);p.drawLine(x,8,x-2,10);}}
    else if(name=="recirc") {a.moveTo(2,15);a.lineTo(4,9);a.lineTo(8,8);a.lineTo(11,5);a.lineTo(17,5);a.lineTo(20,9);a.lineTo(22,10);a.lineTo(22,15);a.closeSubpath();p.drawPath(a);p.drawArc(QRectF(7,10,10,8),0,260*16);p.drawLine(17,13,15,11);}
    else if(name=="play") {QPolygonF q;q<<QPointF(7,4)<<QPointF(20,12)<<QPointF(7,20);p.setBrush(color);p.drawPolygon(q);}
    else if(name=="pause") {p.setPen(Qt::NoPen);p.setBrush(color);p.drawRoundedRect(QRectF(6,4,4,16),1,1);p.drawRoundedRect(QRectF(15,4,4,16),1,1);}
    else if(name=="previous" || name=="next") {if(name=="next"){p.translate(24,0);p.scale(-1,1);}QPolygonF q;q<<QPointF(18,5)<<QPointF(7,12)<<QPointF(18,19);p.setBrush(color);p.drawPolygon(q);p.drawLine(5,5,5,19);}
    else if(name=="arrow") {a.moveTo(5,21);a.lineTo(5,10);a.quadTo(5,6,10,6);a.lineTo(20,6);a.moveTo(15,2);a.lineTo(20,6);a.lineTo(15,11);p.setPen(QPen(color,3));p.drawPath(a);}
    else if(name=="chevron") {a.moveTo(9,4);a.lineTo(16,12);a.lineTo(9,20);p.drawPath(a);}
    p.restore();
}
}
