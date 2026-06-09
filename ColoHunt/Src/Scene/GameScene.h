#pragma once
#include "SceneBase.h"
#include <DxLib.h>
#include <vector>
#include "../Object/Collider/ColliderBase.h"


class SceneManager;
class Stage;
class Enemy;
class Player;
class Item;

class GameScene : public SceneBase
{
public:

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
	
	//カメラとプレイヤーの距離
	static constexpr float CAMERA_TO_PLAYER = 375.0f;
	static constexpr VECTOR PLAYER_HEAD_POS = { 0.0f, 200.0f, 0.0f };

	// 当たり判定距離
	static constexpr float COLLISION_STAGE_DIFF = 25.0f;
	static constexpr float COLLISION_CAMERA_DIFF = 3.0f;

	// 初期値
	static constexpr float DEFAULT_TILT = 0.3f;
	static constexpr float DEFAULT_YAW = 0.0f;

	// 最上点
	static constexpr float MOST_TOP_SIGHT = DX_PI_F / 2.0f - 0.1f;
	// 最下点
	static constexpr float MOST_DOWN_SIGHT = -DX_PI_F / 18.0f;

	// クリアカメラ時の敵の注視点
	static constexpr VECTOR CLEAR_ENEMY_POS = { 0.0f, 180.0f, 0.0f };

	// クリアカメラ移行タイミング
	static constexpr int CLEAR_CAMERA_CHANGE_A = 80;
	static constexpr int CLEAR_CAMERA_CHANGE_B = 160;
	static constexpr int CLEAR_CAMERA_CHANGE_C = 260;

	// ロックオンの補完時間
	static constexpr int ROCKON_LERP_TIME = 10;

	// ブラーの数
	static constexpr int BLUR_NUM = 3;
	// ブラーの時間
	static constexpr int BLUR_CNT = 25;

	// 回避のタイミング
	static constexpr int GREAT_DOGDE_TIMING = 5;
	static constexpr int GOOD_DOGDE_TIMING = 13;
	
	// 回避での画面揺れ時間
	static constexpr int GREAT_DOGDE_TIME = 20;
	static constexpr int GOOD_DOGDE_TIME = 10;

	// エフェクトのサイズ
	static constexpr float EFFECT_SIZE = 20.0f;

	// クリア演出終わり時間
	static constexpr int CHANGE_CLEAR_SCENE_TIME = 90;

	// 画面揺れの攻撃力
	static constexpr float SHAKE_POWER = 12.0f;

	// 画面揺れ時間
	static constexpr int SHAKE_TIME_A = 30;
	static constexpr int SHAKE_TIME_B = 6;
	static constexpr int SHAKE_TIME_C = 15;

	// ヒットストップ時間
	static constexpr int HITSTOP_TIME_A = 3;
	static constexpr int HITSTOP_TIME_B = 5;

	// ステージ
	Stage* stage_;
	Enemy* enemy_;
	Player* player_;
	Item* item_;

	//タイマーのフォント
	int timerHandle_;

	// タイム
	float time_;

	// リザルトロゴ
	int failedImg_;
	int clearImg_;

	// 背景モデルのハンドル
	int backGroundHandle_;

	//描画用のイメージハンドル
	int drawHandle_;

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

	//被弾回数
	int damageNum_;

	// カメラのコライダー
	ColliderBase* cameraColl_;

	VECTOR cameraPos_;
	VECTOR playerPos_;

	// ブラー時のプレイヤー
	int blurImg_[BLUR_NUM];
	bool blurFlg_;
	int blurCnt_;

	//シェーダー
	int shader_;
	int shaderConstBuff_;
	VERTEX2DSHADER mVertex_[4];
	WORD mIndex_[6];

	// シーンチェンジフラグ
	bool changeFlg_;
	int changeCnt_;

	// クリア時のくるくるカメラ
	bool clearCamera_;

	//ブラー用
	void SetBlur(void);

	//シェーダー用
	void MakeSquereVertex(void);

	//当たり判定用
	void Collision(void);
	void CollisionStage(void);
	void CollisionCamera(void);

	// 回避判定
	void Dodge(void);

	//カメラの操作
	void GameCamera(void);
	void SetCameraPos(VECTOR targetPos, float diff) const;
	void ShakeCamera(void);

	//エフェクト
	void Effect(MV1_COLL_RESULT_POLY dim);
};
