#pragma once

#include <QtWidgets>

class MainWindow : public QMainWindow {
	Q_OBJECT
public:

	MainWindow(QWidget* parent = Q_NULLPTR);

	~MainWindow();


	void onButtonEvent();

};