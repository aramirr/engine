#include "mcv_platform.h"
#include "module_tower.h"
#include "components/juan/comp_render.h"
#include "render/render_objects.h"
#include "components/lights/comp_light_dir.h"
#include "components/lights/comp_light_point_shadows.h"
#include "components/controllers/comp_door.h"
#include "components/player/comp_player_controller.h"
#include "components/ia/bt_runner.h"
#include "components/sound/comp_sound.h"
#include "components/camera/comp_camera_manager.h"

bool CModuleTower::start()
{
	changeExposure = false;
	return true;
}

void CModuleTower::update(float delta)
{
	if (changeExposure) {
		if (newExposure > oldExposure) {
			cb_globals.global_exposure_adjustment += 0.02f;
			oldExposure = cb_globals.global_exposure_adjustment;
			if (oldExposure > newExposure) {
				if (oldExposure > defaultExposure)newExposure = defaultExposure;
				else changeExposure = false;
			}
		}
		else if (newExposure < oldExposure) {
			cb_globals.global_exposure_adjustment -= 0.02f;
			oldExposure = cb_globals.global_exposure_adjustment;
			if (oldExposure < newExposure) {
				changeExposure = false;
			}
		}
	}
	if (changeLight) {
		if (newLight > oldLight) {
			cb_globals.global_light_adjustment += 0.01f;
			oldLight = cb_globals.global_light_adjustment;
			if (oldLight > newLight) {
				changeLight = false;
			}
		}
		else if (newLight < oldLight) {
			cb_globals.global_light_adjustment -= 0.01f;
			oldLight = cb_globals.global_light_adjustment;
			if (oldLight < newLight) {
				changeLight = false;
			}
		}
	}
	if (changeVignetting) {
		if (newVignetting > oldVignetting) {
			cb_globals.global_vignetting_adjustment += 0.01f;
			oldVignetting = cb_globals.global_vignetting_adjustment;
			if (oldVignetting > newVignetting) {
				changeVignetting = false;
			}
		}
		else if (newVignetting < oldVignetting) {
			cb_globals.global_vignetting_adjustment -= 0.005f;
			oldVignetting = cb_globals.global_vignetting_adjustment;
			if (oldVignetting < newVignetting) {
				changeVignetting = false;
			}
		}
	}
	if (changeFadeOut) {
		if (newFadeOut > oldFadeOut) {
			cb_globals.global_fadeOut_adjustment += 0.05f;
			oldFadeOut = cb_globals.global_fadeOut_adjustment;
			if (oldFadeOut > newFadeOut) {
				if (oldFadeOut >= 10.f)EngineUI.activateWidget("Pantalla_negra");
				changeFadeOut = false;
			}
		}
		else if (newFadeOut < oldFadeOut) {
			if (oldFadeOut >= 10.f && newFadeOut < 10.f)EngineUI.desactivateWidget("Pantalla_negra");
			cb_globals.global_fadeOut_adjustment -= 0.05f;
			oldFadeOut = cb_globals.global_fadeOut_adjustment;
			if (oldFadeOut < newFadeOut) {
				changeFadeOut = false;
			}
		}
	}
	if (cargar) {
		newExposure = 0.f;
		oldExposure = cb_globals.global_exposure_adjustment;
		defaultExposure = cb_globals.global_exposure_adjustment;
		cargar = false;
	}
	if (time_out) {
		current_time += delta;
		if (current_time > total_wait_time) {
			if (cb_gui.creditos) {
                EngineSound.deleteSounds();
				EngineUI.activateWidget("pantallaCreditos");
                EngineSound.emitEvent("musica_credits");
			}
			else {
				CEntity* player = getEntityByName("The Player");
				TMsgSetFSMVariable pauseMsg;
				pauseMsg.variant.setName("pause");
				pauseMsg.variant.setBool(false);
				player->sendMsg(pauseMsg);

				pauseMsg.variant.setName("idle");
				pauseMsg.variant.setBool(true);
				player->sendMsg(pauseMsg);

				setBandsCinematics(false);
				cb_gui.cinematica = false;
			}
			time_out = false;


			if (current_cinematic != "") {
				deactivateCinematic(current_cinematic);
			}
		}
	}
	if (bandCinematics && bandsValue < 0.15f) {
		bandsValue += 0.01f;
		if (bandsValue > 0.15f)bandsValue = 0.15f;
		cb_globals.global_bandMax_adjustment = bandsValue - 0.02f;
		cb_globals.global_bandMin_adjustment = bandsValue;
	}
	else if (!bandCinematics && bandsValue > 0.f) {
		if (bandsValue >= 0.15f) EngineUI.desactivateWidget("barras_cinematicas");
		bandsValue -= 0.01f;
		cb_globals.global_bandMax_adjustment = bandsValue;
		cb_globals.global_bandMin_adjustment = bandsValue;
	}
	else if (bandCinematics) {
		EngineUI.activateWidget("barras_cinematicas");
	}

	// Activate Runner
	if (activate_runner) {
		timer_runner += delta;
		if (timer_runner >= 0.0f && !start_anim) {
			start_anim = true;
			CEntity* e = getEntityByName("The Player");
			TCompPlayerController * controller = e->get<TCompPlayerController>();
			controller->remove_animation(controller->EAnimations::NajaJumpLoop);
			controller->change_animation(34, true, 0, 0.8, true);
			EngineSound.emitDelayedEvent(0, "naja_monolito");
			EngineTower.activateCinematic("cinematica_monolito");

		}

		else if (timer_runner >= 5.f && !destroy_monolito) {
			destroy_monolito = true;

			CEntity* entity = (CEntity*)getEntityByName("Monolito_001");
			TMsgActivateAnim msg;
			msg.name = "Monolito_001";
			msg.wait_time = 0.f;
			entity->sendMsg(msg);

			EngineSound.emitDelayedEvent(0, "monolito_destruccion");
		}

		else if (timer_runner >= 10.667 && !build_runner) {
			cb_globals.global_runner_interior = 1;
			CEntity* e = getEntityByName("The Player");
			TCompPlayerController* player = e->get<TCompPlayerController>();
			build_runner = true;

			CEntity* entity = (CEntity*)getEntityByName("Runner_father");
			TMsgActivateAnim msg;
			msg.name = "Runner_father";
			msg.wait_time = 0.f;
			entity->sendMsg(msg);
		}
		else if (timer_runner >= 26.667 && !changed_runner_mesh) {
			changed_runner_mesh = true;

			// Kill Runner_father
			CEntity* runner_father = getEntityByName("Runner_father");
			if (runner_father) {
				CHandle(runner_father).destroy();
			}

			//Appear Runner
			appearEntity("Runner");

			CEntity* e = getEntityByName("Runner");
			TCompCollider* e_collider = e->get<TCompCollider>();
			TCompTransform* e_transform = e->get<TCompTransform>();
			e_transform->setPosition(VEC3(2.31185f, 88.f, -31.2941f)); //86.5861f
			e_collider->controller->setPosition(physx::PxExtendedVec3(2.31185f, 88.f, -31.2941f));
			bt_runner * controller = e->get<bt_runner>();
			controller->change_animation(5, true, 0.0, 0.0, true);

		}
		else if (timer_runner >= 27.33f && !runner_scream) {
			runner_scream = true;
			CEntity* e = getEntityByName("Runner");
			bt_runner * controller = e->get<bt_runner>();
			controller->change_animation(4, true, 0.1, 0.0, true);
			TCompSound* sound = e->get<TCompSound>();
			sound->playSound("roar");

			CEntity* cam = (CEntity*)getEntityByName("camera_manager");
			TCompCameraManager* cm = cam->get<TCompCameraManager>();
			cm->activarTemblor();
		}
		else if (timer_runner >= 29.f && !end_temblor) {
			end_temblor = true;
			CEntity* cam = (CEntity*)getEntityByName("camera_manager");
			TCompCameraManager* cm = cam->get<TCompCameraManager>();
			cm->desactivarTemblor();
		}
		else if (timer_runner >= 32.f && !runner_chase) {
			runner_chase = true;
			CEntity* e_runner = getEntityByName("Runner");
			TMsgRunnerStartChase msg;
			e_runner->sendMsg(msg);

			CEntity* player = getEntityByName("The Player");
			TMsgSetFSMVariable pauseMsg;
			pauseMsg.variant.setName("pause");
			pauseMsg.variant.setBool(false);
			player->sendMsg(pauseMsg);

			pauseMsg.variant.setName("idle");
			pauseMsg.variant.setBool(true);
			player->sendMsg(pauseMsg);

			setBandsCinematics(false);
			cb_gui.cinematica = false;
			EngineSound.emitEvent("musica_cinematica_runner");
		}
	}

	if (change_level) {
		timer_runner += delta;
		cb_globals.global_fadeOut_adjustment = 10.f;
		EngineUI.activateWidget("fadeOut");
		EngineTower.setBandsCinematics(true);
		if (timer_runner > 0.4f && !change_level_done) {
			CEntity* e = getEntityByName("The Player");
			TCompPlayerController* player = e->get<TCompPlayerController>();
			if (player->game_state == "level_1") {
				CEngine::get().getModules().changeGameState("level_2");
				player->game_state = "level_2";
			}
			change_level_done = true;
		}
		else if (timer_runner > 1.f) {
			cb_globals.global_fadeOut_adjustment = 0.f;
			EngineUI.desactivateWidget("fadeOut");
			activate_runner = true;
			timer_runner = 0.f;
			change_level = false;
		}
	}

	if (end_game) {
		timer_end += delta;
		if (timer_end >= 12.2f) {
			cb_globals.global_fadeOut_adjustment = 10.f;
			EngineUI.activateWidget("fadeOut");
		}
		if (timer_end >= 14.5f) {
			end_game = false;
			cb_globals.global_fadeOut_adjustment = 0.f;
			EngineUI.desactivateWidget("fadeOut");
		}
	}
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
	if (entity) {
		TCompRender* h_render = entity->get< TCompRender >();
		h_render->is_active = false;
		h_render->refreshMeshesInRenderManager();
	}
}

