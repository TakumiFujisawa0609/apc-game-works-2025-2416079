#pragma once
#include <DxLib.h>


class AnimationController;

class ActorBase {
public:

	ActorBase();
	~ActorBase();

	//全初期化
	virtual void InitAll();
	//ロード以外
	virtual void Init();
	//ロード
	virtual void InitLoad() = 0;
	//アニメーションの初期化
	virtual void InitAnim() = 0;
	//各自の初期化
	virtual void InitOwn() = 0;
	//モデルの初期化
	virtual void InitModel() const;

	//更新
	virtual void Update() = 0;

	//モデルの描画
	virtual void DrawModel() const;
	//他の描画
	virtual void Draw() const = 0;

	//解放
	virtual void Release() const;

	//モデルの取得
	int GetModelId(void) const { return modelId_; }

	// 座標取得
	VECTOR GetPos(void) const { return pos_; }
	VECTOR GetPrevPos(void) const { return prevPos_; }
	void SetPos(VECTOR pos) { pos_ = pos; }

protected:

	//アニメーションコントローラー
	AnimationController* animationCtrl_;

	//モデルのID
	int modelId_;

	//現在の座標
	VECTOR pos_;
	//以前の座標
	VECTOR prevPos_;

	// 大きさ
	VECTOR scales_;

	//角度
	VECTOR angles_;
	VECTOR localAngles_;

	//移動方向
	VECTOR moveDir_;

	//体力
	int hp_;
};