#ifndef PAINT_HELPERS_H
#define PAINT_HELPERS_H
#include <QPainter>
#include <QImage>
#include <QString>
namespace Hmi {
extern const QColor Text, Muted, Cyan, Green, Amber;
void text(QPainter &p, const QRectF &r, const QString &s, int pixels = 13,
          const QColor &color = Text, bool bold = false, int align = Qt::AlignLeft | Qt::AlignVCenter);
void icon(QPainter &p, const QString &name, const QRectF &r, const QColor &color = Cyan);
void panel(QPainter &p, const QRectF &r, bool purple = false);
void photo(QPainter &p, const QString &name, const QRectF &target);
void road(QPainter &p, const QRectF &r);
QString timeText(int seconds);
}
#endif
