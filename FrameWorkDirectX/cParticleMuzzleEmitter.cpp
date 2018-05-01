#include "StdAfx.h"
#include "cParticleMuzzling.h"


cParticleMuzzlingEitter::cParticleMuzzlingEitter(void)
{
}


cParticleMuzzlingEitter::~cParticleMuzzlingEitter(void)
{
}


void cParticleMuzzlingEitter::Init(
	DWORD partcleNum,					//총 파티클 량
	float emission,						//초당 발생량
	float liveTimeMin,					//파티클하나의 시간
	float liveTimeMax,
	const D3DXVECTOR3& velocityMin,		//파티클 시작 속도
	const D3DXVECTOR3& velocityMax,
	const D3DXVECTOR3& accelMin,		//파티클 가속
	const D3DXVECTOR3& accelMax,
	const VEC_COLOR& colors,			//파티클 컬러 벡터배열
	const VEC_SCALE& scales,			//파티클 스케일 컬러배열
	float scaleMin,						//파티클 스케일
	float scaleMax,
	LPDIRECT3DTEXTURE9 pPaticleTexture,	//파티클 Texture
	bool bLocal  				//이게 true 면 파티클의 움직임이 나의Transform Local 기준으로 움직인다.
)
{
	//해당 파티클 랜더의 총 파티클 수
	m_PaticleNum = partcleNum;

	//총파티클 수만큼 버텍스 배열을 만든다.
	m_ParticleVerticles = new PARTICLE_VERTEX[m_PaticleNum];

	//파티클 객체 생성
	m_pParticlesMuzzle = new cParticleMuzzling[m_PaticleNum];

	//초당 생성량
	m_fEmissionPerSec = emission;

	//초당 생성량 따른 발생 간격
	m_fEmisionIntervalTime = 1.0f / m_fEmissionPerSec;

	//지난 시간도 0
	m_fEmisionDeltaTime = 0.0f;

	//발생 여부 false
	m_bEmission = false;


	//컬러 대입
	//m_Colors = colors;
	m_Colors.clear();
	for (int i = 0; i < colors.size(); i++)
		m_Colors.push_back(colors[i]);


	//사이즈 대입
	//m_Scales = scales;
	m_Scales.clear();
	for (int i = 0; i < scales.size(); i++)
		m_Scales.push_back(scales[i]);


	//시작 라이브 타임 대입
	m_fStartLiveTimeMin = liveTimeMin;
	m_fStartLiveTimeMax = liveTimeMax;

	//시작 속도 대입
	m_StartVelocityMin = velocityMin;
	m_StartVelocityMax = velocityMax;

	//시작 가속 대입
	m_StartAccelateMin = accelMin;
	m_StartAccelateMax = accelMax;


	//시작 스케일 대입
	m_fStartScaleMin = scaleMin;
	m_fStartScaleMax = scaleMax;


	//시작순번 초기화
	m_dwParticleCount = 0;

	//Texture 참조
	m_pTex = pPaticleTexture;


	m_bLocal = bLocal;


	EmissionType = PZERO;


}

void cParticleMuzzlingEitter::Release()
{
	SAFE_DELETE_ARR(m_pParticlesMuzzle);
	SAFE_DELETE_ARR(m_ParticleVerticles);
}

//사방 팔방으로 입자 퍼트린다.
void cParticleMuzzlingEitter::Burst(int num, float minSpeed, float maxSpeed, float maxLife, float minLife)
{
	for (int i = 0; i < num; i++)
	{
		D3DXVECTOR3 randVec(
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f));

		D3DXVec3Normalize(&randVec, &randVec);
		randVec *= RandomFloatRange(minSpeed, maxSpeed);


		StartOneParticle(randVec, RandomFloatRange(maxLife, minLife));
	}
}

