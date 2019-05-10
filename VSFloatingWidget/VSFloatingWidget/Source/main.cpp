#include "VSFloatingWidget.h"
#include <QApplication>
#include <QPoint>
#include "VSFloatingWidget.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VSFloatingWidget w(esmShowFixed, QPoint(800, 600));
	w.show();
	return a.exec();
}
