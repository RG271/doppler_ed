#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QPixmap>
#include <QWidget>


class Animator {
    static constexpr int MAX_CIRCLES = 100;

    struct Circle {
        bool valid;   // valid flag: true iff x, y, and r are valid
        int x, y;     // center point
        float r;      // radius (>=0.0f)
    };

    Circle m_circle[MAX_CIRCLES];   // siren's sound waves

    QPixmap m_backgroundPixmap,
            m_ambulancePixmap,
            m_personListeningPixmap,
            m_personWaitingPixmap;
    QBrush m_background;
    QFont m_textFont;
    QPen m_circlePen[4];    // pens of thicknesses 3, 2, and 1 pixels for drawing siren pulses
    QPen m_textPen;

    float m_ambXPosMin,      // minimum m_ambXPos. This will hide the ambulance past window's left edge.
          m_ambXPosMax,      // maximum m_ambXPos. This will hide the ambulance past window's right edge.
          m_ambXPos,         // X-coord. position of center of ambulance's siren [m_ambXPosMin, m_ambXPosMax] (pixels)
          m_ambXVel,         // X-coord. velocity of ambulance (pixels/millisecond; typically 0.01f to 0.10f)
          m_soundSpeed;      // animated sound's speed (pixels/millisecond; typically 0.01f to 0.10f)

    int m_ambSirenTmrTop,    // siren sound wave timer's top in milliseconds (>0; typically 500..1500)
        m_ambSirenTmr;       // siren sound wave timer (0 .. m_pulseTmrTop-1 milliseconds)

    bool m_ambSirenOn,       // siren enable: true = ON, false = OFF
         m_personListening;  // person is listening flag, which simply selects between two images

    static bool inside(int xc, int yc, float rc, int x, int y);

public:
    Animator();
    void paint(QPainter *painter, int w, int h, int dt);
    void setState(int state);
};


#endif
