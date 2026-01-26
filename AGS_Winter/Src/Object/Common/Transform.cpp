#include <DxLib.h>
#include "../../Utility/AngleUtility.h"
#include "../../Utility/Utility.h"
#include "Transform.h"

Transform::Transform(void)
	:
	modelId(-1),
	scl(Utility::VECTOR_ONE),
	rot(Utility::VECTOR_ZERO),
	pos(Utility::VECTOR_ZERO),
	prevPos(Utility::VECTOR_ZERO),
	matScl(MGetIdent()),
	matRot(MGetIdent()),
	matPos(MGetIdent())
{
}

Transform::~Transform(void)
{
}

void Transform::Update(void)
{
	// 回転
	matRot = AngleUtility::Multiplication(localRot, rot);

	// 大きさ
	matScl = MGetScale(scl);

	// 位置
	matPos = MGetTranslate(pos);

	// 行列の合成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matRot);
	mat = MMult(mat, matScl);
	mat = MMult(mat, matPos);

	// 行列をモデルに判定
	if (modelId != -1)
	{
		MV1SetMatrix(modelId, mat);
	}
}

void Transform::Release(void)
{
}

void Transform::SetModel(int model)
{
	modelId = model;
}

VECTOR Transform::GetForward(void) const
{
	return GetDir(Utility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(Utility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(Utility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(Utility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(Utility::DIR_U);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(Utility::DIR_D);
}

VECTOR Transform::GetDir(const VECTOR& dir) const
{
	return VTransform(dir, matRot);
}
