#pragma once
#include "SceneBase.h"
#include <DxLib.h>
#include <vector>


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

	static constexpr int BLUR_NUM = 8;

	// ステージ
	Stage* stage_;
	Enemy* enemy_;
	Player* player_;
	Item* item_;

	//タイマーのフォント
	int timerHandle_;

	// リザルトロゴ
	int failedImg_;
	int clearImg_;

	//影のマップ
	int shadowMap_;

	// ヒット判定を一回だけ行うためのもの
	bool hitFlgP_;
	bool hitFlgE_;

	// カメラの移動値
	float yaw_;
	float pitch_;
		
	// ロックオンしたか
	bool isLockon_;

	// ロックオンの時の画像
	int lockOnImg_;
	bool cntDown_;
	int cnt_;

	// 画面揺らし
	int shakeCnt_;
	float shakeWidVer_;
	float shakeWidSide_;

	// ヒットストップ
	int hitStopCnt_;
	int hitStopImg_;

	int damageNum_;

	// ブラー時のプレイヤー
	int blurImg_[BLUR_NUM];
	bool blurFlg_;
	int blurCnt_;

	// シーンチェンジフラグ
	bool changeFlg_;
	int changeCnt_;

	// クリア時のくるくるカメラ
	bool clearCamera_;

	void SetBlur(void);
	void Blur(void);

	void Collision(void);
	void CollisionStage(void);
	void CollisionCamera(void);
	void GameCamera(void);
	void SetCameraPos(VECTOR targetPos, float diff) const;
	void Effect(MV1_COLL_RESULT_POLY dim);
	void ShakeCamera(void);

	void Dodge(void);
};
