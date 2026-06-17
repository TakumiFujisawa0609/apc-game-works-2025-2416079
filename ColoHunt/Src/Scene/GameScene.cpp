#include <EffekseerForDXLib.h>
#include <cmath>
#include "../Object/Common/AnimationController.h"
#include "../Object/Actor/Stage.h"
#include "../Object/Actor/Enemy.h"
#include "../Object/Actor/Player.h"
#include "../Object/Item.h"
#include "../Manager/Audio/AudioManager.h"
#include "../Manager/Audio/SoundTable.h"
#include "../Manager/EffectResManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/Input/InputManager.h"
#include "../Application.h"
#include "../Object/Collision/CollisionManager.h"
#include "../Object/Collider/ColliderCapsule.h"
#include "../Utility/Utility.h"
#include "../Utility/AngleUtility.h"
#include "../Utility/VectorUtility.h"
#include "GameScene.h"


GameScene::GameScene(void) :
	drawHandle_(-1),

	hitStopCnt_(0),
	shakeCnt_(0), shakeWidVer_(0.0f), shakeWidSide_(0.0f),

	shadowMap_(-1),

	shader_(0),	shaderConstBuff_(0),
	mVertex_(), mIndex_(),

	blurImg_(-1), blurFlg_(false), blurCnt_(0),

	yaw_(DEFAULT_YAW),
	pitch_(DEFAULT_TILT),

	isLockon_(false),
	lockOnImg_(-1),	cntDown_(false), cnt_(10),

	hitFlgE_(false), hitFlgP_(false),

	damageNum_(0),

	time_(0.0f), timerHandle_(-1),

	failedImg_(-1),
	clearImg_(-1),
	clearCamera_(false),

	changeCnt_(0), changeFlg_(false),

	stage_(nullptr),
	item_(nullptr),
	enemy_(nullptr),
	player_(nullptr)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::InitLoad(void)
{
	// ステージのロード
	stage_ = new Stage();
	stage_->InitLoad();

	//アイテムのロード
	item_ = new Item();
	item_->InitLoad();

	//プレイヤーのロード
	player_ = new Player(item_);
	player_->InitLoad();

	//エネミーのロード
	enemy_ = new Enemy(player_);
	enemy_->InitLoad();

	timerHandle_ = CreateFontToHandle("Monserhunterfonts Xtype", TIME_FONT_SIZE, TIME_FONT_THICKNESS, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	backGroundHandle_ = MV1LoadModel((Application::PATH_MODEL + "Sky.mv1").c_str());
	lockOnImg_ = LoadGraph((Application::PATH_IMAGE + "LockOn.png").c_str());
	failedImg_ = LoadGraph((Application::PATH_IMAGE + "Failed.png").c_str());
	clearImg_ = LoadGraph((Application::PATH_IMAGE + "Clear.png").c_str());
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::RESULT);
}

void GameScene::Init(void)
{
	// ステージの初期化
	stage_->Init();

	//プレイヤーの初期化
	player_->Init();

	//アイテムの初期化
	item_->Init();

	//エネミーの初期化
	enemy_->Init();

	cameraPos_ = Camera::GetInstance()->GetCameraPos();
	playerPos_ = VAdd(player_->GetTransform().pos, PLAYER_HEAD_POS);

	cameraColl_ = new ColliderCapsule(cameraPos_, playerPos_, Camera::COLLISION_RADIUS);

	CollisionManager::CreateInstance();

	// サウンドの読み込み
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_BATTLE);

	se_ = true;

	CollisionStage();

	SetCameraPos(playerPos_, CAMERA_TO_PLAYER);

	hitFlgE_ = false;
	hitFlgP_ = false;

	//シェーダーのロード
	shader_ = LoadPixelShader((Application::PATH_SHADER + "PixelShader.cso").c_str());
	//シェーダー用の定数バッファの用意
	shaderConstBuff_ = CreateShaderConstantBuffer(sizeof(float) * 4);

	MakeSquereVertex();

	drawHandle_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TRUE);
	
	// シャドウマップの設定
	shadowMap_ = MakeShadowMap(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	SetShadowMapLightDirection(shadowMap_, SHADOW_MAP_LIGHT_DIR);
	SetShadowMapDrawArea(shadowMap_, SHADOW_MAP_DRAW_AREA_MIN, SHADOW_MAP_DRAW_AREA_MAX);

	for (int i = 0; i < BLUR_NUM; i++) {
		
		blurImg_[i] = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TRUE);
	}

	// 背景の設定
	MV1SetPosition(backGroundHandle_, { 0.0f, 0.0f, 0.0f });
	MV1SetScale(backGroundHandle_, { BACKGROUND_SCALE, BACKGROUND_SCALE, BACKGROUND_SCALE });
}

