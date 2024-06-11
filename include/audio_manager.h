#if defined(_WIN32) || defined(WIN32)
#ifndef AUDIO_MANAGER
#define AUDIO_MANAGER

#include <Windows.h>
#include <mmsystem.h>
#include <string>
#pragma comment(lib, "winmm.lib")

class Audio {
  public:
    Audio(std::string path, std::string name) {
        this->path = path;
        this->name = name;

        std::wstring wPath = std::wstring(path.begin(), path.end());
        std::wstring wName = std::wstring(name.begin(), name.end());
        std::wstring wStr =
            L"open " + wPath + L" type mpegvideo alias " + wName;

        mciSendStringW(wStr.c_str(), NULL, 0, NULL);
    }

    void Play(bool loop) {
        std::wstring wName = std::wstring(name.begin(), name.end());
        std::wstring wStr = L"play " + wName;
        if (loop)
            wStr += L" repeat";

        mciSendStringW(wStr.c_str(), NULL, 0, NULL);
    }

    void Stop(bool close) {
        std::wstring wName = std::wstring(name.begin(), name.end());
        std::wstring wStr = L"stop " + wName;

        mciSendStringW(wStr.c_str(), NULL, 0, NULL);
        if (close) {
            wStr = L"close " + wName;
            mciSendStringW(wStr.c_str(), NULL, 0, NULL);
        }
    }

  private:
    std::string path;
    std::string name;
};

#endif
#endif
