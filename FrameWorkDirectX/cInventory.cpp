#include "stdafx.h"
#include "cInventory.h"


cInventory::cInventory()
{
}


cInventory::~cInventory()
{
}

HRESULT cInventory::Init()
{
	mcs = MCS_NONE;
	Menu = MS_INVEN;
	Inven = IV_LEFT;
	choice = CHOICE_NONE;
	shop = SS_SHOP;

	mouseimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/mouse.png");
	minimapbox = RESOURCE_TEXTURE->GetResource("../Resources/ui/minimapbox.png");
	playerimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/playerimg.png");
	bulletRifleimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/bulletRifle.png");
	bulletShotgunimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/bulletShotgun.png");
	bulletsniperimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/bulletsniper.png");
	healimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/heal.png");
	line = RESOURCE_TEXTURE->GetResource("../Resources/ui/line.png");
	line2 = RESOURCE_TEXTURE->GetResource("../Resources/ui/line2.png");
	shopimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/shopimg.png");
	mapimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/mapimg.png");
	invenimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/invenimg.png");
	shieldimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/shieldimg.png");

	minimapimg = RESOURCE_TEXTURE->GetResource("zoneFile/minimap.png");

	alpha = RESOURCE_TEXTURE->GetResource("../Resources/ui/alpha.png");
	mainbox = RESOURCE_TEXTURE->GetResource("../Resources/ui/mainbox.png");
	smallbox = RESOURCE_TEXTURE->GetResource("../Resources/ui/smallbox.png");

	arimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/2Drifle.png");
	srimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/2dsi.png");
	sgimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/2Dshotgun.png");

	num1 = RESOURCE_TEXTURE->GetResource("../Resources/ui/num1.png");
	num2 = RESOURCE_TEXTURE->GetResource("../Resources/ui/num2.png");
	num3 = RESOURCE_TEXTURE->GetResource("../Resources/ui/num3.png");
	arbulletimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/rbullet.png");
	srbulletimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/srbullet.png");
	sgbulletimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/sgbullet.png");

	shiled1 = RESOURCE_TEXTURE->GetResource("../Resources/ui/shiled1.png");
	shiled2 = RESOURCE_TEXTURE->GetResource("../Resources/ui/shiled2.png");

	leftimgX = 500;
	leftimgY = 100;
	rightimgX = 850;
	rightimgY = 150;

	leftscaleimgX = 0.7;
	leftscaleimgY = 1.16;

	rightscaleX = 0.5;
	rightscaleY = 0.83;
	count = 0;
	gunindex = 0;

	rcLeft = { 500,200 ,510,515 };
	rcRight = { 500,200 ,510,515 };

	tag_shiled* _tag_shiled = new tag_shiled;
	_tag_shiled->name = SHILED_ONE;
	_tag_shiled->shiled = 20;
	_tag_shiled->maxshiled = _tag_shiled->shiled;
	_tag_shiled->recover = 4;
	_tag_shiled->recovertime = 0.0f;
	_tag_shiled->recoverResettime = 1.0f;
	_tag_shiled->recoverResetdmgtime = 5.0f;
	_tag_shiled->price = 50;
	shopshiled.push_back(_tag_shiled);

	_tag_shiled = new tag_shiled;
	_tag_shiled->name = SHILED_TWO;
	_tag_shiled->shiled = 30;
	_tag_shiled->maxshiled = _tag_shiled->shiled;
	_tag_shiled->recover = 2;
	_tag_shiled->recovertime = 0.0f;
	_tag_shiled->recoverResettime = 1.0f;
	_tag_shiled->recoverResetdmgtime = 5.0f;
	_tag_shiled->price = 50;
	shopshiled.push_back(_tag_shiled);

//	invenshiled.push_back(_tag_shiled);

	miniscale = 1;
	return S_OK;
}

void cInventory::Release()
{
}

