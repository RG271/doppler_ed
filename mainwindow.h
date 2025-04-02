#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "animatorwidget.h"
#include "animator.h"
#include "synthesizer.h"

#define APP_TITLE       "Doppler Effect Simulation"
#define APP_BUILD_DATE  "30 March 2025"
#define APP_COPYRIGHT   "&copy;2025 Photonics Ed"
#define APP_VERSION     "1.00"
#define APP_QT_VERSION  "Open Source Qt 6.8.3"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Animator m_animator;
    AnimatorWidget *m_animatorWidget;
    Synthesizer m_synth;

private slots:
    void on_actionAbout_triggered(bool checked);
    void on_actionSiren_triggered(bool checked);
    void on_state0Btn_clicked();
    void on_state1Btn_clicked();
    void on_state2Btn_clicked();
    void on_state3Btn_clicked();
    void on_state4Btn_clicked();
};


#endif // MAINWINDOW_H
