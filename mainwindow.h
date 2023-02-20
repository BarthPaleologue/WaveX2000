#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

   private:
    void displayClock();
    void displayDuration();
    void displayPower();
    void displayMode();
    void displayWeight();

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

    int hours{0};
    int minutes{0};
    QTimer* clockTimer{};

    QTimer* cookingTimer{};

    int duration{60};
    int power{100};
    int mode{0};
    int weight{50};
};
#endif  // MAINWINDOW_H
