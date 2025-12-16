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
#include "../Utility/Utility.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/AngleUtility.h"
#include "../Utility/VectorUtility.h"
#include "GameScene.h"


GameScene::GameScene(void) :enemy_(), hitFlgE_(), hitFlgP_(), isLockon_(false), item_(), pitch_(DEFAULT_TILT), yaw_(DEFAULT_YAW),
	shadowMap_(), stage_(), player_(), cntDown_(false), cnt_(10), lockOnImg_(), changeCnt_(0), changeFlg_(false)
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

	//プレイヤーのロード
	player_ = new Player();
	player_->InitLoad();

	//アイテムのロード
	item_ = new Item();
	item_->InitLoad();

	//エネミーのロード
	enemy_ = new Enemy(player_);
	enemy_->InitLoad();

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

	// サウンドの読み込み
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_BATTLE);
	AudioManager::GetInstance()->SetBgmVolume(150);

	CollisionData data = { player_->GetPos(), player_->GetPrevPos(), COLLISION_TYPE::PLAYER };
	CollisionData datA = { enemy_->GetPos(), enemy_->GetPrevPos(), COLLISION_TYPE::ENEMY };

	CollisionStage(data);
	CollisionStage(datA);

	enemy_->InitModel();
	player_->InitModel();

	VECTOR headPos = VAdd(player_->GetPos(), { 0.0f, 180.0f, 0.0f });
	SetCameraPos(headPos, CAMERA_TO_PLAYER);

	hitFlgE_ = false;
	hitFlgP_ = false;

	shadowMap_ = MakeShadowMap(8192, 8192);

	SetShadowMapLightDirection(shadowMap_, { 0.2f, -0.8f, 0.0f });
	SetShadowMapDrawArea(shadowMap_, { -2500.0f, 0.0f, -2500.0f }, { 2500.0f, 0.0f, 2500.0f });
}