bool cInventory::Update(float timeDelta)
{
	if (KEY_MGR->IsOnceDown(VK_TAB))
	{
		SOUND_MGR->play("인벤토리열때", 1.0f);
		SOUND_MGR->update();
		if (mcs == MCS_NONE)mcs = MCS_MENU;
		else mcs = MCS_NONE;
	}

	if (mcs == MCS_NONE)
	{
		mcs = MCS_NONE;
		Menu = MS_INVEN;
		Inven = IV_LEFT;
		choice = CHOICE_NONE;
		shop = SS_SHOP;
		if (leftimgX<500)
		{
			leftimgX += 60;
			leftimgY -= 5;
			leftscaleimgX *= 1.05;
			leftscaleimgY *= 1.05;
		}
		if (rightimgX<850)
		{
			rightimgX += 60;
			rightimgY += 5;
			rightscaleX /= 1.05;
			rightscaleY /= 1.05;
		}
		return false;
	}
	count++;
	rcLeft = { leftimgX,leftimgY ,leftimgX + 250,leftimgY + 515 };
	rcRight = { rightimgX,rightimgY ,rightimgX + 250,rightimgY + 515 };
	if (mcs == MCS_MENU)
	{
		if (KEY_MGR->IsOnceDown(VK_RBUTTON))
		{
			if (Menu == MS_INVEN)
			{
				if (Inven == IV_LEFT)
				{
					for (int i = 1; i < playerGun[0].size(); i++)
					{
						RECT gunbox = { leftimgX + 80,leftimgY + 40 + i * 91,leftimgX + 80 + 200,leftimgY + 40 + 100 + i * 91 };
						if (PtInRect(&gunbox, GetMousePos()))
						{
							SOUND_MGR->play("버튼클릭", 1.0f);
							SOUND_MGR->update();
							invenGun.push_back(playerGun[0][i]);
							playerGun[0].erase(playerGun[0].begin() + i);
						}
					}
				}
			}
		}

		if (KEY_MGR->IsOnceDown(VK_LBUTTON))
		{
			RECT rc = { 630,20,680,70 };
			RECT rc2 = { 690,20,740,70 };
			if (PtInRect(&rc, GetMousePos()))
			{
				SOUND_MGR->play("버튼클릭", 1.0f);
				SOUND_MGR->update();
				Menu = MS_MAP;
			}
			else if (PtInRect(&rc2, GetMousePos()))
			{
				SOUND_MGR->play("버튼클릭", 1.0f);
				SOUND_MGR->update();
				Menu = MS_INVEN;
			}
			else if (Menu == MS_INVEN)
			{
				if (Inven == IV_LEFT)
				{
					if (choice == CHOICE_GUN)
					{
						for (int i = 0; i < 3; i++)
						{
							RECT gunbox = { leftimgX + 80,leftimgY + 40 + i * 91,leftimgX + 80 + 200,leftimgY + 40 + 100 + i * 91 };
							if (PtInRect(&gunbox, GetMousePos()))
							{
								SOUND_MGR->play("버튼클릭", 1.0f);
								SOUND_MGR->update();
								if (playerGun[0].size()>i)
								{
									invenGun.push_back(playerGun[0][i]);
									playerGun[0].erase(playerGun[0].begin() + i);
									playerGun[0].insert(playerGun[0].begin() + i, invenGun[gunindex]);
									invenGun.erase(invenGun.begin() + gunindex);
									choice = CHOICE_NONE;
									break;

								}
								else
								{
									playerGun[0].push_back(invenGun[gunindex]);
									invenGun.erase(invenGun.begin() + gunindex);
									choice = CHOICE_NONE;
									break;
								}

							}
						}
					}
					else if (choice == CHOICE_SHIELD)
					{
						RECT gunbox = { leftimgX + 80,leftimgY + 470,leftimgX + 80 + 200,leftimgY + 560 };
						if (PtInRect(&gunbox, GetMousePos()))
						{
							SOUND_MGR->play("버튼클릭", 1.0f);
							SOUND_MGR->update();
							if (playerShiled->name != SHILED_NONE)
							{
								tag_shiled* newShiled = new tag_shiled();
								*newShiled = *playerShiled;
								*playerShiled = *invenshiled[gunindex];
								invenshiled.erase(invenshiled.begin() + gunindex);
								invenshiled.push_back(newShiled);
								choice = CHOICE_NONE;
							}
							else
							{
								*playerShiled = *invenshiled[gunindex];
								invenshiled.erase(invenshiled.begin() + gunindex);
								choice = CHOICE_NONE;
							}

						}
					}
					else
					{
						for (int i = 0; i < playerGun[0].size(); i++)
						{
							RECT gunbox = { leftimgX + 80,leftimgY + 40 + i * 91,leftimgX + 80 + 200,leftimgY + 40 + 100 + i * 91 };
							if (PtInRect(&gunbox, GetMousePos()))
							{
								SOUND_MGR->play("버튼클릭", 1.0f);
								SOUND_MGR->update();
								Inven = IV_RIGHT;
								choice = CHOICE_GUN;
								gunindex = i;

								if (invenGun.size() == 0)choice = CHOICE_NONE;
							}
						}
						RECT gunbox = { leftimgX + 80,leftimgY + 470,leftimgX + 80 + 200,leftimgY + 560 };
						if (PtInRect(&gunbox, GetMousePos()) && playerShiled->name != SHILED_NONE)
						{
							SOUND_MGR->play("버튼클릭", 1.0f);
							SOUND_MGR->update();
							tag_shiled* _tag_shiled = new tag_shiled;
							*_tag_shiled = *playerShiled;
							invenshiled.push_back(_tag_shiled);
							playerShiled->name = SHILED_NONE;
						}
					}

					if (PtInRect(&rcRight, GetMousePos())) {
						SOUND_MGR->play("버튼클릭", 1.0f);
						SOUND_MGR->update();
						Inven = IV_RIGHT;
					}
				}
				else if (Inven == IV_RIGHT)
				{

					if (choice == CHOICE_GUN)
					{
						for (int i = 0; i < invenGun.size(); i++)
						{
							RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 40 + 100 + i * 91 };
							if (PtInRect(&gunbox, GetMousePos()))
							{
								SOUND_MGR->play("버튼클릭", 1.0f);
								SOUND_MGR->update();
								Gun* newGun = invenGun[i];
								invenGun[i] = playerGun[0][gunindex];
								playerGun[0][gunindex] = newGun;
								choice = CHOICE_NONE;
							}
						}
					}
					else if (choice == CHOICE_SHIELD)
					{
					}
					else
					{
						for (int i = 0; i < invenGun.size(); i++)
						{
							RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 40 + 100 + i * 91 };
							if (PtInRect(&gunbox, GetMousePos()))
							{
								SOUND_MGR->play("버튼클릭", 1.0f);
								SOUND_MGR->update();
								Inven = IV_LEFT;
								choice = CHOICE_GUN;
								gunindex = i;
							}
						}

						for (int i = 0; i < invenshiled.size(); i++)
						{
							RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
							if (PtInRect(&gunbox, GetMousePos()))
							{
								SOUND_MGR->play("버튼클릭", 1.0f);
								SOUND_MGR->update();
								Inven = IV_LEFT;
								choice = CHOICE_SHIELD;
								gunindex = i;
							}
						}
					}



					if (PtInRect(&rcLeft, GetMousePos())) {
						SOUND_MGR->play("버튼클릭", 1.0f);
						SOUND_MGR->update();
						Inven = IV_LEFT;
					}

				}
			}
			else if (Menu == MS_MAP)
			{

			}
		}
		if (Menu == MS_INVEN)
		{
			if (Inven == IV_LEFT)
			{
				if (leftimgX<500)
				{
					leftimgX += 60;
					leftimgY -= 5;
					leftscaleimgX *= 1.05;
					leftscaleimgY *= 1.05;
				}
				if (rightimgX<850)
				{
					rightimgX += 60;
					rightimgY += 5;
					rightscaleX /= 1.05;
					rightscaleY /= 1.05;
				}
			}
			else if (Inven == IV_RIGHT)
			{
				if (leftimgX>200)
				{
					leftimgX -= 60;
					leftimgY += 5;
					leftscaleimgX /= 1.05;
					leftscaleimgY /= 1.05;
				}

				if (rightimgX>500)
				{
					rightimgX -= 60;
					rightimgY -= 5;
					rightscaleX *= 1.05;
					rightscaleY *= 1.05;
				}
			}

		}
		else if (Menu == MS_MAP)
		{

		}
	}
	else if (mcs == MCS_SHOP)
	{
		if (leftimgX>200)
		{
			leftimgX -= 60;
			leftimgY += 5;
			leftscaleimgX /= 1.05;
			leftscaleimgY /= 1.05;
		}

		if (rightimgX>500)
		{
			rightimgX -= 60;
			rightimgY -= 5;
			rightscaleX *= 1.05;
			rightscaleY *= 1.05;
		}


		if (KEY_MGR->IsOnceDown(VK_LBUTTON))
		{
			RECT rc = { 630,20,680,70 };
			RECT rc2 = { 690,20,740,70 };
			if (PtInRect(&rc, GetMousePos()))
			{
				SOUND_MGR->play("구매", 1.0f);
				SOUND_MGR->update();
				shop = SS_SHOP;
			}
			else if (PtInRect(&rc2, GetMousePos()))
			{
				SOUND_MGR->play("구매", 1.0f);
				SOUND_MGR->update();
				shop = SS_INVEN;
			}
			else if (shop == SS_SHOP)
			{
				for (int i = 0; i < shopGun.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SOUND_MGR->play("구매", 1.0f);
						SOUND_MGR->update();
						if (*money >= shopGun[i]->Getgun().m_price)
						{
							*money -= shopGun[i]->Getgun().m_price;
							invenGun.push_back(shopGun[i]);
						}
					}
				}
				for (int i = 0; i < shopshiled.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + (shopGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + (shopGun.size() + i) * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SOUND_MGR->play("구매", 1.0f);
						SOUND_MGR->update();
						if (*money >= shopshiled[i]->price)
						{
							*money -= shopshiled[i]->price;
							invenshiled.push_back(shopshiled[i]);
						}
					}
				}
			}
			else if (shop == SS_INVEN)
			{
				/*for (int i = 0; i < invenGun.size(); i++)
				{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))

				}
				for (int i = 0; i < invenshiled.size(); i++)
				{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				ShiledTextRender2(shopshiled[i]);
				}*/
			}
		}
		if (KEY_MGR->IsOnceDown(VK_RBUTTON))
		{
			if (shop == SS_INVEN)
			{
				for (int i = 0; i < invenGun.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SOUND_MGR->play("구매", 1.0f);
						SOUND_MGR->update();
						*money += invenGun[i]->Getgun().m_price / 2;
						invenGun.erase(invenGun.begin() + i);
					}
				}
				for (int i = 0; i < invenshiled.size(); i++)
				{
					SOUND_MGR->play("구매", 1.0f);
					SOUND_MGR->update();
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						*money += invenshiled[i]->price / 2;
						invenshiled.erase(invenshiled.begin() + i);
					}
				}
			}
		}
	}
	else if (mcs == MCS_BULLETSHOP)
	{
		if (leftimgX>200)
		{
			leftimgX -= 60;
			leftimgY += 5;
			leftscaleimgX /= 1.05;
			leftscaleimgY /= 1.05;
		}

		if (rightimgX>500)
		{
			rightimgX -= 60;
			rightimgY -= 5;
			rightscaleX *= 1.05;
			rightscaleY *= 1.05;
		}


		if (KEY_MGR->IsOnceDown(VK_LBUTTON))
		{
			RECT rc = { 630,20,680,70 };
			RECT rc2 = { 690,20,740,70 };
			if (PtInRect(&rc, GetMousePos()))
			{
				SOUND_MGR->play("구매", 1.0f);
				SOUND_MGR->update();
				shop = SS_SHOP;
			}
			else if (PtInRect(&rc2, GetMousePos()))
			{
				SOUND_MGR->play("구매", 1.0f);
				SOUND_MGR->update();
				shop = SS_INVEN;
			}
			else if (shop == SS_SHOP)
			{
				/*	ITEM_RIFLEBULLET,
				ITEM_SHOTGUNBULLET,
				ITEM_SNIPERBULLET,
				ITEM_HEAL,
				ITEM_NONE*/
				for (int i = 0; i < ITEM_NONE; i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SOUND_MGR->play("구매", 1.0f);
						SOUND_MGR->update();
						if (i == ITEM_RIFLEBULLET)
						{
							if (*money >= 10)
							{
								*bulletRifleNum += 20;
								*money -= 10;
							}
						}
						else if (i == ITEM_SHOTGUNBULLET)
						{
							if (*money >= 10)
							{
								*bulletShotgunNum += 20;
								*money -= 10;
							}
						}
						else if (i == ITEM_SNIPERBULLET)
						{
							if (*money >= 10)
							{
								*bulletSniperNum += 20;
								*money -= 10;
							}

						}
						else if (i == ITEM_HEAL)
						{
							if (*money >= 10 && *hp < *maxhp)
							{
								*hp = *maxhp;
								*money -= 10;
							}
						}
					}
				}
			}
			else if (shop == SS_INVEN)
			{
			}
		}
		if (KEY_MGR->IsOnceDown(VK_RBUTTON))
		{
			if (shop == SS_INVEN)
			{
				for (int i = 0; i < invenGun.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SOUND_MGR->play("구매", 1.0f);
						SOUND_MGR->update();
						*money += invenGun[i]->Getgun().m_price / 2;
						invenGun.erase(invenGun.begin() + i);
					}
				}
				for (int i = 0; i < invenshiled.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SOUND_MGR->play("구매", 1.0f);
						SOUND_MGR->update();
						*money += invenshiled[i]->price / 2;
						invenshiled.erase(invenshiled.begin() + i);
					}
				}
			}
		}
	}


	return true;
}

