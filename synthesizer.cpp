#include <QAudioSink>
#include <QDebug>
#include <QMediaDevices>
#include <cmath>
#include "synthesizer.h"


// Constructor
Synthesizer::Synthesizer() {
    m_audio = nullptr;

    // Create audio buffer
    m_buffer = new float[BUFFER_FRAMES][2];
    memset(m_buffer, 0, sizeof(float) * BUFFER_FRAMES * 2);

    // Create audio loop of a FREQ Hz sine wave, optionally with +/-1% vibrato at 1 Hz
    constexpr double FREQ = 1000.0;   // nominal frequency (Hz)
    constexpr bool VIBRATO = false;   // vibrato enable
    m_loop = new float[LOOP_LENGTH];
    for (int i = 0; i < LOOP_LENGTH; i++)
        m_loop[i] = VIBRATO ? cos((2 * M_PI * FREQ / 48000) * (1 + 0.01 * cos(2 * M_PI / 48000 * i)) * i)
                            : cos((2 * M_PI * FREQ / 48000) * i);
    m_loopIndex = 0.0f;
    m_loopIndexInc = 1.0f;
    m_volume = 0.1f;
}


// Destructor
Synthesizer::~Synthesizer() {
    stop();
    delete[] m_loop;
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
}


// Generate audio
// If stopped, do nothing; otherwise, generate up to BUFFER_FRAMES of audio frames,
// and enqueue them to play.  This method should be called more often than every
// 0.5 seconds (on Windows 10 the audio device's queue size appears to be 24000
// stereo float frames -- equal to 0.5 seconds of audio at 48000 samples per second).
void Synthesizer::generate() {
    if (isStopped())  return;
    const int bytes = int(m_audio->bytesFree());
    if (bytes & 7)  qDebug() << "ERROR: m_audio->bytesFree() not a multiple of 8";
    const int n = std::min(bytes >> 3, BUFFER_FRAMES);
    for (int i = 0; i < n; i++) {
        int j = int(m_loopIndex);
        m_loopIndex += m_loopIndexInc;
        if (m_loopIndex >= LOOP_LENGTH)  m_loopIndex -= LOOP_LENGTH;
        m_buffer[i][0] = m_buffer[i][1] = m_volume * m_loop[j];
    }
    m_io->write(reinterpret_cast<const char *>(m_buffer), qint64(sizeof(float) * 2) * n);
}


// Stop
// Stop audio playing, if it was playing.
// on exit: m_audio = nullptr
void Synthesizer::stop() {
    qDebug() << "INFO: Synthesizer::stop()";
    if (m_audio)  m_audio->stop();
    m_sourceFile.close();
    delete m_audio;
    m_audio = nullptr;
}
