#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <QFile>
#include <QAudioSink>


class Synthesizer {
    QFile m_sourceFile;
    QAudioSink *m_audio;    // pointer to an audio sink object, or nullptr
    QIODevice *m_io;        // device to write audio data to; valid iff m_audio != nullptr

    static constexpr int LOOP_LENGTH = 48000;    // audio loop's length in samples
    float *m_loop;          // audio loop samples (pointer to array of LOOP_LENGTH float values)
    float m_loopIndex,      // index into m_loop buffer [0.0f, LOOP_LENGTH)
          m_loopIndexInc,   // increment for m_loopIndex [0.0f, 10.0f], nominally 1.0f
          m_volume;         // volume [0.0f, 1.0f]

    static constexpr int BUFFER_FRAMES = 24000;  // output buffer's size in frames (each frame is 2 float's for stereo audio)
    float (*m_buffer)[2];   // output buffer of stereo audio samples (pointer to a float[BUFFER_FRAMES][2] array)

public:
    Synthesizer();
    Synthesizer(const Synthesizer &) = delete;
    Synthesizer &operator=(const Synthesizer &) = delete;
    ~Synthesizer();

    bool isStopped() { return m_audio == nullptr; }
    void start();
    void generate();
    void stop();
};


#endif  // SYNTHESIZER_H
