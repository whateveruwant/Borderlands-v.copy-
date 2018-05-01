#include "stdafx.h"
#include "cSoundManager.h"


cSoundManager::cSoundManager()
{
}


cSoundManager::~cSoundManager()
{
}

void cSoundManager::Setup(void)
{
	////사운드 시스템 생성

	System_Create(&m_p3Dsystem);
	System_Create(&m_pSystem);
	m_pSystem->init(TOTALSOUNDBUFFER, FMOD_INIT_NORMAL, 0);
	m_p3Dsystem->init(128, FMOD_INIT_DISTANCE_FILTERING, NULL);
	m_p3Dsystem->set3DSettings(20.0f, 20.0f, 20.0f);
	m_p3Dsystem->getMasterChannelGroup(m_p3DchannelGroup);

	// >> : 3D 사운드 , 채널 동적할당
	m_p3DSound = new Sound *[TOTALSOUNDBUFFER];
	m_p3DChannel = new Channel *[TOTALSOUNDBUFFER];

	// >> : 3D 메모리 초기화
	memset(m_p3DSound, 0, sizeof(Sound*)* (TOTALSOUNDBUFFER));
	memset(m_p3DChannel, 0, sizeof(Channel*)* (TOTALSOUNDBUFFER));


	// >> : 2D 사운드 , 채널 동적할당
	m_pSound = new Sound *[TOTALSOUNDBUFFER];
	m_pChannel = new Channel *[TOTALSOUNDBUFFER];

	// >> : 2D 메모리 초기화
	memset(m_pSound, 0, sizeof(Sound*)* (TOTALSOUNDBUFFER));
	memset(m_pChannel, 0, sizeof(Channel*)* (TOTALSOUNDBUFFER));
	s_volume = 1.0f;
	b_volume = 1.0f;

}

void cSoundManager::Destroy(void)
{
	//사운드 및 채널 삭제
	if (m_pChannel != NULL || m_pSound != NULL)
	{
		for (int i = 0; i < TOTALSOUNDBUFFER; i++)
		{
			if (m_pChannel != NULL)
			{
				if (m_pChannel[i]) m_pChannel[i]->stop();
			}

			if (m_pSound != NULL)
			{
				if (m_pSound[i]) m_pSound[i]->release();
			}
		}
	}

	if (m_p3DChannel != NULL || m_p3DSound != NULL)
	{
		for (int i = 0; i < TOTALSOUNDBUFFER; i++)
		{
			if (m_p3DChannel != NULL)
			{
				if (m_p3DChannel[i]) m_p3DChannel[i]->stop();
			}

			if (m_p3DSound != NULL)
			{
				if (m_p3DSound[i]) m_p3DSound[i]->release();
			}
		}
	}

	//메모리 지우기
	SAFE_DELETE(m_pChannel);
	SAFE_DELETE(m_pSound);
	SAFE_DELETE(m_p3DChannel);
	SAFE_DELETE(m_p3DSound);

	//시스템 닫기
	if (m_pSystem != NULL)
	{
		m_pSystem->release();
		m_pSystem->close();
	}
	if (m_p3Dsystem != NULL)
	{
		m_p3Dsystem->release();
		m_p3Dsystem->close();
	}
}

void cSoundManager::update(void)
{
	// : >> 3D 사운드 업데이트
	m_p3Dsystem->update();
	//FMOD_System_Update(m_p3Dsystem);
}


void cSoundManager::addSound(string keyName, string soundName, bool bgm, bool loop)
{

	if (loop)//브금 or 이펙트
	{
		if (bgm)
		{
			//1: 파일 URL의 이름 
			//2: 사운드를 열기 위한 옵션
			//3: 사운드를 재생하는 동안 유저에게 정보를 제공하는 FMOD_CREATESOUNDEXINFO의 포인터 , 이옵션을 무시하려면 0 또는 NULL을 지정하면 됨
			//4: FMOD::SOUND 오브젝트를 받는 변수의 주소
			m_pSystem->createStream(soundName.c_str(), FMOD_LOOP_NORMAL, 0, &m_pSound[m_pTotalSounds.size()]);
		}
		else
		{
			m_pSystem->createSound(soundName.c_str(), FMOD_LOOP_NORMAL, 0, &m_pSound[m_pTotalSounds.size()]);
		}
	}

	else  //이펙트
	{
		m_pSystem->createSound(soundName.c_str(), FMOD_DEFAULT, 0, &m_pSound[m_pTotalSounds.size()]);
	}

	//맵에 사운드를 키값과 함께 담아준다
	m_pTotalSounds.insert(make_pair(keyName, &m_pSound[m_pTotalSounds.size()]));
}


void cSoundManager::play(string keyName, float volume)
{
	bool playing;

	arrSoundIter iter = m_pTotalSounds.begin();
	for (iter; iter != m_pTotalSounds.end(); ++iter)
	{
		if (keyName == iter->first)
		{
			for (int i = 0; i < TOTALSOUNDBUFFER; i++)
			{
				m_pChannel[i]->isPlaying(&playing);
				if (playing) continue;

				//사운드 플레이
				m_pSystem->playSound(FMOD_CHANNEL_FREE, *iter->second, false, &m_pChannel[i]);
				//볼륨셋팅
				m_pChannel[i]->setVolume(volume * s_volume);
				break;
			}

		}
	}
}

