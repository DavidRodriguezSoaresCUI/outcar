#ifndef OUTCAR_ANDROID_AUDIO_SFX_H
    #define OUTCAR_ANDROID_AUDIO_SFX_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <SDL.h>

    int init_sfx(
        char* path,
        SDL_AudioDeviceID* device_id,
        uint32_t* wav_length,
        uint8_t **wav_buffer);
    int play_sfx(
        SDL_AudioDeviceID device_id,
        uint32_t wav_length,
        uint8_t *wav_buffer);

    void auto_pause_sfx(SDL_AudioDeviceID device_id);

    void exit_sfx(
        SDL_AudioDeviceID device_id,
        uint8_t *wav_buffer);

#endif //OUTCAR_ANDROID_AUDIO_SFX_H
