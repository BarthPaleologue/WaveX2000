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

    clockDisplay = findChild<QLCDNumber *>("clockDisplay");

    stateMachine = new QStateMachine();

    clockTimer = new QTimer(this);
    clockTimer->setInterval(60 * 1000);  // 1 minute
    connect(clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));
    clockTimer->start();

    idleState = new QState();
    stateMachine->addState(idleState);

    auto cookingState = new QState();
    stateMachine->addState(cookingState);
    addTrans(idleState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));
    addTrans(cookingState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(cookingState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    auto durationEditState = new QState();
    stateMachine->addState(durationEditState);
    addTrans(durationEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(durationEditState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    ////////////////////// Clock Edit State //////////////////////

    auto clockEditState = new QState();
    stateMachine->addState(clockEditState);
    addTrans(idleState, clockEditState, clockButton, SIGNAL(clicked()), this, SLOT(setClockEditState()));
    addTrans(clockEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    // add substates to the clockEditState (edit hours, edit minutes)
    auto editHoursState = new QState(clockEditState);
    auto editMinutesState = new QState(clockEditState);

    // add transitions to the substates
    addTrans(editHoursState, editMinutesState, clockButton, SIGNAL(clicked()), this, SLOT(setClockMinuteEditState()));
    addTrans(editMinutesState, idleState, clockButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    // the default state is editHoursState
    clockEditState->setInitialState(editHoursState);

    ///////////////////////////////////////////////////////////////

    auto defrostState = new QState();
    stateMachine->addState(defrostState);
    addTrans(idleState, defrostState, defrostButton, SIGNAL(clicked()), this, SLOT(setDefrost()));
    addTrans(defrostState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(defrostState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    auto powerEditState = new QState();
    stateMachine->addState(powerEditState);
    addTrans(idleState, powerEditState, powerButton, SIGNAL(clicked()), this, SLOT(setPowerEdit()));
    addTrans(powerEditState, durationEditState, powerButton, SIGNAL(clicked()), this, SLOT(setDurationEdit()));
    addTrans(powerEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    auto modeEditState = new QState();
    stateMachine->addState(modeEditState);
    addTrans(idleState, modeEditState, modeButton, SIGNAL(clicked()), this, SLOT(setModeEdit()));
    addTrans(modeEditState, durationEditState, modeButton, SIGNAL(clicked()), this, SLOT(setDurationEdit()));
    addTrans(modeEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    stateMachine->setInitialState(idleState);
    setIdle();
    stateMachine->start();
}

void MainWindow::setIdle() {
    // TODO:
    std::cout << "Set state to Idle" << std::endl;
    cookIndicator->setText("Idle");

    // display the current time stored in this.hours and this.minutes
    displayClock();
}

void MainWindow::setClockEditState() {
    // TODO:
    std::cout << "Set state to ClockEdit (Hours)" << std::endl;

    displayClock();
}

void MainWindow::setClockMinuteEditState() {
    // TODO:
    std::cout << "Set state to ClockEdit (Minutes)" << std::endl;

    displayClock();
}

void MainWindow::setCooking() {
    // TODO:
    // Starts cooking and decrements the time
    std::cout << "Set state to Cooking" << std::endl;
    cookIndicator->setText("Cooking");

    displayDuration();
}

void MainWindow::setDurationEdit() {
    // TODO:
    std::cout << "Set state to DurationEdit" << std::endl;

    displayDuration();
}

void MainWindow::setDefrost() {
    // TODO:
    // The user will enter the weight of the food to be defrosted
    // The time will be calculated based on the weight
    std::cout << "Set state to Defrost" << std::endl;
    cookIndicator->setText("Defrost");

    displayWeight();
}

void MainWindow::setPowerEdit() {
    // TODO:
    std::cout << "Set state to PowerEdit" << std::endl;

    displayPower();
}

void MainWindow::setModeEdit() {
    // TODO:
    std::cout << "Set state to ModeEdit" << std::endl;

    displayMode();
}

void MainWindow::updateClock() {
    std::cout << "Update clock" << std::endl;
    minutes++;
    if (minutes == 60) {
        minutes = 0;
        hours++;
        if (hours == 24) {
            hours = 0;
        }
    }
    // if current state is idle, update the clock display
    if (stateMachine->configuration().contains(idleState)) {
        displayClock();
    }
}

void MainWindow::displayClock() {
    clockDisplay->display(QString("%1:%2").arg(this->hours, 2, 10, QChar('0')).arg(this->minutes, 2, 10, QChar('0')));
}

void MainWindow::displayDuration() {
    // display the remaining duration
    clockDisplay->display(QString("%1").arg(this->duration, 2, 10, QChar('0')));
}

void MainWindow::displayPower() {
    // display the power level
    clockDisplay->display(QString("%1").arg(this->power, 2, 10, QChar('0')));
}

void MainWindow::displayMode() {
    // display the mode
    clockDisplay->display(QString("%1").arg(this->mode, 2, 10, QChar('0')));
}

void MainWindow::displayWeight() {
    // display the weight
    clockDisplay->display(QString("%1").arg(this->weight, 2, 10, QChar('0')));
}

MainWindow::~MainWindow() {
    delete ui;
}
