#ifndef REPL_WIDGET_HPP
#define REPL_WIDGET_HPP

#include <QWidget>
#include <QLineEdit>
#include <QString>
#include <QVector>

class REPLWidget: public QWidget{
Q_OBJECT

protected:
	void keyPressEvent(QKeyEvent* keyE);

private:
	QLineEdit *replEdit;
	QVector<QString> historyVec;
	bool firstRun = false;
	int whereAt = 0;


public:
  REPLWidget(QWidget * parent = nullptr);

signals:

  void lineEntered(QString entry);

private slots:
  void changed();
};

#endif
