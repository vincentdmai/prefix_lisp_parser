#include "message_widget.hpp"

#include <QLabel>
#include <QLayout>
#include <QLineEdit>

MessageWidget::MessageWidget(QWidget *parent) : QWidget(parent) {
  // TODO: your code here...

	//create message labels
	QLabel *messageLabel = new QLabel("Message: ");

	//create a new instance of the message result line edit box and set it to read only
	messageResult = new QLineEdit;
	messageResult->setReadOnly(true);

	//create a layout box and add the widgets to it
	QHBoxLayout *box = new QHBoxLayout;
	box->addWidget(messageLabel);
	box->addWidget(messageResult, 30);
	setLayout(box);

}

void MessageWidget::info(QString message) {
  // TODO: your code here...
	// the QString is the info message. Set it with setText within the line edit widget
	clear();
	messageResult->setText(message);
}

void MessageWidget::error(QString message) {
  // TODO: your code here...
	// the QString is the error message. Update the message result (the line edit widget) by setting the text
	//NOTE: has to be highlighted in RED
	clear();
	QPalette p;
	p.setColor(QPalette::Highlight,Qt::red);
	messageResult->setPalette(p);
	messageResult->setText(message);
	
	//highlight box
	messageResult->selectAll();
}

void MessageWidget::clear() {
  // TODO: your code here...
	//clear the current message
	messageResult->clear();
	QPalette p;
	messageResult->setPalette(p);
}