void cSoundManager::bgmPlay(string keyName, float volume)
{
	int count = 0;
	arrSoundIter iter = m_pTotalSounds.begin();
	for (iter; iter != m_pTotalSounds.end(); ++iter, count++)
	{
		if (keyName == iter->first)
		{
			//사운드 플레이
			m_pSystem->playSound(FMOD_CHANNEL_FREE, *iter->second, false, &m_pChannel[count]);
			//볼륨셋팅
			m_pChannel[count]->setVolume(volume * b_volume);
		}
	}
}

void cSoundManager::stop(string keyName)
{
	int count = 0;
	arrSoundIter iter = m_pTotalSounds.begin();
	for (iter; iter != m_pTotalSounds.end(); ++iter, count++)
	{
		if (keyName == iter->first)
		{
			//사운드 정지
			m_pChannel[count]->stop();
			break;
		}
	}
}

void cSoundManager::pause(string keyName)
{
	int count = 0;
	arrSoundIter iter = m_pTotalSounds.begin();
	for (iter; iter != m_pTotalSounds.end(); ++iter, count++)
	{
		if (keyName == iter->first)
		{
			//사운드 정지
			m_pChannel[count]->setPaused(true);
			break;
		}
	}
}

void cSoundManager::resume(string keyName)
{
	int count = 0;
	arrSoundIter iter = m_pTotalSounds.begin();
	for (iter; iter != m_pTotalSounds.end(); ++iter, count++)
	{
		if (keyName == iter->first)
		{
			//사운드 정지
			m_pChannel[count]->setPaused(true);
			break;
		}
	}
}

void cSoundManager::Play3DSound(string keyName, D3DXVECTOR3 pos, float volume)
{
	FMOD_VECTOR vel;
	FMOD_VECTOR Pos;
	vel.x = 1;
	vel.y = 1;
	vel.z = 1;
	Pos.x = pos.x;
	Pos.y = pos.y;
	Pos.z = pos.z;

	bool playing;
	auto iter = m_p3DsoundList.begin();
	for (iter; iter != m_p3DsoundList.end(); ++iter)
	{
		if (keyName == iter->first)
		{
			for (int i = 0; i < TOTALSOUNDBUFFER; i++)
			{
				m_p3DChannel[i]->isPlaying(&playing);
				if (playing) continue;

				//사운드 플레이
				m_p3Dsystem->playSound(FMOD_CHANNEL_FREE, *iter->second, false, &m_p3DChannel[i]);
				m_p3DChannel[i]->set3DAttributes(&Pos, &vel);
				//볼륨셋팅
				m_p3DChannel[i]->setVolume(volume * s_volume);
				break;
			}

		}
	}

}

void cSoundManager::Stop3DSound(string keyName)
{
	int count = 0;
	auto iter = m_p3DsoundList.begin();
	for (iter; iter != m_p3DsoundList.end(); ++iter, count++)
	{
		if (keyName == iter->first)
		{
			//사운드 정지
			m_p3DChannel[count]->stop();
			break;
		}
	}
}

bool cSoundManager::add3DSound(string keyName, string soundName, float MinDistance, float MaxDistance)
{
	unsigned int length;
	FMOD_RESULT result =
		m_p3Dsystem->createSound(soundName.c_str(), FMOD_3D_LINEARSQUAREROLLOFF, NULL, &m_p3DSound[m_p3DsoundList.size()]);
	result = m_p3DSound[m_p3DsoundList.size()]->set3DMinMaxDistance(MinDistance, MaxDistance);

	bool bLoadedOk;

	if (result != FMOD_OK) {
		bLoadedOk = false;
		printf("Sound Load fail : < %s > 사운드 파일을 찾을수 없습니다. \n", soundName.c_str());
	}
	else {
		bLoadedOk = true;
		printf("Sound Load success : < %s > 사운드 파일 추가완료.\n", soundName.c_str());
		m_p3DSound[m_p3DsoundList.size()]->getLength(&length, FMOD_TIMEUNIT_PCM);
		m_p3DsoundList.insert(make_pair(keyName, &m_p3DSound[m_p3DsoundList.size()]));
	}
	return bLoadedOk;

}

void cSoundManager::Update_Listener(D3DXVECTOR3 vPosition)
{
	// : 리스너 위치 셋팅
	listenerPos.x = vPosition.x;
	listenerPos.y = vPosition.y;
	listenerPos.z = vPosition.z;

	// : 퍼지는 속도 ? 셋팅
	listenerVelocity.x = 1.0f;
	listenerVelocity.y = 1.0f;
	listenerVelocity.z = 1.0f;

	// : z 방향 셋팅
	listenerForward.x = 0.0f;
	listenerForward.y = 0.0f;
	listenerForward.z = 1.0f;

	// : 서있는 방향 셋팅 ( up )
	listenerUp.x = 0.0f;
	listenerUp.y = 1.0f;
	listenerUp.z = 0.0f;

	m_p3Dsystem->set3DListenerAttributes(0, &listenerPos, &listenerVelocity, &listenerForward, &listenerUp);
}

void cSoundManager::Update_3Dsound(string keyName, D3DXVECTOR3 vPosition)
{
	soundPosition.x = vPosition.x;
	soundPosition.y = vPosition.y;
	soundPosition.z = vPosition.z;

	int count = 0;
	auto iter = m_p3DsoundList.begin();
	for (iter; iter != m_p3DsoundList.end(); ++iter, count++)
	{
		if (keyName == iter->first)
		{
			m_p3DChannel[count]->set3DAttributes(&soundPosition, &soundVelocity);
			break;
		}
	}
}
void cSoundManager::Allstop()
{
	for (int i = 0; i < TOTALSOUNDBUFFER; i++)
	{
		m_pChannel[i]->stop();
	}

}