void GameScene::Update(void)
{
	// ブラーフラグが立っている
	if (blurFlg_) {

		// ブラーを作ってカウントを減らす
		SetBlur();
		blurCnt_--;

		// ブラーのカウントがなくなった
		if (blurCnt_ <= 0) {

			// フラグをおろす
			blurFlg_ = false;

			// 全部のブラーをリセットする
			for (int i = 0; i < BLUR_NUM; i++) {

				SetDrawScreen(blurImg_[i]);
				ClearDrawScreen();
				SetDrawScreen(DX_SCREEN_BACK);
			}
		}
	}

	// 各更新～～～～～～～～～～
	player_->Update();
	item_->Update();
	enemy_->Update();
	// ～～～～～～～～～～各更新

	cameraPos_ = Camera::GetInstance()->GetCameraPos();
	playerPos_ = VAdd(player_->GetTransform().pos, PLAYER_HEAD_POS);

	GameCamera();
	UpdateEffekseer3D();

	// ステージの更新
	stage_->Update();

	Collision();
	ShakeCamera();

	// ヒットストップの更新
	if (hitStopCnt_ >= 0) {

		hitStopCnt_--;
	}

	// 敵が死んだ
	if (enemy_->ClearFlg()) {
		// 1カウント目に
		if (changeCnt_ <= 1) {

			// タイム、スコアの保存
			SceneManager::GetInstance().SetTime(static_cast<int>(time_));
			SceneManager::GetInstance().SetScore(damageNum_, item_->GetItemNum());

			// SEを消す
			AudioManager::GetInstance()->StopSE();
			se_ = false;
			player_->NoSe();
			
			// BGMを鳴らす
			AudioManager::GetInstance()->PlayBGM(SoundID::BGM_CLEAR);
			// 音量を設定しなおす
			AudioManager::GetInstance()->SetBgmVolume(CLEAR_BGM_VOLUME);
		}
		// シーンが変わるフラグ判定
		if (!changeFlg_) {

			// アニメーションが終わりカメラ演出が終わったら
			if (enemy_->GetEnemyAnim()->IsEnd() && clearCamera_) {

				//シーンが変わる
				changeFlg_ = true;
				changeCnt_ = 0;
			}
		}
		else {
			// 指定のカウント数までクリアロゴを出す
			changeCnt_++;

			// 経ったら
			if (changeCnt_ >= CLEAR_LOGO_TIME) {

				// シーンを変える
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
			}
		}
	}
	//敵が死んでない
	else {

		// タイムの加算
		time_ += SceneManager::GetInstance().GetDeltaTime();
		
		// プレイヤーが死んだ
		if (player_->OverFlg()) {
			// 1カウント目に
			if (changeCnt_ <= 1) {

				// BGMを鳴らす
				AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAMEOVER);
			}
			// シーンが変わるフラグ判定
			if (!changeFlg_) {
				// アニメーションが終わりカメラ演出が終わったら
				if (player_->GetPlayerAnim()->IsEnd()) {

					//シーンが変わる
					changeFlg_ = true;
					changeCnt_ = 0;
				}
			}
			else {

				// 90カウントまで失敗ロゴを出す
				changeCnt_++;

				if (changeCnt_ >= CHANGE_CLEAR_SCENE_TIME) {

					//シーンを変える
					SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
				}
			}
		}
	}
}

