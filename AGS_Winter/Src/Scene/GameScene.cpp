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
#include "../Manager/Input/Controller.h"
#include "../Application.h"
#include "../Object/Collision/CollisionManager.h"
#include "../Utility/Utility.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/AngleUtility.h"
#include "../Utility/VectorUtility.h"
#include "GameScene.h"


GameScene::GameScene(void) :enemy_(), hitFlgE_(), hitFlgP_(), isLockon_(false), item_(), pitch_(DEFAULT_TILT), yaw_(DEFAULT_YAW),
shadowMap_(), stage_(), player_(), cntDown_(false), cnt_(10), lockOnImg_(), changeCnt_(0), changeFlg_(false), clearCamera_(false),
shakeCnt_(0), blurFlg_(false), blurCnt_(0)
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

	CollisionManager::CreateInstance();

	// サウンドの読み込み
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_BATTLE);

	CollisionStage();

	VECTOR headPos = VAdd(player_->GetTransform().pos, { 0.0f, 180.0f, 0.0f });
	SetCameraPos(headPos, CAMERA_TO_PLAYER);

	hitFlgE_ = false;
	hitFlgP_ = false;

	shadowMap_ = MakeShadowMap(8192, 8192);

	SetShadowMapLightDirection(shadowMap_, { 0.2f, -0.8f, 0.0f });
	SetShadowMapDrawArea(shadowMap_, { -2500.0f, 0.0f, -2500.0f }, { 2500.0f, 0.0f, 2500.0f });

	for (int i = 0; i < BLUR_NUM; i++) {
		
		blurImg_[i] = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TRUE);
	}
}

void GameScene::Update(void)
{
	if (blurFlg_) {

		SetBlur();
		blurCnt_--;

		if (blurCnt_ <= 0) {

			blurFlg_ = false;

			for (int i = 0; i < BLUR_NUM; i++) {

				SetDrawScreen(blurImg_[i]);
				ClearDrawScreen();
				SetDrawScreen(DX_SCREEN_BACK);
			}
		}
	}

	player_->Update();
	item_->Update();
	enemy_->Update();
	GameCamera();

	// ステージの更新
	stage_->Update();

	Collision();
	ShakeCamera();

	if (enemy_->ClearFlg()) {
		AudioManager::GetInstance()->StopSE();
		player_->NoSe();
		if (!changeFlg_) {
			if (changeCnt_ <= 1) {

				AudioManager::GetInstance()->PlayBGM(SoundID::BGM_CLEAR);
				AudioManager::GetInstance()->SetBgmVolume(185);
			}
			if (enemy_->GetEnemyAnim()->IsEnd() && clearCamera_) {

				changeFlg_ = true;
				changeCnt_ = 0;
			}
		}
		else {

			changeCnt_++;

			if (changeCnt_ >= 90) {

				SceneManager::GetInstance().SetScore(damageNum_, item_->GetItemNum());
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
			}
		}
	}
	else if (player_->OverFlg()) {
		if (changeCnt_ <= 1) {

			AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAMEOVER);
		}
		if (!changeFlg_) {
			if (player_->GetPlayerAnim()->IsEnd()) {

				changeFlg_ = true;
				changeCnt_ = 0;
			}
		}
		else {

			changeCnt_++;

			if (changeCnt_ >= 90) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
			}
		}
	}
}

void GameScene::SetBlur(void)
{
	for (int i = 0; i < BLUR_NUM; i++) {
		if (blurCnt_ % BLUR_NUM == i) {
			
			SetDrawScreen(blurImg_[i]);

			ClearDrawScreen();

			SceneManager::GetInstance().GetCamera()->SetBeforeDraw();
			player_->DrawModel();

			SetDrawScreen(DX_SCREEN_BACK);

			GraphFilter(blurImg_[i], DX_GRAPH_FILTER_HSB, 1, 240, 150, 80);
			break;
		}
	}
}

void GameScene::Blur(void)
{
	for (int i = 0; i < BLUR_NUM; i++) {

		DrawGraph(0, 0, blurImg_[i], true);
	}
}

