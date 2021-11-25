#include "MainWindow.h"
#include <QtWidgets>

#include <iostream>
#include "Interface.h"

Interface::Interface(int argc, char* argv[]) {

    QApplication app(argc, argv);

    QWidget window;

    //Main button
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QPushButton* button = new QPushButton("Costi Button");

    mainLayout->addWidget(button);
    window.setLayout(mainLayout);

    // Set up the model and configure the view...
    window.setWindowTitle(
        QApplication::translate("nestedlayouts", "Nested layouts"));

    //connect(button, &QPushButton::released, this, &event );

    window.show();

    app.exec();
}