void GameScene::SetBlur(void)
{
	// ブラーの数まで回す
	for (int i = 0; i < BLUR_NUM; i++) {
		// 最古のモノを変える
		if (blurCnt_ % BLUR_NUM == i) {
			
			// ブラー用の画面に今のプレイヤーを描画する
			SetDrawScreen(blurImg_[i]);

			ClearDrawScreen();

			Camera::GetInstance()->SetBeforeDraw();
			player_->DrawModel();

			// 元の描画画面に戻す
			SetDrawScreen(DX_SCREEN_BACK);

			// フィルターをかける
			GraphFilter(
				blurImg_[i], DX_GRAPH_FILTER_HSB, 1,
				HSB_BRIGHTNESS, HSB_SATURATION, HSB_HUE
			);
			break;
		}
	}
}

void GameScene::MakeSquereVertex(void)
{
	// 頂点データの作成
	int cnt = 0;
	float sX = static_cast<float>(0);
	float sY = static_cast<float>(0);
	float eX = static_cast<float>(Application::SCREEN_SIZE_X - 1);
	float eY = static_cast<float>(Application::SCREEN_SIZE_Y - 1);

	// ４頂点の初期化
	for (int i = 0; i < 4; i++)
	{
		mVertex_[i].rhw = 1.0f;
		mVertex_[i].dif = GetColorU8(255, 255, 255, 255);
		mVertex_[i].spc = GetColorU8(255, 255, 255, 255);
		mVertex_[i].su = 0.0f;
		mVertex_[i].sv = 0.0f;
	}

	// 左上
	mVertex_[cnt].pos = VGet(sX, sY, 0.0f);
	mVertex_[cnt].u = 0.0f;
	mVertex_[cnt].v = 0.0f;
	cnt++;

	// 右上
	mVertex_[cnt].pos = VGet(eX, sY, 0.0f);
	mVertex_[cnt].u = 1.0f;
	mVertex_[cnt].v = 0.0f;
	cnt++;

	// 右下
	mVertex_[cnt].pos = VGet(eX, eY, 0.0f);
	mVertex_[cnt].u = 1.0f;
	mVertex_[cnt].v = 1.0f;
	cnt++;

	// 左下
	mVertex_[cnt].pos = VGet(sX, eY, 0.0f);
	mVertex_[cnt].u = 0.0f;
	mVertex_[cnt].v = 1.0f;

	/*
	　～～～～～～
		0-----1
		|     |
		|     |
		3-----2
	　～～～～～～
		0-----1
		|  ／
		|／
		3
	　～～～～～～
			  1
		   ／ |
		 ／   |
		3-----2
	　～～～～～～
	*/


	// 頂点インデックス
	cnt = 0;
	mIndex_[cnt++] = 0;
	mIndex_[cnt++] = 1;
	mIndex_[cnt++] = 3;

	mIndex_[cnt++] = 1;
	mIndex_[cnt++] = 2;
	mIndex_[cnt++] = 3;

}

