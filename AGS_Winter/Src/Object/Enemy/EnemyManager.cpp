#include <DxLib.h>
#include "../../Application.h"
#include "EnemyBirb.h"
#include "EnemyBee.h"
#include "EnemyManager.h"


EnemyManager::EnemyManager(void)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	// エネミーモデルのロード
	enemyModelIds_.emplace_back(MV1LoadModel((Application::PATH_MODEL + "Enemy/Birb.mv1").c_str()));
	enemyModelIds_.emplace_back(MV1LoadModel((Application::PATH_MODEL + "Enemy/Armabee.mv1").c_str()));
	// 最初から出現させる
	Reset();

	for (int i = 0; i < enemyModelIds_.size(); i++) {

		MV1SetupCollInfo(enemyModelIds_[i]);
	}
}

void EnemyManager::Update(int wave)
{
	if (spawnNum_ <= SPAWN_MAX) {
		// 出現カウンタ
		cntSpawn_++;
		// 一定間隔でエネミーを出現させる
		if (cntSpawn_ % SPAWN_INTERVAL == 0) {
			if (wave == 0) {

				EnemyBase* enemy = new EnemyBirb();
				enemy->Init(EnemyBase::TYPE::BIRB, enemyModelIds_[static_cast<int>(EnemyBase::TYPE::BIRB)]);
				enemys_.emplace_back(enemy);
				spawnNum_++;
			}
			else if (wave == 1) {

				EnemyBase* enemy = new EnemyBee();
				enemy->Init(EnemyBase::TYPE::BEE, enemyModelIds_[static_cast<int>(EnemyBase::TYPE::BEE)]);
				enemys_.emplace_back(enemy);
				spawnNum_++;
			}
		}
	}
	// エネミーの更新
	for (EnemyBase* enemy : enemys_){

		enemy->Update();
		MV1RefreshCollInfo(enemy->GetModelId());
	}
}

void EnemyManager::Draw(void)
{
	for (EnemyBase* enemy : enemys_) {

		enemy->Draw();
	}
}

void EnemyManager::Release(void)
{
	for (EnemyBase* enemy : enemys_) {

		enemy->Release();
		delete enemy;
	}
	enemys_.clear();

	for (int enemyModelId : enemyModelIds_) {
	
		MV1DeleteModel(enemyModelId);
	}
	enemyModelIds_.clear();
}

void EnemyManager::Reset(void)
{
	deadCnt_ = 0;
	cntSpawn_ = SPAWN_INTERVAL - 1;
	spawnNum_ = 0;
}
