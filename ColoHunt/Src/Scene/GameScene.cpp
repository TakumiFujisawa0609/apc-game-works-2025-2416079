#include <EffekseerForDXLib.h>
#include <cmath>
#include "../Object/Common/AnimationController.h"
#include "../Object/Actor/Stage.h"
#include "../Object/Actor/ActorBase.h"
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


GameScene::GameScene(void) : enemy_(nullptr), hitFlgE_(false), hitFlgP_(false), isLockon_(false), item_(nullptr), pitch_(DEFAULT_TILT), yaw_(DEFAULT_YAW),
	shadowMap_(-1), stage_(nullptr), player_(nullptr), cntDown_(false), cnt_(10), lockOnImg_(-1), changeCnt_(0), changeFlg_(false), clearCamera_(false),
	shakeCnt_(0), blurFlg_(false), blurCnt_(0), timerHandle_(-1), shakeWidVer_(0.0f), shakeWidSide_(0.0f), shader_(0), shaderConstBuff_(0), mVertex_(),
	mIndex_(), hitStopCnt_(0), failedImg_(-1), clearImg_(-1), drawHandle_(-1), damageNum_(0), blurImg_(-1), time_(0.0f)
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

	timerHandle_ = CreateFontToHandle("Monserhunterfonts Xtype", 45, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

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
	shadowMap_ = MakeShadowMap(4096, 4096);
	SetShadowMapLightDirection(shadowMap_, { 0.2f, -0.8f, 0.1f });
	SetShadowMapDrawArea(shadowMap_, { -2500.0f, 0.0f, -2500.0f }, { 2500.0f, 0.0f, 2500.0f });

	for (int i = 0; i < BLUR_NUM; i++) {
		
		blurImg_[i] = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TRUE);
	}

	// 背景の設定
	MV1SetPosition(backGroundHandle_, { 0.0f, 0.0f, 0.0f });
	MV1SetScale(backGroundHandle_, { 8.0f, 8.0f, 8.0f });
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

	// 各更新
	player_->Update();
	item_->Update();
	enemy_->Update();

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
			player_->NoSe();
			
			// BGMを鳴らす
			AudioManager::GetInstance()->PlayBGM(SoundID::BGM_CLEAR);
			AudioManager::GetInstance()->SetBgmVolume(185);
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
			// 90カウントまでクリアロゴを出す
			changeCnt_++;

			// 経ったら
			if (changeCnt_ >= 90) {

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

				if (changeCnt_ >= 90) {

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
			GraphFilter(blurImg_[i], DX_GRAPH_FILTER_HSB, 1, 240, 150, 80);
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
	if (player_->IsAttack()) {

		auto info = CollisionManager::GetInstance().Hit(enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), player_->GetOwnCollider(ActorBase::COLLIDER_TAG::SWORD));

		if (!hitFlgE_) {
			if (info.HitNum > 0) {

				AudioManager::GetInstance()->PlaySE(SoundID::SE_ATTACK);
				Effect(info.Dim[info.HitNum - 1]);

				hitFlgE_ = true;

				if (!enemy_->ClearFlg()) {

					enemy_->Damage((int)(player_->GetPower() * player_->GetBuff()));

					if (player_->GetPower() * player_->GetBuff() >= 12.0f) {

						shakeCnt_ = 30;
						hitStopCnt_ = 5;
					}
					else {

						hitStopCnt_ = 3;
					}
					player_->ResetBuff();
				}
			}
		}
		//当たり判定の後処理
		MV1CollResultPolyDimTerminate(info);
	}
	else {

		hitFlgE_ = false;
	}
	if (!player_->OverFlg() && !enemy_->ClearFlg()) {
		if (player_->IsHit()) {
			if (enemy_->IsAttackA()) {

				auto info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::SPHERE));

				if (info) {
					if (!hitFlgP_) {
						if (!player_->SuccessDodge()) {
							if (!player_->IsDodge()) {

								hitFlgP_ = true;
								player_->Damage(13, enemy_->GetTransform().rot.y);
								shakeCnt_ = 6;
								damageNum_++;
								AudioManager::GetInstance()->PlaySE(SoundID::SE_LIGHT_DAMAGE);
							}
							else {

								Dodge();
							}
						}
					}
				}
			}
			if (enemy_->IsAttackB()) {

				auto info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::ARM_R));

				if (enemy_->IsAttackB()) {
					if (info) {
						if (!hitFlgP_) {
							if (!player_->SuccessDodge()) {
								if (!player_->IsDodge()) {

									hitFlgP_ = true;
									player_->Damage(20, enemy_->GetTransform().rot.y);
									shakeCnt_ = 15;
									damageNum_++;
									AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAVY_DAMAGE);
								}
								else {

									Dodge();
								}
							}
						}
					}
				}
			}
			if(enemy_->IsAttackC()){

				auto info = CollisionManager::GetInstance().IsHit(player_->GetOwnCollider(ActorBase::COLLIDER_TAG::MODEL), enemy_->GetOwnCollider(ActorBase::COLLIDER_TAG::HEAD));
				
				if (enemy_->IsAttackC()) {
					if (info) {
						if (!hitFlgP_) {
							if (!player_->SuccessDodge()) {
								if (!player_->IsDodge()) {

									hitFlgP_ = true;
									player_->Damage(30, enemy_->GetTransform().rot.y);
									shakeCnt_ = 15;
									damageNum_++;
									AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAVY_DAMAGE);
								}
								else {

									Dodge();
								}
							}
						}
					}
				}
			}
		}
	}
	if (!enemy_->IsAttack()) {

		hitFlgP_ = false;
	}
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

		int num = res.HitNum;

		while (num > 0) {

			opacityIndex.push_back(res.Dim[num - 1].FrameIndex);
			num--;
		}
	}
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
			VECTOR targetPos = VAdd(enemy_->GetTransform().pos, { 0.0f, 180.0f, 0.0f });

			//ロックオンカメラの固定をやめる
			if (cntDown_) {
				
				cntDown_ = false;
			}
			changeCnt_++;

			//時間に応じてカメラを動かす
			if (changeCnt_ < 80) {

				yaw_ = DEFAULT_YAW - changeCnt_ * 0.01f;
				pitch_ = -0.1f;
			}
			else if (changeCnt_ < 160) {

				yaw_ = -DEFAULT_YAW - changeCnt_ * 0.01f;
				pitch_ = DEFAULT_TILT;
			}
			else if (changeCnt_ < 260) {

				yaw_ = enemy_->GetTransform().rot.y - DX_PI_F;
				pitch_ = DX_PI_F / 2.0f - 0.1f;
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

			if (pitch_ > DX_PI_F / 2.0f - 0.1f) {

				pitch_ = DX_PI_F / 2.0f - 0.1f;
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

			cnt_ = 10;
			isLockon_ = true;
			cntDown_ = true;
		}
	}
	else {

		VECTOR enemyPos = VAdd(enemy_->GetTransform().pos, { 0.0f, 200.f, 0.0f });
		VECTOR dir = VSub(enemyPos, headPos);

		float prevPitch = pitch_;
		float prevYaw = yaw_;

		pitch_ = -VNorm(dir).y;
		yaw_ = atan2f(VNorm(dir).x, VNorm(dir).z);

		if (pitch_ >= 0.5f) {

			pitch_ = 0.5f;
		}
		pitch_ = AngleUtility::LerpAngle(prevPitch, pitch_, 0.8f);
		yaw_ = AngleUtility::LerpAngle(prevYaw, yaw_, 0.8f);

		if ((std::abs(prevPitch - pitch_) < 0.1f && std::abs(prevYaw - yaw_) < 0.1f) || VSize(VSub(player_->GetTransform().pos, enemy_->GetTransform().pos)) <= 300.0f) {

			isLockon_ = false;
		}
	}
	if (cntDown_) {

		cnt_--;
		if (cnt_ <= 0) {

			cnt_ = 10;
			cntDown_ = false;
		}
	}

	// ピッチに制限（真上と床下を防ぐ）
	if (pitch_ > DX_PI_F / 2.0f - 0.1f) {

		pitch_ = DX_PI_F / 2.0f - 0.1f;
	}
	if (pitch_ < -DX_PI_F / 18.0f) {

		pitch_ = -DX_PI_F / 18.0f;
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
	SetScalePlayingEffekseer3DEffect(effect, 20.0f, 20.0f, 20.0f);
	SetRotationPlayingEffekseer3DEffect(effect, 0.0f, 0.0f, 0.0f);
}

