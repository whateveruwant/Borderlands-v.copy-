#include "stdafx.h"
#include "cParticleBullet.h"


cParticleBullet::cParticleBullet()
{
}


cParticleBullet::~cParticleBullet()
{
}

void cParticleBullet::Init(float _speed, const VEC_COLOR & colors, const VEC_SCALE & scales, LPDIRECT3DTEXTURE9 pPaticleTexture)
{
	mTransform = new cTransform();

	m_Colors = colors;											//��ƼŬ �÷� �迭
	m_Scales = scales;											//��ƼŬ ������ �迭
	terrainpos = D3DXVECTOR3(0, 0, 0);
	speed = _speed;
	randAngle = 0.0;
	m_pTex = pPaticleTexture;
	m_box = new cBoundBox();
	m_box->SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0.3, 0.3, 2.3));
	speed2 = 0;
	range = 0;

	randAngleX = 0;
	randAngleY = 0;
}

void cParticleBullet::StartOneParticle()
{

	//���̺� Ÿ�� ����

	//��ƼŬ�� ������ ��ġ
	D3DXVECTOR3 position;

	position = this->mTransform->GetWorldPosition();

	//���� ����
	D3DXVECTOR3 velocity;
	float angleX = randAngleX;
	float angleY = randAngleY;
	velocity.x = RandomFloatRange(-randAngle, randAngle) + angleX;
	velocity.y = RandomFloatRange(-randAngle, randAngle) + angleY;
	velocity.z = speed;

	D3DXVECTOR3 accelation;
	if (speed2 == 0)
	{
		accelation.x = 0;
		accelation.y = 0;

	}
	else
	{

		accelation.x = (-angleX / range * 4)*(speed);
		accelation.y = (-angleY / range * 4)*(speed);
	}
	accelation.z = speed - speed2;


	//�ڽ��� ���� ��Ʈ������ ������ �´�.
	//	if (m_bLocal == false)
	{
		D3DXMATRIXA16 matWorld = this->mTransform->GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matWorld);
		D3DXVec3TransformNormal(&accelation, &accelation, &matWorld);
	}


	//�����ϵ� ����
	float scale = RandomFloatRange(0.2, 0.2);
	cParticle* _cParticle = new cParticle();
	LPPARTICLE_VERTEX ParticleVerticles = new tagPARTICLE_VERTEX();
	m_ParticleVerticles.push_back(ParticleVerticles);
	m_pPaticles.push_back(_cParticle);
	//������� �߻���Ų��
	//m_pPaticles.back()->m_Transform = *mTransform;
	m_pPaticles.back()->m_Transform.SetWorldPosition(mTransform->GetWorldPosition());
	m_pPaticles.back()->m_Transform.SetRotateWorld(mTransform->GetWorldRotateQuaternion());
	m_pPaticles.back()->Start(
		1000,
		&position, &velocity, &accelation, 0.7);
	speed2 = 0;
	range = 0;
	randAngleX = 0;
	randAngleY = 0;

}

void cParticleBullet::Update(float timeDelta)
{
	for (int i = 0; i < m_pPaticles.size();)
	{
		m_pPaticles[i]->bulletUpdate(timeDelta);
		D3DXVECTOR3 dir = mTransform->GetWorldPosition() - m_pPaticles[i]->m_Transform.GetWorldPosition();
		float distSq = D3DXVec3Length(&dir);
		if (distSq > 1000)
		{
			m_pPaticles.erase(m_pPaticles.begin() + i);
			m_ParticleVerticles.erase(m_ParticleVerticles.begin() + i);
		}
		else
		{
			i++;
		}
	}
}

void cParticleBullet::UpdateForPlayer(float timeDelta)
{
	for (int i = 0; i < m_pPaticles.size();)
	{
		m_pPaticles[i]->bulletUpdate(timeDelta);
		D3DXVECTOR3 dir = m_pPaticles[i]->originPos - m_pPaticles[i]->m_Transform.GetWorldPosition();
		float distSq = D3DXVec3Length(&dir);
		if (distSq > 1000)
		{
			LOG_MGR->AddLog("�Ÿ������� ������");
			m_pPaticles.erase(m_pPaticles.begin() + i);
			m_ParticleVerticles.erase(m_ParticleVerticles.begin() + i);
		}
		else if (COLLISIONMGR->CheckPlayerOneBullet(i)) {
		}
		else
		{
			i++;
		}
	}
}

