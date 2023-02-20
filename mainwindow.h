#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStateMachine>

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

   private:
    Ui::MainWindow* ui;

    QStateMachine* stateMachine{};

    QLabel* cookIndicator{};

    QPushButton* clockButton{};
    QPushButton* powerButton{};

    QPushButton* modeButton{};
    QPushButton* defrostButton{};

    QPushButton* startButton{};
    QPushButton* stopButton{};
};
#endif  // MAINWINDOW_H
