#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_OPENAL

void implementation::openal::audio::init(int sources) {
    ALCint attributes[] = { ALC_FREQUENCY, 44100, 0 };

    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context = alcCreateContext(device, attributes);

    alcMakeContextCurrent(context);

    float orientation[6] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };

    alListener3f(AL_POSITION, 0., 1.5, 0.);
    alListener3f(AL_VELOCITY, 0., 0., 0.);
    alListenerfv(AL_ORIENTATION, orientation);

    this->sources = sources;
    this->identifiers = (unsigned int*)malloc(this->sources * sizeof(unsigned int));

    alGenSources(this->sources, this->identifiers);
}

void implementation::openal::audio::compile(type::sound& sound) {
    if (sound.resource == NULL) {
        sound.resource = new type::info::opaque_t();
    }

    alGenBuffers(1, &sound.resource->context);

    alBufferData(sound.resource->context, AL_FORMAT_MONO16, sound.buffer.data(), sound.size, 44100);
    //alBufferData(sound.context, AL_FORMAT_MONO8, (const ALvoid *)sound.buffer.data(), sound.size, 11000);
}

void implementation::openal::audio::shutdown(void) {
    //alDeleteSources(1, &sound.source);
    //alDeleteBuffers(1, &sound.context);
    //alcDestroyContext(context);
    //alcCloseDevice(device);
}

int implementation::openal::audio::start(type::sound& sound) {
    unsigned int selection;
    for (selection = 0; selection < this->sources; selection++) {
        ALenum state;
        alGetSourcei(this->identifiers[selection], AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            break;
        }
    }

    unsigned int id = this->identifiers[selection];

    alSourcef(id, AL_PITCH, 1.0f);
    alSourcef(id, AL_GAIN, 1.0f);
    alSource3f(id, AL_POSITION, 0.0f, 0.0f, -1.0f);
    alSource3f(id, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(id, AL_LOOPING, AL_FALSE);

    alSourcei(id, AL_BUFFER, sound.resource->context);
    alSourcePlay(id);
    alSource3f(id, AL_POSITION, 0.0f, 0.0f, -1.0f);
    //alSource3f(id, AL_VELOCITY, 0.0f, 0.0f, -1.0f);

    return id;
}

void implementation::openal::audio::stop(int id) {
    alSourceStop(id);
}
#endif
