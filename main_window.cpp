#include "main_window.hpp"

#include <iostream>
#include <fstream>
#include <QLayout>
#include <QTextStream>
#include <QFile>

#include "message_widget.hpp"
#include "canvas_widget.hpp"
#include "repl_widget.hpp"
#include "interpreter_semantic_error.hpp"

MainWindow::MainWindow(QWidget * parent): MainWindow("", parent){
  // TODO: your code here...

}

MainWindow::MainWindow(std::string filename, QWidget * parent): QWidget(parent){
  // TODO: your code here...
	
	MessageWidget *msg = new MessageWidget(parent);
	CanvasWidget *canvas = new CanvasWidget(parent);
	REPLWidget* repl = new REPLWidget(parent);

	//create layout
	QGridLayout *layout = new QGridLayout(parent);

	//TODO: add message widgets, canvas widgets, and REPL widgets
	layout->addWidget(msg);
	layout->addWidget(canvas);
	layout->addWidget(repl);
	//set to grid

	setLayout(layout);

	//repl to the interpreter
	connect(repl, &REPLWidget::lineEntered, &interp, &QtInterpreter::parseAndEvaluate);

	//connect the interp to info slot
	connect(&interp, &QtInterpreter::info, msg, &MessageWidget::info);

	//connect the interp to error slot
	connect(&interp, &QtInterpreter::error, msg, &MessageWidget::error);

	//connect the interp to draw slot
	connect(&interp, &QtInterpreter::drawGraphic, canvas, &CanvasWidget::addGraphic);

	//connect file argument signals to message
	connect(this, &MainWindow::fileInfo, &interp, &QtInterpreter::parseAndEvaluate);
	connect(this, &MainWindow::error, msg, &MessageWidget::error);

	if (!(filename == std::string("")))
	{
		//TODO: with preloaded file

		QString fileName = QString::fromStdString(filename);
		QFile in(fileName);

		if (!in.exists())
		{
			emit error("Unable to open file");
		}
		else if (in.open(QIODevice::ReadOnly))
		{
			QTextStream parseFile(&in);
			QString line;
			while (!parseFile.atEnd())
			{
				line = parseFile.readAll();
			}
			in.close();
			emit fileInfo(line);
		}


	}

}
