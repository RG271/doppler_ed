#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <cmath>
#include "animator.h"


// Test if a Point is Inside a Circle
// in: xc, yc = circle's center
//     rc = circle's radius
//     x, y = a point
bool Animator::inside(int xc, int yc, float rc, int x, int y) {
    float dx = x - xc,
          dy = y - yc;
    return  dx*dx + dy*dy <= rc*rc;
}


Animator::Animator() {
    // Load images
    QString imgDir = R"(img\)";
    if (!m_backgroundPixmap.load(imgDir + "background_1800x800.jpg"))  qDebug() << "ERROR: Cannot load the background image";
    if (!m_ambulancePixmap.load(imgDir + "ambulance_207x112.png"))  qDebug() << "ERROR: Cannot load the ambulance image";
    if (!m_personListeningPixmap.load(imgDir + "person_listening_64x100.png"))  qDebug() << "ERROR: Cannot load the person listening image";
    if (!m_personWaitingPixmap.load(imgDir + "person_waiting_64x100.png"))  qDebug() << "ERROR: Cannot load the person waiting image";

    // Create brushes, pens, and fonts
    m_background = QBrush(QColor(0xF4F4F4));
    m_circlePen[0] = QPen(Qt::blue);  m_circlePen[0].setWidth(3);
    m_circlePen[1] = QPen(Qt::blue);  m_circlePen[1].setWidth(2);
    m_circlePen[2] = QPen(Qt::blue);  m_circlePen[2].setWidth(1);
    m_circlePen[3] = QPen(Qt::blue);  m_circlePen[3].setWidth(1);  m_circlePen[3].setStyle(Qt::PenStyle::DotLine);

    // Clear the m_circle array
    memset(m_circle, 0, sizeof m_circle);

    // Initialize animation's parameters
    m_ambXPosMin      = -970.0f;
    m_ambXPosMax      = 1070.0f;
    m_ambSirenTmrTop  = 1000;
    m_soundSpeed      = 0.10f;     // speed 0.02f and siren top 1500 gives nice dense waves

    // Initialize animation's state
    m_ambXPos         = m_ambXPosMin;
    m_ambXVel         = 0.0f;
    m_ambSirenTmr     = 0;
    m_ambSirenOn      = false;
    m_t               = 0;
}


