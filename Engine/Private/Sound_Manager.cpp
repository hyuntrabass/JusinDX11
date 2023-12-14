#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
	Init();
}

HRESULT CSound_Manager::Init()
{
	FMOD::System_Create(&m_pSystem);

	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	if (FAILED(LoadSoundFile()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSound_Manager::Play_Sound(const wstring& strSoundTag, SoundChannel eChannel, _float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(strSoundTag);

	if (!pSound)
	{
		return;
	}

	_bool bPlay = FALSE;

	if (m_pChannelArr[eChannel]->isPlaying(&bPlay))
	{
		m_pSystem->playSound(pSound, 0, false, &m_pChannelArr[eChannel]);
	}

	m_pChannelArr[eChannel]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Manager::PlayBGM(const wstring& strSoundTag, float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(strSoundTag);
	if (!pSound)
	{
		return;
	}

	m_pSystem->playSound(pSound, 0, false, &m_pChannelArr[SC_BGM]);

	m_pChannelArr[SC_BGM]->setMode(FMOD_LOOP_NORMAL);

	m_pChannelArr[SC_BGM]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Manager::StopSound(SoundChannel eChannel)
{
	m_pChannelArr[eChannel]->stop();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < SC_MAX; ++i)
	{
		m_pChannelArr[i]->stop();
	}
}

void CSound_Manager::SetChannelVolume(SoundChannel eChannel, _float fVolume)
{
	m_pChannelArr[SC_BGM]->setVolume(fVolume);

	m_pSystem->update();
}

HRESULT CSound_Manager::LoadSoundFile()
{
	string FilePath = "../Bin/Resources/Sound/";

	for (const auto& entry : filesystem::recursive_directory_iterator(FilePath))
	{
		if (entry.is_regular_file())
		{
			FMOD::Sound* pSound = nullptr;

			FMOD_RESULT eRes = m_pSystem->createSound(entry.path().string().c_str(), FMOD_DEFAULT, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				m_Sound.emplace(entry.path().stem().wstring(), pSound);
			}
		}
	}

	return S_OK;
}

FMOD::Sound* CSound_Manager::Find_Sound(const wstring& strSoundTag)
{
	auto& it = m_Sound.find(strSoundTag);

	if (it == m_Sound.end())
	{
		return nullptr;
	}

	return it->second;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_Sound)
	{
		Mypair.second->release();
	}
	m_Sound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
