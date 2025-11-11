#pragma once
#include <DxLib.h>


class Camera
{
public:


	// カメラの初期位置
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 500.0f, -1000.0f };
	// カメラの初期角度
	static constexpr VECTOR DEFAULT_ANGLES = { DX_PI_F / 9.0f, 0.0f, 0.0f };

	// カメラクリップ：NEAR
	static constexpr float CAMERA_NEAR = 3.0f;
	// カメラクリップ：FAR
	static constexpr float CAMERA_FAR = 15000.0f;

	// コンストラクタ
	Camera(void);
	// デストラクタ
	~Camera(void);
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

	void SetCameraPos(VECTOR pos) { pos_ = VAdd(pos_, pos); }
	void SetAbsCameraPos(VECTOR pos) { pos_ = pos; }
	void SetLookAt(VECTOR target);
	void SetCameraAngles(VECTOR angle) { angles_ = VAdd(angles_, angle); }
	void SetAbsCameraAngles(VECTOR angle) { angles_ = angle; }
	VECTOR GetCameraPos(void) { return pos_; }
	VECTOR GetCameraAngles(void) { return angles_; }

private:

	// カメラの位置
	VECTOR pos_;
	// カメラの角度
	VECTOR angles_;
};