void cParticleMuzzlingEitter::ParticleBurst(int num, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float minSpeed, float maxSpeed, float maxLife, float minLife)
{
	for (int i = 0; i < num; i++)
	{
		D3DXVECTOR3 randVec(
			RandomFloatRange(minX, maxX), //-0.5f, 0.5f),
			RandomFloatRange(minY, maxY), //-0.5f, 0.5f ),
			RandomFloatRange(minZ, maxZ)); //-0.5f,0.5f ) );

		D3DXVec3Normalize(&randVec, &randVec);
		randVec *= 100;// RandomFloatRange(minSpeed, maxSpeed);


		StartOneParticle(randVec, RandomFloatRange(maxLife, minLife));
	}
}

void cParticleMuzzlingEitter::BaseObjectUpdate(float timeDelta)
{
	//모든 파티클 업데이트
	for (int i = 0; i < m_PaticleNum; i++) {
		m_pParticlesMuzzle[i].Update(timeDelta);
	}

	//너가 지금 발생 상태니?
	if (m_bEmission) {

		//하나 발생하고 지난시간
		m_fEmisionDeltaTime += timeDelta;

		while (m_fEmisionDeltaTime >= m_fEmisionIntervalTime) {

			m_fEmisionDeltaTime -= m_fEmisionIntervalTime;
			//파티클 하나 발사
			StartOneParticleMuzle();
		}

	}





}
void cParticleMuzzlingEitter::BaseObjectRender()
{
	//그릴 파티클 수
	DWORD drawParticleNum = 0;

	for (int i = 0; i < m_PaticleNum; i++) {

		//해당파티클이 활성화 중이니?
		//if( m_pPaticles[i].isLive() ){
		//
		//	//해당 파티클의 정보를 얻는다.
		//	m_pPaticles[i].GetParticleVertex(
		//		m_ParticleVerticles + drawParticleNum,
		//		m_Colors, m_Scales );
		//
		//
		//	drawParticleNum++;
		//
		//}

		if (m_pParticlesMuzzle[i].isLive()) {

			//해당 파티클의 정보를 얻는다.
			m_pParticlesMuzzle[i].GetParticleVertex(
				m_ParticleVerticles + drawParticleNum,
				m_Colors, m_Scales);


			drawParticleNum++;

		}

	}


	Device->SetRenderState(D3DRS_LIGHTING, false);		//라이팅을 끈다.
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);	//포인트 스플라이트 활성화
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);	//포인트의 스케일값 먹이겠다.
	Device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));		//포인트의 최소 크기 ( 화면기준 )
	Device->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(500.0f));		//포인트의 최대 크기 ( 화면기준 )
																			//Device->SetRenderState( D3DRS_POINTSIZE, FloatToDWORD( 10.0f ) );			//포인트 기준 사이즈 ( 정점의 포인트 사이즈가 있으면 무시되는듯 );

	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);	//z 버퍼의 쓰기를 막는다.



														//출력되는 POINT size 
														//finalSize = viewportHeight * pointSize *  sqrt( 1 / ( A + B(D) + C(D^2) )  );
														//아래와 같이 하면 자연스러운 거리에 따른 스케일이 나타남
	Device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(0.0f));
	Device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(0.0f));
	Device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f));

	//알파 블렌딩 셋팅
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//Texture 의 값과 Diffuse 여기서는 정점컬러의 알파값 을 섞어 최종 출력을 한다.
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	D3DXMATRIXA16 matWorld;

	if (m_bLocal == false)
		D3DXMatrixIdentity(&matWorld);
	else
		matWorld = this->pTransform->GetFinalMatrix();

	Device->SetTransform(D3DTS_WORLD, &matWorld);


	//파티클 Texture 셋팅
	Device->SetTexture(0, m_pTex);

	//파티클 정점 출력
	Device->SetFVF(PARTICLE_VERTEX::FVF);
	Device->DrawPrimitiveUP(
		D3DPT_POINTLIST,
		drawParticleNum,
		m_ParticleVerticles,
		sizeof(PARTICLE_VERTEX));


	//파티클 그리고 난후 후처리
	Device->SetRenderState(D3DRS_LIGHTING, true);
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	Device->SetTexture(0, NULL);

}

//파티클 생성 시작
void cParticleMuzzlingEitter::StartEmission()
{
	m_bEmission = true;
}

