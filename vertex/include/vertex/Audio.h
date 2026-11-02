#pragma once

#include <miniaudio.h>
#include <string>

namespace vertex {

class Audio {
public:
    Audio() = default;
    ~Audio();

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    bool init();
    void shutdown();

    // from file
    bool loadSound(const std::string& filepath);

    // playback
    void play();
    void playDelay(float delay);
    void stop();
    void stopAll();

    float getDuration() const;
    void setLooping(bool loop);

private:
    ma_engine mEngine;
    ma_sound mSound;
    double mLastPlay{0.0};
    bool mInitialized{false};
    bool mLoaded{false};
};
}