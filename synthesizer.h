#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <QFile>
#include <QAudioSink>


// Class for synthesizing and outputing a sine-wave audio signal.  The frequency is
// nominally FREQ Hz.  The amplitude (volume) and frequency (warp factor) can be varied.
class Synthesizer {
    QAudioSink *m_audio;    // pointer to an audio sink object, or nullptr
    QIODevice *m_io;        // device to write audio data to; valid iff m_audio != nullptr

    static constexpr int FRAME_RATE = 48000,              // audio output stream's frame rate in Hz (a multiple of 1000; each frame is 2 float's for stereo audio)
                         BUFFER_FRAMES = FRAME_RATE / 2;  // output buffer's size in frames (each frame is 2 float's for stereo audio)
    float (*m_buffer)[2];   // output buffer of stereo audio samples (pointer to a float[BUFFER_FRAMES][2] array)

    static constexpr int FREQ = 1000,               // siren or pure tone's nominal frequency (Hz)
                         SINE_LENGTH = 1000;        // length of sine table
    static constexpr float VOL_MIN = 0.0f,          // volume's minimum value (always 0.0f)
                           VOL_MAX = 1.0f,          // volume's maximum value (always 1.0f)
                           WARP_MIN = 0.0f,         // warp's minimum value
                           WARP_NOM = 1.0f,         // warp's nominal value (always 1.0f)
                           WARP_MAX = 10.0f;        // warp's maximum value
    float *m_sine,          // table of one cycle of a sine wave (pointer to array of SINE_LENGTH float values)
          m_sineIndex;      // index into m_loop buffer [0.0f, SINE_LENGTH)

public:
    Synthesizer();
    Synthesizer(const Synthesizer &) = delete;
    Synthesizer &operator=(const Synthesizer &) = delete;
    ~Synthesizer();

    bool isStopped() { return m_audio == nullptr; }
    void start();
    void generate(float volume, float warp, int dt);
    void stop();
};


#endif  // SYNTHESIZER_H
