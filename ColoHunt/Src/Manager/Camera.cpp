#include <EffekseerForDxLib.h>
#include "Camera.h"


Camera* Camera::inst_ = nullptr;

Camera::Camera(void) : pos_(), angles_()
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
}

Camera::~Camera()
{
}

void Camera::Init(void) 
{
	pos_ = DEFAULT_POS;
	angles_ = DEFAULT_ANGLES;
}

void Camera::Update(void)
{
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);
	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle( pos_, angles_.x, angles_.y, angles_.z );

	Effekseer_Sync3DSetting();
}

void Camera::Draw(void)
{
	//デバック表示
	DrawFormatString(10, 10, GetColor(255, 255, 255), "Camera Position: (%.2f, %.2f, %.2f)", pos_.x, pos_.y, pos_.z);
	
	VECTOR angleA = VScale(angles_, 180.0f);
	float angleX = angleA.x / DX_PI_F;
	float angleY = angleA.y / DX_PI_F;
	float angleZ = angleA.z / DX_PI_F;

	while (angleX >= 360.0f) {

		angleX -= 360.0f;
	}
	while (angleY >= 360.0f) {

		angleY -= 360.0f;
	}
	while (angleZ >= 360.0f) {

		angleZ -= 360.0f;
	}
	
	while (angleX < 0.0f) {

		angleX += 360.0f;
	}
	while (angleY < 0.0f) {

		angleY += 360.0f;
	}
	while (angleZ < 0.0f) {

		angleZ += 360.0f;
	}

	DrawFormatString(10, 30, GetColor(255, 255, 255), "Camera Angles: (%.2f, %.2f, %.2f)", angleX, angleY, angleZ);
}

void Camera::Release(void)
{
}

void Camera::SetLookAt(VECTOR target)
{
	// 注視方向ベクトルを角度に変換
	angles_.x = atan2f(target.y, sqrtf(target.x * target.x + target.z * target.z));
	angles_.y = atan2f(target.x, target.z);
	angles_.z = 0.0f;
}