void GameScene::Update(void)
{
	player_->Update();
	enemy_->Update();
	GameCamera();

	if (player_->HealUsed() || !player_->Healable()) {

		item_->SetUsing(true);
	}
	else {

		item_->SetUsing(false);
	}

	item_->Update();

	if (item_->IsUse()) {
		switch (item_->GetType())
		{
			case Item::TYPE::HP:
			
			player_->Heal();
			AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAL);
			break;
			
			case Item::TYPE::HP_MAX:
			
			player_->HealMax();
			AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAL);
			break;

			case Item::TYPE::STAMINA:

			player_->StaminaMax();
			break;
		}
	}

	// ステージの更新
	stage_->Update();

	Collision();
	
	if (enemy_->ClearFlg()) {
		if (!changeFlg_) {
			if (changeCnt_ <= 1) {

				AudioManager::GetInstance()->PlayBGM(SoundID::BGM_CLEAR);
				AudioManager::GetInstance()->SetBgmVolume(255);
			}
			if (enemy_->GetEnemyAnim()->IsEnd()) {

				changeFlg_ = true;
				changeCnt_ = 0;
			}
		}
		else {

			changeCnt_++;

			if (changeCnt_ >= 90) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
			}
		}
	}
	else if (player_->OverFlg()) {
		if (!changeFlg_) {
			if (changeCnt_ <= 1) {

				AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAMEOVER);
				AudioManager::GetInstance()->SetBgmVolume(255);
			}
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

void GameScene::Collision(void)
{
	MV1_COLL_RESULT_POLY_DIM info{};

	if (player_->IsAttack()) {
		
		info = MV1CollCheck_Capsule(enemy_->GetModelId(), -1, player_->GetAttackStartPos(), player_->GetAttackEndPos(), 10.0f);

		if (!hitFlgE_) {
			if (info.HitNum > 0) {

				AudioManager::GetInstance()->PlaySE(SoundID::SE_ATTACK);
				Effect(info.Dim[info.HitNum - 1]);

				hitFlgE_ = true;
				enemy_->Damage(player_->GetPower() * player_->GetBuff());
				player_->ResetBuff();
			}
		}
	}
	else {

		hitFlgE_ = false;
	}
	if (!player_->OverFlg()) {
		if (player_->IsHit()) {
			if (enemy_->IsAttackA()) {

				info = MV1CollCheck_Sphere(player_->GetModelId(), -1, enemy_->GetAttackStartPos(), Enemy::ATTACK_RADIUS);

				if (info.HitNum > 0) {
					if (!hitFlgP_) {
						if (!player_->SuccessDodge()) {
							if (!player_->IsDodge()) {

								hitFlgP_ = true;
								player_->Damage(13, enemy_->GetAngle().y);
								AudioManager::GetInstance()->PlaySE(SoundID::SE_LIGHT_DAMAGE);
							}
							else {
								if (player_->DodgeCount() <= 3) {

									player_->GreatDodge();
								}
								else {

									player_->GoodDodge();
								}
							}
						}
					}
				}
			}
			if (enemy_->IsAttackB()) {

				info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemy_->GetAttackStartPos(), enemy_->GetAttackEndPos(), Enemy::ATTACK_RADIUS);

				if (info.HitNum > 0) {
					if (!hitFlgP_) {
						if (!player_->SuccessDodge()) {
							if (!player_->IsDodge()) {
								hitFlgP_ = true;
								player_->Damage(17, enemy_->GetAngle().y);
								AudioManager::GetInstance()->PlaySE(SoundID::SE_LIGHT_DAMAGE);
							}
							else {
								if (player_->DodgeCount() <= 3) {

									player_->GreatDodge();
								}
								else {
									player_->GoodDodge();
								}
							}
						}
					}
				}
			}
			if (enemy_->IsAttackC()) {

				info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemy_->GetAttackStartPos(), enemy_->GetAttackEndPos(), Enemy::ATTACK_RADIUS * 2);

				if (info.HitNum > 0) {
					if (!hitFlgP_) {
						if (!player_->SuccessDodge()) {
							if (!player_->IsDodge()) {
								hitFlgP_ = true;
								player_->Damage(20, enemy_->GetAngle().y);
								AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAVY_DAMAGE);
							}
							else {
								if (player_->DodgeCount() <= 3) {

									player_->GreatDodge();
								}
								else {

									player_->GoodDodge();
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
	//当たり判定の後処理
	MV1CollResultPolyDimTerminate(info);
	
	//ステージと
	CollisionData data = { player_->GetPos(), player_->GetPrevPos(), COLLISION_TYPE::PLAYER };
	CollisionData datA = { enemy_->GetPos(), enemy_->GetPrevPos(), COLLISION_TYPE::ENEMY };
	if (enemy_->IsAttackA()) {
		CollisionData daTA = { enemy_->GetAttackStartPos(), enemy_->GetAttackPrevPos(), COLLISION_TYPE::ENEMY_ATTACK };
		CollisionStage(daTA);
	}
	CollisionStage(data);
	CollisionStage(datA);
}

void GameScene::CollisionStage(CollisionData data)
{
	//上下(線)
	VECTOR topPos = data.pos;
	topPos.y += COLLISION_STAGE_DIFF * 2.0f;

	VECTOR downPos = data.pos;
	downPos.y -= COLLISION_STAGE_DIFF * 2.0f;

	MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(stage_->GetModelId(), -1, topPos, downPos);
	MV1_COLL_RESULT_POLY_DIM res = {};

	if (result.HitFlag == 1) {
		switch (data.type) {
		case COLLISION_TYPE::PLAYER:

			player_->SetPos(result.HitPosition);
			break;

		case COLLISION_TYPE::ENEMY:

			enemy_->SetPos(result.HitPosition);
			break;
		}
	}
	//前後左右(球)
	if (!VectorUtility::Equals(data.pos, data.prev)) {
		switch (data.type) {
		case COLLISION_TYPE::PLAYER:

			res = MV1CollCheck_Sphere(stage_->GetModelId(), -1,
				VAdd(data.pos, { 0.0f, COLLISION_STAGE_DIFF * 2.0f, 0.0f }), COLLISION_STAGE_DIFF);

			if (res.HitNum > 0) {

				player_->SetPos(data.prev);
			}
			break;

		case COLLISION_TYPE::ENEMY:

			res = MV1CollCheck_Sphere(stage_->GetModelId(), -1,
				VAdd(data.pos, { 0.0f, COLLISION_STAGE_DIFF * 3.5f, 0.0f }), COLLISION_STAGE_DIFF);

			if (res.HitNum > 0) {

				enemy_->SetPos(data.prev);
			}
			break;

		case COLLISION_TYPE::ENEMY_ATTACK:

			res = MV1CollCheck_Sphere(stage_->GetModelId(), -1, data.pos , Enemy::ATTACK_RADIUS);

			if (res.HitNum > 0) {

				enemy_->DeleteAttackA();
			}
			break;
		}
	}
	MV1CollResultPolyDimTerminate(res);
}

void GameScene::GameCamera(void)
{
	//カメラのインスタンスとプレイヤーの注視点の位置を取る
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR headPos = VAdd(player_->GetPos(), { 0.0f, 180.0f, 0.0f });

	if (player_->OverFlg()) {
		if (!changeFlg_) {

			pitch_ += 0.005f;
			yaw_ += 0.01f;

			if (changeCnt_ == 0) {

				pitch_ = -DEFAULT_TILT;
				yaw_ = player_->GetAngle().y - DX_PI_F;
			}

			changeCnt_++;

			if (pitch_ > DX_PI_F / 2.0f - 0.1f) {

				pitch_ = DX_PI_F / 2.0f - 0.1f;
			}

			SetCameraPos(headPos, CAMERA_TO_PLAYER + (changeCnt_ - 50.0f) * 3.0f);
		}
		else {

			pitch_ = DEFAULT_TILT;
			yaw_ = DEFAULT_YAW;

			SetCameraPos(headPos, CAMERA_TO_PLAYER);
		}
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

		VECTOR enemyPos = VAdd(enemy_->GetPos(), { 0.0f, 200.f, 0.0f });
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

		if ((std::abs(prevPitch - pitch_) < 0.1f && std::abs(prevYaw - yaw_) < 0.1f) || VSize(VSub(player_->GetPos(), enemy_->GetPos())) <= 300.0f) {

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
}

void GameScene::SetCameraPos(VECTOR targetPos, float diff)
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
	SetScalePlayingEffekseer3DEffect(effect, 15.0f, 15.0f, 15.0f);
	SetRotationPlayingEffekseer3DEffect(effect, 0.0f, 0.0f, 0.0f);
}

void GameScene::Draw(void)
{
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
	player_->DrawModel();
	enemy_->DrawModel();
	enemy_->Draw();

	SetUseShadowMap(0, -1);

	//影に関係ないものの描画(後)
	player_->Draw();
	item_->Draw();

	if (changeFlg_) {

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

		VECTOR enemyPos = VAdd(enemy_->GetPos(), { 0.0f, 200.f, 0.0f });

		VECTOR pos = ConvWorldPosToScreenPos(enemyPos);
		DrawRotaGraph (pos.x, pos.y, 1.0 * cnt_, 0.0, lockOnImg_, true);
	}
	//if (hitFlgP_) {

	//	DrawString(0, 0, "あたった", 0xff00ff);
	//}

	//std::vector<ShotBase*> shots = cannon_->GetShots();
	//for (ShotBase* shot : shots) {
	//	if (shot->GetType() == ShotBase::TYPE::BEAM) {
	//		if (shot->IsCollisionState()) {

	//			DrawCapsule3D(shot->GetPos(), VAdd(shot->GetPos(), VScale(shot->GetDirection(), 500.0f)), shot->GetCollisionRadius(), 16, 0xffffff, 0xffffff, false);
	//		}
	//	}
	//}
}

void GameScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
	DeleteShadowMap(shadowMap_);

	stage_->Release();
	delete stage_;

	player_->Release();
	delete player_;

	enemy_->Release();
	delete enemy_;
	
	item_->Release();
	delete item_;
}
