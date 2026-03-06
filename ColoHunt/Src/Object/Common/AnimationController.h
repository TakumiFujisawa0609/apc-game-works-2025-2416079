#pragma once
#include <string>
#include <map>


class AnimationController
{

public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

	// コンストラクタ
	AnimationController(int modelId);
	// デストラクタ
	~AnimationController(void);

	// 外部FBXからアニメーション追加
	void Add(int type, float speed, const std::string path);
	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);

	// アニメーション再生
	void Play(int type, bool isLoop);
	void End(int type);
	void Update(void);
	void Release(void);

	//現在のアニメーションタイプ
	int GetPlayType(void) const { return playType_; }

	//終了しているかどうか
	bool IsEnd(void) const;
	//ループするかどうか
	bool IsLoop(void) const { return isLoop_; }

	//アニメーション全体の時間
	float GetTotalTime(void) { return playAnim_.totalTime; }
	//アニメーション現在の時間
	float GetTime(void) { return playAnim_.step; }

	//ポーズ状態を変更する
	void ChangePause(bool p) { isPause_ = p; }
	//ポーズしているかどうか
	bool IsPause(void) { return isPause_; }

	//速度を変える
	void ChangeSpeed(int type, float speed);

private:

	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// 再生中のアニメーション
	int playType_;
	Animation playAnim_;
	Animation prevAnim_;

	//アニメーションブレンドの割合
	float blendRate_;

	//デタッチできているか
	int isDetach_;
	//アニメーションループさせるか
	bool isLoop_;
	//一時停止しているか
	bool isPause_;

	// アニメーション追加の共通処理
	void Add(int type, float speed, Animation& animation);
};