#include <DxLib.h>
#include "../Common/Transform.h"
#include "ColliderSphere.h"


ColliderSphere::ColliderSphere(const Transform* follow, const VECTOR& localPos, float radius)	:
	ColliderBase(SHAPE::SPHERE, follow),
	localPos_(localPos),
	radius_(radius)
{
}

ColliderSphere::~ColliderSphere(void)
{
}

const VECTOR& ColliderSphere::GetLocalPos(void) const
{
	return localPos_;
}

void ColliderSphere::SetLocalPos(const VECTOR& pos)
{
	localPos_ = pos;
}

VECTOR ColliderSphere::GetPos(void) const
{
	return GetRotPos(localPos_);
}

float ColliderSphere::GetRadius(void) const
{
	return radius_;
}

void ColliderSphere::SetRadius(float radius)
{
	radius_ = radius;
}

VECTOR ColliderSphere::GetCenter(void) const
{
	return VAdd(follow_->pos, localPos_);
}

void ColliderSphere::DrawDebug(int color)
{
	// カプセルの中心
	DrawSphere3D(GetCenter(), radius_, 10, color, color, true);
}
