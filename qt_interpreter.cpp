#include "qt_interpreter.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <fstream>

#include <QBrush>
#include <QDebug>


#include "qgraphics_arc_item.hpp"

#include "interpreter_semantic_error.hpp"

QtInterpreter::QtInterpreter(QObject * parent): QObject(parent){
  // TODO: your code here...
	//I don't have to do anything.
}

void QtInterpreter::parseAndEvaluate(QString entry){
	std::string program = entry.toStdString();
	std::istringstream iss(program);
	bool ok = interp.parse(iss);
	if (!ok) {
		emit error("Error : Failed to Parse");
	}
	try{
		Expression results = interp.eval();
		std::stringstream ss;
		ss << results; //string stream obtains the overloading results of eval()
		emit info(QString::fromStdString(ss.str())); //send the results over to the info signal
		ss.clear();
		for (auto &a : interp.getGraphics()){ //check for graphics if correct
			if (a.type == PointType) {
				//parse Point
				QRect qp(a.value.point_value.x, a.value.point_value.y, 2, 2);
				QGraphicsEllipseItem *qe = new QGraphicsEllipseItem(qp);
				emit drawGraphic(qe);
			}
			else if (a.type == ArcType) {
				//parse Arc
				qreal centerX = a.value.arc_value.center.x;
				qreal centerY = a.value.arc_value.center.y;
				qreal startX = a.value.arc_value.start.x;
				qreal startY = a.value.arc_value.start.y;
				qreal radius = std::sqrt(pow(centerY - startY, 2) + pow(centerX - startX, 2)); //distance formula
				qreal width = 2*radius; //width and height is the rectangle surrounding the arc
				qreal height = 2*radius;
				qreal rectX = centerX - radius;
				qreal rectY = centerY - radius;
				QGraphicsArcItem * qa = new QGraphicsArcItem(rectX, rectY, width, height);
				//set span angle
				qreal spanRadian = a.value.arc_value.span;
				qreal spanDegree = (spanRadian * 180 * 16) / (std::atan2(0, -1)); //multiply by an addition 16 from qt forums
				qa->setSpanAngle(spanDegree);
				//set start angle
				qreal startAngle = (std::atan2(-startY + centerY, startX- centerX) * (180 * 16)) / (std::atan2(0, -1));
				qa->setStartAngle(startAngle);
				emit drawGraphic(qa);
			}
			else if (a.type == LineType) {
				//parse line
				QLine qf(a.value.line_value.first.x, a.value.line_value.first.y, a.value.line_value.second.x, a.value.line_value.second.y);
				QGraphicsLineItem *qg = new QGraphicsLineItem(qf);
				emit drawGraphic(qg);
			}
			else {
				//throw error, invalid types in graphics type
				emit error("Error : Invalid Types in Graphics Parsing");
			}
		}
	}
	catch (InterpreterSemanticError &w) {
		emit error(w.what());
	}
}

