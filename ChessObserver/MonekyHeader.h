#pragma once


#include <QtWidgets/QMainWindow>
#include "ui_MainMonke.h"

class Ui_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui_MainWindow(QWidget* parent = Q_NULLPTR);

private:
    Ui::MainWindow ui;
};
