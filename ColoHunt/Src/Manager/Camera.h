#pragma once
#include <DxLib.h>


class Camera
{
public:

	// コンストラクタ
	Camera(void);
	// デストラクタ
	~Camera(void);

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (inst_ == nullptr) { inst_ = new Camera(); } }
	static Camera* GetInstance(void) { return inst_; }
	static void DeleteInstance(void) { if (inst_ != nullptr) { delete inst_; inst_ = nullptr; } }

	// 初期化
	void Init(void);
	// 更新
	void Update(void);
	// 描画前のカメラ設定
	void SetBeforeDraw(void);
	// デバッグ用描画
	void Draw(void);
	// 解放
	void Release(void);

	//カメラの位置の移動
	void SetCameraPos(VECTOR pos) { pos_ = VAdd(pos_, pos); }
	void SetAbsCameraPos(VECTOR pos) { pos_ = pos; }
	
	//カメラの注視点の設定
	void SetLookAt(VECTOR target);
	
	//カメラの角度の移動
	void SetCameraAngles(VECTOR angle) { angles_ = VAdd(angles_, angle); }
	void SetAbsCameraAngles(VECTOR angle) { angles_ = angle; }
	
	// カメラの位置と角度の取得
	VECTOR GetCameraPos(void) { return pos_; }
	VECTOR GetCameraAngles(void) { return angles_; }

	// コピー・ムーブ操作を禁止
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(Camera&&) = delete;


private:

	// カメラの初期位置
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 500.0f, -1000.0f };
	// カメラの初期角度
	static constexpr VECTOR DEFAULT_ANGLES = { DX_PI_F / 9.0f, 0.0f, 0.0f };

	// カメラクリップ：NEAR
	static constexpr float CAMERA_NEAR = 3.0f;
	// カメラクリップ：FAR
	static constexpr float CAMERA_FAR = 15000.0f;

	// インスタンス
	static Camera* inst_;

	// カメラの位置
	VECTOR pos_;
	// カメラの角度
	VECTOR angles_;
};