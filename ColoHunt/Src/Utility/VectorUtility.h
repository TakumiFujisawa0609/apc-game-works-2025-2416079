#pragma once
#include <DxLib.h>


class VectorUtility
{
public:

	//ベクトルの大きさ
	static double Magnitude(const VECTOR& v);
	static float MagnitudeF(const VECTOR& v);

	//ベクトルの大きさの二乗
	static float SqrMagnitudeF(const VECTOR& v);
	static double SqrMagnitude(const VECTOR& v);

	//ベクトル同士の距離
	static double Distance(const VECTOR& v1, const VECTOR& v2);
	static float DistanceF(const VECTOR& v1, const VECTOR& v2);
	//ベクトル同士の距離の二乗
	static double SqrDistance(const VECTOR& v1, const VECTOR& v2);
	static float SqrDistanceF(const VECTOR& v1, const VECTOR& v2);

	// 比較
	static bool Equals(const VECTOR& v1, const VECTOR& v2);
	static bool EqualsVZero(const VECTOR& v1);
	static bool Comparison(const VECTOR& v1, const VECTOR& v2);

	// ベクトル同士の足し算
	static VECTOR Add(const VECTOR& v1, const VECTOR& v2);
	// ベクトル同士の引き算
	static VECTOR Sub(const VECTOR& v1, const VECTOR& v2);
};