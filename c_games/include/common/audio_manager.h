#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

/**
 * @brief Initialize the audio manager
 * 
 * @return true if initialization was successful, false otherwise
 */
bool AudioManager_Init(void);

/**
 * @brief Clean up audio manager resources
 */
void AudioManager_Cleanup(void);

/**
 * @brief Load a sound effect
 * 
 * @param filename Path to the sound file
 * @param id Identifier to associate with the sound
 * @return true if the sound was loaded successfully, false otherwise
 */
bool AudioManager_LoadSound(const char* filename, const char* id);

/**
 * @brief Load a music track
 * 
 * @param filename Path to the music file
 * @param id Identifier to associate with the music
 * @return true if the music was loaded successfully, false otherwise
 */
bool AudioManager_LoadMusic(const char* filename, const char* id);

/**
 * @brief Play a sound effect
 * 
 * @param id Identifier of the sound to play
 * @param loops Number of times to loop the sound (-1 for infinite)
 * @param volume Volume level (0-128)
 * @return int Channel the sound is playing on, or -1 on error
 */
int AudioManager_PlaySound(const char* id, int loops, int volume);

/**
 * @brief Play a music track
 * 
 * @param id Identifier of the music to play
 * @param loops Number of times to loop the music (-1 for infinite)
 * @param volume Volume level (0-128)
 * @return true if the music started playing, false otherwise
 */
bool AudioManager_PlayMusic(const char* id, int loops, int volume);

/**
 * @brief Stop all sound effects
 */
void AudioManager_StopAllSounds(void);

/**
 * @brief Stop the currently playing music
 */
void AudioManager_StopMusic(void);

/**
 * @brief Pause the currently playing music
 */
void AudioManager_PauseMusic(void);

/**
 * @brief Resume the paused music
 */
void AudioManager_ResumeMusic(void);

/**
 * @brief Set the master volume for sound effects
 * 
 * @param volume Volume level (0-128)
 */
void AudioManager_SetSoundVolume(int volume);

/**
 * @brief Set the master volume for music
 * 
 * @param volume Volume level (0-128)
 */
void AudioManager_SetMusicVolume(int volume);

#endif /* AUDIO_MANAGER_H */