void GameScene::ShakeCamera(void)
{
	if (shakeCnt_ <= 0) return;

	//画面揺れしてほしいフレーム数の3回に一回ずらす
	if (shakeCnt_ % 3 == 0) {
		
		shakeWidSide_ = (float)GetRand(2);
		shakeWidSide_ -= 1;
		shakeWidSide_ *= 2.5;

		shakeWidVer_ = (float)GetRand(2);
		shakeWidVer_ -= 1;
		shakeWidVer_ *= 10.0;
	}
	shakeCnt_--;
	
	if (shakeCnt_ <= 0) {

		shakeWidSide_ = shakeWidVer_ = 0.0f;
	}
}

void GameScene::Draw(void)
{
	if (hitStopCnt_ <= 0) {
		
		SetDrawScreen(drawHandle_);

		ClearDrawScreen();
		Camera::GetInstance()->SetBeforeDraw();

		//シャドウマップに描画
		ShadowMap_DrawSetup(shadowMap_);

		stage_->DrawModel();
		player_->DrawModel();
		enemy_->DrawModel();

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

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		DrawBox(10, 25, 210, 80, 0x000000, true);
		DrawBox(30, 10, 190, 25, 0x000000, true);
		DrawBox(30, 80, 190, 95, 0x00000, true);
		DrawTriangle(10, 25, 30, 25, 30, 10, 0x000000, true);
		DrawTriangle(210, 25, 190, 25, 190, 10, 0x000000, true);
		DrawTriangle(210, 80, 190, 95, 190, 80, 0x000000, true);
		DrawTriangle(10, 80, 30, 95, 30, 80, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	DrawFormatStringToHandle(30, 30, 0xffffff, timerHandle_, "%d:%02d", (int)time_ / 60, (int)time_ % 60);

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

		VECTOR enemyPos = VAdd(enemy_->GetTransform().pos, { 0.0f, 200.f, 0.0f });

		VECTOR pos = ConvWorldPosToScreenPos(enemyPos);
		DrawRotaGraph((int)pos.x, (int)pos.y, cnt_, 0.0, lockOnImg_, true);
	}
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
	//3フレ以内ならジャストにする
	if (player_->DodgeCount() <= 3) {

		shakeCnt_ = 20;
		player_->GreatDodge();
	}
	//13フレ以内ならゲージはたまる
	else if (player_->DodgeCount() <= 13) {

		shakeCnt_ = 10;
		player_->GoodDodge();
	}
	//それ以上でフラグが立っているなら避けられてはいる
	else {

		player_->Dodge();
	}

	if (player_->SuccessDodge()) {

		blurFlg_ = true;
		blurCnt_ = 25;
	}
}