#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDial>
#include <QLCDNumber>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStateMachine>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
   public slots:
    void setIdle();
    void setClockEditState();
    void setClockMinuteEditState();
    void setCooking();
    void setDurationEdit();
    void setDefrost();
    void setPowerEdit();
    void setModeEdit();
    void updateClock();
    void decreaseDuration();

    void pulseHour();
    void pulseMinute();

    void manageDial(int value);

   private:
    static const int DEFAULT_DURATION = 60;

    void displayClock();
    void displayDuration();
    void displayPower();
    void displayMode();
    void displayWeight();

    void resetStateMachine();

    Ui::MainWindow* ui;

    QStateMachine* stateMachine{};
    QState* idleState{};
    QState* cookingState{};

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
    QTimer* pulseHourTimer{};
    int pulseHourParity{0};
    QTimer* pulseMinuteTimer{};
    int pulseMinuteParity{0};

    QTimer* cookingTimer{};

    int duration{DEFAULT_DURATION};
    int power{100};
    int mode{0};
    int weight{50};
};
#endif  // MAINWINDOW_H