const void CModuleTower::appearEntity(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	if (entity) {
		TCompRender* h_render = entity->get< TCompRender >();
		h_render->is_active = true;
		h_render->refreshMeshesInRenderManager();
	}
}

const void CModuleTower::renderOnlyShadows(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	if (entity) {
		TCompRender* h_render = entity->get< TCompRender >();
		h_render->only_shadows = true;
		h_render->refreshMeshesInRenderManager();
	}
}

const void CModuleTower::renderEverything(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	if (entity) {
		TCompRender* h_render = entity->get< TCompRender >();
		h_render->only_shadows = false;
		h_render->refreshMeshesInRenderManager();
	}
}


const void CModuleTower::activateCinematic(const std::string& name) {
	cb_gui.cinematica = true;
	CEntity* cinematic = (CEntity*)getEntityByName(name);
	TMsgActivateCinematic activate_cinematic;
	cinematic->sendMsg(activate_cinematic);
	current_cinematic = name;
}

const void CModuleTower::deactivateCinematic(const std::string& name) {
	CEntity* cinematic = (CEntity*)getEntityByName(name);
	TMsgDeactivateCinematic deactivate_cinematic;
	deactivate_cinematic.escaleras = false;
	cinematic->sendMsg(deactivate_cinematic);
	current_cinematic = "";
}

