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
int offset = 0;

MainWindow::MainWindow(QWidget* parent) {

    //Size of the window
    this->setFixedSize(1000, 1000);
    this->setWindowTitle(tr("Chess Observer"));


    //Button 1
    QPushButton* button1 = new QPushButton("Take webcam screenshot", this);
    button1->setGeometry(100, 0, 200, 50);

    //Button 2
    QPushButton* button2 = new QPushButton("Open file", this);
    button2->setGeometry(300, 0, 200, 50);

    //Button 3
    QPushButton* button3 = new QPushButton("Recalculate", this);
    button3->setGeometry(100, 50, 200, 50);

    //Some funky sliders

    QSlider* sliderMax = new QSlider(Qt::Horizontal, this);
    sliderMax->setMinimum(0);
    sliderMax->setMaximum(300);
    sliderMax->setGeometry(500, 0, 200, 50);

    QSlider* sliderMin = new QSlider(Qt::Horizontal, this);
    sliderMin->setMinimum(0);
    sliderMin->setMaximum(300);
    sliderMin->setGeometry(700, 0, 200, 50);

    QSlider* sliderOffset = new QSlider(Qt::Horizontal, this);
    sliderOffset->setMinimum(0);
    sliderOffset->setMaximum(300);
    sliderOffset->setGeometry(700, 50, 200, 50);

    //Here we do a connec
    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow::takeScreenshot);
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow::readFileFromDisk);
    QObject::connect(button3, &QPushButton::clicked, this, &MainWindow::recalculate);

    QObject::connect(sliderMax, &QSlider::valueChanged, this, &MainWindow::sliderMaximumSetValue);
    QObject::connect(sliderMin, &QSlider::valueChanged, this, &MainWindow::sliderMinimumSetValue);
    QObject::connect(sliderOffset, &QSlider::valueChanged, this, &MainWindow::sliderOffsetSetValue);

    //Open up the webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "BAD";
    }

    imageLabel = new QLabel(this);
    imageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    imageLabel->setMinimumSize(200, 200);
    imageLabel->setMaximumSize(1000, 1000);
    imageLabel->setGeometry(0, 100, 1000, 1000);

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

void MainWindow::recalculate() {

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
}

void MainWindow::sliderMinimumSetValue(int value) {
    minimumThreshold = value;
}

void MainWindow::sliderOffsetSetValue(int value) {
    offset = value;
}

void MainWindow::processImage() {

    if (!img.empty()) {
        Mat resized = Mat(Size(600 , 600) , CV_8UC1);
        cv::resize(img, resized, Size(600 , 600), INTER_LINEAR);
        Mat img2 = transformImage(resized, minimumThreshold, maximumThreshold);
        
        vector<Vec2f> lines = findLines(img2);

        lines = lineRefinement(lines, img2);

        
        vector<Point> puncte = intersectLines( point2Lines(lines));

        Mat img3 = img2.clone();
        Mat img4 = img2.clone();
        Mat img5 = img2.clone();

        imshow("Pucte", displayPoints(puncte, img2));

        Polygon4 extremes = extremePoints(puncte, img2.size() , offset);

        vector<Point> extremeVector{ extremes.bottomLeft, extremes.bottomRight, extremes.topLeft, extremes.topRight };
        
        imshow("Pucte extreme", displayPoints(extremeVector, img3));

        Polygon4 grid[8][8] = { Polygon4() };

        gridMake(extremes, grid);


        vector<Point> aruncate;

        for(int x = 0 ; x < 8 ; x++)
            for(int y = 0 ; y < 8 ; y++)
            {
                aruncate.push_back(grid[x][y].topLeft);
                aruncate.push_back(grid[x][y].topRight);
                aruncate.push_back(grid[x][y].bottomLeft);
                aruncate.push_back(grid[x][y].bottomRight);
            }

        imshow("Gridare finala", displayPoints(aruncate, img4));
        
        Mat tiles[8][8];

        tileCutter(img5, grid, tiles);

        imshow("monke", tiles[0][0]);

        processedImg = img3;

        emptyTileFounder(tiles);
    }
}

void MainWindow::displayImage()
{
    cvtColor(processedImg, processedImg, COLOR_BGR2RGB);
    QPixmap image = QPixmap::fromImage(QImage(processedImg.data, processedImg.cols, processedImg.rows, processedImg.step, QImage::Format_RGB888));

    imageLabel->setPixmap(image);
}