void cParticleBullet::UpdateForEnemy(float timeDelta)
{
	for (int i = 0; i < m_pPaticles.size();)
	{
		//m_pPaticles[i]->Update(timeDelta);
		m_pPaticles[i]->bulletUpdate(timeDelta);
		D3DXVECTOR3 dir = mTransform->GetWorldPosition() - m_pPaticles[i]->m_Transform.GetWorldPosition();
		float distSq = D3DXVec3Length(&dir);
		if (distSq > 1000)
		{
			LOG_MGR->AddLog("���ʹ̰Ÿ������� ������");
			m_pPaticles.erase(m_pPaticles.begin() + i);
			m_ParticleVerticles.erase(m_ParticleVerticles.begin() + i);
		}
		else if (COLLISIONMGR->CheckEnemyOneBullet(i)) {
		}
		else
		{
			i++;
		}
	}
}

void cParticleBullet::UpdateForBoss(float timeDelta)
{
	for (int i = 0; i < m_pPaticles.size();)
	{
		//m_pPaticles[i]->Update(timeDelta);
		m_pPaticles[i]->bulletUpdate(timeDelta);
		D3DXVECTOR3 dir = mTransform->GetWorldPosition() - m_pPaticles[i]->m_Transform.GetWorldPosition();
		float distSq = D3DXVec3Length(&dir);
		if (distSq > 1000)
		{
			LOG_MGR->AddLog("���ʹ̰Ÿ������� ������");
			m_pPaticles.erase(m_pPaticles.begin() + i);
			m_ParticleVerticles.erase(m_ParticleVerticles.begin() + i);
		}
		else if (COLLISIONMGR->CheckBossOneBullet(i)) {
		}
		else
		{
			i++;
		}
	}
}

void cParticleBullet::Render()
{
	//if (m_pPaticles.size() == 0)return;

	Device->SetRenderState(D3DRS_LIGHTING, false);		//�������� ����.
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);	//����Ʈ ���ö���Ʈ Ȱ��ȭ
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);	//����Ʈ�� �����ϰ� ���̰ڴ�.
	Device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));		//����Ʈ�� �ּ� ũ�� ( ȭ����� )
	Device->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(500.0f));		//����Ʈ�� �ִ� ũ�� ( ȭ����� )
																			//Device->SetRenderState( D3DRS_POINTSIZE, FloatToDWORD( 10.0f ) );			//����Ʈ ���� ������ ( ������ ����Ʈ ����� ������ ���õǴµ� );

	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);	//z ������ ���⸦ ���´�.



														//��µǴ� POINT size 
														//finalSize = viewportHeight * pointSize *  sqrt( 1 / ( A + B(D) + C(D^2) )  );
														//�Ʒ��� ���� �ϸ� �ڿ������� �Ÿ��� ���� �������� ��Ÿ��
	Device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(0.0f));
	Device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(0.0f));
	Device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f));

	//���� ���� ����
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//Texture �� ���� Diffuse ���⼭�� �����÷��� ���İ� �� ���� ���� ����� �Ѵ�.
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	D3DXMATRIXA16 matWorld;

	D3DXMatrixIdentity(&matWorld);

	Device->SetTransform(D3DTS_WORLD, &matWorld);


	//��ƼŬ Texture ����
	Device->SetTexture(0, m_pTex);

	//��ƼŬ ���� ���
	Device->SetFVF(PARTICLE_VERTEX::FVF);
	for (int i = 0; i < m_pPaticles.size(); i++)
	{
		m_pPaticles[i]->GetParticleVertex(m_ParticleVerticles[i], m_Colors, m_Scales);
		//m_pPaticles[i]->m_Transform.RenderGimozo();
		Device->DrawPrimitiveUP(
			D3DPT_POINTLIST,
			1,
			m_ParticleVerticles[i],
			sizeof(PARTICLE_VERTEX));
	}
	/*Device->DrawPrimitiveUP(
	D3DPT_POINTLIST,
	m_ParticleVerticles.size(),
	m_ParticleVerticles[0],
	sizeof(PARTICLE_VERTEX));*/
	/*Device->DrawPrimitiveUP(
	D3DPT_POINTLIST,
	m_pPaticles.size(),
	&m_ParticleVerticles,
	sizeof(PARTICLE_VERTEX));*/
	//��ƼŬ �׸��� ���� ��ó��
	Device->SetRenderState(D3DRS_LIGHTING, true);
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	Device->SetTexture(0, NULL);
}
