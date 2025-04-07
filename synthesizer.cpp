#include <QAudioSink>
#include <QDebug>
#include <QMediaDevices>
#include <cmath>
#include "synthesizer.h"


// Constructor
Synthesizer::Synthesizer() {
    // Create audio buffer
    m_buffer = new float[BUFFER_FRAMES][2];
    memset(m_buffer, 0, sizeof(float) * BUFFER_FRAMES * 2);

    // Create sine table
    m_sine = new float[SINE_LENGTH];
    for (int i = 0; i < SINE_LENGTH; i++)
        m_sine[i] = sin((2 * M_PI / SINE_LENGTH) * i);

    m_audio = nullptr;
    m_sineIndex = 0.0f;
}


// Destructor
Synthesizer::~Synthesizer() {
    stop();
    delete[] m_sine;
    delete[] m_buffer;
}


// Start
void Synthesizer::start() {
    stop();
    qDebug() << "INFO: Synthesizer::start()";

    QAudioDevice audioDev = QMediaDevices::defaultAudioOutput();
    QAudioFormat audioFmt = audioDev.preferredFormat();
    QAudioFormat::SampleFormat f = audioFmt.sampleFormat();
    const char *sampleFmt = f==QAudioFormat::Float ? "Float" :
                            f==QAudioFormat::Int32 ? "Int32" :
                            f==QAudioFormat::Int16 ? "Int16" :
                            f==QAudioFormat::UInt8 ? "UInt8" : "???";
    qDebug() << QStringLiteral("INFO Synthesizer::start() : Using default audio device's preferred format: %1 channels of %2 at %3 Hz")
                    .arg(audioFmt.channelCount()).arg(sampleFmt).arg(audioFmt.sampleRate()).toStdString().c_str();
    // On Windows 10 this seems to be stereo Float at 48000 Hz

    m_audio = new QAudioSink(audioFmt);
//  connect(audio, QAudioSink::stateChanged, this, &AudioInputExample::handleStateChanged);
    m_io = m_audio->start();
    if (m_io == nullptr)  qDebug() << "ERROR in Synthesizer::start() : m_audio->start() Failed";
    else if (m_audio->state() == QAudio::StoppedState)  qDebug() << "ERROR in Synthesizer::start() : Audio stopped";
//  qDebug() << "DEBUG Info: m_audio->bytesFree() =" << int(m_audio->bytesFree());
}


// Generate audio
// If stopped, does nothing; otherwise, generates up to dt milliseconds of audio.  The audio frames
// are enqueued to the audio output device's FIFO.  This method should be called frequently enough
// to avoid a FIFO underrun.  Note, on Windows 10 the audio device's queue size appears to be 24000
// stereo float frames -- equal to 0.5 seconds of audio at 48000 samples per second.
// in: volume = audio volume, which will be saturated into the range [VOL_MIN, VOL_MAX]
//     warp = frequency warping factor, which will be saturated into the range [WARP_MIN, WARP_MAX]
//     dt = desired duration in milliseconds for which to generate and enqueue audio output (>0)
void Synthesizer::generate(float volume, float warp, int dt) {
    if (isStopped() || dt <= 0)  return;
    if (volume < VOL_MIN)  volume = VOL_MIN;
    if (volume > VOL_MAX)  volume = VOL_MAX;
    if (warp < WARP_MIN)  warp = WARP_MIN;
    if (warp > WARP_MAX)  warp = WARP_MAX;
    const int nMax = (FRAME_RATE / 1000) * dt,    // max. number of frames to generate
              bytes = int(m_audio->bytesFree());
    if (bytes & 7)  qDebug() << "ERROR: m_audio->bytesFree() not a multiple of 8";
    const int n = std::min( std::min(bytes >> 3, BUFFER_FRAMES), nMax );   // number of frames to generate
    const float sineIndexInc = SINE_LENGTH * FREQ * (1.0f / FRAME_RATE) * warp;
    for (int i = 0; i < n; i++) {
        m_sineIndex += sineIndexInc;
        if (m_sineIndex >= SINE_LENGTH)  m_sineIndex -= SINE_LENGTH;
        m_buffer[i][0] = m_buffer[i][1] = volume * m_sine[int(m_sineIndex)];
    }
    m_io->write(reinterpret_cast<const char *>(m_buffer), qint64(sizeof(float) * 2) * n);
}


// Stop
// Stop audio playing, if it was playing.
// on exit: m_audio = nullptr
void Synthesizer::stop() {
    qDebug() << "INFO: Synthesizer::stop()";
    if (m_audio)  m_audio->stop();
    delete m_audio;
    m_audio = nullptr;
}