void GameScene::Collision(void)
{
#pragma region // プレイヤーの攻撃 × 敵

	// プレイヤーが攻撃しているか
	if (player_->IsAttack()) {

		// プレイヤーの剣と敵の当たり判定情報を取得する
		auto info = CollisionManager::GetInstance().Hit(enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), player_->GetOwnCollider(ActorBase::COLLIDER_TAG::SWORD));

		// 多段ヒットを防ぐためのフラグが立っていない かつ 当たっていたら
		if (!hitFlgE_ && info.HitNum > 0) {
			
			// SEを出してもよいなら
			if (se_) {
				// 効果音
				AudioManager::GetInstance()->PlaySE(SoundID::SE_ATTACK);
			}

			// エフェクト
			Effect(info.Dim[info.HitNum - 1]);

			// 多段ヒットを防ぐためのフラグを立てる
			hitFlgE_ = true;

			// 敵が死んでいないか
			if (!enemy_->ClearFlg()) {

				// 当たったパーツのタグを取得
				ActorBase::COLLIDER_TAG hitParts = FindHitParts();

				// 敵にダメージ
				enemy_->Damage(hitParts, (int)(player_->GetPower() * player_->GetBuff()));

				// 画面揺れとヒットストップの時間を決める
				if (player_->GetPower() * player_->GetBuff() >= SHAKE_POWER) {

					shakeCnt_ = SHAKE_TIME;
					hitStopCnt_ = HITSTOP_TIME_B;
				}
				else {

					hitStopCnt_ = HITSTOP_TIME_A;
				}

				// バフのリセット
				player_->ResetBuff();
			}
		}
		//当たり判定の後処理
		MV1CollResultPolyDimTerminate(info);
	}
	else {
		// 攻撃していないなら多段ヒットを防ぐためのフラグを下ろす
		hitFlgE_ = false;
	}
#pragma endregion

#pragma region // 敵の攻撃 × プレイヤー

	// プレイヤー 敵 の生存を確認する
	if (!player_->OverFlg() && !enemy_->ClearFlg()) {

		// プレイヤーの当たり判定が有効か
		if (player_->IsHit()) {

			bool info = false;

			// 火の玉攻撃
			if (enemy_->IsAttackA()) {

				// 敵の攻撃とプレイヤーの当たり判定情報を取得する
				info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::SPHERE));
			}
			// 腕攻撃
			if (enemy_->IsAttackB()) {

				// 敵の攻撃とプレイヤーの当たり判定情報を取得する
				info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::ARM_R));
			}
			// 頭攻撃
			if(enemy_->IsAttackC()){

				// 敵の攻撃とプレイヤーの当たり判定情報を取得する
				info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::HEAD));
			}
			// 取得確認
			if (info) {
				// 多段ヒットを防ぐためのフラグが立っていないか
				if (!hitFlgP_) {

					// 回避判定
					if (!player_->IsDodge()) {
						// 多段ヒットを防ぐためのフラグを立てる
						hitFlgP_ = true;
						// プレイヤーにダメージと画面揺れ
						shakeCnt_ = player_->Damage(enemy_->GetDamage(), enemy_->GetTransform().rot.y);
						// ダメージの数を増やす
						damageNum_++;

						// SEを出してもよいなら
						if (se_) {
							// 効果音
							AudioManager::GetInstance()->PlaySE(SoundID::SE_LIGHT_DAMAGE);
						}
					}
					// 回避していたら
					else { Dodge(); }
				}
			}
		}
	}

	// 攻撃していないなら多段ヒットを防ぐためのフラグを下ろす 
	if (!enemy_->IsAttack()) {
		hitFlgP_ = false;
	}
#pragma endregion

	// ステージとの当たり判定
	CollisionStage();
}

void GameScene::CollisionStage(void)
{
	CollisionManager::GetInstance().PushBack(stage_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), player_->GetOwnCollider(ActorBase::COLLIDER_TAG::LINE), &player_->GetTransform(), 75, 0.15f);
	CollisionManager::GetInstance().PushBack(enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), &player_->GetTransform(), 20, 0.1f);
	CollisionManager::GetInstance().PushBack(stage_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::LINE), &enemy_->GetTransform(), 50, 0.1f);
	
	if (enemy_->IsAttackA()) {
		if (CollisionManager::GetInstance().IsHit(stage_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::SPHERE))) {

			enemy_->DeleteShot();
		}
	}
}

