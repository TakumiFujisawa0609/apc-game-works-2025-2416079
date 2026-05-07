#include <DxLib.h>
#include "Keyboard.h"
#include "../../Application.h"


void Keyboard::Init(void)
{
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false;
}

void Keyboard::Update(void)
{
	// 初期化
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false; 

	// キーボード検知
	for (auto& p : infos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckHitKey(p.second.key);
		InputBase::Update(p);
	}
}

void Keyboard::Release(void)
{
}

Keyboard::Keyboard(void)
{
}
