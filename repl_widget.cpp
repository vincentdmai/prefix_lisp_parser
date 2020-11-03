#include "repl_widget.hpp"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QKeyEvent>

REPLWidget::REPLWidget(QWidget * parent) : QWidget(parent) {
	// TODO: your code here

	  //create the label
	QLabel *replLabel = new QLabel("vtscript> ");

	//create the line edit
	replEdit = new QLineEdit;

	//create box layout
	QHBoxLayout *box = new QHBoxLayout;
	box->addWidget(replLabel);
	box->addWidget(replEdit);

	setLayout(box);
	connect(replEdit, &QLineEdit::returnPressed, this, &REPLWidget::changed);

}

void REPLWidget::changed() {
	// TODO: your code here
	  // emit change that repl box has text

	  //clear repl edit
	QString entry = replEdit->text();
	replEdit->clear();

	//push back into the history vector
	historyVec.push_back(entry);
	whereAt = historyVec.size() - 1;
	this->firstRun = true;

	emit lineEntered(entry);
}


void REPLWidget::keyPressEvent(QKeyEvent* keyE)
{
	//key press event would check if the event is an up key or down key
	if (keyE->key() == Qt::Key_Up)
	{
		//move up in history back to the past entries
		if (whereAt >= 0 && firstRun)
		{
			replEdit->setText(historyVec.at(whereAt));
			if (whereAt > 0)
			{
				whereAt--;
			}
		}
		
	}
	else if (keyE->key() == Qt::Key_Down)
	{
		//move down in history towards the present
		if (whereAt + 1 < historyVec.size())
		{
			whereAt++;
			replEdit->setText(historyVec.at(whereAt));
		}
		else
		{
			replEdit->clear();
		}
	}
}