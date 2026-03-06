#pragma once
#include <DxLib.h>
#include <chrono>
#include "Audio/SoundTable.h"


class SceneBase;
class Fader;
class Grid;
class Pause;

class SceneManager
{
public:

	static constexpr float GRAVITY = 9.81f;

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		GAME,
		OVER,
		CLEAR,
	};
	
	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void) const;

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	//親クラスの取得
	SceneBase* GetSceneBase(void) { return scene_; }

	//現在の画面の保存/呼び出し
	void SetScreenImage(void) const;
	int GetScreenImage(void) const { return screenImg_; }

	//時間の取得
	int GetTime(void) { return time_ / 60; }
	int GetTimer(void) { return timer_ / 60; }
	//クリアタイムの一時保存
	void SetTime(void) { time_ = timer_; }

	//被弾回数・アイテム使用回数の取得
	void SetScore(int damage, int item) { damageNum_ = damage; itemNum_ = item; }
	int GetDamageNum(void) { return damageNum_; }
	int GetItemNum(void) { return itemNum_; }
	
	//時間の呼び出し
	std::vector<int> LoadTime(void);

private:

	//背景の大きさ
	static constexpr VECTOR BACKGROUND_SCR = { 7.0f, 7.0f, 7.0f };

	//現在の画面を保存するハンドル
	int screenImg_;

	//時間
	int timer_;
	//クリア時間
	int time_;

	//被弾回数
	int damageNum_;
	//アイテム使用回数
	int itemNum_;

	// 静的インスタンス
	static SceneManager* instance_;

	//現在のシーンID
	SCENE_ID sceneId_;
	//更新待ちのシーンID
	SCENE_ID waitSceneId_;

	//現在止めてる音のID
	SoundID pauseId_;

	// グリッド
	Grid* grid_;

	// フェード
	Fader* fader_;

	// 各種シーン
	SceneBase* scene_;

	Pause* pause_;

	int backGround_;

	// シーン遷移中判定
	bool isSceneChanging_;
	bool isLoad_;

	int loadImg_;
	int loadCnt_;
	
	bool showFlg_[3];
	int font_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);

	// コピーコンストラクタも同様
	SceneManager(const SceneManager& instance) = default;

	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

	void Init3D(void);

	void SaveTime(void);
};