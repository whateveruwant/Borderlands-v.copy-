#pragma once
#include "cSingletonBase.h"
#include "inc/fmod.hpp"
#pragma comment(lib, "lib/fmodex_vc.lib")
using namespace FMOD;

#define EXTRACHANNERBUFFER 50      //   채널버퍼
#define SOUNDBUFFER 100            //  사운드버퍼
#define TOTALSOUNDBUFFER SOUNDBUFFER + EXTRACHANNERBUFFER // 총사운드 버퍼설정: 버퍼가 작아서 씹히는 문제를 해결해주기 위함

class cSoundManager : public cSingletonBase<cSoundManager>
{
private:
	typedef map<string, Sound**>            arrSounds;
	typedef map<string, Sound**>::iterator      arrSoundIter;
	//////////////////////////////////////////////////////////////////////

	System*         m_pSystem;      //fmod의 시스템 클래스
	Sound**         m_pSound;      //fmod의 사운드 클래스
	Channel**      m_pChannel;      //fmod의 채널 클래스
	arrSounds      m_pTotalSounds;//맵에 담긴 사운드 목록


								  //////////////////////////////////////////////////////////////////////
	typedef map<string, Sound**>       m_p3DSoundsListmap;

	//// >> :
	System*               m_p3Dsystem;      //fmod의 시스템 클래스
	Sound**               m_p3DSound;         //fmod의 3d 사운드 클래스
	Channel**            m_p3DChannel;      //fmod의 3d 채널 클래스
	m_p3DSoundsListmap      m_p3DsoundList;
	ChannelGroup**      m_p3DchannelGroup;
	// << :

	bool                    m_bFmod3DInitialized;
	bool                    m_isStreaming;

	FMOD_VECTOR            listenerVelocity, listenerUp, listenerForward, listenerPos;
	FMOD_VECTOR            soundPosition, soundVelocity;

public:
	float s_volume;
	float b_volume;

public:
	cSoundManager();
	~cSoundManager();

	void Setup(void);
	void Destroy(void);
	void update(void);

	// >> : #################### 2D 사운드 ######################## //
	//사운드 추가(키값, 파일이름, BGM? 루프냐?)
	void addSound(string keyName, string soundName, bool bgm = FALSE, bool loop = FALSE);
	//사운드 플레이(키값, 불륨) 0.0f ~ 1.0f

	void play(string keyName, float volume = 1.0f);
	void bgmPlay(string keyName, float volume = 1.0f);
	//사운드 정지
	void stop(string keyName);
	//사운드 일시정지
	void pause(string keyName);
	//사운드 다시 재생
	void resume(string keyName);
	// << : ####################################################### //


	// >> :#################### 3D 사운드 ######################## //
	// : 3D Sound 최초 재생 ( Default volume = 1.0f )
	void Play3DSound(string keyName, D3DXVECTOR3 pos, float volume = 1.0f);
	// : 3D sound Stop
	void Stop3DSound(string keyName);
	// : 3D sound 추가
	bool add3DSound(string keyName, string soundName, float MinDistance, float MaxDistance);
	// : Listener 위치 업데이트
	void Update_Listener(D3DXVECTOR3 vPosition);
	// : 3D Sound 위치 업데이트
	void Update_3Dsound(string keyName, D3DXVECTOR3 vPosition);
	// << : ####################################################### //

	void Allstop();
};


#define SOUND_MGR cSoundManager::GetInstance()