bool cInventory::Render()
{
	if (mcs == MCS_NONE)return false;

	return true;
}

bool cInventory::Scene_RenderSprite()
{
	if (mcs == MCS_NONE)return false;


	RECT numbox = { 0,0,63,31 };
	RECT boxsmall = { 0,0,510,125 };
	RECT boxrc = { 0,0,510,514 };
	RECT boxgun = { 0,0,510,514 };
	RECT boxshile = { 0,0,64,64 };
	RECT mcrc = { 0,0,64,64 };
	RECT linebox = { 0,0,2100,850 };
	RECT linebox2 = { 0,250,WINSIZE_X,500 };

	//
	SPRITE_MGR->DrawTexture(line, &linebox, 0, 170, 0.65, 1, 0);
	SPRITE_MGR->DrawLoop(line2, -count, 170, &linebox, &linebox2);
	SPRITE_MGR->DrawLoop(line2, WINSIZE_X / 2 - count / 2, 170, &linebox, &linebox2);
	RECT rc = { 630,20,680,70 };
	RECT rc2 = { 690,20,740,70 };

	if (mcs == MCS_MENU)
	{
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 630, 20, 0.1, 0.4, 0);
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 690, 20, 0.1, 0.4, 0);


		SPRITE_MGR->DrawTexture(mapimg, &mcrc, 633, 21, 0.7, 0.7, 0);
		SPRITE_MGR->DrawTexture(invenimg, &mcrc, 693, 21, 0.7, 0.7, 0);

		if (PtInRect(&rc, GetMousePos()))
		{
			SPRITE_MGR->DrawTexture(alpha, &boxsmall, 630, 20, 0.1, 0.4, 0);
		}
		else if (PtInRect(&rc2, GetMousePos()))
		{
			SPRITE_MGR->DrawTexture(alpha, &boxsmall, 690, 20, 0.1, 0.4, 0);
		}

		if (Menu == MS_INVEN)
		{

			if (Inven == IV_LEFT)
			{
				//alpha
				SPRITE_MGR->DrawTexture(mainbox, &boxrc, rightimgX, rightimgY, rightscaleX, rightscaleY, 0);
				for (int i = 0; i < invenGun.size(); i++)
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 60, rightimgY + 36 + i * 75, rightscaleX / 1.5, rightscaleY / 1.6, 0);

					if (invenGun[i]->Getgun()._guntype == GUN_AR)
					{
						SPRITE_MGR->DrawTexture(arimg, &boxgun, rightimgX + 72, rightimgY + 38 + i * 75, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
					else if (invenGun[i]->Getgun()._guntype == GUN_SHOTGUN)
					{
						SPRITE_MGR->DrawTexture(sgimg, &boxgun, rightimgX + 72, rightimgY + 38 + i * 75, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
					else if (invenGun[i]->Getgun()._guntype == GUN_SR)
					{
						SPRITE_MGR->DrawTexture(srimg, &boxgun, rightimgX + 72, rightimgY + 38 + i * 75, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
				}
				for (int i = 0; i < invenshiled.size(); i++)
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 60, rightimgY + 38 + (invenGun.size() + i) * 75, rightscaleX / 1.5, rightscaleY / 1.6, 0);

					if (invenshiled[i]->name == SHILED_ONE)
						SPRITE_MGR->DrawTexture(shiled1, &boxshile, rightimgX + 105, rightimgY + 43 + (invenGun.size() + i) * 75, rightscaleX / 0.5, rightscaleY / 1.0, 0);

					else if (invenshiled[i]->name == SHILED_TWO)
						SPRITE_MGR->DrawTexture(shiled2, &boxshile, rightimgX + 105, rightimgY + 43 + (invenGun.size() + i) * 75, rightscaleX / 0.5, rightscaleY / 1.0, 0);
				}
				SPRITE_MGR->DrawTexture(mainbox, &boxrc, leftimgX, leftimgY, leftscaleimgX, leftscaleimgY, 0);
				for (int i = 0; i < 4; i++)
				{
					if (i == 0)
						SPRITE_MGR->DrawTexture(num1, &numbox, leftimgX + 25, leftimgY + 40 + i * 91, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);
					if (i == 1)
						SPRITE_MGR->DrawTexture(num2, &numbox, leftimgX + 25, leftimgY + 40 + i * 91, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);
					if (i == 2)
						SPRITE_MGR->DrawTexture(num3, &numbox, leftimgX + 25, leftimgY + 40 + i * 91, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);


					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, leftimgX + 80, leftimgY + 40 + i * 91, leftscaleimgX / 1.5, leftscaleimgY / 1.6, 0);
					if (playerGun[0].size() > i)
					{
						if (playerGun[0][i]->Getgun()._guntype == GUN_AR)
						{
							SPRITE_MGR->DrawTexture(arimg, &boxgun, leftimgX + 95, leftimgY + 40 + i * 91, leftscaleimgX / 1.8, leftscaleimgY / 3.2, 0);
						}
						else if (playerGun[0][i]->Getgun()._guntype == GUN_SHOTGUN)
						{
							SPRITE_MGR->DrawTexture(sgimg, &boxgun, leftimgX + 95, leftimgY + 40 + i * 91, leftscaleimgX / 1.8, leftscaleimgY / 3.2, 0);
						}
						else if (playerGun[0][i]->Getgun()._guntype == GUN_SR)
						{
							SPRITE_MGR->DrawTexture(srimg, &boxgun, leftimgX + 95, leftimgY + 40 + i * 91, leftscaleimgX / 1.8, leftscaleimgY / 3.2, 0);
						}

					}
					RECT gunbox = { leftimgX + 80,leftimgY + 40 + i * 91,leftimgX + 80 + 200,leftimgY + 40 + 90 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SPRITE_MGR->DrawTexture(alpha, &boxsmall, leftimgX + 20, leftimgY + 40 + i * 91, leftscaleimgX / 1.1, leftscaleimgY / 1.6, 0);
						if (playerGun[0].size() > i)
						{
							SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.5, 0);
						}
					}
				}
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, leftimgX + 80, leftimgY + 470, leftscaleimgX / 3, leftscaleimgY / 1.6, 0);

				SPRITE_MGR->DrawTexture(shieldimg, &numbox, leftimgX + 25, leftimgY + 478, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);

				if (playerShiled->name == SHILED_ONE)
				{
					SPRITE_MGR->DrawTexture(shiled1, &boxshile, leftimgX + 100, leftimgY + 478, leftscaleimgX / 0.5, leftscaleimgY / 1.0, 0);
				}
				else if (playerShiled->name == SHILED_TWO)
				{
					SPRITE_MGR->DrawTexture(shiled2, &boxshile, leftimgX + 100, leftimgY + 478, leftscaleimgX / 0.5, leftscaleimgY / 1.0, 0);
				}

				RECT gunbox = { leftimgX + 80,leftimgY + 470,leftimgX + 80 + 200,leftimgY + 560 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(alpha, &boxsmall, leftimgX + 60, leftimgY + 470, leftscaleimgX / 2.5, leftscaleimgY / 1.6, 0);
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.30, 0);
				}
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, leftimgX + 200, leftimgY + 470, leftscaleimgX / 3, leftscaleimgY / 1.6, 0);

			}
			else if (Inven == IV_RIGHT)
			{

				SPRITE_MGR->DrawTexture(mainbox, &boxrc, leftimgX, leftimgY, leftscaleimgX, leftscaleimgY, 0);
				for (int i = 0; i < 4; i++)
				{
					if (i == 0)
						SPRITE_MGR->DrawTexture(num1, &numbox, leftimgX + 22, leftimgY + 40 + i * 75, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);
					if (i == 1)
						SPRITE_MGR->DrawTexture(num2, &numbox, leftimgX + 22, leftimgY + 40 + i * 75, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);
					if (i == 2)
						SPRITE_MGR->DrawTexture(num3, &numbox, leftimgX + 22, leftimgY + 40 + i * 75, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);

					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, leftimgX + 60, leftimgY + 36 + i * 75, leftscaleimgX / 1.5, leftscaleimgY / 1.6, 0);
					if (playerGun[0].size() > i)
					{
						if (playerGun[0][i]->Getgun()._guntype == GUN_AR)
						{
							SPRITE_MGR->DrawTexture(arimg, &boxgun, leftimgX + 72, leftimgY + 38 + i * 75, leftscaleimgX / 1.8, leftscaleimgY / 3.2, 0);
						}
						else if (playerGun[0][i]->Getgun()._guntype == GUN_SHOTGUN)
						{
							SPRITE_MGR->DrawTexture(sgimg, &boxgun, leftimgX + 72, leftimgY + 38 + i * 75, leftscaleimgX / 1.8, leftscaleimgY / 3.2, 0);
						}
						else if (playerGun[0][i]->Getgun()._guntype == GUN_SR)
						{
							SPRITE_MGR->DrawTexture(srimg, &boxgun, leftimgX + 72, leftimgY + 38 + i * 75, leftscaleimgX / 1.8, leftscaleimgY / 3.2, 0);
						}
					}
				}
				SPRITE_MGR->DrawTexture(mainbox, &boxrc, rightimgX, rightimgY, rightscaleX, rightscaleY, 0);


				SPRITE_MGR->DrawTexture(shieldimg, &numbox, leftimgX + 25, leftimgY + 380, leftscaleimgX / 0.8, leftscaleimgY / 0.8, 0);
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, leftimgX + 60, leftimgY + 370, leftscaleimgX / 3, leftscaleimgY / 1.6, 0);
				if (playerShiled->name == SHILED_ONE)
				{
					SPRITE_MGR->DrawTexture(shiled1, &boxshile, leftimgX + 78, leftimgY + 375, leftscaleimgX / 0.5, leftscaleimgY / 1.0, 0);
				}
				else if (playerShiled->name == SHILED_TWO)
				{
					SPRITE_MGR->DrawTexture(shiled2, &boxshile, leftimgX + 78, leftimgY + 375, leftscaleimgX / 0.5, leftscaleimgY / 1.0, 0);
				}
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, leftimgX + 152, leftimgY + 370, leftscaleimgX / 3, leftscaleimgY / 1.6, 0);

				for (int i = 0; i < invenGun.size(); i++)
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + i * 91, rightscaleX / 1.5, rightscaleY / 1.6, 0);
					if (invenGun.size() > i)
					{
						if (invenGun[i]->Getgun()._guntype == GUN_AR)
						{
							SPRITE_MGR->DrawTexture(arimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
						}
						else if (invenGun[i]->Getgun()._guntype == GUN_SHOTGUN)
						{
							SPRITE_MGR->DrawTexture(sgimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
						}
						else if (invenGun[i]->Getgun()._guntype == GUN_SR)
						{
							SPRITE_MGR->DrawTexture(srimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
						}
					}
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SPRITE_MGR->DrawTexture(alpha, &boxsmall, rightimgX + 20, rightimgY + 40 + i * 91, rightscaleX / 1.1, rightscaleY / 1.6, 0);
						SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.5, 0);
					}
				}
				for (int i = 0; i < invenshiled.size(); i++)
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + (invenGun.size() + i) * +91, rightscaleX / 1.5, rightscaleY / 1.6, 0);

					if (invenshiled[i]->name == SHILED_ONE)
						SPRITE_MGR->DrawTexture(shiled1, &boxshile, rightimgX + 140, rightimgY + 50 + (invenGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

					else if (invenshiled[i]->name == SHILED_TWO)
						SPRITE_MGR->DrawTexture(shiled2, &boxshile, rightimgX + 140, rightimgY + 50 + (invenGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

					RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						SPRITE_MGR->DrawTexture(alpha, &boxsmall, rightimgX + 20, rightimgY + 40 + (invenGun.size() + i) * 91, rightscaleX / 1.1, rightscaleY / 1.6, 0);
						SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.30, 0);
					}
				}
			}
			//총알수 할꺼임
			SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 160, WINSIZE_Y - 190, 0.6, 1.2, 0);

			//돈뜰꺼임
			SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 510, WINSIZE_Y - 100, 0.6, 0.5, 0);


			RECT bulletnumimg = { 0,0,130,120 };
			SPRITE_MGR->DrawTexture(arbulletimg, &bulletnumimg, 180, WINSIZE_Y - 130, 0.2, 0.25, 0);
			SPRITE_MGR->DrawTexture(srbulletimg, &bulletnumimg, 320, WINSIZE_Y - 130, 0.25, 0.3, 0);
			SPRITE_MGR->DrawTexture(sgbulletimg, &bulletnumimg, 320, WINSIZE_Y - 90, 0.25, 0.3, 0);
		}
		else if (Menu == MS_MAP)
		{
			//if(KEY_MGR->IsOnceDown(VK_UP))
			//	miniscale+=0.1;
			//
			//if (KEY_MGR->IsOnceDown(VK_DOWN))
			//	miniscale-=0.1;
			//
			//
			//RECT minirc = { 1024 - 1024 / miniscale,1024 - 1024 / miniscale,1024 / miniscale,1024 / miniscale };
			//
			//
			////SPRITE_MGR->DrawTexture(minimapimg, &minirc, 360, 100, 0.65*miniscale, 0.65*miniscale, 0, 0xffffffff, &D3DXVECTOR3(512, 512, 0));
			////SPRITE_MGR->DrawTexture(minimapimg, &minirc, WINSIZE_X / 2 + 55, WINSIZE_Y / 2 + 25, 0.65*miniscale, 0.65*miniscale, 0 , 0xffffffff, &D3DXVECTOR3(512 / miniscale, 512 / miniscale, 0));
			////SPRITE_MGR->DrawTexture(minimapimg, &minirc, 512 / miniscale,512 / miniscale, 0.65*miniscale, 0.65*miniscale, 0);// , 0xffffffff, &D3DXVECTOR3(512 / miniscale, 512 / miniscale, 0));
			//
			//
			//RECT minip = { 0,0,250,255 };
			//D3DXVECTOR3 pos = pPlayer->GetWorldPosition();
			//
			//pos /= 1.6/miniscale;
			//SPRITE_MGR->DrawTexture(playerimg, &minip, WINSIZE_X / 2 + 55 + pos.x, WINSIZE_Y / 2 + 25 - pos.z, 0.05*miniscale, 0.08*miniscale, (float)atan2(pPlayer->GetForward().x, pPlayer->GetForward().z), 0xffffffff, &D3DXVECTOR3(125, 127.5, 0));


			//SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 100, 200, 4,4, 0);
			//SPRITE_MGR->DrawTexture(mainbox, &boxrc, 100, 100, 2,2, 0);
			RECT minirc = { 0,0,1024,1024 };
			SPRITE_MGR->DrawTexture(minimapbox, &minirc, 200, 80, 0.95, 0.69, 0);
			SPRITE_MGR->DrawTexture(minimapimg, &minirc, 360, 100, 0.65, 0.65, 0);


			RECT minip = { 0,0,250,255 };
			D3DXVECTOR3 pos = pPlayer->GetWorldPosition();

			pos /= 1.6;

			minimapbox;


			SPRITE_MGR->DrawTexture(playerimg, &minip, WINSIZE_X / 2 + 55 + pos.x, WINSIZE_Y / 2 + 25 - pos.z, 0.05, 0.08, (float)atan2(pPlayer->GetForward().x, pPlayer->GetForward().z), 0xffffffff, &D3DXVECTOR3(125, 127.5, 0));

		}
	}
	else if (mcs == MCS_SHOP)
	{
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 630, 20, 0.1, 0.4, 0);
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 690, 20, 0.1, 0.4, 0);


		SPRITE_MGR->DrawTexture(shopimg, &mcrc, 633, 21, 0.7, 0.7, 0);
		SPRITE_MGR->DrawTexture(invenimg, &mcrc, 693, 21, 0.7, 0.7, 0);
		if (PtInRect(&rc, GetMousePos()))
		{
			SPRITE_MGR->DrawTexture(alpha, &boxsmall, 630, 20, 0.1, 0.4, 0);
		}
		else if (PtInRect(&rc2, GetMousePos()))
		{
			SPRITE_MGR->DrawTexture(alpha, &boxsmall, 690, 20, 0.1, 0.4, 0);
		}
		if (shop == SS_SHOP)
		{
			SPRITE_MGR->DrawTexture(mainbox, &boxrc, rightimgX, rightimgY, rightscaleX, rightscaleY, 0);

			for (int i = 0; i < shopGun.size(); i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + i * 91, rightscaleX / 1.5, rightscaleY / 1.6, 0);

				if (shopGun[i]->Getgun()._guntype == GUN_AR)
				{
					SPRITE_MGR->DrawTexture(arimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
				}
				else if (shopGun[i]->Getgun()._guntype == GUN_SHOTGUN)
				{
					SPRITE_MGR->DrawTexture(sgimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
				}
				else if (shopGun[i]->Getgun()._guntype == GUN_SR)
				{
					SPRITE_MGR->DrawTexture(srimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
				}

				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(alpha, &boxsmall, rightimgX + 20, rightimgY + 40 + i * 91, rightscaleX / 1.1, rightscaleY / 1.6, 0);

					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.5, 0);
				}
			}
			for (int i = 0; i < shopshiled.size(); i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + (shopGun.size() + i) * 91, rightscaleX / 1.5, rightscaleY / 1.6, 0);

				if (shopshiled[i]->name == SHILED_ONE)
					SPRITE_MGR->DrawTexture(shiled1, &boxshile, rightimgX + 150, rightimgY + 50 + (shopGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

				else if (shopshiled[i]->name == SHILED_TWO)
					SPRITE_MGR->DrawTexture(shiled2, &boxshile, rightimgX + 150, rightimgY + 50 + (shopGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (shopGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + (shopGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(alpha, &boxsmall, rightimgX + 20, rightimgY + 40 + (shopGun.size() + i) * 91, rightscaleX / 1.1, rightscaleY / 1.6, 0);

					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.30, 0);
				}
			}
		}
		else if (shop == SS_INVEN)
		{

			SPRITE_MGR->DrawTexture(mainbox, &boxrc, rightimgX, rightimgY, rightscaleX, rightscaleY, 0);
			for (int i = 0; i < invenGun.size(); i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + i * 91, rightscaleX / 1.5, rightscaleY / 1.6, 0);
				if (invenGun.size() > i)
				{
					if (invenGun[i]->Getgun()._guntype == GUN_AR)
					{
						SPRITE_MGR->DrawTexture(arimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
					else if (invenGun[i]->Getgun()._guntype == GUN_SHOTGUN)
					{
						SPRITE_MGR->DrawTexture(sgimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
					else if (invenGun[i]->Getgun()._guntype == GUN_SR)
					{
						SPRITE_MGR->DrawTexture(srimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
				}
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.5, 0);

				}
			}
			for (int i = 0; i < invenshiled.size(); i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + (invenGun.size() + i) * +91, rightscaleX / 1.5, rightscaleY / 1.6, 0);

				if (invenshiled[i]->name == SHILED_ONE)
					SPRITE_MGR->DrawTexture(shiled1, &boxshile, rightimgX + 140, rightimgY + 50 + (invenGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

				else if (invenshiled[i]->name == SHILED_TWO)
					SPRITE_MGR->DrawTexture(shiled2, &boxshile, rightimgX + 140, rightimgY + 50 + (invenGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.30, 0);
				}
			}
		}
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 160, WINSIZE_Y - 100, 0.6, 0.5, 0);
	}
	else if (mcs == MCS_BULLETSHOP)
	{
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 630, 20, 0.1, 0.4, 0);
		SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 690, 20, 0.1, 0.4, 0);


		SPRITE_MGR->DrawTexture(shopimg, &mcrc, 633, 21, 0.7, 0.7, 0);
		SPRITE_MGR->DrawTexture(invenimg, &mcrc, 693, 21, 0.7, 0.7, 0);
		if (PtInRect(&rc, GetMousePos()))
		{
			SPRITE_MGR->DrawTexture(alpha, &boxsmall, 630, 20, 0.1, 0.4, 0);
		}
		else if (PtInRect(&rc2, GetMousePos()))
		{
			SPRITE_MGR->DrawTexture(alpha, &boxsmall, 690, 20, 0.1, 0.4, 0);
		}
		if (shop == SS_SHOP)
		{
			SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 160, WINSIZE_Y - 190, 0.6, 1.2, 0);

			RECT bulletnumimg = { 0,0,130,120 };
			SPRITE_MGR->DrawTexture(arbulletimg, &bulletnumimg, 180, WINSIZE_Y - 130, 0.2, 0.25, 0);
			SPRITE_MGR->DrawTexture(srbulletimg, &bulletnumimg, 320, WINSIZE_Y - 130, 0.25, 0.3, 0);
			SPRITE_MGR->DrawTexture(sgbulletimg, &bulletnumimg, 320, WINSIZE_Y - 90, 0.25, 0.3, 0);


			SPRITE_MGR->DrawTexture(mainbox, &boxrc, rightimgX, rightimgY, rightscaleX, rightscaleY, 0);

			for (int i = 0; i < ITEM_NONE; i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + i * 91, rightscaleX / 1.5, rightscaleY / 1.6, 0);
				if (i == ITEM_RIFLEBULLET)
				{
					SPRITE_MGR->DrawTexture(bulletRifleimg, &boxgun, rightimgX + 150, rightimgY + 45 + i * 91, rightscaleX / 4, rightscaleY / 8, 0);
				}
				else if (i == ITEM_SHOTGUNBULLET)
				{
					SPRITE_MGR->DrawTexture(bulletShotgunimg, &boxgun, rightimgX + 150, rightimgY + 45 + i * 91, rightscaleX / 4, rightscaleY / 8, 0);
				}
				else if (i == ITEM_SNIPERBULLET)
				{
					SPRITE_MGR->DrawTexture(bulletsniperimg, &boxgun, rightimgX + 150, rightimgY + 45 + i * 91, rightscaleX / 4, rightscaleY / 8, 0);
				}
				else if (i == ITEM_HEAL)
				{
					SPRITE_MGR->DrawTexture(healimg, &boxgun, rightimgX + 150, rightimgY + 40 + i * 91, rightscaleX / 3, rightscaleY / 6, 0);
				}

				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					if (i == ITEM_RIFLEBULLET)
					{
						SPRITE_MGR->DrawTexture(alpha, &boxsmall, 180, WINSIZE_Y - 130, rightscaleX / 4, rightscaleY / 3.6, 0);
					}
					else if (i == ITEM_SHOTGUNBULLET)
					{
						SPRITE_MGR->DrawTexture(alpha, &boxsmall, 320, WINSIZE_Y - 90, rightscaleX / 4, rightscaleY / 3.6, 0);
					}
					else if (i == ITEM_SNIPERBULLET)
					{
						SPRITE_MGR->DrawTexture(alpha, &boxsmall, 320, WINSIZE_Y - 130, rightscaleX / 4, rightscaleY / 3.6, 0);
					}
					SPRITE_MGR->DrawTexture(alpha, &boxsmall, rightimgX + 20, rightimgY + 40 + i * 91, rightscaleX / 1.1, rightscaleY / 1.6, 0);
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 0.85, 0);
				}
			}

			SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 510, WINSIZE_Y - 100, 0.6, 0.5, 0);

		}
		else if (shop == SS_INVEN)
		{

			SPRITE_MGR->DrawTexture(mainbox, &boxrc, rightimgX, rightimgY, rightscaleX, rightscaleY, 0);
			for (int i = 0; i < invenGun.size(); i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + i * 91, rightscaleX / 1.5, rightscaleY / 1.6, 0);
				if (invenGun.size() > i)
				{
					if (invenGun[i]->Getgun()._guntype == GUN_AR)
					{
						SPRITE_MGR->DrawTexture(arimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
					else if (invenGun[i]->Getgun()._guntype == GUN_SHOTGUN)
					{
						SPRITE_MGR->DrawTexture(sgimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
					else if (invenGun[i]->Getgun()._guntype == GUN_SR)
					{
						SPRITE_MGR->DrawTexture(srimg, &boxgun, rightimgX + 92, rightimgY + 40 + i * 91, rightscaleX / 1.8, rightscaleY / 3.2, 0);
					}
				}
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.5, 0);

				}
			}
			for (int i = 0; i < invenshiled.size(); i++)
			{
				SPRITE_MGR->DrawTexture(smallbox, &boxsmall, rightimgX + 80, rightimgY + 40 + (invenGun.size() + i) * +91, rightscaleX / 1.5, rightscaleY / 1.6, 0);

				if (invenshiled[i]->name == SHILED_ONE)
					SPRITE_MGR->DrawTexture(shiled1, &boxshile, rightimgX + 140, rightimgY + 50 + (invenGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

				else if (invenshiled[i]->name == SHILED_TWO)
					SPRITE_MGR->DrawTexture(shiled2, &boxshile, rightimgX + 140, rightimgY + 50 + (invenGun.size() + i) * 91, rightscaleX / 0.5, rightscaleY / 1.0, 0);

				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					SPRITE_MGR->DrawTexture(smallbox, &boxsmall, 140, 130, 0.65, 1.30, 0);
				}
			}
		}

	}
	RECT mouserc = { 0,0,550,1050 };
	SPRITE_MGR->DrawTexture(mouseimg, &mouserc, GetMousePos().x, GetMousePos().y, 0.02, 0.02, 0);
	return true;
}

