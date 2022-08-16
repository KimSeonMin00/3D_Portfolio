#pragma once

#include "Base.h"

BEGIN(Engine)
class CSound_Device final :
	public CBase
{
	DECLARE_SINGLETON(CSound_Device)

public:
	enum CHANNELID {
		CHANNEL_BGM,
		CHANNEL_PLAYER,
		CHANNEL_MONSTER,
		CHANNEL_GUN,
		CHANNEL_PLAYER_BULLET,
		CHANNEL_EFFECT,
		CHANNEL_MAX
	};

private:
	CSound_Device();
	virtual ~CSound_Device() = default;

public:
	HRESULT Initialize_Sound_Device(const char* pSoundFilePath);

public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile(const char* pSoundFilePath);

private:
	map<TCHAR*, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[CHANNEL_MAX];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem;

public:
	virtual void Free() override;
};
END
