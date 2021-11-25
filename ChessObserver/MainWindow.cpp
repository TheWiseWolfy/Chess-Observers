
#include <iostream>
#include <opencv2\opencv.hpp>
#include "MainWindow.h"

#include "ImageProcessing.h"

using namespace cv;

VideoCapture cap(0);
QLabel* imageLabel;

MainWindow::MainWindow(QWidget* parent){

    this->setFixedSize(500, 500);

    //Main button
    QVBoxLayout* mainLayout = new QVBoxLayout();
    //mainLayout->addWidget(label);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setAlignment(Qt::AlignCenter);
    this->setLayout(mainLayout);

    QPushButton* button = new QPushButton("Costi Button", this);
    button->setGeometry(150, 0, 200, 50);
    button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    mainLayout->addWidget(button);
 
    // Set up the model and configure the view...
    this->setWindowTitle(tr("Where are you costi butoon."));
    
    //Here we do a connec
    QObject::connect(button, &QPushButton::clicked, this, &MainWindow::onButtonEvent);


    VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "BAD";
    }

    imageLabel = new QLabel(this);
    imageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    imageLabel->setMinimumSize(200, 200);
    imageLabel->setMaximumSize(500, 500);

    imageLabel->setGeometry(0, 50, 500, 500);

    mainLayout->addWidget(imageLabel);

    //onButtonEvent();
    this->show();
}

MainWindow::~MainWindow(){


}

void MainWindow::onButtonEvent(){
   
    Mat img;
    cap >> img;

    Mat img2 = process(img);

    cvtColor(img2, img2, COLOR_BGR2RGB);

    QPixmap image = QPixmap::fromImage(QImage(img2.data, img2.cols, img2.rows, img2.step, QImage::Format_RGB888));


    imageLabel->setPixmap(image);


}

