#pragma once
#include "SceneBase.h"
#include <DxLib.h>


class SceneManager;
class Stage;
class EnemyBase;
class Player;
class Item;

class GameScene : public SceneBase
{
public:
	//カメラとプレイヤーの距離
	static constexpr float CAMERA_TO_PLAYER = 400.0f;

	static constexpr float COLLISION_STAGE_DIFF = 25.0f;

	// コンストラクタ
	GameScene(void);
	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;
	
	void EnemyToPlayer(void);

private:

	enum class COLLISION_TYPE {

		PLAYER,
		ENEMY,
		ENEMY_ATTACK,
	};

	struct CollisionData {

		VECTOR pos;
		VECTOR prev;
		COLLISION_TYPE type;
	};

	// ステージ
	Stage* stage_;
	EnemyBase* enemyBase_;
	Player* player_;
	Item* item_;

	int shadowMap_;

	bool hitFlgP_;
	bool hitFlgE_;

	float yaw_;
	float pitch_;

	bool isFirst_;
	bool isLockon_;

	void Collision(void);
	void CollisionStage(CollisionData data);
	void GameCamera(void);
	void Effect(MV1_COLL_RESULT_POLY dim);
};
