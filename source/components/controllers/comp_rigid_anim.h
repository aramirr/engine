#ifndef INC_COMP_RIGID_ANIM_H_
#define INC_COMP_RIGID_ANIM_H_

#include "geometry/transform.h"
#include "geometry/rigid_anim.h"

class CRigidAnimResource;

struct TCompRigidAnim : public TCompBase {
  RigidAnims::CController controller;
  float                   current_time = 0.f;
  bool                    loops = true;
  bool                    is_moving = false;
  float                   speed_factor = 1.0f;
  void load(const json& j, TEntityParseContext& ctx);
  void update(float dt);
  void debugInMenu();
  static void registerMsgs();
  void onGroupCreated(const TMsgEntitiesGroupCreated& msg);
  DECL_SIBLING_ACCESS();
};

#endif
