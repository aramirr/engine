#pragma once

#include "components/comp_base.h"
#include "geometry/transform.h"
#include "entity/common_msgs.h"

class TCompSpiralController : public TCompBase {

  float       radius;
  CHandle     h_entity;
  float       speed;
  VEC3		  center;
  VEC3		  direction;
  float		  offsetY;

  DECL_SIBLING_ACCESS();

public:
  void debugInMenu();
  void load(const json& j, TEntityParseContext& ctx);
  void update(float dt);
  void setEntity(CHandle new_entity);
};