bool cInventory::Scene_RenderFont()
{
	if (mcs == MCS_NONE)return false;

	string text;
	if (mcs == MCS_MENU)
	{
		if (Menu == MS_INVEN)
		{

			if (Inven == IV_LEFT)
			{
				for (int i = 0; i < 4; i++)
				{
					RECT gunbox = { leftimgX + 80,leftimgY + 40 + i * 91,leftimgX + 80 + 200,leftimgY + 40 + 90 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						if (playerGun[0].size() > i)
						{
							GunTextRender(playerGun[0][i]);
						}
					}
				}
				RECT gunbox = { leftimgX + 80,leftimgY + 470,leftimgX + 80 + 200,leftimgY + 560 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					ShiledTextRender(playerShiled);
				}

			}
			else if (Inven == IV_RIGHT)
			{
				for (int i = 0; i < invenGun.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						GunTextRender(invenGun[i]);
					}
				}
				for (int i = 0; i < invenshiled.size(); i++)
				{
					RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
					if (PtInRect(&gunbox, GetMousePos()))
					{
						ShiledTextRender(invenshiled[i]);
					}
				}
			}
			DXFONT_MGR->PrintStyleTextOutline("playerUI", "AMMO", 190, WINSIZE_Y - 170, 0XFFFFFFFF);

			//왼쪽위
			text = to_string(*bulletRifleNum);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 210, WINSIZE_Y - 130, 0XFFFFFFFF);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", "1000", 210, WINSIZE_Y - 110, 0X88888888);

			//오른쪽위
			text = to_string(*bulletSniperNum);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 350, WINSIZE_Y - 130, 0XFFFFFFFF);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", "1000", 350, WINSIZE_Y - 110, 0X88888888);

			//오른쪽아래
			text = to_string(*bulletShotgunNum);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 350, WINSIZE_Y - 90, 0XFFFFFFFF);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", "1000", 350, WINSIZE_Y - 70, 0X88888888);

			text = "$";
			text += to_string(*money);
			DXFONT_MGR->PrintStyleTextOutline("playerUI", text, 530, WINSIZE_Y - 82, 0XFFFFFFFF);
		}
		else if (Menu == MS_MAP)
		{

		}
	}
	else if (mcs == MCS_SHOP)
	{
		if (shop == SS_SHOP)
		{
			for (int i = 0; i < shopGun.size(); i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
					GunTextRender(shopGun[i]);
			}

			for (int i = 0; i < shopshiled.size(); i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (shopGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + (shopGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
					ShiledTextRender(shopshiled[i]);
			}
			DXFONT_MGR->PrintStyleTextOutline("playerUI3", "SHOP", rightimgX + 220, rightimgY - 10, 0XFFFFFFFF);
		}
		if (shop == SS_INVEN)
		{
			DXFONT_MGR->PrintStyleTextOutline("playerUI3", "Inventory", rightimgX + 140, rightimgY - 10, 0XFFFFFFFF);

			for (int i = 0; i < invenGun.size(); i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
					GunTextRender2(shopGun[i]);

			}
			for (int i = 0; i < invenshiled.size(); i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
					ShiledTextRender2(shopshiled[i]);
			}
		}
		text = "$";
		text += to_string(*money);
		DXFONT_MGR->PrintStyleTextOutline("playerUI", text, 180, WINSIZE_Y - 82, 0XFFFFFFFF);
	}
	else if (mcs == MCS_BULLETSHOP)
	{
		if (shop == SS_SHOP)
		{
			for (int i = 0; i < ITEM_NONE; i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 30 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
				{
					int textX = 170;
					int numX = 370;
					int textY = 160;
					if (i == ITEM_RIFLEBULLET)
					{
						string text = "Rifle Shells";
						DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 45, textY - 40, 0XFFFFFFFF);

						text = "Ammunition";
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFF00ccff);
						textY += 30;
						text = "·Contains rifle shells.";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);

						textY += 35;
						text = "$10";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX + 110, textY, 0XFFFFFFFF);

					}
					else if (i == ITEM_SHOTGUNBULLET)
					{
						string text = "Shogun Shells";
						DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 30, textY - 40, 0XFFFFFFFF);

						text = "Ammunition";
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFF00ccff);
						textY += 30;
						text = "·Contains Shogun shells.";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);

						textY += 35;
						text = "$10";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX + 110, textY, 0XFFFFFFFF);
					}
					else if (i == ITEM_SNIPERBULLET)
					{
						string text = "Sniper Shells";
						DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 30, textY - 40, 0XFFFFFFFF);

						text = "Ammunition";
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFF00ccff);
						textY += 30;
						text = "·Contains Sniper shells.";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);

						textY += 35;
						text = "$10";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX + 110, textY, 0XFFFFFFFF);
					}
					else if (i == ITEM_HEAL)
					{
						string text = "HEAL Shell";
						DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 70, textY - 40, 0XFFFFFFFF);

						text = "Heal";
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFF00ccff);
						textY += 30;
						text = "·Heal shells.";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);

						textY += 35;
						text = "$10";//정확도
						DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX + 110, textY, 0XFFFFFFFF);
					}
				}
			}

			DXFONT_MGR->PrintStyleTextOutline("playerUI", "AMMO", 190, WINSIZE_Y - 170, 0XFFFFFFFF);

			//왼쪽위
			text = to_string(*bulletRifleNum);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 210, WINSIZE_Y - 130, 0XFFFFFFFF);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", "1000", 210, WINSIZE_Y - 110, 0X88888888);

			//오른쪽위
			text = to_string(*bulletSniperNum);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 350, WINSIZE_Y - 130, 0XFFFFFFFF);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", "1000", 350, WINSIZE_Y - 110, 0X88888888);

			//오른쪽아래
			text = to_string(*bulletShotgunNum);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 350, WINSIZE_Y - 90, 0XFFFFFFFF);
			DXFONT_MGR->PrintStyleTextOutline("playerUI2", "1000", 350, WINSIZE_Y - 70, 0X88888888);

			text = "$";
			text += to_string(*money);
			DXFONT_MGR->PrintStyleTextOutline("playerUI", text, 530, WINSIZE_Y - 82, 0XFFFFFFFF);
		}
		if (shop == SS_INVEN)
		{
			DXFONT_MGR->PrintStyleTextOutline("playerUI3", "Inventory", rightimgX + 140, rightimgY - 10, 0XFFFFFFFF);

			for (int i = 0; i < invenGun.size(); i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + i * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + i * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
					GunTextRender2(shopGun[i]);

			}
			for (int i = 0; i < invenshiled.size(); i++)
			{
				RECT gunbox = { rightimgX + 80,rightimgY + 40 + (invenGun.size() + i) * 91,rightimgX + 80 + 200,rightimgY + 10 + 100 + (invenGun.size() + i) * 91 };
				if (PtInRect(&gunbox, GetMousePos()))
					ShiledTextRender2(shopshiled[i]);
			}
		}

	}


	return true;
}

