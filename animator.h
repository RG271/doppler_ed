#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include "synthesizer.h"


class Animator {
    static constexpr int MAX_CIRCLES = 50,              // max. number of sound-wave circles
                         AMBULANCE_Y = 77,              // Y-coord. of ambulance image's top in the client area
                         SIREN_DX = 142,                // X-coord. of center of siren relative to ambulance image's upper-left corner
                         SIREN_DY = 9,                  // Y-coord. of center of siren relative to ambulance image's upper-left corner
                         PERSON_X = 0,                  // person image's horizontal center in the client area
                         PERSON_Y = 225,                // person image's top in the client area
                         PERSON_EAR_X = PERSON_X + 23,  // X-coord. of center of person's ear in the client area
                         PERSON_EAR_Y = PERSON_Y + 15;  // Y-coord. of center of person's ear in the client area


    struct Circle {                // A circle representing a sound wave
        bool valid;                  // valid flag: true iff x, y, and r are valid
        int x, y;                    // center point
        float r;                     // radius (>=0.0f)
    };

    Circle m_circle[MAX_CIRCLES];  // siren's sound waves

    QPixmap m_backgroundPixmap,
            m_ambulancePixmap,
            m_personListeningPixmap,
            m_personWaitingPixmap;
    QBrush m_background;
    QPen m_circlePen[4];     // pens of thicknesses 3, 2, and 1 pixels for drawing siren pulses

    float m_ambXPosMin,      // minimum m_ambXPos. This will hide the ambulance past window's left edge.
          m_ambXPosMax,      // maximum m_ambXPos. This will hide the ambulance past window's right edge.
          m_ambXPos,         // X-coord. position of center of ambulance's siren [m_ambXPosMin, m_ambXPosMax] (pixels)
          m_ambXVel,         // X-coord. velocity of ambulance (pixels/millisecond; typically 0.01f to 0.10f)
          m_soundSpeed;      // animated sound's speed (pixels/millisecond; typically 0.01f to 0.10f)

    int m_ambSirenTmrTop,    // siren sound wave timer's top in milliseconds (>0; typically 500..1500)
        m_ambSirenTmr,       // siren sound wave timer (0 .. m_pulseTmrTop-1 milliseconds)
        m_t;                 // time in milliseconds since animation started (0..999999)

    bool m_ambSirenOn;       // siren enable: true = ON, false = OFF

    static bool inside(int xc, int yc, float rc, int x, int y);

public:
    Animator();
    void paint(QPainter *painter, int w, int h, int dt, Synthesizer *synth);
    void setState(int state);
};


#endif
