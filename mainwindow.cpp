#include "aboutdialog.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>


// Constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    m_animatorWidget = new AnimatorWidget(this, &m_animator);
    ui->innerAnimWidget->layout()->replaceWidget(ui->dummyLbl, m_animatorWidget);
    delete ui->dummyLbl;

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, m_animatorWidget, &AnimatorWidget::animate);
    timer->start(50);
}


// Destructor
MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_actionAbout_triggered(bool checked) {
    AboutDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_actionSiren_triggered(bool checked) {
    m_synth.start();
}


void MainWindow::on_state0Btn_clicked() { m_animatorWidget->setState(0); }
void MainWindow::on_state1Btn_clicked() { m_animatorWidget->setState(1); }
void MainWindow::on_state2Btn_clicked() { m_animatorWidget->setState(2); }
void MainWindow::on_state3Btn_clicked() { m_animatorWidget->setState(3); }
void MainWindow::on_state4Btn_clicked() { m_animatorWidget->setState(4); }
