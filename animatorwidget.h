#ifndef ANIMATORWIDGET_H
#define ANIMATORWIDGET_H

#include <QWidget>
#include "animator.h"
#include "synthesizer.h"


class AnimatorWidget : public QWidget {
    Q_OBJECT

    Animator *m_animator;   // pointer to animator object (not owned by this object)
    Synthesizer *m_synth;   // pointer to audio synthesizer object (owned by this object)
    qint64 m_t;             // time of last paint event (milliseconds since epoch)

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    AnimatorWidget(QWidget *parent, Animator *animator);
    AnimatorWidget(const AnimatorWidget &) = delete;
    AnimatorWidget &operator=(const AnimatorWidget &) = delete;
    ~AnimatorWidget();

    void setState(int state);

public slots:
    void animate();
};


#endif
