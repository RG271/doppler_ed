#include <QDateTime>
#include <QPainter>
#include <QTimer>
#include "animatorwidget.h"


// Paint Event Handler
void AnimatorWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    qint64 t = QDateTime::currentMSecsSinceEpoch();
    int dt = int(t - m_t);
    m_t = t;
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    m_animator->paint(&painter, width(), height(), dt);
}


// Constructor
AnimatorWidget::AnimatorWidget(QWidget *parent, Animator *animator) : QWidget(parent), m_animator(animator) {
    m_synth = new Synthesizer;
    m_t = QDateTime::currentMSecsSinceEpoch();
    setFixedSize(1800, 800);
    setAutoFillBackground(false);
    m_synth->start();   // !!!TEST
}


// Destructor
AnimatorWidget::~AnimatorWidget() {
    delete m_synth;
}


// Set Animation's State
// in: state = state (see Animator::setState())
void AnimatorWidget::setState(int state) {
    m_animator->setState(state);
    update();
}


// Animate
void AnimatorWidget::animate() {
    update();
    m_synth->generate();
}