void GameScene::CollisionCamera(void)
{
	std::vector<int> opacityIndex = {};

	MV1_COLL_RESULT_POLY_DIM res = CollisionManager::GetInstance().Hit(stage_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), cameraColl_);

	if (res.HitNum > 0) {
		for (int num = 0; num < res.HitNum; num++) {

			// もう入っているインデックス判定用
			bool equal = false;

			// 返り値すべて回す
			for (int i = 0; i < opacityIndex.size(); i++) {
				// 返り値と当たった時のフレームが一致したらフラグを立てる
				if (opacityIndex.at(i) == res.Dim[num].FrameIndex) {

					equal = true;
					break;
				}
			}
			// まだ入っていないインデックスなら入れる
			if (!equal) {
				opacityIndex.push_back(res.Dim[num].FrameIndex);
			}
		}
	}
	// ステージに渡す
	stage_->SetOpacityIndex(opacityIndex);
}

void GameScene::GameCamera(void)
{
	//カメラのインスタンスとプレイヤーの注視点の位置を取る
	VECTOR headPos = VAdd(player_->GetTransform().pos, PLAYER_HEAD_POS);
	headPos.x += shakeWidSide_ * cos(player_->GetTransform().rot.y);
	headPos.y += shakeWidVer_;
	headPos.z += shakeWidSide_ * sin(player_->GetTransform().rot.y);

	//シーン遷移フラグが立っていない
	if (!changeFlg_) {
		//敵が死んでいる
		if (enemy_->ClearFlg()) {
			
			//敵を注視点にする
			VECTOR targetPos = VAdd(enemy_->GetTransform().pos, CLEAR_ENEMY_POS);

			//ロックオンカメラの固定をやめる
			if (cntDown_) {
				
				cntDown_ = false;
			}
			changeCnt_++;

			//時間に応じてカメラを動かす
			if (changeCnt_ < CLEAR_CAMERA_CHANGE_A) {

				yaw_ = DEFAULT_YAW - changeCnt_ * 0.01f;
				pitch_ = -0.1f;
			}
			else if (changeCnt_ < CLEAR_CAMERA_CHANGE_B) {

				yaw_ = -DEFAULT_YAW - changeCnt_ * 0.01f;
				pitch_ = DEFAULT_TILT;
			}
			else if (changeCnt_ < CLEAR_CAMERA_CHANGE_C) {

				yaw_ = enemy_->GetTransform().rot.y - DX_PI_F;
				pitch_ = MOST_TOP_SIGHT;
				targetPos.y += changeCnt_ * 2.0f;
			}
			else {

				clearCamera_ = true;
			}

			SetCameraPos(targetPos, CAMERA_TO_PLAYER * 1.8f);

			return;
		}
		//プレイヤーが死んでいる
		if (player_->OverFlg()) {

			//ロックオンカメラの固定をやめる
			if (cntDown_) {
				
				cntDown_ = false;
			}

			//カメラをプレイヤー中心に回す
			pitch_ += 0.005f;
			yaw_ += 0.01f;

			//プレイヤーに近づけて徐々に離す
			if (changeCnt_ == 0) {

				pitch_ = -DEFAULT_TILT;
				yaw_ = player_->GetTransform().rot.y - DX_PI_F;
			}
			changeCnt_++;

			if (pitch_ > MOST_TOP_SIGHT) {

				pitch_ = MOST_TOP_SIGHT;
			}

			SetCameraPos(headPos, CAMERA_TO_PLAYER + (changeCnt_ - 55.0f) * 2.0f);
			return;
		}
	}
	//シーン遷移フラグが立っている
	else {

		//デフォルト位置にする
		pitch_ = DEFAULT_TILT;
		yaw_ = DEFAULT_YAW;

		SetCameraPos(headPos, CAMERA_TO_PLAYER);
		return;
	}

	if (!isLockon_) {
	
		//方向の取得
		pitch_ += InputManager::GetInstance().GetDirectionXZAKeyR().at(InputManager::GetInstance().GetMostPriority()).z;
		yaw_ += InputManager::GetInstance().GetDirectionXZAKeyR().at(InputManager::GetInstance().GetMostPriority()).x;

		if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::LOCK_ON).keyTrgDown) {

			cnt_ = ROCKON_LERP_TIME;
			isLockon_ = true;
			cntDown_ = true;
		}
	}
	else {

		VECTOR enemyPos = VAdd(enemy_->GetTransform().pos, PLAYER_HEAD_POS);
		VECTOR dir = VSub(enemyPos, headPos);

		float prevPitch = pitch_;
		float prevYaw = yaw_;

		// 方向から角度を求める
		pitch_ = -VNorm(dir).y;
		yaw_ = atan2f(VNorm(dir).x, VNorm(dir).z);

		// ピッチに制限
		if (pitch_ >= MAX_PITCH) { pitch_ = MAX_PITCH; }

		// 角度を徐々に変える
		pitch_ = AngleUtility::LerpAngle(prevPitch, pitch_, CAMERA_LERP_RATE);
		yaw_ = AngleUtility::LerpAngle(prevYaw, yaw_, CAMERA_LERP_RATE);

		// ロックオンを解除する条件（角度の差が一定以下 かつ プレイヤーと敵の距離が一定以下）
		if ((std::abs(prevPitch - pitch_) < LOCKON_RELEASE_ANGLE_DIFF &&
			std::abs(prevYaw - yaw_) < LOCKON_RELEASE_ANGLE_DIFF) ||
			VSize(VSub(player_->GetTransform().pos, enemy_->GetTransform().pos)) <= LOCKON_RELEASE_DISTANCE) {

			isLockon_ = false;
		}
	}
	if (cntDown_) {
		if (--cnt_ <= 0) { cntDown_ = false; }
	}

	// ピッチに制限（真上と床下を防ぐ）
	if (pitch_ > MOST_TOP_SIGHT) {

		pitch_ = MOST_TOP_SIGHT;
	}
	if (pitch_ < MOST_DOWN_SIGHT) {

		pitch_ = MOST_DOWN_SIGHT;
	}

	SetCameraPos(headPos, CAMERA_TO_PLAYER);
	CollisionCamera();
}

