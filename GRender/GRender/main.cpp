#include "GRender.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GRender w;
	w.show();
	return a.exec();
}
