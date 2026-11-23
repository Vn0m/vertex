#include <vertex/Audio.h>

#include <cstdio>

#include "Check.h"

namespace {

bool deviceAvailable() {
    vertex::Audio audio;
    return audio.init();
}

void callsBeforeInitAreNoOps() {
    vertex::Audio audio;

    CHECK(!audio.available());
    CHECK(!audio.loadSound("hit", "does/not/exist.wav"));
    CHECK(!audio.hasSound("hit"));
    CHECK(audio.getDuration("hit") == 0.0f);

    audio.play("hit");
    audio.playDelay("hit", 0.1f);
    audio.stop("hit");
    audio.stopAll();
    audio.setLooping("hit", true);
    audio.setVolume("hit", 0.5f);

    CHECK(!audio.available());
    CHECK(!audio.hasSound("hit"));
}

void initIsIdempotent(bool hasDevice) {
    vertex::Audio audio;

    CHECK(audio.init() == hasDevice);
    CHECK(audio.init() == hasDevice);
    CHECK(audio.available() == hasDevice);
}

void shutdownIsSafeWithoutInit() {
    vertex::Audio audio;

    audio.shutdown();
    CHECK(!audio.available());
}

void shutdownIsSafeToRepeatAndReinit(bool hasDevice) {
    vertex::Audio audio;
    audio.init();

    audio.shutdown();
    CHECK(!audio.available());

    audio.shutdown();
    CHECK(!audio.available());

    CHECK(audio.init() == hasDevice);
    CHECK(audio.available() == hasDevice);
}

void missingFileIsNotLoaded() {
    vertex::Audio audio;
    audio.init();

    CHECK(!audio.loadSound("missing", "does/not/exist.wav"));
    CHECK(!audio.hasSound("missing"));
    CHECK(audio.getDuration("missing") == 0.0f);
}

void unknownNamesAreNoOps() {
    vertex::Audio audio;
    audio.init();

    audio.play("nope");
    audio.playDelay("nope", 0.1f);
    audio.stop("nope");
    audio.setLooping("nope", true);
    audio.setVolume("nope", 0.5f);

    CHECK(!audio.hasSound("nope"));
    CHECK(audio.getDuration("nope") == 0.0f);
}

}

int main() {
    const bool hasDevice = deviceAvailable();

    callsBeforeInitAreNoOps();
    initIsIdempotent(hasDevice);
    shutdownIsSafeWithoutInit();
    shutdownIsSafeToRepeatAndReinit(hasDevice);
    missingFileIsNotLoaded();
    unknownNamesAreNoOps();

    std::printf("Audio: all tests passed (%s)\n",
                hasDevice ? "with device" : "no device");
    return 0;
}