#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include <iostream>
using namespace std;
using namespace sf;

namespace SoundSystem {

    namespace {
        Music backgroundMusic;
        map<string, SoundBuffer> soundBuffers;
        map<string, Sound> sounds;
        float musicVolume = 100.0f;
        float soundEffectVolume = 100.0f;
    }

    inline void init() {}

    inline void playBackgroundMusic(const string& musicFile) {
        if (!backgroundMusic.openFromFile(musicFile)) {
            cerr << "Error loading background music from file: " << musicFile << endl;
            return;
        }
        backgroundMusic.setVolume(musicVolume);
        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    inline void stopBackgroundMusic() {
        backgroundMusic.stop();
    }

    inline void playSoundEffect(const string& soundFile) {
        if (soundBuffers.find(soundFile) == soundBuffers.end()) {
            SoundBuffer buffer;
            if (!buffer.loadFromFile(soundFile)) {
                cerr << "Error loading sound effect from file: " << soundFile << endl;
                return;
            }
            soundBuffers[soundFile] = buffer;
        }

        Sound sound;
        sound.setBuffer(soundBuffers[soundFile]);
        sound.setVolume(soundEffectVolume);
        sound.play();

        sounds[soundFile] = sound;
    }

    inline void setMusicVolume(float volume) {
        musicVolume = volume;
        backgroundMusic.setVolume(volume);
    }

    inline void setSoundEffectVolume(float volume) {
        soundEffectVolume = volume;
        for (auto& sound : sounds) {
            sound.second.setVolume(volume);
        }
    }

    inline void cleanup() {
        backgroundMusic.stop();
        sounds.clear();
        soundBuffers.clear();
    }
}
