#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDial>
#include <QGraphicsPixmapItem>
#include <QLCDNumber>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>
#include <QStateMachine>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace Mode {
enum Mode {
    MICROWAVE = 0,
    GRILL = 1,
    MICROWAVE_GRILL = 2,
};
}

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
   public slots:
    void setIdle();
    void setClockHourEditState();
    void setClockMinuteEditState();
    void setCooking();
    void setDurationEdit();
    void setDefrost();
    void setPowerEdit();
    void setModeEdit();
    void updateClock();
    void decreaseDuration();

    void increaseDuration30();

    void pulseClock();

    void manageDial(int value);

    void rumble();

   private:
    static const int DEFAULT_DURATION = 60;
    static const int DEFAULT_POWER = 100;
    static const int DEFAULT_MODE = Mode::Mode::MICROWAVE;
    static const int DEFAULT_WEIGHT = 50;

    void displayClock();
    void displayDuration();
    void displayPower();
    void displayMode();
    void displayWeight();

    void resetStateMachine();

    Ui::MainWindow* ui;

    QStateMachine* stateMachine{};
    QState* idleState{};
    QState* notIdleState{};

    QState* cookingState{};

    QState* editHoursState{};
    QState* editMinutesState{};

    QState* durationEditState{};
    QState* powerEditState{};
    QState* modeEditState{};
    QState* defrostState{};

    QLabel* cookIndicator{};

    QPushButton* clockButton{};
    QPushButton* powerButton{};

    QPushButton* modeButton{};
    QPushButton* defrostButton{};

    QPushButton* startButton{};
    QPushButton* stopButton{};

    QLCDNumber* clockDisplay{};

    QDial* dial{};

    int hours{0};
    int minutes{0};
    QTimer* clockTimer{};
    QTimer* pulseTimer{};
    int pulseHourParity{0};
    int pulseMinuteParity{0};

    QTimer* cookingTimer{};

    QTimer* rumbleTimer{};

    bool isFinishedCooking{true};

    int duration{DEFAULT_DURATION};
    int power{DEFAULT_POWER};
    int mode{DEFAULT_MODE};
    int weight{DEFAULT_WEIGHT};

    QGraphicsPixmapItem* idleGrid{};
    QGraphicsPixmapItem* cookingGrid{};
};
#endif  // MAINWINDOW_H
