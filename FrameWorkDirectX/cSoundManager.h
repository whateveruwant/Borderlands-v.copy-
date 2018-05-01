#pragma once
#include "cSingletonBase.h"
#include "inc/fmod.hpp"
#pragma comment(lib, "lib/fmodex_vc.lib")
using namespace FMOD;

#define EXTRACHANNERBUFFER 50      //   ä�ι���
#define SOUNDBUFFER 100            //  �������
#define TOTALSOUNDBUFFER SOUNDBUFFER + EXTRACHANNERBUFFER // �ѻ��� ���ۼ���: ���۰� �۾Ƽ� ������ ������ �ذ����ֱ� ����

class cSoundManager : public cSingletonBase<cSoundManager>
{
private:
	typedef map<string, Sound**>            arrSounds;
	typedef map<string, Sound**>::iterator      arrSoundIter;
	//////////////////////////////////////////////////////////////////////

	System*         m_pSystem;      //fmod�� �ý��� Ŭ����
	Sound**         m_pSound;      //fmod�� ���� Ŭ����
	Channel**      m_pChannel;      //fmod�� ä�� Ŭ����
	arrSounds      m_pTotalSounds;//�ʿ� ��� ���� ���


								  //////////////////////////////////////////////////////////////////////
	typedef map<string, Sound**>       m_p3DSoundsListmap;

	//// >> :
	System*               m_p3Dsystem;      //fmod�� �ý��� Ŭ����
	Sound**               m_p3DSound;         //fmod�� 3d ���� Ŭ����
	Channel**            m_p3DChannel;      //fmod�� 3d ä�� Ŭ����
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

	// >> : #################### 2D ���� ######################## //
	//���� �߰�(Ű��, �����̸�, BGM? ������?)
	void addSound(string keyName, string soundName, bool bgm = FALSE, bool loop = FALSE);
	//���� �÷���(Ű��, �ҷ�) 0.0f ~ 1.0f

	void play(string keyName, float volume = 1.0f);
	void bgmPlay(string keyName, float volume = 1.0f);
	//���� ����
	void stop(string keyName);
	//���� �Ͻ�����
	void pause(string keyName);
	//���� �ٽ� ���
	void resume(string keyName);
	// << : ####################################################### //


	// >> :#################### 3D ���� ######################## //
	// : 3D Sound ���� ��� ( Default volume = 1.0f )
	void Play3DSound(string keyName, D3DXVECTOR3 pos, float volume = 1.0f);
	// : 3D sound Stop
	void Stop3DSound(string keyName);
	// : 3D sound �߰�
	bool add3DSound(string keyName, string soundName, float MinDistance, float MaxDistance);
	// : Listener ��ġ ������Ʈ
	void Update_Listener(D3DXVECTOR3 vPosition);
	// : 3D Sound ��ġ ������Ʈ
	void Update_3Dsound(string keyName, D3DXVECTOR3 vPosition);
	// << : ####################################################### //

	void Allstop();
};


#define SOUND_MGR cSoundManager::GetInstance()