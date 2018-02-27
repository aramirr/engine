#include "mcv_platform.h"
#include "components/comp_transform.h"
#include "ai_controller.h"

void IAIController::change_color(VEC4 color) {
	TCompRender *my_render = getMyRender();
	my_render->color = color;
}

TCompTransform* IAIController::getMyTransform() {
  TCompTransform* c = h_transform;
  if (!c) {
    CEntity* e = h_entity;
    assert(e);
    h_transform = e->get< TCompTransform >();
    assert(h_transform.isValid());
    c = h_transform;
  }
  return c;
}

TCompRender* IAIController::getMyRender() {
	TCompRender* r = h_render;
	if (!r) {
		CEntity* e = h_entity;
		assert(e);
		h_render = e->get< TCompRender >();
		assert(h_render.isValid());
		r = h_render;
	}
	return r;
}

// Show common information for the AIControllers
void IAIController::debugInMenu() {
  ImGui::Text("State: %s", state.c_str());
  if (ImGui::TreeNode("States")) {
    for (auto it = statemap.begin(); it != statemap.end(); ++it)
      ImGui::Text("%s", it->first.c_str());
    ImGui::TreePop();
  }
}

void IAIController::update(float dt)
{
  assert(!state.empty());
  assert(statemap.find(state) != statemap.end());
  // this is a trusted jump as we've tested for coherence in ChangeState
  (this->*statemap[state])();
}

void IAIController::ChangeState(const std::string& newstate)
{
  // try to find a state with the suitable name
  if (statemap.find(newstate) == statemap.end())
  {
    // the state we wish to jump to does not exist. we abort
    fatal("Invalid ChangeState(%s)\n", newstate.c_str());
  }
  state = newstate;
}

void IAIController::AddState(const std::string& name, statehandler sh)
{
  // try to find a state with the suitable name
  if (statemap.find(name) != statemap.end())
  {
    // the state we wish to jump to does exist. we abort
    fatal("Invalid AddState(%s). Already defined.\n", name.c_str());
  }
  statemap[name] = sh;
}

void IAIController::setEntity(CHandle new_entity) {
  h_entity = new_entity;
  assert(h_entity.isValid());
}

void IAIController::change_mesh(int mesh_index) {
	TCompRender *my_render = getMyRender();
	//my_render->mesh = my_render->meshes_leo[mesh_index];
	for (int i = 0; i < my_render->meshes.size(); ++i) {
			my_render->meshes[i].enabled = false;		
	}
	my_render->meshes[mesh_index].enabled = true;
	my_render->refreshMeshesInRenderManager();
}