//파티클 생성 중지
void cParticleMuzzlingEitter::StopEmission()
{
	m_bEmission = false;
}





///////////////////////////////////////////////////


//파티클 하나 생성
void cParticleMuzzlingEitter::StartOneParticle()
{
	//라이브 타임 랜덤
	float liveTime = RandomFloatRange(
		m_fStartLiveTimeMin, m_fStartLiveTimeMax);


	//파티클이 생성될 위치
	D3DXVECTOR3 position;

	//로컬이 아닌경우 자신의 월드 위치에서 시작하고 
	if (this->m_bLocal == false)
		position = this->pTransform->GetWorldPosition();

	//로컬인 경우 0 에서 시작한다.
	else
		position = D3DXVECTOR3(0, 0, 0);

	//생성 범위에 따른 추가 위치....
	if (EmissionType == PATICLE_EMISSION_TYPE::SPHERE)
	{
		//랜덤벡터
		D3DXVECTOR3 randDir(
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f));

		D3DXVec3Normalize(&randDir, &randDir);

		//랜덤거리
		float randDistance = RandomFloatRange(0, SphereEmissionRange);

		//추가위치
		position += randDir * randDistance;
	}

	else if (EmissionType == PATICLE_EMISSION_TYPE::SPHERE_OUTLINE)
	{
		//랜덤벡터
		D3DXVECTOR3 randDir(
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f));

		D3DXVec3Normalize(&randDir, &randDir);

		//추가위치
		position += randDir * SphereEmissionRange;
	}

	else if (EmissionType == PATICLE_EMISSION_TYPE::BOX)
	{
		//랜덤벡터
		D3DXVECTOR3 randDir(
			RandomFloatRange(MinEmissionRangeX, MaxEmissionRangeX),
			RandomFloatRange(MinEmissionRangeY, MaxEmissionRangeY),
			RandomFloatRange(MinEmissionRangeZ, MaxEmissionRangeZ));


		//추가위치
		position += randDir;
	}

	//벡터 랜덤
	D3DXVECTOR3 velocity;
	velocity.x = RandomFloatRange(m_StartVelocityMin.x, m_StartVelocityMax.x);
	velocity.y = RandomFloatRange(m_StartVelocityMin.y, m_StartVelocityMax.y);
	velocity.z = RandomFloatRange(m_StartVelocityMin.z, m_StartVelocityMax.z);

	D3DXVECTOR3 accelation;
	accelation.x = RandomFloatRange(m_StartAccelateMin.x, m_StartAccelateMax.x);
	accelation.y = RandomFloatRange(m_StartAccelateMin.y, m_StartAccelateMax.y);
	accelation.z = RandomFloatRange(m_StartAccelateMin.z, m_StartAccelateMax.z);


	//자신의 월드 매트릭스를 가지고 온다.
	if (m_bLocal == false)
	{
		D3DXMATRIXA16 matWorld = this->pTransform->GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matWorld);
		D3DXVec3TransformNormal(&accelation, &accelation, &matWorld);
	}


	//스케일도 랜덤
	float scale = RandomFloatRange(m_fStartScaleMin, m_fStartScaleMax);


	//순번대로 발생시킨다
	m_pParticlesMuzzle[m_dwParticleCount].Start(
		liveTime,
		&position, &velocity, &accelation, scale, pTransform);

	//다음 파티클을 위한 순번 증가
	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;
}

