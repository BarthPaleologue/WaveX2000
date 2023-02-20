#include "mainwindow.h"

#include <QtUiTools/QtUiTools>
#include <iostream>

#include "Transitions.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    cookIndicator = findChild<QLabel *>("cookIndicator");

    clockButton = findChild<QPushButton *>("clockButton");
    powerButton = findChild<QPushButton *>("powerButton");

    modeButton = findChild<QPushButton *>("modeButton");
    defrostButton = findChild<QPushButton *>("defrostButton");

    startButton = findChild<QPushButton *>("startButton");
    stopButton = findChild<QPushButton *>("stopButton");

    stateMachine = new QStateMachine();

    auto idleState = new QState();
    stateMachine->addState(idleState);

    auto clockEditState = new QState();
    stateMachine->addState(clockEditState);
    addTrans(idleState, clockEditState, clockButton, SIGNAL(clicked()), this, SLOT(setClockEditState()));
    addTrans(clockEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    auto cookingState = new QState();
    stateMachine->addState(cookingState);
    addTrans(idleState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));
    addTrans(cookingState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    auto defrostState = new QState();
    stateMachine->addState(defrostState);
    addTrans(idleState, defrostState, defrostButton, SIGNAL(clicked()), this, SLOT(setDefrost()));
    addTrans(defrostState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(defrostState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    auto powerEditState = new QState();
    stateMachine->addState(powerEditState);
    addTrans(idleState, powerEditState, powerButton, SIGNAL(clicked()), this, SLOT(setPowerEdit()));
    addTrans(powerEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(powerEditState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    stateMachine->setInitialState(idleState);
    stateMachine->start();
}

void MainWindow::setIdle() {
    // TODO:
    std::cout << "Set state to Idle" << std::endl;
    cookIndicator->setText("Idle");
}

void MainWindow::setClockEditState() {
    // TODO:
    std::cout << "Set state to ClockEdit" << std::endl;
}

void MainWindow::setCooking() {
    // TODO:
    std::cout << "Set state to Cooking" << std::endl;
    cookIndicator->setText("Cooking");
}

void MainWindow::setDefrost() {
    // TODO:
    std::cout << "Set state to Defrost" << std::endl;
    cookIndicator->setText("Defrost");
}

void MainWindow::setPowerEdit() {
    // TODO:
    std::cout << "Set state to PowerEdit" << std::endl;
}

MainWindow::~MainWindow() {
    delete ui;
}
