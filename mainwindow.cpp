#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtUiTools/QtUiTools>
#include <iostream>
#include "Transitions.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clockButton = findChild<QPushButton*>("clockButton");
    powerButton = findChild<QPushButton*>("powerButton");

    modeButton = findChild<QPushButton*>("modeButton");
    defrostButton = findChild<QPushButton*>("defrostButton");

    startButton = findChild<QPushButton*>("startButton");
    stopButton = findChild<QPushButton*>("stopButton");

    connect(clockButton, &QPushButton::clicked, [=]() {std::cout << "!" << std::endl;});
}

MainWindow::~MainWindow()
{
    delete ui;
}

