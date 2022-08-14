#include "..\Public\Sound_Device.h"

IMPLEMENT_SINGLETON(CSound_Device)

CSound_Device::CSound_Device()
{
}

HRESULT CSound_Device::Initialize_Sound_Device(const char* pSoundFilePath)
{
	if (FMOD_OK != FMOD::System_Create(&m_pSystem))
		return E_FAIL;

	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 

	LoadSoundFile(pSoundFilePath);

	return S_OK;
}

void CSound_Device::PlaySound(TCHAR * pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE;


	if (m_pChannelArr[eID]->isPlaying(&bPlay))
	{
		m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannelArr[eID]);
	}

	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Device::PlayBGM(TCHAR * pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannelArr[CHANNEL_BGM]);
	m_pChannelArr[CHANNEL_BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[CHANNEL_BGM]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Device::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();
}

void CSound_Device::StopAll()
{
	for (int i = 0; i < CHANNEL_MAX; ++i)
		m_pChannelArr[i]->stop();
}

void CSound_Device::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Device::LoadSoundFile(const char* pSoundFilePath)
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	char szFirstPath[128] = "";

	strcpy_s(szFirstPath, "../Bin/Resources/Sound/*.wav");

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst(szFirstPath, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurrentPath[128] = "../Bin/Resources/Sound/";

	//strncpy_s(szCurrentPath, szFirstPath, strlen(szFirstPath) - 5);

	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurrentPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}


void CSound_Device::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}




