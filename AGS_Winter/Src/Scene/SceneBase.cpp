#include <DxLib.h>
#include "SceneBase.h"


SceneBase::SceneBase(void)
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
	DrawCube3D({ -5000.0f, 0.0f, -5000.0f }, { 5000.0f, 0.0f, 5000.0f }, 0xffffff, 0xffffff, true);
}

void SceneBase::Release(void)
{
}
