#include "mcv_platform.h"
#include "module_tower.h"
#include "components/juan/comp_render.h"
#include "render/render_objects.h"
#include "components/comp_light_dir.h"
#include "components/comp_light_point.h"


bool CModuleTower::start()
{
	return true;
}

void CModuleTower::update(float delta)
{
}

void CModuleTower::render()
{
}

float CModuleTower::getTowerRadius() {
	return tower_radius;
}

const VEC3 CModuleTower::getLastCheckpoint()
{
	return last_checkpoint_position;
}

const void CModuleTower::setLastCheckpoint(VEC3 checkpoint) {
	last_checkpoint_position = checkpoint;
}

const float CModuleTower::getLastCheckpointYaw()
{
	return last_checkpoint_yaw;
}

const void CModuleTower::setLastCheckpointYaw(float checkpoint_yaw) {
	last_checkpoint_yaw = checkpoint_yaw;
}

const bool CModuleTower::getLastCheckpointLeft()
{
	return last_checkpoint_looking_left;
}

const void CModuleTower::setLastCheckpointLeft(bool checkpoint_left) {
	last_checkpoint_looking_left = checkpoint_left;	
}

const void CModuleTower::disappearEntity(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	TCompRender* h_render = entity->get< TCompRender >();
	h_render->is_active = false;
	h_render->refreshMeshesInRenderManager();
}

const void CModuleTower::appearEntity(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	TCompRender* h_render = entity->get< TCompRender >();
	h_render->is_active = true;
	h_render->refreshMeshesInRenderManager();
}

const void CModuleTower::setAmbientAdjustment(float ambient) {
	cb_globals.global_ambient_adjustment = ambient;
}

const void CModuleTower::setDirLightIntensity(const std::string& name, float intensity) {
  CEntity* entity = (CEntity*)getEntityByName(name);
  TCompLightDir* light_dir = entity->get<TCompLightDir>();
  light_dir->setIntensity(intensity);
}

const void CModuleTower::setPointLightIntensity(const std::string& name, float intensity) {
  CEntity* entity = (CEntity*)getEntityByName(name);
  TCompLightPoint* light_point = entity->get<TCompLightPoint>();
  light_point->setIntensity(intensity);
}