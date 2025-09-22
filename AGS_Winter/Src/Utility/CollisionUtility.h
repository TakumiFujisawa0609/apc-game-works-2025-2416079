#pragma once
#include <DxLib.h>


class CollisionUtility
{
public:

	// ‹…‘Ì“¯m‚ÌÕ“Ë”»’è
	static bool IsHitSpheres(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2);

	// ‹…‘Ì‚ÆƒJƒvƒZƒ‹‚ÌÕ“Ë”»’è
	static bool IsHitSphereCapsule(const VECTOR& sphPos, float sphRadius, const VECTOR& capPos1, const VECTOR& capPos2, float capRadius);
};