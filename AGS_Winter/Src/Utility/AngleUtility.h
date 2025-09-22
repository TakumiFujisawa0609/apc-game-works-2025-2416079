#pragma once
#include <DxLib.h>


class AngleUtility
{
public:

	// ラジアン(rad)から度(deg)
	static double Rad2DegD(double rad);
	static float Rad2DegF(float rad);
	static int Rad2DegI(int rad);

	// 度(deg)からラジアン(rad)
	static double Deg2RadD(double deg);
	static float Deg2RadF(float deg);
	static int Deg2RadI(int deg);

	// オイラー角から回転行列XYZ順を取得
	static MATRIX GetMatrixRotateXYZ(const VECTOR& euler);

	//親子の角度を合成
	static MATRIX Multiplication(const VECTOR& child, const VECTOR& parent);
	static MATRIX Multiplication(const MATRIX& child, const MATRIX& parent);
};