void cInventory::GunTextRender(Gun * _gun)
{

	//Capacity 수용양
	//Recharge Rate재충전비율
	//Recharge   충전 까지걸리는시간?
	int textX = 170;
	int numX = 370;
	int textY = 160;
	string text = _gun->Getgun().name;

	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 130, textY - 40, 0XFFFFFFFF);
	if (_gun->Getgun()._guntype == GUN_SHOTGUN)
	{
		text = "Damage";
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
		text = to_string((int)_gun->Getgun().m_dmg);
		text += "x";
		text += to_string((int)_gun->Getgun().m_bulletnum);
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);
	}
	else
	{
		text = "Damage";
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
		text = to_string((int)_gun->Getgun().m_dmg);
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);
	}


	textY += 30;
	text = "Accuracy";//정확도
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_gun->Getgun().m_ar);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 30;
	text = "Fire Rate";//초당 발사수
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string(_gun->Getgun().m_fireSpeed);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 30;
	text = "Magazine Size";//총알수
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_gun->Getgun().m_tAmmo);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);


	textY += 50;
	text = "$";
	text += to_string((int)_gun->Getgun().m_price);
	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, numX - 101, textY, 0XFFFFFFFF);
}

void cInventory::ShiledTextRender(tag_shiled * _shiled)
{
	int textX = 170;
	int numX = 370;
	int textY = 160;
	string text = "Shiled";

	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 140, textY - 40, 0XFFFFFFFF);

	text = "Capacity";
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_shiled->shiled);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);


	textY += 30;
	text = "Recharge";//정확도
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_shiled->recover);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 30;
	text = "Recharge";//초당 발사수
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_shiled->recoverResettime);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 50;
	text = "$";
	text += to_string((int)_shiled->price);
	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, numX - 101, textY, 0XFFFFFFFF);
}