void GameScene::SetCameraPos(VECTOR targetPos, float diff) const
{
	// カメラの位置を計算
	VECTOR newPos{};
	newPos.x = targetPos.x - diff * cosf(pitch_) * sinf(yaw_);
	newPos.y = targetPos.y + diff * sinf(pitch_);
	newPos.z = targetPos.z - diff * cosf(pitch_) * cosf(yaw_);

	//角度の設定
	Camera::GetInstance()->SetAbsCameraAngles({ pitch_, yaw_, 0.0f });
	Camera::GetInstance()->SetAbsCameraPos(newPos);
}

void GameScene::Effect(MV1_COLL_RESULT_POLY dim)
{
	//リソースを得る
	int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::ENEMY_HIT);
	
	//エフェクトの再生
	int effect = PlayEffekseer3DEffect(resource);

	//位置等々の設定
	VECTOR pos = dim.Position[0];
	SetPosPlayingEffekseer3DEffect(effect, pos.x, pos.y, pos.z);
	SetScalePlayingEffekseer3DEffect(effect, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);
	SetRotationPlayingEffekseer3DEffect(effect, 0.0f, 0.0f, 0.0f);
}

ActorBase::COLLIDER_TAG GameScene::FindHitParts(void)
{
	bool info = false;
	ActorBase::COLLIDER_TAG temp = ActorBase::COLLIDER_TAG::NON;

	for (auto hit : enemy_->GetOwnColliders()) {
		// 被攻撃パーツじゃない場合飛ばす
		if (hit.first == ActorBase::COLLIDER_TAG::MODEL || 
			hit.first == ActorBase::COLLIDER_TAG::LINE || 
			hit.first == ActorBase::COLLIDER_TAG::CAPSULE ||
			hit.first == ActorBase::COLLIDER_TAG::SPHERE ) {

			continue;
		}
		info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::SWORD), hit.second);

		// 当たったら
		if (info) {
			// 壊れていた時
			if (enemy_->IsPartsBroke(hit.first)) {
				// 一時に入っていないなら入れる
				if (temp == ActorBase::COLLIDER_TAG::NON) {
					
					temp = hit.first;
				}
				// 飛ばす
				continue;
			}
			// そのタグを返す
			return hit.first;
		}
	}
	return temp;
}

