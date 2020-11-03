#include "qgraphics_arc_item.hpp"

#include <cmath>

#include <QDebug>
#include <QPainter>

QGraphicsArcItem::QGraphicsArcItem(qreal x, qreal y, qreal width, qreal height,
                                   QGraphicsItem *parent) : QGraphicsEllipseItem(x,y,width, height) {
  // TODO: your code here ...
}


void QGraphicsArcItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget) {
  // TODO: your code here ...

	//configure / "overwrite" the paint option
	 painter->drawArc(rect(), startAngle(), spanAngle());
}