void GameScene::Collision(void)
{
	if (player_->IsAttack()) {

		auto info = CollisionManager::GetInstance().HitCapsule(enemy_->GetOwnColliders(), player_->GetSwordColliders());

		if (!hitFlgE_) {
			if (info.HitNum > 0) {

				AudioManager::GetInstance()->PlaySE(SoundID::SE_ATTACK);
				Effect(info.Dim[info.HitNum - 1]);

				hitFlgE_ = true;

				if (!enemy_->ClearFlg()) {

					enemy_->Damage(player_->GetPower() * player_->GetBuff());

					if (player_->GetPower() * player_->GetBuff() >= 12.0f) {

						shakeCnt_ = 30;
						SceneManager::GetInstance().SetScreenImage();
						hitStopImg_ = SceneManager::GetInstance().GetScreenImage();
						hitStopCnt_ = 5;
					}
					else {

						SceneManager::GetInstance().SetScreenImage();
						hitStopImg_ = SceneManager::GetInstance().GetScreenImage();
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

				auto info = CollisionManager::GetInstance().IsHitSphere(player_->GetOwnColliders(), enemy_->GetShotColliders());

				if (info) {
					if (!hitFlgP_) {
						if (!player_->SuccessDodge()) {
							if (!player_->IsDodge()) {

								hitFlgP_ = true;
								player_->Damage(13, enemy_->GetTransform().rot.y);
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
			if (enemy_->IsAttackB() || enemy_->IsAttackC()) {

				auto info = CollisionManager::GetInstance().IsHitCapsule(player_->GetOwnColliders(), enemy_->GetAttackStartPos(), enemy_->GetAttackEndPos(), Enemy::ATTACK_RADIUS);

				if (enemy_->IsAttackB()) {
					if (info) {
						if (!hitFlgP_) {
							if (!player_->SuccessDodge()) {
								if (!player_->IsDodge()) {

									hitFlgP_ = true;
									player_->Damage(20, enemy_->GetTransform().rot.y);
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
				if (enemy_->IsAttackC()) {
					if (info) {
						if (!hitFlgP_) {
							if (!player_->SuccessDodge()) {
								if (!player_->IsDodge()) {

									hitFlgP_ = true;
									player_->Damage(30, enemy_->GetTransform().rot.y);
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
	CollisionManager::GetInstance().PushBack(stage_->GetOwnColliders(), player_->GetOwnColliders(), &player_->GetTransform(), 75.0f, 0.15f);
	CollisionManager::GetInstance().PushBack(enemy_->GetOwnColliders(), player_->GetOwnColliders(), &player_->GetTransform(), 20.0f, 0.1f);
	CollisionManager::GetInstance().PushBack(stage_->GetOwnColliders(), enemy_->GetOwnColliders(), &enemy_->GetTransform(), 50.0f, 0.1f);
	
	if (enemy_->IsAttackA()) {
		if (CollisionManager::GetInstance().IsHitSphere(stage_->GetOwnColliders(), enemy_->GetShotColliders())) {

			enemy_->DeleteShot();
		}
	}
}

void GameScene::CollisionCamera(void)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR cPos = camera->GetCameraPos();
	VECTOR pPos = VAdd(player_->GetTransform().pos, { 0.0f, 100.0f, 0.0f });
	
	std::vector<int> opacityIndex = {};

	MV1_COLL_RESULT_POLY_DIM res = CollisionManager::GetInstance().HitCapsule(stage_->GetOwnColliders(), cPos, pPos, Player::COL_CAPSULE_RADIUS);

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
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR headPos = VAdd(player_->GetTransform().pos, { 0.0f, 180.0f, 0.0f });
	headPos.x += shakeWidSide_ * cos(player_->GetTransform().rot.y);
	headPos.y += shakeWidVer_;
	headPos.z += shakeWidSide_ * sin(player_->GetTransform().rot.y);

	if (!changeFlg_) {
		if (enemy_->ClearFlg()) {
			VECTOR targetPos = VAdd(enemy_->GetTransform().pos, { 0.0f, 180.0f, 0.0f });

			if (cntDown_) {
				
				cntDown_ = false;
			}
			changeCnt_++;

			if (changeCnt_ < 80) {

				yaw_ = DEFAULT_YAW - changeCnt_ * 0.01f;
				pitch_ = -DEFAULT_TILT;
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
		if (player_->OverFlg()) {

			if (cntDown_) {

				cntDown_ = false;
			}
			pitch_ += 0.005f;
			yaw_ += 0.01f;

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
	else {

		pitch_ = DEFAULT_TILT;
		yaw_ = DEFAULT_YAW;

		SetCameraPos(headPos, CAMERA_TO_PLAYER);
		return;
	}

	////前後左右の移動処理
	//if (GetJoypadNum() == 0) {

	//	if (CheckHitKey(KEY_INPUT_UP) == 1) {

	//		pitch += 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_DOWN) == 1) {

	//		pitch -= 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {

	//		yaw_ += 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_LEFT) == 1) {

	//		yaw_ -= 0.1f;
	//	}
	//}
	//else {
	//}

	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (!isLockon_) {
	
		//方向の取得
		pitch_ += padState.AKeyRY / 25000.0f;
		yaw_ += padState.AKeyRX / 12000.0f;

		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::L)] || CheckHitKey(KEY_INPUT_O)) {

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
	Camera* camera = SceneManager::GetInstance().GetCamera();
	camera->SetAbsCameraAngles({ pitch_, yaw_, 0.0f });
	camera->SetAbsCameraPos(newPos);
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

	Camera* camera = SceneManager::GetInstance().GetCamera();

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
		//シャドウマップに描画
		ShadowMap_DrawSetup(shadowMap_);

		//MV1DrawModel(player_->GetModelId());
		//MV1DrawModel(enemy_->GetModelId());

		player_->DrawModel();
		enemy_->DrawModel();

		ShadowMap_DrawEnd();

		//影に関係のあるものの描画
		SetUseShadowMap(0, shadowMap_);

		stage_->DrawModel();
		if (blurFlg_) {

			Blur();
		}
		player_->DrawModel();
		enemy_->DrawModel();
		enemy_->Draw();
		stage_->Draw();

		SetUseShadowMap(0, -1);

		//影に関係ないものの描画(後)
		player_->Draw();
		item_->Draw();
		
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 185);
		DrawBox(10, 25, 210, 80, 0x000000, true);
		DrawBox(30, 10, 190, 25, 0x000000, true);
		DrawBox(30, 80, 190, 95, 0x00000, true);
		DrawTriangle(10, 25, 30, 25, 30, 10, 0x000000, true);
		DrawTriangle(210, 25, 190, 25, 190, 10, 0x000000, true);
		DrawTriangle(210, 80, 190, 95, 190, 80, 0x000000, true);
		DrawTriangle(10, 80, 30, 95, 30, 80, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	if (!enemy_->ClearFlg()) {
	
		int time = SceneManager::GetInstance().GetTimer();
		DrawFormatStringToHandle(30, 30, 0xffffff, timerHandle_, "%d:%02d", time / 60, time % 60);
	}
	else {

		int time = SceneManager::GetInstance().GetTime();
		DrawFormatStringToHandle(30, 30, 0xffffff, timerHandle_, "%d:%02d", time / 60, time % 60);
	}

	if (!changeFlg_) {
		if (hitStopCnt_ > 0) {

			DrawGraph(0, 0, hitStopImg_, true);
			hitStopCnt_--;
		}
	}
	else {

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xaa5500, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

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
		DrawRotaGraph (pos.x, pos.y, cnt_, 0.0, lockOnImg_, true);
	}
	//if (hitFlgP_) {

	//	DrawString(0, 0, "あたった", 0xff00ff);
	//}

	//std::vector<ShotBase*> shots = cannon_->GetShots();
	//for (ShotBase* shot : shots) {
	//	if (shot->GetType() == ShotBase::TYPE::BEAM) {
	//		if (shot->IsCollisionState()) {

	//			DrawCapsule3D(shot->GetTransform().pos, VAdd(shot->GetTransform().pos, VScale(shot->GetDirection(), 500.0f)), shot->GetCollisionRadius(), 16, 0xffffff, 0xffffff, false);
	//		}
	//	}
	//}
}

void GameScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
	DeleteShadowMap(shadowMap_);

	for (int i = BLUR_NUM - 1; i >= 0; i--) {
	
		DeleteGraph(blurImg_[i]);
	}
	DeleteFontToHandle(timerHandle_);

	CollisionManager::GetInstance().Release();

	stage_->Release();
	delete stage_;

	player_->Release();
	delete player_;

	enemy_->Release();
	delete enemy_;
	
	item_->Release();
	delete item_;
}

void GameScene::Dodge(void) {

	if (player_->DodgeCount() <= 3) {

		shakeCnt_ = 20;
		player_->GreatDodge();
	}
	else if (player_->DodgeCount() <= 13) {

		shakeCnt_ = 10;
		player_->GoodDodge();
	}
	else {

		player_->Dodge();
	}

	if (player_->SuccessDodge()) {

		blurFlg_ = true;
		blurCnt_ = 25;
	}
}