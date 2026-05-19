#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "AnimationController.h"


AnimationController::AnimationController(int modelId)
{
	modelId_ = modelId;
	playType_ = -1;
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(int type, float speed, const std::string path)
{
	Animation animation;
	animation.model = MV1LoadModel(path.c_str());
	animation.animIndex = 0;

	Add(type, speed, animation);
}

void AnimationController::AddInFbx(int type, float speed, int animIndex)
{
	Animation animation;
	animation.model = -1;
	animation.animIndex = animIndex;

	Add(type, speed, animation);
}

void AnimationController::Play(int type, bool isLoop)
{
	if (playType_ == type)
	{
		//続行
		return;
	}
	if (playType_ != -1) {
		if (isDetach_ == -1) {

			MV1DetachAnim(modelId_, prevAnim_.attachNo);
		}
		MV1DetachAnim(modelId_, playAnim_.attachNo);
	}

	// アニメーション種別を変更
	prevAnim_ = playAnim_;
	playType_ = type;
	playAnim_ = animations_[type];

	if (prevAnim_.speed != 0) {

		prevAnim_.attachNo = MV1AttachAnim(modelId_, prevAnim_.animIndex, prevAnim_.model);
		MV1SetAttachAnimTime(modelId_, prevAnim_.attachNo, prevAnim_.step);
	}

	//初期化
	playAnim_.step = 0.0f;

	// モデルにアニメーションを付ける
	if (playAnim_.model == -1)
	{
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex);
		//総持続時間の取得
		playAnim_.totalTime = MV1GetAnimTotalTime(modelId_, playAnim_.animIndex);
	}
	else
	{
		// 別のモデルファイルからアニメーションをアタッチする
		// DxModelViewerを確認すること(大体0か1)
		playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex, playAnim_.model);
		//総持続時間の取得
		playAnim_.totalTime = MV1GetAnimTotalTime(playAnim_.model, playAnim_.animIndex);
	}
	isLoop_ = isLoop;
	blendRate_ = 0.1f;
	isDetach_ = -1;

	if (prevAnim_.speed != 0) {

		MV1SetAttachAnimBlendRate(modelId_, prevAnim_.attachNo, 1.0f - blendRate_);
		MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, blendRate_);
	}
}

void AnimationController::End(int type)
{
}

void AnimationController::Update(void)
{
	if (isPause_) {
		
		return;
	}
	if (prevAnim_.speed != 0) {
		while (blendRate_ <= 1.0f) {
			if (blendRate_ >= 0.9f) {

				MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, 1.0f);
				isDetach_ = MV1DetachAnim(modelId_, prevAnim_.attachNo);

				break;
			}
			MV1SetAttachAnimBlendRate(modelId_, prevAnim_.attachNo, 1.0f - blendRate_);
			MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, blendRate_);

			break;
		}
		blendRate_ += 0.1f;
	}

	if (isLoop_){
		if (playAnim_.step > playAnim_.totalTime){

			playAnim_.step = 0.0f;
		}
	}

	//プレイヤーの移動するインデックス
	int hipIndex = MV1SearchFrame(modelId_, "mixamorig:Hips");
	//インデックスの行列をリセット
	MV1ResetFrameUserLocalMatrix(modelId_, hipIndex);

	// 経過時間の取得
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	// 再生
	playAnim_.step += (deltaTime * playAnim_.speed);

	//移動するインデックスのZ座標を0に固定
	MATRIX defo = MV1GetFrameLocalMatrix(modelId_, hipIndex);
	defo.m[3][2] = 0.0f;
	MV1SetFrameUserLocalMatrix(modelId_, hipIndex, defo);

	// アニメーション設定
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);
}


bool AnimationController::IsEnd(void) const
{
	if (isLoop_) {

		return false;
	}
	else if (playAnim_.step > playAnim_.totalTime) {

		return true;
	}
	return false;
}

void AnimationController::ChangeSpeed(int type, float speed)
{
	if (animations_.count(type) == 0) return;
	
	Animation& anim = animations_.at(type);
	anim.speed = speed;
	animations_.emplace(type, speed);
}

void AnimationController::Release(void)
{

	for (const std::pair<int, Animation>& pair : animations_) {
		if (pair.second.model != -1) {

			MV1DeleteModel(pair.second.model);
		}
	}

	animations_.clear();
}

void AnimationController::Add(int type, float speed, Animation& animation)
{
	animation.speed = speed;

	if (animations_.count(type) == 0) {

		animations_.emplace(type, animation);
	}
}