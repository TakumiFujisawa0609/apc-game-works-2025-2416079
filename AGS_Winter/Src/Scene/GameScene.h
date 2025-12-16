#pragma once
#include "SceneBase.h"
#include <DxLib.h>


class SceneManager;
class Stage;
class Enemy;
class Player;
class Item;

class GameScene : public SceneBase
{
public:
	//カメラとプレイヤーの距離
	static constexpr float CAMERA_TO_PLAYER = 350.0f;

	static constexpr float COLLISION_STAGE_DIFF = 25.0f;
	static constexpr float COLLISION_CAMERA_DIFF = 3.0f;

	static constexpr float DEFAULT_TILT = 0.3f;
	static constexpr float DEFAULT_YAW = 0.0f;

	// コンストラクタ
	GameScene(void);
	// デストラクタ
	~GameScene(void);

	//ロード
	void InitLoad(void) override;
	//その他の初期化
	void Init(void) override;
	//更新
	void Update(void) override;
	//描画
	void Draw(void) override;
	//解放
	void Release(void) override;

private:

	enum class COLLISION_TYPE {

		PLAYER,
		ENEMY,
		ENEMY_ATTACK
	};

	struct CollisionData {

		VECTOR pos;
		VECTOR prev;
		COLLISION_TYPE type;
	};

	// ステージ
	Stage* stage_;
	Enemy* enemy_;
	Player* player_;
	Item* item_;

	int failedImg_;
	int clearImg_;

	int shadowMap_;

	bool hitFlgP_;
	bool hitFlgE_;

	float yaw_;
	float pitch_;

	bool isLockon_;

	int lockOnImg_;
	bool cntDown_;
	int cnt_;

	bool changeFlg_;
	int changeCnt_;

	bool clearCamera_;

	void Collision(void);
	void CollisionStage(CollisionData data);
	void GameCamera(void);
	void SetCameraPos(VECTOR targetPos, float diff);
	void Effect(MV1_COLL_RESULT_POLY dim);
};
