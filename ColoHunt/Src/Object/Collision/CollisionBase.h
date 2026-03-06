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

	//モデルと線(当たった最初の場所のみ)
	const MV1_COLL_RESULT_POLY CollisionLine(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> line);
	//モデルと線(当たったすべての場所)
	const MV1_COLL_RESULT_POLY_DIM CollisionPolyLine(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> line);
	//モデルとカプセル(コライダーのあるもの{相対位置})
	const MV1_COLL_RESULT_POLY_DIM CollisionCapsule(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> capsule);
	//モデルとカプセル(コライダーのないもの{絶対位置})
	const MV1_COLL_RESULT_POLY_DIM CollisionCapsule(const std::map<int, ColliderBase*> model, const VECTOR start, const VECTOR end, const float radius);
	//モデルと球
	const MV1_COLL_RESULT_POLY_DIM CollisionSphere(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> sphere);

	//押し戻し
	const VECTOR GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, std::map<int, ColliderBase*> capsule, int maxTryCnt, float pushDistance) const;

	//あるコライダーにキャストしたい
	template<class T, class U, class V>
	T* SearchType(U coll, const V type) const {

		int i = static_cast<int>(type);

		if (coll.count(i) != 0) {

			return dynamic_cast<T*>(coll.at(i));
		}
		return nullptr;
	}
};