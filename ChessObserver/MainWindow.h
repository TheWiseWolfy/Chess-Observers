#pragma once

#include <QtWidgets>
#include <qslider.h>

class MainWindow : public QMainWindow {
	Q_OBJECT
public:

	MainWindow(QWidget* parent = Q_NULLPTR);

	~MainWindow();

	//Interface buttons
	void takeScreenshot();
	void readFileFromDisk();
	void sliderMaximumSetValue(int value);
	void sliderMinimumSetValue(int value);

	//Local fuctions
	void processImage();
	void displayImage();
};