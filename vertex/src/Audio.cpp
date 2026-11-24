#include <vertex/Audio.h>

#include <miniaudio.h>

#include <chrono>
#include <iostream>
#include <unordered_map>

namespace vertex {

namespace {

using Clock = std::chrono::steady_clock;

struct Sound {
    ma_sound sound;
    Clock::time_point lastPlay{};
    bool hasPlayed{false};
};

void playFromStart(Sound& sound) {
    ma_sound_seek_to_pcm_frame(&sound.sound, 0);
    ma_sound_start(&sound.sound);
    sound.lastPlay = Clock::now();
    sound.hasPlayed = true;
}

void stopAndRewind(Sound& sound) {
    ma_sound_stop(&sound.sound);
    ma_sound_seek_to_pcm_frame(&sound.sound, 0);
}

}

struct Audio::Impl {
    ma_engine engine;
    bool initialized{false};
    std::unordered_map<std::string, std::unique_ptr<Sound>> sounds;

    Sound* find(const std::string& soundName) {
        auto it = sounds.find(soundName);
        return it == sounds.end() ? nullptr : it->second.get();
    }

    Sound* lookup(const std::string& soundName) {
        Sound* sound = find(soundName);
        if (!sound && initialized) {
            std::cerr << "audio: no sound named '" << soundName << "'\n";
        }
        return sound;
    }
};

Audio::Audio() : mImpl(std::make_unique<Impl>()) {}

Audio::~Audio() {
    shutdown();
}

bool Audio::init() {
    if (mImpl->initialized) {
        return true;
    }

    ma_result result = ma_engine_init(nullptr, &mImpl->engine);
    if (result != MA_SUCCESS) {
        std::cerr << "audio: no audio device (" << result
                  << "), continuing without sound\n";
        return false;
    }

    mImpl->initialized = true;
    return true;
}

void Audio::shutdown() {
    for (auto& [name, sound] : mImpl->sounds) {
        ma_sound_uninit(&sound->sound);
    }
    mImpl->sounds.clear();

    if (mImpl->initialized) {
        ma_engine_uninit(&mImpl->engine);
        mImpl->initialized = false;
    }
}

bool Audio::available() const {
    return mImpl->initialized;
}

bool Audio::loadSound(const std::string& soundName, const std::string& filepath,
                      bool stream) {
    if (!available()) {
        std::cerr << "audio: sound '" << soundName << "' not loaded (no audio device)\n";
        return false;
    }
    if (hasSound(soundName)) {
        return true;
    }

    auto sound = std::make_unique<Sound>();
    const ma_uint32 flags = stream ? MA_SOUND_FLAG_STREAM : 0;
    ma_result result = ma_sound_init_from_file(&mImpl->engine, filepath.c_str(), flags,
                                               nullptr, nullptr, &sound->sound);
    if (result != MA_SUCCESS) {
        std::cerr << "audio: sound '" << soundName << "' not loaded from '" << filepath
                  << "' (" << result << ")\n";
        return false;
    }

    mImpl->sounds.emplace(soundName, std::move(sound));
    return true;
}

bool Audio::hasSound(const std::string& soundName) const {
    return mImpl->find(soundName) != nullptr;
}

void Audio::play(const std::string& soundName) {
    if (Sound* sound = mImpl->lookup(soundName)) {
        playFromStart(*sound);
    }
}

void Audio::playDelay(const std::string& soundName, float delaySeconds) {
    Sound* sound = mImpl->lookup(soundName);
    if (!sound) {
        return;
    }

    if (sound->hasPlayed) {
        const std::chrono::duration<float> elapsed = Clock::now() - sound->lastPlay;
        if (elapsed.count() < delaySeconds) {
            return;
        }
    }
    playFromStart(*sound);
}

void Audio::stop(const std::string& soundName) {
    if (Sound* sound = mImpl->lookup(soundName)) {
        stopAndRewind(*sound);
    }
}

void Audio::stopAll() {
    for (auto& [name, sound] : mImpl->sounds) {
        stopAndRewind(*sound);
    }
}

void Audio::setLooping(const std::string& soundName, bool loop) {
    if (Sound* sound = mImpl->lookup(soundName)) {
        ma_sound_set_looping(&sound->sound, loop ? MA_TRUE : MA_FALSE);
    }
}

void Audio::setVolume(const std::string& soundName, float volume) {
    if (Sound* sound = mImpl->lookup(soundName)) {
        ma_sound_set_volume(&sound->sound, volume);
    }
}

float Audio::getDuration(const std::string& soundName) const {
    float durationSeconds = 0.0f;
    if (Sound* sound = mImpl->lookup(soundName)) {
        ma_sound_get_length_in_seconds(&sound->sound, &durationSeconds);
    }
    return durationSeconds;
}

}
