#include "ColliderModel.h"
#include "../Common/Transform.h"
#include "../../Utility/AsoUtility.h"


ColliderModel::ColliderModel(const Transform* follow) : ColliderBase(SHAPE::MODEL, follow)
{
}

ColliderModel::~ColliderModel(void)
{
}
