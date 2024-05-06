#ifndef AUDIO_MANAGER
#define AUDIO_MANAGER

#include <Windows.h>
#include <mmsystem.h>
#include <string>
#pragma comment(lib, "winmm.lib")

class Audio
{
public:
	Audio(std::string path, std::string name)
	{
		this->path = path;
		this->name = name;

		std::string str = "open " + path + " type mpegvideo alias " + name;
		mciSendString(str.c_str(), NULL, 0, NULL);
	}

	void Play(bool loop)
	{
		std::string str = "play " + name;
		if (loop) str += " repeat";
		mciSendString(str.c_str(), NULL, 0, NULL);
	}

	void Stop(bool close)	//若不再播放则close=true
	{
		std::string str = "stop " + name;
		mciSendString(str.c_str(), NULL, 0, NULL);
		if (close)
		{
			str = "close " + name;
			mciSendString(str.c_str(), NULL, 0, NULL);
		}
	}

private:
	std::string path;
	std::string name;
};

#endif