#pragma once
#include "cTransform.h"
enum GUNTYPE
{
	GUN_AR,
	GUN_SHOTGUN,
	GUN_SR,
	GUN_NONE
};

struct tag_GUN
{
	GUNTYPE _guntype;
	string name;
	float m_range;		// ��Ÿ�
	float m_dmg;			// ���ݷ�
	float m_fireSpeed;	// ����
	float m_reaction;		// �ݵ�
	float m_reactionX;		// �ݵ�
	float m_ar;			// ���߷� ���� 
	float m_argap;		// ���߷� ��������
	float m_arpower;	// ���߷� �������� ����
	int m_nAmmo;		// ���� �Ѿ�
	int m_tAmmo;		// ���� ���� �� �ִ� �� �Ѿ�
	int m_bulletnum;
	int m_price;
	float shotTime;
	float cri;
};
class Gun
{
private:
	string fileName;

	cBaseObject* m_body;
	cBaseObject* m_bullet;

	cTransform* m_handle;
	cTransform* transmain;
	cTransform* m_muzzle;
	cTransform* scope;
	cTransform* m_muzzle2;

	tag_GUN _gun;
	float Max_ar;

	bool* is_reload;
	//int m_range;		// ��Ÿ�
	//int m_dmg;			// ���ݷ�
	//int m_fireSpeed;	// ����
	//int m_reaction;		// �ݵ�
	//int m_ar;			// ���߷�
	//int m_nAmmo;		// ���� �Ѿ�
	//int m_tAmmo;		// ���� ���� �� �ִ� �� �Ѿ�
	//float shotTime;

public:
	Gun();
	~Gun();

	void Sethandle(cTransform* handle)
	{
		handle->AddChild(m_body->pTransform);
	}

	void Init(GUNTYPE _gunType, cTransform* _hand);
	void Update(float timeDelta);
	void Render();

	//bool Getfirespeed(float timeDelta, int nVKey);
	bool Getfirespeed();
	void SetPlay(string an);
	inline bool GetReload() { return *is_reload; }
	inline cTransform* GetMuzzle(bool zoom) {
		if (zoom)
		{
			return m_muzzle2;
		}
		else
		{
			return m_muzzle;
		}
	}
	inline tag_GUN Getgun() { return _gun; }
	inline cBaseObject* getdd() { return m_body; }

	inline void SetFirebullet() { _gun.m_nAmmo--; }
	inline void Fire()
	{
		_gun.shotTime = _gun.m_fireSpeed;
		if (_gun.m_argap > Max_ar)
		{
			_gun.m_argap -= _gun.m_arpower*1.5;
		}
	}
	void NoFire(bool zoom) {

		if (zoom&&_gun._guntype != GUN_SHOTGUN)
		{
			_gun.m_argap += _gun.m_arpower*0.7;
			//if (_gun.shotTime <= 0.0f)
			{
				if (100 < _gun.m_argap)
				{
					//���� ���߷����� �������� ����
					_gun.m_argap = 100;
				}
			}
		}
		else
		{
			_gun.m_argap += _gun.m_arpower*0.7;
			//if (_gun.shotTime <= 0.0f)
			{
				if (_gun.m_ar < _gun.m_argap)
				{
					//���� ���߷����� �������� ����
					_gun.m_argap = _gun.m_ar;
				}
			}
		}

	}

	void SetReload(int* bulletnum);

};

