#pragma once

#include <memory>
#include <string>

namespace vertex {

class Audio {
public:
    Audio();
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    bool init();
    void shutdown();
    bool available() const;
    bool loadSound(const std::string& soundName, const std::string& filepath,
                   bool stream = false);
    bool hasSound(const std::string& soundName) const;
    void play(const std::string& soundName);
    void playDelay(const std::string& soundName, float delaySeconds);
    void stop(const std::string& soundName);
    void stopAll();
    void setLooping(const std::string& soundName, bool loop);
    void setVolume(const std::string& soundName, float volume);
    float getDuration(const std::string& soundName) const;

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
