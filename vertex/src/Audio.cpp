#include <vertex/Audio.h>

#include <GLFW/glfw3.h>
#include <cstdio>

namespace vertex {

Audio::~Audio() {
    shutdown();
}

bool Audio::init() {
    if (mInitialized) return true;

    ma_result result = ma_engine_init(nullptr, &mEngine);
    if (result != MA_SUCCESS) {
        std::fprintf(stderr, "vertex::Audio Failed to Initialize miniaudio engine (%d)\n",
                     result);
        return false;
    }

    mInitialized = true;
    return true;
}

void Audio::shutdown() {
    if (mLoaded) {
        ma_sound_uninit(&mSound);
        mLoaded = false;
    }

    if (mInitialized) {
        ma_engine_uninit(&mEngine);
        mInitialized = false;
    }
}

bool Audio::loadSound(const std::string& filepath) {
    if (!mInitialized && !init()) {
        return false;
    }

    if (mLoaded) {
        ma_sound_uninit(&mSound);
        mLoaded = false;
    }

    ma_result result =
        ma_sound_init_from_file(&mEngine, filepath.c_str(), 0, nullptr, nullptr, &mSound);

    if (result != MA_SUCCESS) {
        std::fprintf(stderr, "vertex::Audio: Failed to load sound '%s'(%d)\n",
                     filepath.c_str(), result);
        return false;
    }

    mLoaded = true;
    return true;
}

void Audio::play() {
    if (!mLoaded) {
        return;
    }
    // Rewind to beginning and play
    ma_sound_seek_to_pcm_frame(&mSound, 0);
    ma_sound_start(&mSound);
    mLastPlay = glfwGetTime();
}

void Audio::playDelay(float delay) {
    if (!mLoaded) {
        return;
    }

    const double now = glfwGetTime();
    if (now - mLastPlay >= static_cast<double>(delay)) {
        play();
    }
}

void Audio::stop() {
    if (!mLoaded) {
        return;
    }

    ma_sound_stop(&mSound);
    ma_sound_seek_to_pcm_frame(&mSound, 0);
}

void Audio::stopAll() {
    if (!mInitialized) {
        return;
    }

    ma_engine_stop(&mEngine);
    ma_engine_start(&mEngine);
}

float Audio::getDuration() const {
    if (!mLoaded) {
        return 0.0f;
    }

    float durationSeconds = 0.0f;
    ma_sound_get_length_in_seconds(const_cast<ma_sound*>(&mSound), &durationSeconds);
    return durationSeconds;
}

void Audio::setLooping(bool loop) {
    if (!mLoaded) {
        return;
    }

    ma_sound_set_looping(&mSound, loop ? MA_TRUE : MA_FALSE);
}

}