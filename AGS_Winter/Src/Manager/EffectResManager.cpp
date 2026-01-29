#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "EffectResManager.h"


EffectResManager* EffectResManager::instance_ = nullptr;

void EffectResManager::CreateInstance(void)
{
	if (instance_ == nullptr){

		instance_ = new EffectResManager();
	}
	instance_->Init();
}

EffectResManager& EffectResManager::GetInstance(void)
{
	return *instance_;
}

void EffectResManager::Init(void)
{
	// リソースのロード
	Load();
}

void EffectResManager::Destroy(void)
{
	for (const auto& pair : resourceIds_){

		// 爆発エフェクトのメモリ解放
		DeleteEffekseerEffect(pair.second);
	}
	// インスタンスのメモリ解放
	delete instance_;
}

int EffectResManager::GetResourceId(TYPE type)
{
	if (resourceIds_.count(type) == 0){

		return -1;
	}
	return resourceIds_[type];
}

EffectResManager::EffectResManager(void)
{
}

void EffectResManager::Load(void)
{
	// ヒットエフェクトのロード
	resourceIds_.emplace(TYPE::ENEMY_HIT, LoadEffekseerEffect((Application::PATH_EFFECT + "Blood.efkproj").c_str()));
	resourceIds_.emplace(TYPE::SHOT, LoadEffekseerEffect((Application::PATH_EFFECT + "Fire_Shot.efkefc").c_str()));
	resourceIds_.emplace(TYPE::BURST, LoadEffekseerEffect((Application::PATH_EFFECT + "Shot_Finish.efkefc").c_str()));
}