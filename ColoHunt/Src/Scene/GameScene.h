#pragma once
#include "SceneBase.h"
#include <DxLib.h>
#include <vector>
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Actor/ActorBase.h"


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

#pragma region 定数定義

	// タイム表示用のフォントのサイズ
	static constexpr int TIME_FONT_SIZE = 45;
	// タイム表示用のフォントの太さ
	static constexpr int TIME_FONT_THICKNESS = 3;
	// タイマー表示のX座標
	static constexpr int TIMER_DRAW_X = 30;
	// タイマー表示のY座標
	static constexpr int TIMER_DRAW_Y = 30;

	// タイマーフレームの色
	static constexpr int TIMER_FRAME_COLOR = 0x000000;

	// タイマーフレームの四角形オフセット構造体
	struct TimerFrameBoxOffset { int x1; int y1; int x2; int y2; };
	// タイマーフレームの三角形オフセット構造体
	struct TimerFrameTriangleOffset { int x1; int y1; int x2; int y2; int x3; int y3; };

	// タイマーフレームの四角形オフセット一覧
	static constexpr TimerFrameBoxOffset TIMER_FRAME_BOX_OFFSETS[] = {
		{ -20, -5, 180, 50 },
		{   0, -20, 160, -5 },
		{   0,  50, 160, 65 },
	};

	// タイマーフレームの三角形オフセット一覧
	static constexpr TimerFrameTriangleOffset TIMER_FRAME_TRIANGLE_OFFSETS[] = {
		{ -20, -5,   0, -5,   0, -20 },
		{ 180, -5, 160, -5, 160, -20 },
		{ 180, 50, 160, 65, 160,  50 },
		{ -20, 50,   0, 65,   0,  50 },
	};

	// シャドウマップのサイズ
	static constexpr int SHADOW_MAP_SIZE = 4096;
	// シャドウマップのライトの方向
	static constexpr VECTOR SHADOW_MAP_LIGHT_DIR = { 0.2f, -0.8f, 0.1f };
	// シャドウマップの描画範囲 (最小/最大 座標)
	static constexpr VECTOR SHADOW_MAP_DRAW_AREA_MIN = { -2500.0f, 0.0f, -2500.0f };
	static constexpr VECTOR SHADOW_MAP_DRAW_AREA_MAX = { 2500.0f, 0.0f, 2500.0f };

	// 背景モデルのスケール
	static constexpr float BACKGROUND_SCALE = 8.0f;

	//カメラとプレイヤーの距離
	static constexpr float CAMERA_TO_PLAYER = 375.0f;
	static constexpr VECTOR PLAYER_HEAD_POS = { 0.0f, 200.0f, 0.0f };

	// カメラの移動値の最大値
	static constexpr float MAX_PITCH = 0.5f;
	// カメラの移動値の補完率
	static constexpr float CAMERA_LERP_RATE = 0.8f;

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

	// クリアBGMの音量
	static constexpr int CLEAR_BGM_VOLUME = 185;

	// クリアカメラ時の敵の注視点
	static constexpr VECTOR CLEAR_ENEMY_POS = { 0.0f, 180.0f, 0.0f };

	// クリアカメラ移行タイミング
	static constexpr int CLEAR_CAMERA_CHANGE_A = 80;
	static constexpr int CLEAR_CAMERA_CHANGE_B = 160;
	static constexpr int CLEAR_CAMERA_CHANGE_C = 260;

	// クリアロゴを出す時間
	static constexpr int CLEAR_LOGO_TIME = 90;

	// ロックオンの補完時間
	static constexpr int ROCKON_LERP_TIME = 10;

	// ロックオンUIの表示高さオフセット
	static constexpr float LOCKON_HEIGHT_OFFSET = 200.0f;

	// ロックオン解除判定に使う角度差
	static constexpr float LOCKON_RELEASE_ANGLE_DIFF = 0.1f;

	// ロックオン解除判定に使うプレイヤーと敵の距離
	static constexpr float LOCKON_RELEASE_DISTANCE = 300.0f;

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
	static constexpr int SHAKE_TIME = 30;

	// ヒットストップ時間
	static constexpr int HITSTOP_TIME_A = 3;
	static constexpr int HITSTOP_TIME_B = 5;


	// HSBフィルターの明度
	static constexpr int HSB_BRIGHTNESS = 240;

	// HSBフィルターの彩度
	static constexpr int HSB_SATURATION = 150;

	// HSBフィルターの色相
	static constexpr int HSB_HUE = 80;

#pragma endregion


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

	// SEを鳴らしてもよいか
	bool se_;

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

	// 当たったパーツ探し
	ActorBase::COLLIDER_TAG FindHitParts(void);
};
