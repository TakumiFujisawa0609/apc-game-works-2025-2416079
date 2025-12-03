#include <EffekseerForDXLib.h>
#include <cmath>
#include "../Object/Stage.h"
#include "../Object/Actor/EnemyBase.h"
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


GameScene::GameScene(void):enemy_(), hitFlgE_(), hitFlgP_(), isFirst_(), isLockon_(), item_(), pitch_(), yaw_(),
	shadowMap_(), stage_(), player_()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// ステージの初期化
	stage_ = new Stage();
	stage_->Init();

	//プレイヤーの初期化
	player_ = new Player();
	player_->Init();

	//アイテムの初期化
	item_ = new Item();
	item_->Init();

	//エネミーの初期化
	enemy_ = new EnemyBase(player_);
	enemy_->Init();

	// サウンドの読み込み
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_BATTLE);

	pitch_ = 0.3f;
	yaw_ = 0.0f;
	isFirst_ = false;
	isLockon_ = false;
		
	GameCamera();

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

	if (enemy_->ClearFlg() == true) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
	if (player_->OverFlg() == true) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
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
				enemy_->Damage(player_->GetPower());
			}
		}
	}
	else {

		hitFlgE_ = false;
	}
	if (player_->IsHit()) {
		if (enemy_->IsAttackA()) {

			info = MV1CollCheck_Sphere(player_->GetModelId(), -1, enemy_->GetAttackStartPos(), EnemyBase::ATTACK_RADIUS);

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

			info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemy_->GetAttackStartPos(), enemy_->GetAttackEndPos(), EnemyBase::ATTACK_RADIUS);

			if (info.HitNum > 0) {
				if (!hitFlgP_) {
					if (!player_->SuccessDodge()) {
						if (!player_->IsDodge()) {
							hitFlgP_ = true;
							player_->Damage(10, enemy_->GetAngle().y);
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

			info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemy_->GetAttackStartPos(), enemy_->GetAttackEndPos(), EnemyBase::ATTACK_RADIUS * 2);

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

			res = MV1CollCheck_Sphere(stage_->GetModelId(), -1, data.pos , EnemyBase::ATTACK_RADIUS);

			if (res.HitNum > 0) {

				enemy_->DeleteAttackA();
			}
			break;
		}
	}
	MV1CollResultPolyDimTerminate(res);
}

void GameScene::CollisionCamera(CollisionData data)
{
	//線分のポジションと焦点の角度の設定
	VECTOR startPos = data.pos;
	VECTOR endPos = data.prev;
	VECTOR dir = VNorm(VSub(startPos, endPos));

	//ステージとの当たり判定
	MV1_COLL_RESULT_POLY resultS = MV1CollCheck_Line(stage_->GetModelId(), -1, startPos, endPos);
	VECTOR distS = Utility::VECTOR_ZERO;

	//敵との当たり判定
	MV1_COLL_RESULT_POLY resultE = MV1CollCheck_Line(enemy_->GetModelId(), -1, startPos, endPos);
	VECTOR distE = Utility::VECTOR_ZERO;

	if (resultS.HitFlag == 1) {
		if (resultE.HitFlag == 1) {

			//どちらも当たっているのでカメラの位置から当たった場所へのベクトルを取る
			distS = VSub(endPos, resultS.HitPosition);
			distE = VSub(endPos, resultE.HitPosition);

			//取ったベクトル大きさがの大きい方に位置を合わせる
			SceneManager::GetInstance().GetCamera()->SetAbsCameraPos(VAdd(VectorUtility::Comparison(distS, distE)? resultS.HitPosition : resultE.HitPosition,
				VScale(dir, COLLISION_CAMERA_DIFF)));
		}
		else {

			//ステージのみ当たっているので当たった場所に移す
			SceneManager::GetInstance().GetCamera()->SetAbsCameraPos(VAdd(resultS.HitPosition, VScale(dir, COLLISION_CAMERA_DIFF)));
		}
	}
	else {
		if (resultE.HitFlag == 1) {

			//敵のみ当たっているので当たった場所に移す
			SceneManager::GetInstance().GetCamera()->SetAbsCameraPos(VAdd(resultE.HitPosition, VScale(dir, COLLISION_CAMERA_DIFF)));
		}
		else {
			//当たってない場合引数でもらったカメラの位置にする
			SceneManager::GetInstance().GetCamera()->SetAbsCameraPos(endPos);
		}
	}
}

void GameScene::GameCamera(void)
{
	//カメラのインスタンスとプレイヤーの注視点の位置を取る
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR headPos = VAdd(player_->GetPos(), { 0.0f, 180.0f, 0.0f });

	// 上下左右回転

	if (!isFirst_) {

		// カメラの位置を計算
		VECTOR newPos{};
		newPos.x = headPos.x - CAMERA_TO_PLAYER * cosf(pitch_) * sinf(yaw_);
		newPos.y = headPos.y + CAMERA_TO_PLAYER * sinf(pitch_);
		newPos.z = headPos.z - CAMERA_TO_PLAYER * cosf(pitch_) * cosf(yaw_);

		//カメラの位置の設定
		camera->SetAbsCameraPos(newPos);
		camera->SetAbsCameraAngles({ pitch_, yaw_, 0.0f });

		isFirst_ = true;

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

			isLockon_ = true;
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

	// ピッチに制限（真上と床下を防ぐ）
	if (pitch_ > DX_PI_F / 2.0f - 0.1f) {

		pitch_ = DX_PI_F / 2.0f - 0.1f;
	}
	if (pitch_ < -DX_PI_F / 18.0f) {

		pitch_ = -DX_PI_F / 18.0f;
	}

	// カメラの位置を計算
	VECTOR newPos{};
	newPos.x = headPos.x - CAMERA_TO_PLAYER * cosf(pitch_) * sinf(yaw_);
	newPos.y = headPos.y + CAMERA_TO_PLAYER * sinf(pitch_);
	newPos.z = headPos.z - CAMERA_TO_PLAYER * cosf(pitch_) * cosf(yaw_);

	CollisionData data = { headPos, newPos, COLLISION_TYPE::CAMERA };
	CollisionCamera(data);

	//角度の設定
	camera->SetAbsCameraAngles({ pitch_, yaw_, 0.0f });
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

	stage_->Draw();
	player_->DrawModel();
	enemy_->DrawModel();
	enemy_->Draw();

	SetUseShadowMap(0, -1);

	//影に関係ないものの描画(後)
	player_->Draw();
	item_->Draw();

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

void GameScene::EnemyToPlayer(void)
{
	MATRIX angles = AngleUtility::GetMatrixRotateXYZ(VSub(player_->GetPos(), enemy_->GetPos()));
	enemy_->SetAngle(VTransform(enemy_->GetAngle(), angles));
}
