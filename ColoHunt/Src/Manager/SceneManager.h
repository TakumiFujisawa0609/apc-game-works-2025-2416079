#pragma once

#include<memory>
#include<list>

class SceneBase;

class SceneManager
{
public:
	// シーン管理用
	enum class SCENE_ID
	{
		NONE = -1,

		TITLE,
		GAME,
		CLEAR,
		OVER,
		PAUSE,

		MAX
	};

public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new SceneManager(); } };
	static SceneManager& GetInstance(void) { return *instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// デストラクタも同様
	~SceneManager(void);

	// コピー・ムーブ操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:

	void Init(void);	// 初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Release(void);	// 解放

	// 状態遷移
	void ChangeScene(std::shared_ptr<SceneBase>scene);
	void ChangeScene(SCENE_ID scene);

	// シーンを新しく積む
	void PushScene(std::shared_ptr<SceneBase>scene);
	void PushScene(SCENE_ID scene);

	// 最後に追加したシーンを削除する。
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする。
	void JumpScene(std::shared_ptr<SceneBase>scene);
	void JumpScene(SCENE_ID scene);

	// シーンIDの取得
	SCENE_ID GetSceneID(void) const { return sceneId_; };

	const float GetDeltaTime(void)const { return (1.0f / 60); }

	// スコアの一時保存
	void SetScore(int dam, int item) { damageNum_ = dam; itemNum_ = item; }

	// スコアの取得
	int GetDamage(void) { return damageNum_; }
	int GetItem(void) { return itemNum_; }

private:
	// 静的インスタンス
	static SceneManager* instance_;

	//Drawの関係上Backを最新のシーンとする
	//基本的には要素は一つだけだがポーズシーンなどが積み重なる形
	std::list<std::shared_ptr<SceneBase>>scenes_;

	// シーンID
	SCENE_ID sceneId_;

	// スコアの一時保存用
	int damageNum_;
	int itemNum_;
};

using SCENE_ID = SceneManager::SCENE_ID;