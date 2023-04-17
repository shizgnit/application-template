/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

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