void cParticleMuzzlingEitter::StartOneParticleMuzle()
{
	//라이브 타임 랜덤
	float liveTime = RandomFloatRange(
		m_fStartLiveTimeMin, m_fStartLiveTimeMax);


	//파티클이 생성될 위치
	D3DXVECTOR3 position;

	//로컬이 아닌경우 자신의 월드 위치에서 시작하고 
	if (this->m_bLocal == false)
		position = this->pTransform->GetWorldPosition();

	//로컬인 경우 0 에서 시작한다.
	else
		position = D3DXVECTOR3(0, 0, 0);

	//생성 범위에 따른 추가 위치....
	if (EmissionType == PATICLE_EMISSION_TYPE::SPHERE)
	{
		//랜덤벡터
		D3DXVECTOR3 randDir(
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f));

		D3DXVec3Normalize(&randDir, &randDir);

		//랜덤거리
		float randDistance = RandomFloatRange(0, SphereEmissionRange);

		//추가위치
		position += randDir * randDistance;
	}

	else if (EmissionType == PATICLE_EMISSION_TYPE::SPHERE_OUTLINE)
	{
		//랜덤벡터
		D3DXVECTOR3 randDir(
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f),
			RandomFloatRange(-1.0f, 1.0f));

		D3DXVec3Normalize(&randDir, &randDir);

		//추가위치
		position += randDir * SphereEmissionRange;
	}

	else if (EmissionType == PATICLE_EMISSION_TYPE::BOX)
	{
		//랜덤벡터
		D3DXVECTOR3 randDir(
			RandomFloatRange(MinEmissionRangeX, MaxEmissionRangeX),
			RandomFloatRange(MinEmissionRangeY, MaxEmissionRangeY),
			RandomFloatRange(MinEmissionRangeZ, MaxEmissionRangeZ));


		//추가위치
		position += randDir;
	}

	//벡터 랜덤
	D3DXVECTOR3 velocity;
	velocity.x = RandomFloatRange(m_StartVelocityMin.x, m_StartVelocityMax.x);
	velocity.y = RandomFloatRange(m_StartVelocityMin.y, m_StartVelocityMax.y);
	velocity.z = RandomFloatRange(m_StartVelocityMin.z, m_StartVelocityMax.z);

	D3DXVECTOR3 accelation;
	accelation.x = RandomFloatRange(m_StartAccelateMin.x, m_StartAccelateMax.x);
	accelation.y = RandomFloatRange(m_StartAccelateMin.y, m_StartAccelateMax.y);
	accelation.z = RandomFloatRange(m_StartAccelateMin.z, m_StartAccelateMax.z);


	//자신의 월드 매트릭스를 가지고 온다.
	if (m_bLocal == false)
	{
		D3DXMATRIXA16 matWorld = this->pTransform->GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matWorld);
		D3DXVec3TransformNormal(&accelation, &accelation, &matWorld);
	}


	//스케일도 랜덤
	float scale = RandomFloatRange(m_fStartScaleMin, m_fStartScaleMax);


	//순번대로 발생시킨다
	m_pParticlesMuzzle[m_dwParticleCount].Start(
		liveTime,
		&position, &velocity, &accelation, scale, pTransform);

	//다음 파티클을 위한 순번 증가
	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;
}

//파티클 하나 생성 ( 방향 넣어서 )
void cParticleMuzzlingEitter::StartOneParticle(D3DXVECTOR3 dir, float life)
{

	float liveTime = life;

	//파티클이 생성될 위치
	D3DXVECTOR3 position;
	if (this->m_bLocal == false)
		position = pTransform->GetWorldPosition();
	else
		position = D3DXVECTOR3(0, 0, 0);


	//벡터 랜덤
	D3DXVECTOR3 velocity;
	velocity.x = dir.x;
	velocity.y = dir.y;
	velocity.z = dir.z;

	D3DXVECTOR3 accelation;
	accelation.x = RandomFloatRange(m_StartAccelateMin.x, m_StartAccelateMax.x);
	accelation.y = RandomFloatRange(m_StartAccelateMin.y, m_StartAccelateMax.y);
	accelation.z = RandomFloatRange(m_StartAccelateMin.z, m_StartAccelateMax.z);



	//자신의 월드 매트릭스를 가지고 온다.
	if (m_bLocal == false)
	{
		D3DXMATRIXA16 matWorld = this->pTransform->GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matWorld);
		D3DXVec3TransformNormal(&accelation, &accelation, &matWorld);
	}

	//스케일도 랜덤
	float scale = RandomFloatRange(m_fStartScaleMin, m_fStartScaleMax);


	//발생시킨다
	m_pParticlesMuzzle[m_dwParticleCount].Start(
		liveTime,
		&position, &velocity, &accelation, scale, pTransform);


	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;

}