void GameScene::ShakeCamera(void)
{
	if (shakeCnt_ <= 0) return;

	//画面揺れしてほしいフレーム数の3回に一回ずらす
	if (shakeCnt_ % 3 == 0) {
		// 左右の揺れの幅をランダムに決める(2.5 or 0 or -2.5)～～
		shakeWidSide_ = (float)GetRand(1);	//    0 or 1 or 2
		shakeWidSide_ -= 1;					//   -1 or 0 or 1
		shakeWidSide_ *= 2.5;				// -2.5 or 0 or 2.5
		// ～～～～～～～～～～～～～～～～～～～～～～～～～～～

		// 上下の揺れの幅をランダムに決める(10.0 or 0 or -10.0)～～
		shakeWidVer_ = (float)GetRand(2);	//    0 or 1 or 2
		shakeWidVer_ -= 1;					//   -1 or 0 or 1
		shakeWidVer_ *= 10.0;				// -10.0 or 0 or 10.0
		// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～
	}

	// 画面揺れのカウンターを減らす
	if (--shakeCnt_ <= 0) { shakeWidSide_ = shakeWidVer_ = 0.0f; }
}

void GameScene::Draw(void)
{
	if (hitStopCnt_ <= 0) {
		
		// 描画先を変更する
		SetDrawScreen(drawHandle_);

		// スクリーンをクリア
		ClearDrawScreen();

		// カメラ情報を設定
		Camera::GetInstance()->SetBeforeDraw();

		//シャドウマップに描画
		ShadowMap_DrawSetup(shadowMap_);

		// 各オブジェクトの描画～～～
		stage_->DrawModel();
		player_->DrawModel();
		enemy_->DrawModel();
		// ～～～各オブジェクトの描画

		// シャドウマップの描画終了
		ShadowMap_DrawEnd();

		// 背景の描画
		SetUseLighting(false);
		MV1DrawModel(backGroundHandle_);
		SetUseLighting(true);

		//影を落とすものの描画
		SetUseShadowMap(0, shadowMap_);

		stage_->DrawModel();

		SetUseShadowMap(0, -1);

		//影を落とさないものの描画
		if (blurFlg_) {

			// ブラーの描画
			for (int i = 0; i < BLUR_NUM; i++) {

				DrawGraph(0, 0, blurImg_[i], true);
			}
		}
		enemy_->DrawModel();
		enemy_->Draw();
		player_->DrawModel();
		DrawEffekseer3D();
		stage_->Draw();

		player_->Draw();
		item_->Draw();

		// タイマーフレームの描画～～～～～～～～～～～～～～～～～～～～～～～～～～～～～
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		for (const TimerFrameBoxOffset& offset : TIMER_FRAME_BOX_OFFSETS) {
			DrawBox(
				TIMER_DRAW_X + offset.x1, TIMER_DRAW_Y + offset.y1,
				TIMER_DRAW_X + offset.x2, TIMER_DRAW_Y + offset.y2,
				TIMER_FRAME_COLOR, true
			);
		}
		for (const TimerFrameTriangleOffset& offset : TIMER_FRAME_TRIANGLE_OFFSETS) {
			DrawTriangle(
				TIMER_DRAW_X + offset.x1, TIMER_DRAW_Y + offset.y1,
				TIMER_DRAW_X + offset.x2, TIMER_DRAW_Y + offset.y2,
				TIMER_DRAW_X + offset.x3, TIMER_DRAW_Y + offset.y3,
				TIMER_FRAME_COLOR, true
			);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～タイマーフレームの描画
	}
	// タイマーの描画
	DrawFormatStringToHandle(TIMER_DRAW_X, TIMER_DRAW_Y, 0xffffff, timerHandle_, "%d:%02d", (int)time_ / 60, (int)time_ % 60);

	if (changeFlg_) {

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xaa5500, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (player_->OverFlg()) {
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0, 0.0, failedImg_, true);
		}
		else {
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0, 0.0, clearImg_, true);
		}
	}

	if (cntDown_) {

		// 敵の上部にロックオンマークを描画するための座標を計算する
		VECTOR enemyPos = enemy_->GetTransform().pos;
		enemyPos.y += LOCKON_HEIGHT_OFFSET;

		// 3D座標をスクリーン座標に変換する
		VECTOR pos = ConvWorldPosToScreenPos(enemyPos);

		// 算出した座標にロックオンマークを描画する
		DrawRotaGraph((int)pos.x, (int)pos.y, cnt_, 0.0, lockOnImg_, true);
	}

	// 描画先をもとに戻す
	SetDrawScreen(DX_SCREEN_BACK);

	if (shakeCnt_ > 0) {
		// シェーダーにテクスチャを転送
		SetUseTextureToShader(0, drawHandle_);
		SetUsePixelShader(shader_);

		COLOR_F* buff = (COLOR_F*)GetBufferShaderConstantBuffer(shaderConstBuff_);
		buff->r = (float)Application::SCREEN_SIZE_X + 1.0f;
		buff->g = (float)Application::SCREEN_SIZE_Y + 1.0f;
		buff->b = 1.0f + (float)shakeCnt_;

		UpdateShaderConstantBuffer(shaderConstBuff_);
		SetShaderConstantBuffer(shaderConstBuff_, DX_SHADERTYPE_PIXEL, 0);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0);
		DrawPolygonIndexed2DToShader(mVertex_, 4, mIndex_, 2);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else {

		DrawGraph(0, 0, drawHandle_, true);
	}
}

void GameScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
	DeleteShadowMap(shadowMap_);

	for (int i = BLUR_NUM - 1; i >= 0; i--) {
	
		DeleteGraph(blurImg_[i]);
	}
	DeleteGraph(drawHandle_);

	DeleteFontToHandle(timerHandle_);

	CollisionManager::GetInstance().Release();

	if (stage_) {
		stage_->Release();
		delete stage_;
		stage_ = nullptr;
	}

	if (player_) {
		player_->Release();
		delete player_;
		player_ = nullptr;
	}

	if (enemy_) {
		enemy_->Release();
		delete enemy_;
		enemy_ = nullptr;
	}
	
	if (item_) {
		item_->Release();
		delete item_;
		item_ = nullptr;
	}
}

void GameScene::Dodge(void) 
{
	//5フレ以内ならジャストにする
	if (player_->DodgeCount() <= GREAT_DOGDE_TIMING) {

		shakeCnt_ = GREAT_DOGDE_TIME;
		player_->GreatDodge();
	}
	//13フレ以内ならゲージはたまる
	else if (player_->DodgeCount() <= GOOD_DOGDE_TIMING) {

		shakeCnt_ = GOOD_DOGDE_TIME;
		player_->GoodDodge();
	}
	//それ以上でフラグが立っているなら避けられてはいる
	else {

		player_->Dodge();
	}

	if (player_->SuccessDodge()) {

		blurFlg_ = true;
		blurCnt_ = BLUR_CNT;
		hitFlgP_ = true;
	}
}