const void CModuleTower::setAmbientAdjustment(float ambient) {
	cb_globals.global_ambient_adjustment = ambient;
}

const void CModuleTower::setExposureAdjustment(float exposure) {
	cb_globals.global_exposure_adjustment = exposure;

}
void CModuleTower::setVignettingAdjustment(float vignetting)
{
	newVignetting = vignetting;
	changeVignetting = true;
}
void CModuleTower::setFadeOutAdjustment(float fadeout)
{
	newFadeOut = fadeout;
	changeFadeOut = true;
}
const void CModuleTower::setBandsCinematics(bool _band)
{
	bandCinematics = _band;
}
const void CModuleTower::setDirLightIntensity(const std::string& name, float intensity) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	if (entity) {
		TCompLightDir* light_dir = entity->get<TCompLightDir>();
		light_dir->setIntensity(intensity);
	}
}

const void CModuleTower::setPointLightIntensity(const std::string& name, float intensity) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	if (entity) {
		TCompLightPointShadows* light_point = entity->get<TCompLightPointShadows>();
		light_point->setIntensity(intensity);
	}
}

float CModuleTower::checkAngle(float alpha, VEC3 pos1) {
	if (alpha < 0.f) alpha += deg2rad(360.f);

	float cosa = cos(alpha);
	float sina = sin(alpha);
	float aux = rad2deg(alpha);

	if (pos1.x > 0.f and pos1.z > 0.f) {
		if (cos(alpha) < 0) {
			alpha = deg2rad(180) - alpha;
		}
	}
	else if (pos1.x < 0.f and pos1.z > 0.f) {
		if (cos(alpha) > 0) {
			alpha = deg2rad(180) - alpha;
		}
	}
	else if (pos1.x < 0.f and pos1.z < 0.f) {
		if (cos(alpha) > 0) {
			alpha = deg2rad(180) + (deg2rad(360) - alpha);
		}
	}
	else if (pos1.x > 0.f and pos1.z < 0.f) {
		if (cos(alpha) < 0) {
			alpha = deg2rad(270) + (deg2rad(270) - alpha);
		}
	}

	return alpha;
}

const void CModuleTower::openDoor(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	TMsgOpenDoor msg;
	entity->sendMsg(msg);
}

const void CModuleTower::closeDoor(const std::string& name) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	TMsgCloseDoor msg;
	entity->sendMsg(msg);
}

const void CModuleTower::activateAnim(const std::string& name, float wait_time) {
	CEntity* entity = (CEntity*)getEntityByName(name);
	TMsgActivateAnim msg;
	msg.name = name;
	msg.wait_time = wait_time;
	entity->sendMsg(msg);
}

void CModuleTower::setExposure(float _exposure) {
	newExposure = _exposure;
	changeExposure = true;
}

void CModuleTower::setLight(float _light)
{
	newLight = _light;
	changeLight = true;
}

void CModuleTower::wait_seconds(float num_seconds) {
	current_time = 0.0f;
	total_wait_time = num_seconds;
	time_out = true;
}

void CModuleTower::activateRunner() {
	cb_gui.cinematica = true;
	change_level = true;
	timer_runner = 0.f;
}

void CModuleTower::endGame() {
	timer_end = 0.f;
	end_game = true;
}
