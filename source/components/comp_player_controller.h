#pragma once

#include "comp_base.h"
#include "geometry/transform.h"
#include "ia/ai_controller.h"

class TCompPlayerController : public IAIController {
  float   speedFactor = 8.0f;
  VEC3    speed;
  VEC3	  center;
  float	  radius;
	float   dashingMax;
	float   dashingAmount;
	float   max_jump;
	int     dashingSpeed;
	bool    lookingLeft;

  DECL_SIBLING_ACCESS();

public:
  void debugInMenu();
  void load(const json& j, TEntityParseContext& ctx);

  // IA
  void IdleState(float dt);
  void RunningState(float dt);
  void JumpingState(float dt);
	void OmniDashingState(float dt);
	void DashingState(float dt);
	void DeadState(float dt);
	void MovePlayer(bool left, float dt);

  void Init();
};