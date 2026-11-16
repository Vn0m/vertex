// Trim unused features to keep the compiled library small. WAV and MP3 decoding stay.
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_FLAC

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
