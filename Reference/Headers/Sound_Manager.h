#pragma once
#include "Base.h"

BEGIN(Engine)

enum SoundChannel
{
	SC_BGM,
	SC_EFFECT_PLAYER,
	SC_EFFECT,
	SC_EFFECT2,
	SC_EFFECT3,
	SC_AMBIENT,
	SC_DOOR,
	SC_BUTTON,
	SC_EFFECT_MONSTER1,
	SC_EFFECT_MONSTER2,
	SC_EFFECT_MONSTER3,
	SC_EFFECT_MONSTER4,
	SC_EFFECT_MONSTER5,
	SC_MAX
};

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Init();

public:
	void Play_Sound(const wstring& strSoundTag, SoundChannel eChannel, _float fVolume);
	void PlayBGM(const wstring& strSoundTag, float fVolume);
	void StopSound(SoundChannel eChannel);
	void StopAll();

	void SetChannelVolume(SoundChannel eChannel, _float fVolume);

private:
	map<const wstring, FMOD::Sound*> m_Sound;
	FMOD::Channel* m_pChannelArr[SC_MAX]{};
	FMOD::System* m_pSystem{ nullptr };
	
private:
	HRESULT LoadSoundFile();
	FMOD::Sound* Find_Sound(const wstring& strSoundTag);

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END