// Paint
// in: painter = QPainter object with render hints already set
//     w, h = width and height of the widget being painted
//     dt = time in milliseconds since the last call to this method (>=0)
//     synth = sound-effects synthesizer object
void Animator::paint(QPainter *painter, int w, int h, int dt, Synthesizer *synth) {
    const int w2 = w >> 1,
              h2 = h >> 1;
//  painter->fillRect(0, 0, w, h, m_background);    // fill client area with a solid color
    painter->drawPixmap(0, 0, m_backgroundPixmap);  // fill client area with a background image
    painter->translate(w2, h2);                     // translate; so, origin is at the client area's center

    // Draw the ambulance
    float ambXPos = m_ambXPos + m_ambXVel * m_t;
    if (ambXPos < m_ambXPosMin) { ambXPos = m_ambXPosMin;  m_ambSirenOn = false; }
    if (ambXPos > m_ambXPosMax) { ambXPos = m_ambXPosMax;  m_ambSirenOn = false; }
    const int xs = int(roundf(ambXPos)),     // X-coord. of center of ambulance's siren
              ys = AMBULANCE_Y + SIREN_DY;   // Y-coord. of center of ambulance's siren
    painter->drawPixmap(xs - SIREN_DX, AMBULANCE_Y, m_ambulancePixmap);

    // Draw crosshairs on the ambulance's siren
    // // painter->setPen(QPen(Qt::blue));
    // // painter->drawLine(xs, ys - 5, xs, ys + 5);
    // // painter->drawLine(xs - 5, ys, xs + 5, ys);

    // Add a circle when siren pulses and ambulance is far enough away from the window's left edge
    if (m_ambSirenOn  &&  ambXPos >= m_ambXPosMin + 150.0f) {
        m_ambSirenTmr += dt;
        if (m_ambSirenTmr >= m_ambSirenTmrTop) {
            m_ambSirenTmr -= m_ambSirenTmrTop;
            if (m_ambSirenTmr >= m_ambSirenTmrTop)  m_ambSirenTmr = 0;
            bool success = false;
            for (int i = 0; i < MAX_CIRCLES; i++)
                if (!m_circle[i].valid) {
                    Circle *p = m_circle + i;
                    p->valid = true;
                    p->r = m_soundSpeed * m_ambSirenTmr;
                    p->x = xs;
                    p->y = ys;
                    success = true;
                    break;
                }
            if (!success)  qDebug() << "ERROR in Animator::paint() : Circle buffer overflowed. Increase MAX_CIRCLES to fix.";
        }
    }
    else  m_ambSirenTmr = 0;

    // Draw circles
    painter->setBrush(Qt::transparent);
    const float dr = m_soundSpeed * dt;
    for (int i = 0; i < MAX_CIRCLES; i++) {
        Circle *p = m_circle + i;
        if (p->valid) {
            p->r += dr;
            if (inside(p->x, p->y, p->r, -w2, -h2) &&
                inside(p->x, p->y, p->r, -w2,  h2) &&
                inside(p->x, p->y, p->r,  w2, -h2) &&
                inside(p->x, p->y, p->r,  w2,  h2))  p->valid = false;  // circle is no longer visible
            else {
                constexpr int PEN_STEP = 200;  // pen width steps down in thickness every PEN_STEP pixels that a siren circle grows
                int r = int(p->r + 0.5f);
                painter->setPen(m_circlePen[std::min(r / PEN_STEP, 3)]);
                painter->drawEllipse(p->x - r, p->y - r, 2*r, 2*r);
            }
        }
    }

    // Is the person hearing the siren?
    bool hearing = false;
    if (m_ambSirenOn)
        for (int i = 0; i < MAX_CIRCLES; i++) {
            const Circle *p = m_circle + i;
            if (p->valid && inside(p->x, p->y, p->r, PERSON_EAR_X, PERSON_EAR_Y)) { hearing = true;  break; }
        }
    else
        for (int i = 0; i < MAX_CIRCLES; i++) {
            const Circle *p = m_circle + i;
            if (p->valid && !inside(p->x, p->y, p->r, PERSON_EAR_X, PERSON_EAR_Y)) { hearing = true;  break; }
        }

    // Draw the person, who is listening to the ambulance's siren
    if (hearing)  painter->drawPixmap(PERSON_X - m_personListeningPixmap.width()/2, PERSON_Y, m_personListeningPixmap);
    else  painter->drawPixmap(PERSON_X - m_personWaitingPixmap.width()/2, PERSON_Y, m_personWaitingPixmap);

    // // Draw crosshairs on the person's ear on the m_personListeningPixmap image
    // painter->setPen(QPen(Qt::blue));
    // painter->drawLine(PERSON_EAR_X, PERSON_EAR_Y - 5, PERSON_EAR_X, PERSON_EAR_Y + 5);
    // painter->drawLine(PERSON_EAR_X - 5, PERSON_EAR_Y, PERSON_EAR_X + 5, PERSON_EAR_Y);

    // Generate/enqueue ambulance's siren audio for dt milliseconds
    float volume = 0.0f,
          warp = 1.0f;
    if (hearing) {
        // Compute volume according to the inverse-square law, and make the volume maximum (at closest approach)
        // be about 1.0f.  Also, compute the frequency warping factor.
        constexpr float GAIN = (AMBULANCE_Y + SIREN_DY - PERSON_EAR_Y) * (AMBULANCE_Y + SIREN_DY - PERSON_EAR_Y),
                        SPEED_OF_SOUND = 1.0f,              // speed of sound in pixels per millisecond (selected experimentally to sound right)
                        ySiren = AMBULANCE_Y + SIREN_DY;    // Y-coord. of center of ambulance's siren
        float xSiren = m_ambXPos + m_ambXVel * m_t;         // X-coord. of center of ambulance's siren
        if (xSiren < m_ambXPosMin)  xSiren = m_ambXPosMin;
        if (xSiren > m_ambXPosMax)  xSiren = m_ambXPosMax;
        const float x = xSiren - PERSON_EAR_X,
                    y = ySiren - PERSON_EAR_Y,
                    d2 = x*x + y*y;
        volume = GAIN / d2,
        warp = 1.0f - (1.0f / SPEED_OF_SOUND) * m_ambXVel * x / sqrtf(d2);
    }
    synth->generate(volume, warp, dt);

    // Update time
    m_t += dt;
    if (m_t > 999999)  m_t = 999999;
}


// Set Animation's State
// in: state =
//      0 to hide the ambulance stopped with siren off
//      1 to show the ambulance stopped with siren off
//      2 to show the ambulance stopped with siren on
//      3 to begin slowly running the ambulance left-to-right across the window with siren on
//      4 to begin quickly running the ambulance left-to-right across the window with siren on
//      other values have no effect
void Animator::setState(int state) {

    // Set animation's state variables
    switch (state) {
    case 0:
        m_ambXPos         = m_ambXPosMin;
        m_ambXVel         = 0.0f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = false;
        break;
    case 1:
        m_ambXPos         = m_ambXPosMin + 550.0f;
        m_ambXVel         = 0.0f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = false;
        break;
    case 2:
        m_ambXPos         = m_ambXPosMin + 550.0f;
        m_ambXVel         = 0.0f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = true;
        break;
    case 3:
        m_ambXPos         = m_ambXPosMin;
        m_ambXVel         = 0.025f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = true;
        break;
    case 4:
        m_ambXPos         = m_ambXPosMin;
        m_ambXVel         = 0.050f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = true;
        break;
    default:
        return;
    }

    // Clear the m_circle array
    memset(m_circle, 0, sizeof m_circle);

    // Reset animation time
    m_t = 0;
}
