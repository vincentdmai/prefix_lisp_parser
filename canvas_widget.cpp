#include "canvas_widget.hpp"

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLayout>

CanvasWidget::CanvasWidget(QWidget * parent): QWidget(parent){
  // TODO: your code here...
	//instantiate graphics canvas where all graphical elements will be displayed
	scene = new QGraphicsScene(this);

	//create the graphics view with the scene instantiated
	QGraphicsView * qview = new QGraphicsView(scene, this);

	//create layout to add the view onto there and set it
	QGridLayout *box = new QGridLayout;
	box->addWidget(qview);
	setLayout(box);
}

void CanvasWidget::addGraphic(QGraphicsItem * item){
  // TODO: your code here...
  // A public slot that accepts a signal in the form of a QGraphicsItem pointer containing an object derived from QGraphicsItem to draw
	this->scene->addItem(item);
	
}
