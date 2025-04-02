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
    QString imgDir = R"(C:\Users\jm3\Documents\Keep\Business\Qt6\doppler_effect\img\)";
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
    m_textPen = QPen(QColor(64, 32, 64));
    m_textFont.setPixelSize(50);

    // Clear the m_circle array
    memset(m_circle, 0, sizeof m_circle);

    // Initialize animation's parameters
    m_ambXPosMin      = -970.0f;
    m_ambXPosMax      = 1070.0f;
    m_ambSirenTmrTop  = 1500;
    m_soundSpeed      = 0.10f;     // speed 0.02f and siren top 1500 gives nice dense waves

    // Initialize animation's state
    m_ambXPos         = m_ambXPosMin;
    m_ambXVel         = 0.0f;
    m_ambSirenTmr     = 0;
    m_ambSirenOn      = false;
    m_personListening = false;
}


// Paint
// in: painter = QPainter object with render hints already set
//     w, h = width and height of the widget being painted
//     dt = time in milliseconds since the last call to this method (>=0)
void Animator::paint(QPainter *painter, int w, int h, int dt) {
    int w2 = w >> 1,
        h2 = h >> 1;
//  painter->fillRect(0, 0, w, h, m_background);    // fill client area with a solid color
    painter->drawPixmap(0, 0, m_backgroundPixmap);  // fill client area with a background image
    painter->translate(w2, h2);                     // translate; so, origin is at the client area's center

/*
    static int t = 0;
    t = (t + dt) & 0x3FF;

    painter->save();
    painter->setBrush(circleBrush);
    painter->setPen(circlePen);
    painter->rotate(t * 0.030);
    qreal r = t / 1024.0;
    int n = 30;
    for (int i = 0; i < n; ++i) {
        painter->rotate(30);
        qreal factor = (i + r) / n;
        qreal radius = 0 + 120.0 * factor;
        qreal circleRadius = 1 + factor * 20;
        painter->drawEllipse(QRectF(radius, -circleRadius,
                                    circleRadius * 2, circleRadius * 2));
    }
    painter->restore();
*/

/*
    painter->setPen(textPen);
    painter->setFont(textFont);
    painter->drawText(QRect(-50, -50, 100, 100), Qt::AlignCenter, QStringLiteral("Qt"));
*/

    // Draw the ambulance
    m_ambXPos += m_ambXVel * dt;
    if (m_ambXPos < m_ambXPosMin) { m_ambXPos = m_ambXPosMin;  m_ambSirenOn = false; }
    if (m_ambXPos > m_ambXPosMax) { m_ambXPos = m_ambXPosMax;  m_ambSirenOn = false; }
    const int xs = int(roundf(m_ambXPos)),  // X-coord. of center of ambulance's siren
              ys = 77 + 9;                  // Y-coord. of center of ambulance's siren
    painter->drawPixmap(xs - 142, 77, m_ambulancePixmap);

    // Draw crosshairs on the ambulance's siren
    // // painter->setPen(QPen(Qt::blue));
    // // painter->drawLine(xs, ys - 5, xs, ys + 5);
    // // painter->drawLine(xs - 5, ys, xs + 5, ys);

    // Add a circle when siren pulses and ambulance is far enough away from the window's left edge
    if (m_ambSirenOn  &&  m_ambXPos >= m_ambXPosMin + 150.0f) {
        m_ambSirenTmr += dt;
        if (m_ambSirenTmr >= m_ambSirenTmrTop) {
            m_ambSirenTmr -= m_ambSirenTmrTop;
            if (m_ambSirenTmr >= m_ambSirenTmrTop)  m_ambSirenTmr = 0;
            for (int i = 0; i < MAX_CIRCLES; i++)
                if (!m_circle[i].valid) {
                    Circle *p = m_circle + i;
                    p->valid = true;
                    p->r = m_soundSpeed * m_ambSirenTmr;
                    p->x = xs;
                    p->y = ys;
                    break;
                }
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

    // Draw the person, who is listening to the ambulance's siren
    constexpr int PERSON_Y = 225;
    if (m_personListening)  painter->drawPixmap( -m_personListeningPixmap.width()/2, PERSON_Y, m_personListeningPixmap);
    else  painter->drawPixmap( -m_personWaitingPixmap.width()/2, PERSON_Y, m_personWaitingPixmap);
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
        m_personListening = false;
        break;
    case 1:
        m_ambXPos         = m_ambXPosMin + 250.0f;
        m_ambXVel         = 0.0f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = false;
        m_personListening = false;
        break;
    case 2:
        m_ambXPos         = m_ambXPosMin + 250.0f;
        m_ambXVel         = 0.0f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = true;
        m_personListening = true;
        break;
    case 3:
        m_ambXPos         = m_ambXPosMin;
        m_ambXVel         = 0.025f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = true;
        m_personListening = true;
        break;
    case 4:
        m_ambXPos         = m_ambXPosMin;
        m_ambXVel         = 0.050f;
        m_ambSirenTmr     = 0;
        m_ambSirenOn      = true;
        m_personListening = true;
        break;
    default:
        return;
    }

    // Clear the m_circle array
    memset(m_circle, 0, sizeof m_circle);
}
