#include <iostream>
#include <opencv2\opencv.hpp>
#include "MainWindow.h"

#include "ImageProcessing.h"

using namespace cv;

VideoCapture cap(0);
QLabel* imageLabel;
Mat img;
Mat processedImg;

//Our sins
int maximumThreshold = 200;
int minimumThreshold = 100;

MainWindow::MainWindow(QWidget* parent) {

    //Size of the window
    this->setFixedSize(1000, 1000);
    this->setWindowTitle(tr("Chess Observer"));

    //Main layout
    //QVBoxLayout* mainLayout = new QVBoxLayout();
   // mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    //mainLayout->setAlignment(Qt::AlignCenter);
    //this->setLayout(mainLayout);

    //Button 1
    QPushButton* button1 = new QPushButton("Take webcam screenshot", this);
    button1->setGeometry(100, 0, 200, 50);
    //button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    //mainLayout->addWidget(button1);

    //Button 2
    QPushButton* button2 = new QPushButton("Open file", this);
    button2->setGeometry(300, 0, 200, 50);
    //button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    //mainLayout->addWidget(button2);

    //Some funky sliders

    QSlider* sliderMax = new QSlider(Qt::Horizontal, this);
    //slider->setFocusPolicy(Qt::StrongFocus);
    sliderMax->setMinimum(0);
    sliderMax->setMaximum(300);
    //slider->setTickPosition(QSlider::TicksBothSides);
    //slider->setTickInterval(10);
    //slider->setSingleStep(1);
    sliderMax->setGeometry(500, 0, 200, 50);

    QSlider* sliderMin = new QSlider(Qt::Horizontal, this);
    sliderMin->setMinimum(0);
    sliderMin->setMaximum(300);
    sliderMin->setGeometry(700, 0, 200, 50);


    //mainLayout->addWidget(slider);

    //Here we do a connec
    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow::takeScreenshot);
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow::readFileFromDisk);
    QObject::connect(sliderMax, &QSlider::valueChanged, this, &MainWindow::sliderMaximumSetValue);
    QObject::connect(sliderMin, &QSlider::valueChanged, this, &MainWindow::sliderMinimumSetValue);


    //Open up the webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "BAD";
    }

    imageLabel = new QLabel(this);
    imageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    imageLabel->setMinimumSize(200, 200);
    imageLabel->setMaximumSize(1000, 1000);
    imageLabel->setGeometry(0, 50, 1000, 1000);

    //mainLayout->addWidget(imageLabel);

    this->show();
}

MainWindow::~MainWindow() {


}

void MainWindow::takeScreenshot() {

    cap >> img;

    processImage();

    displayImage();
}

void MainWindow::readFileFromDisk() {

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        String test = fileName.toStdString();
        img = imread(test);

        processImage();

        displayImage();
    }
}

void MainWindow::sliderMaximumSetValue(int value){
    maximumThreshold = value;
    processImage();
}

void MainWindow::sliderMinimumSetValue(int value) {
    minimumThreshold = value;
    processImage();
}

void MainWindow::processImage() {

    if (!img.empty()) {
        Mat resized = Mat(Size(600 * (img.cols / img.rows), 600) , CV_8UC1);
        cv::resize(img, resized, Size(600 * (img.cols / img.rows), 600), INTER_LINEAR);
        Mat img2 = transformImage(resized, minimumThreshold, maximumThreshold);
        //Mat img3 = findLines(img2);
        //processedImg = img3; 
    }
}

void MainWindow::displayImage()
{
    cvtColor(processedImg, processedImg, COLOR_BGR2RGB);
    QPixmap image = QPixmap::fromImage(QImage(processedImg.data, processedImg.cols, processedImg.rows, processedImg.step, QImage::Format_RGB888));

    imageLabel->setPixmap(image);
}
