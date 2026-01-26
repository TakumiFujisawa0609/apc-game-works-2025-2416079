#pragma once
#include <DxLib.h>
#include <map>


class Transform;
class ColliderBase;
class ColliderCapsule;

class CollisionBase {

public:

	CollisionBase();
	~CollisionBase();

protected:

	const MV1_COLL_RESULT_POLY CollisionLine(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> line);
	const MV1_COLL_RESULT_POLY_DIM CollisionPolyLine(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> line);
	const MV1_COLL_RESULT_POLY_DIM CollisionCapsule(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> capsule);
	const MV1_COLL_RESULT_POLY_DIM CollisionSphere(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> sphere);

	const VECTOR GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, std::map<int, ColliderBase*> capsule, int maxTryCnt, float pushDistance) const;

	template<class T, class U, class V>
	T* SearchType(U coll, const V type) const {

		int i = static_cast<int>(type);

		if (coll.count(i) != 0) {

			return dynamic_cast<T*>(coll.at(i));
		}
		return nullptr;
	}
};