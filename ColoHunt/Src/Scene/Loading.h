#pragma once

class Loading
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new Loading(); } };
	static Loading* GetInstance(void) { return instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Loading();
	~Loading();

	// コピー・ムーブ操作を禁止
	Loading(const Loading&) = delete;
	Loading& operator=(const Loading&) = delete;
	Loading(Loading&&) = delete;
	Loading& operator=(Loading&&) = delete;

public:

	void Init(void);		// 初期化
	void InitLoad(void);		// 読み込み
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

	void StartAsyncLoad(void);	// 非同期ロードの開始
	void EndAsyncLoad(void);	// 非同期ロードの終了

	// ロード中かを返す。
	bool IsLoading(void) { return isLoading_; }

private:

	// 静的インスタンス
	static Loading* instance_;

	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 60;	// 60fps(1秒) * x

	// 画像ハンドル
	int handle_;

	// 座標
	int posX_;	// X座標
	int posY_;	// Y座標

	// ロード中の判定用
	bool isLoading_;

	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_;
};
