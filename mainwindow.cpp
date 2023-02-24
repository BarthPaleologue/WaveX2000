#include "mainwindow.h"

#include <QtUiTools/QtUiTools>
#include <iostream>

#include "Transitions.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setFixedSize(size());

    cookIndicator = findChild<QLabel *>("cookIndicator");

    clockButton = findChild<QPushButton *>("clockButton");
    powerButton = findChild<QPushButton *>("powerButton");

    modeButton = findChild<QPushButton *>("modeButton");
    defrostButton = findChild<QPushButton *>("defrostButton");

    startButton = findChild<QPushButton *>("startButton");
    stopButton = findChild<QPushButton *>("stopButton");

    clockDisplay = findChild<QLCDNumber *>("clockDisplay");

    dial = findChild<QDial *>("dial");
    connect(dial, SIGNAL(valueChanged(int)), this, SLOT(manageDial(int)));

    stateMachine = new QStateMachine();

    clockTimer = new QTimer(this);
    clockTimer->setInterval(60 * 1000);  // 1 minute
    connect(clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));
    clockTimer->start();

    pulseTimer = new QTimer(this);
    pulseTimer->setInterval(1000);  // 1 second
    connect(pulseTimer, SIGNAL(timeout()), this, SLOT(pulseClock()));
    pulseTimer->start();

    cookingTimer = new QTimer(this);
    cookingTimer->setInterval(1000);  // 1 second
    connect(cookingTimer, SIGNAL(timeout()), this, SLOT(decreaseDuration()));

    idleState = new QState();
    stateMachine->addState(idleState);

    cookingState = new QState();
    stateMachine->addState(cookingState);
    addTrans(idleState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));
    addTrans(cookingState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(cookingState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    durationEditState = new QState();
    stateMachine->addState(durationEditState);
    addTrans(durationEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(durationEditState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    ////////////////////// Clock Edit State //////////////////////

    clockEditState = new QState();
    stateMachine->addState(clockEditState);
    addTrans(idleState, clockEditState, clockButton, SIGNAL(clicked()), this, SLOT(setClockEditState()));
    addTrans(clockEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    // add substates to the clockEditState (edit hours, edit minutes)
    editHoursState = new QState(clockEditState);
    editMinutesState = new QState(clockEditState);

    // add transitions to the substates
    addTrans(editHoursState, editMinutesState, clockButton, SIGNAL(clicked()), this, SLOT(setClockMinuteEditState()));
    addTrans(editMinutesState, idleState, clockButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    // the default state is editHoursState
    clockEditState->setInitialState(editHoursState);

    ///////////////////////////////////////////////////////////////

    defrostState = new QState();
    stateMachine->addState(defrostState);
    addTrans(idleState, defrostState, defrostButton, SIGNAL(clicked()), this, SLOT(setDefrost()));
    addTrans(defrostState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));
    addTrans(defrostState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    powerEditState = new QState();
    stateMachine->addState(powerEditState);
    addTrans(idleState, powerEditState, powerButton, SIGNAL(clicked()), this, SLOT(setPowerEdit()));
    addTrans(powerEditState, durationEditState, powerButton, SIGNAL(clicked()), this, SLOT(setDurationEdit()));
    addTrans(powerEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    modeEditState = new QState();
    stateMachine->addState(modeEditState);
    addTrans(idleState, modeEditState, modeButton, SIGNAL(clicked()), this, SLOT(setModeEdit()));
    addTrans(modeEditState, durationEditState, modeButton, SIGNAL(clicked()), this, SLOT(setDurationEdit()));
    addTrans(modeEditState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    stateMachine->setInitialState(idleState);
    setIdle();
    stateMachine->start();
}

void MainWindow::setIdle() {
    std::cout << "Set state to Idle" << std::endl;
    cookIndicator->setStyleSheet("background-color: #000000;");

    cookingTimer->stop();

    duration = MainWindow::DEFAULT_DURATION;
    power = MainWindow::DEFAULT_POWER;
    mode = MainWindow::DEFAULT_MODE;
    weight = MainWindow::DEFAULT_WEIGHT;

    // display the current time stored in this.hours and this.minutes
    displayClock();
}

void MainWindow::setClockEditState() {
    std::cout << "Set state to ClockEdit (Hours)" << std::endl;

    displayClock();
}

void MainWindow::setClockMinuteEditState() {
    std::cout << "Set state to ClockEdit (Minutes)" << std::endl;

    displayClock();
}

void MainWindow::setCooking() {
    // Starts cooking and decrements the time
    std::cout << "Set state to Cooking" << std::endl;
    cookIndicator->setStyleSheet("background-color: #ffff00;");

    cookingTimer->start();

    displayDuration();
}

void MainWindow::setDurationEdit() {
    std::cout << "Set state to DurationEdit" << std::endl;
    displayDuration();
}

void MainWindow::setDefrost() {
    // The user will enter the weight of the food to be defrosted
    // The time will be calculated based on the weight
    std::cout << "Set state to Defrost" << std::endl;
    cookIndicator->setStyleSheet("background-color: #0000ff;");
    displayWeight();
}

void MainWindow::setPowerEdit() {
    std::cout << "Set state to PowerEdit" << std::endl;
    displayPower();
}

void MainWindow::setModeEdit() {
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

void MainWindow::decreaseDuration() {
    std::cout << "Decrease duration" << std::endl;
    duration--;
    if (duration == 0) {
        cookingTimer->stop();

        resetStateMachine();

        return;
    }

    displayDuration();
}

void MainWindow::pulseClock() {
    // if current state is clockEditHours, pulse the hours
    // if current state is clockEditMinutes, pulse the minutes
    if (stateMachine->configuration().contains(editHoursState)) {
        pulseHourParity = pulseHourParity == 0 ? 1 : 0;
        if (pulseHourParity) {
            clockDisplay->display(QString("--:%1").arg(this->minutes, 2, 10, QChar('0')));
        } else {
            clockDisplay->display(QString("%1:%2").arg(this->hours, 2, 10, QChar('0')).arg(this->minutes, 2, 10, QChar('0')));
        }
    } else if (stateMachine->configuration().contains(editMinutesState)) {
        pulseMinuteParity = pulseMinuteParity == 0 ? 1 : 0;
        if (pulseMinuteParity) {
            clockDisplay->display(QString("%1:--").arg(this->hours, 2, 10, QChar('0')));
        } else {
            clockDisplay->display(QString("%1:%2").arg(this->hours, 2, 10, QChar('0')).arg(this->minutes, 2, 10, QChar('0')));
        }
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

void MainWindow::resetStateMachine() {
    // reset state machine
    stateMachine->stop();
    stateMachine->setInitialState(idleState);
    stateMachine->start();
    setIdle();
}

void MainWindow::manageDial(int value) {
    std::cout << "Dial value: " << value << std::endl;

    // if the current state is clockEditState, update the hours
    if (stateMachine->configuration().contains(editHoursState)) {
        std::cout << "Update hours" << std::endl;
        hours = int(value / 4) % 24;
        displayClock();
    } else if (stateMachine->configuration().contains(editMinutesState)) {
        std::cout << "Update minutes" << std::endl;
        minutes = value % 60;
        displayClock();
    } else if (stateMachine->configuration().contains(durationEditState)) {
        std::cout << "Update duration" << std::endl;
        duration = value;
        displayDuration();
    } else if (stateMachine->configuration().contains(powerEditState)) {
        std::cout << "Update power" << std::endl;
        power = 10 + 10 * int(value / 10) % 100;
        displayPower();
    } else if (stateMachine->configuration().contains(modeEditState)) {
        std::cout << "Update mode" << std::endl;
        mode = 1 + value % 2;
        displayMode();
    } else if (stateMachine->configuration().contains(defrostState)) {
        std::cout << "Update weight" << std::endl;
        weight = value % 1000;
        displayWeight();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
