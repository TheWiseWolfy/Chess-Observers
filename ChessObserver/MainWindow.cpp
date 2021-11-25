
#include <iostream>
#include <opencv2\opencv.hpp>
#include "MainWindow.h"

#include "ImageProcessing.h"

using namespace cv;

VideoCapture cap(0);
QLabel* imageLabel;

MainWindow::MainWindow(QWidget* parent){

    //Size of the window
    this->setFixedSize(500, 500);
    this->setWindowTitle(tr("Chess Observer"));

    //Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    //mainLayout->setAlignment(Qt::AlignCenter);
    this->setLayout(mainLayout);

    //Button 1
    QPushButton* button1 = new QPushButton("Take webcam screenshot", this);
    button1->setGeometry(100, 0, 200, 50);
    //button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    mainLayout->addWidget(button1);

    //Button 2
    QPushButton* button2 = new QPushButton("Open file", this);
    button2->setGeometry(300, 0, 200, 50);
    //button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    mainLayout->addWidget(button2);

 
    //Here we do a connec
    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow::takeScreenshot);
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow::readFileFromDisk);

    //Open up the webcam
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

void MainWindow::takeScreenshot(){
   
    Mat img;
    cap >> img;

    Mat img2 = process(img);

    cvtColor(img2, img2, COLOR_BGR2RGB);

    QPixmap image = QPixmap::fromImage(QImage(img2.data, img2.cols, img2.rows, img2.step, QImage::Format_RGB888));

    imageLabel->setPixmap(image);
}

void MainWindow::readFileFromDisk() {

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        Mat img;
        String test = fileName.toStdString();
        img = imread(test);
        Mat img2 = process(img);
        cvtColor(img2, img2, COLOR_BGR2RGB);
        QPixmap image = QPixmap::fromImage(QImage(img2.data, img2.cols, img2.rows, img2.step, QImage::Format_RGB888));

        imageLabel->setPixmap(image);
    }
}
