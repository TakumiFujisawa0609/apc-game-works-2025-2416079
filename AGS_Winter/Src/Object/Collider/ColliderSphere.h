#pragma once
#include <DxLib.h>
#include "ColliderBase.h"


class Transform;

class ColliderSphere : public ColliderBase
{
public:

	// コンストラクタ
	ColliderSphere(const Transform* follow, const VECTOR& localPos, float radius);
	// デストラクタ
	~ColliderSphere(void);

	// 親Transformからの相対位置を取得
	const VECTOR& GetLocalPos(void) const;

	// 親Transformからの相対位置をセット
	void SetLocalPos(const VECTOR& pos);

	// ワールド座標を取得
	VECTOR GetPos(void) const;

	// 半径
	float GetRadius(void) const;
	void SetRadius(float radius);

	// 球の中心座標
	VECTOR GetCenter(void) const;

protected:

	// デバッグ用描画
	void DrawDebug(int color) override;

private:

	// オフセット
	VECTOR localPos_;

	// 半径
	float radius_;
};