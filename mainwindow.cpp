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

    idleGrid = new QGraphicsPixmapItem(QPixmap(":/grids/gridIdle.jpg"));
    idleGrid->setTransformationMode(Qt::SmoothTransformation);

    cookingGrid = new QGraphicsPixmapItem(QPixmap(":/grids/gridRunning.jpg"));
    cookingGrid->setTransformationMode(Qt::SmoothTransformation);

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

    rumbleTimer = new QTimer(this);
    rumbleTimer->setInterval(16);
    connect(rumbleTimer, SIGNAL(timeout()), this, SLOT(rumble()));
    rumbleTimer->start();

    idleState = new QState();
    stateMachine->addState(idleState);

    notIdleState = new QState();
    stateMachine->addState(notIdleState);
    addTrans(notIdleState, idleState, stopButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    cookingState = new QState(notIdleState);
    stateMachine->addState(cookingState);
    addTrans(cookingState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(increaseDuration30()));
    addTrans(idleState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));
    addTrans(isFinishedCooking, cookingState, idleState, cookingTimer, SIGNAL(timeout()), this, SLOT(setIdle()));

    durationEditState = new QState(notIdleState);
    stateMachine->addState(durationEditState);
    addTrans(durationEditState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    editHoursState = new QState(notIdleState);
    stateMachine->addState(editHoursState);
    addTrans(idleState, editHoursState, clockButton, SIGNAL(clicked()), this, SLOT(setClockHourEditState()));
    addTrans(editHoursState, editMinutesState, clockButton, SIGNAL(clicked()), this, SLOT(setClockMinuteEditState()));

    editMinutesState = new QState(notIdleState);
    stateMachine->addState(editMinutesState);
    addTrans(editHoursState, editMinutesState, clockButton, SIGNAL(clicked()), this, SLOT(setClockMinuteEditState()));
    addTrans(editMinutesState, idleState, clockButton, SIGNAL(clicked()), this, SLOT(setIdle()));

    defrostState = new QState(notIdleState);
    stateMachine->addState(defrostState);
    addTrans(idleState, defrostState, defrostButton, SIGNAL(clicked()), this, SLOT(setDefrost()));
    addTrans(defrostState, cookingState, startButton, SIGNAL(clicked()), this, SLOT(setCooking()));

    powerEditState = new QState(notIdleState);
    stateMachine->addState(powerEditState);
    addTrans(idleState, powerEditState, powerButton, SIGNAL(clicked()), this, SLOT(setPowerEdit()));
    addTrans(powerEditState, durationEditState, powerButton, SIGNAL(clicked()), this, SLOT(setDurationEdit()));

    modeEditState = new QState(notIdleState);
    stateMachine->addState(modeEditState);
    addTrans(idleState, modeEditState, modeButton, SIGNAL(clicked()), this, SLOT(setModeEdit()));
    addTrans(modeEditState, durationEditState, modeButton, SIGNAL(clicked()), this, SLOT(setDurationEdit()));

    stateMachine->setInitialState(idleState);
    setIdle();
    stateMachine->start();
}

void MainWindow::setIdle() {
    std::cout << "Set state to Idle" << std::endl;

    cookingTimer->stop();

    cookIndicator->setPixmap(idleGrid->pixmap());
    cookIndicator->setScaledContents(true);

    duration = MainWindow::DEFAULT_DURATION;
    power = MainWindow::DEFAULT_POWER;
    mode = MainWindow::DEFAULT_MODE;
    weight = MainWindow::DEFAULT_WEIGHT;

    // display the current time stored in this.hours and this.minutes
    displayClock();
}

void MainWindow::setClockHourEditState() {
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
    cookingTimer->start();

    isFinishedCooking = false;

    cookIndicator->setPixmap(cookingGrid->pixmap());
    cookIndicator->setScaledContents(true);

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
        isFinishedCooking = true;
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
    // display the remaining duration in minutes and seconds
    int minutes = this->duration / 60;
    int seconds = this->duration % 60;
    clockDisplay->display(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
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
        duration = int(value / 10) * 30;
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

void MainWindow::increaseDuration30() {
    if (!isFinishedCooking) {
        duration += 30;
        displayDuration();
    }
}

void MainWindow::rumble() {
    if (!cookingTimer->isActive()) return;
    // move window by random x and y
    int x = rand() % 3 - 1;
    int y = rand() % 3 - 1;
    this->move(this->x() + x, this->y() + y);
}

MainWindow::~MainWindow() {
    delete ui;
}