void cInventory::GunTextRender2(Gun * _gun)
{

	//Capacity 수용양
	//Recharge Rate재충전비율
	//Recharge   충전 까지걸리는시간?
	int textX = 170;
	int numX = 370;
	int textY = 160;
	string text = _gun->Getgun().name;

	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 130, textY - 40, 0XFFFFFFFF);
	if (_gun->Getgun()._guntype == GUN_SHOTGUN)
	{
		text = "Damage";
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
		text = to_string((int)_gun->Getgun().m_dmg);
		text += "x";
		text += to_string((int)_gun->Getgun().m_bulletnum);
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);
	}
	else
	{
		text = "Damage";
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
		text = to_string((int)_gun->Getgun().m_dmg);
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);
	}


	textY += 30;
	text = "Accuracy";//정확도
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_gun->Getgun().m_ar);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 30;
	text = "Fire Rate";//초당 발사수
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string(_gun->Getgun().m_fireSpeed);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 30;
	text = "Magazine Size";//총알수
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_gun->Getgun().m_tAmmo);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);


	textY += 50;
	text = "$";
	text += to_string((int)_gun->Getgun().m_price / 2);
	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, numX - 101, textY, 0XFFFF0000);
}

void cInventory::ShiledTextRender2(tag_shiled * _shiled)
{
	int textX = 170;
	int numX = 370;
	int textY = 160;
	string text = "Shiled";

	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, textX + 140, textY - 40, 0XFFFFFFFF);

	text = "Capacity";
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_shiled->shiled);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);


	textY += 30;
	text = "Recharge";//정확도
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_shiled->recover);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 30;
	text = "Recharge";//초당 발사수
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, textX, textY, 0XFFFFFFFF);
	text = to_string((int)_shiled->recoverResettime);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, numX, textY, 0XFFFFFFFF);

	textY += 50;
	text = "$";
	text += to_string((int)_shiled->price / 2);
	DXFONT_MGR->PrintStyleTextOutline("playerUI4", text, numX - 101, textY, 0XFFFF0000);
}
