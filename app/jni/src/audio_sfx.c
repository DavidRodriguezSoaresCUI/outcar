#include "audio_sfx.h"

int init_sfx(char* path, SDL_AudioDeviceID* device_id, uint32_t* wav_length, uint8_t **wav_buffer)
{
    // load wav

    SDL_AudioSpec wav_spec;

    if (SDL_LoadWAV(path, &wav_spec, wav_buffer, wav_length) == NULL)
    {
        SDL_Log("wav load error: %s", SDL_GetError());
        return -1;
    }

    // open audio device
    if (*device_id == 0)
    {
        *device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
        if (*device_id == 0)
        {
            SDL_Log("OpenAudioDevice error: %s", SDL_GetError());
            return -2;
        }
    }

    return 0;
}

int play_sfx(SDL_AudioDeviceID device_id, uint32_t wav_length, uint8_t *wav_buffer)
{
    // clearing buffer, stopping playing audio if there's any
    if (SDL_GetAudioDeviceStatus(device_id) == SDL_AUDIO_PLAYING)
    {
        SDL_PauseAudioDevice(device_id, 1);
        SDL_ClearQueuedAudio(device_id);
    }

    // play sfx
    if (SDL_QueueAudio(device_id, wav_buffer, wav_length) < 0)
    {
        SDL_Log("QueueAudio error: %s", SDL_GetError());
        return -1;
    }
    SDL_PauseAudioDevice(device_id, 0);

    return 0;
}

void auto_pause_sfx(SDL_AudioDeviceID device_id)
{
    if (SDL_GetAudioDeviceStatus(device_id) == SDL_AUDIO_PLAYING)
    {
        if (SDL_GetQueuedAudioSize(device_id) == 0)
        {
            SDL_PauseAudioDevice(device_id, 1);
        }
    }
}

void exit_sfx(SDL_AudioDeviceID device_id, uint8_t *wav_buffer)
{
    SDL_CloseAudioDevice(device_id);
    SDL_FreeWAV(wav_buffer);
}