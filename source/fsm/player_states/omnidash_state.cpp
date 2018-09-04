#include "mcv_platform.h"
#include "omnidash_state.h"
#include "components/player/comp_player_controller.h"
#include "entity/entity_parser.h"
#include "fsm/context.h"
#include "components/postfx/comp_render_blur_radial.h"

namespace FSM
{
	void OmnidashState::onStart(CContext& ctx) const
	{
		ctx.setVariable("can_omni", false);
		CEntity* e = ctx.getOwner();
		TCompPlayerController* player = e->get<TCompPlayerController>();
		player->anim1 = calculateAnimation(e);
		player->clear_animations(0.3f);
		player->change_animation(player->anim1, _is_action, _delay_in, _delay_out, true);
		TCompTransform *c_my_transform = e->get<TCompTransform>();
		player->jumping_start_height = c_my_transform->getPosition().y;
		EngineTimer.setTimeSlower(0.25f);
		EngineUI.setOmindash(true);

		CEntity* o_camera = EngineCameras.getOutputCamera();

		TCompCamera* c_camera = o_camera->get< TCompCamera >();

        VEC2 player_center = VEC2(player->player_position.x, player->player_position.y);
		TCompRenderBlurRadial* c_render_blur_radial = o_camera->get< TCompRenderBlurRadial >();
		if (c_render_blur_radial)
		{
			c_render_blur_radial->setCenter(player_center);
			c_render_blur_radial->setActive(true);
		}

		_mouseStartPosition = EngineInput.mouse()._position;
	}

	bool OmnidashState::load(const json& jData)
	{
		_is_action = jData.value("is_action", false);
		_delay_out = jData.value("delay_out", 0.01f);
		_delay_in = jData.value("delay_in", 0.01f);
		return true;
	}

	bool OmnidashState::update(float dt, CContext& ctx) const
	{
		CEntity* e = ctx.getOwner();
		TCompPlayerController* player = e->get<TCompPlayerController>(); 
		CEntity* o_camera = EngineCameras.getOutputCamera();
		TCompTransform *c_my_transform = e->get<TCompTransform>();
		VEC2 screen_projected_pos;
		screen_projected_pos.x = player->player_position.x / Render.width;
		screen_projected_pos.y = player->player_position.y / Render.height;


		TCompRenderBlurRadial* c_render_blur_radial = o_camera->get< TCompRenderBlurRadial >();
		if (c_render_blur_radial)
		{
			c_render_blur_radial->setCenter(screen_projected_pos);
		}


		// Chequea si hay que realizar el salto
		if (!EngineInput["omnidash"].isPressed()) {
			ctx.setVariable("omnijump", true);
		}

		int anim = calculateAnimation(e);
		if (player->anim2 != anim) {
			if (player->anim2 != -1)
				player->remove_animation(player->anim2);
			player->anim2 = player->anim1;
			player->anim1 = anim;
			player->change_animation(player->anim1, _is_action, _delay_in, _delay_out, false);
		}
		return false;
	}

	int OmnidashState::calculateAnimation(CEntity* e) {
		CEntity* e_camera = EngineCameras.getActiveCamera();
		TCompCamera* c_camera = e_camera->get< TCompCamera >();
		TCompTransform *c_my_transform = e->get<TCompTransform>();
		const Input::TInterface_Mouse& mouse = EngineInput.mouse();
		VEC3 my_pos = c_my_transform->getPosition();
		VEC3 player_position;
		c_camera->getScreenCoordsOfWorldCoord(my_pos, &player_position);
		VEC3 omnidash_arrow = mouse._position - VEC2(player_position.x, player_position.y);
		omnidash_arrow.Normalize();
		TCompPlayerController* player = e->get<TCompPlayerController>();
		if (omnidash_arrow.x >= 0.75) {
			return player->EAnimations::NajaOmniPrepFr;
		}
		else if (omnidash_arrow.x <= -0.80){
			return player->EAnimations::NajaOmniPrepBk;
		}
		else if (omnidash_arrow.y <= -0.80) {
			return player->EAnimations::NajaOmniPrepUp;
		} 
		else if (omnidash_arrow.y >= 0.80) {
			return player->EAnimations::NajaOmniPrepDn;
		} 
		else if (omnidash_arrow.x >= 0.20 && omnidash_arrow.y >= 0.20) {
			return player->EAnimations::NajaOmniPrepFrDn;
		}
		else if (omnidash_arrow.x <= -0.20 && omnidash_arrow.y <= -0.20) {
			return player->EAnimations::NajaOmniPrepFrBk;
		}
		else if (omnidash_arrow.x <= -0.20 && omnidash_arrow.y >= 0.20) {
			return player->EAnimations::NajaOmniPrepBkDn;
		}
		else {
			return player->EAnimations::NajaOmniPrepFrUp;
		}
	}

	void OmnidashState::onFinish(CContext& ctx) const {
		ctx.setVariable("omnidash", false);
        CEntity* e_player = (CEntity*)getEntityByName("The Player");
		TCompPlayerController* player = e_player->get<TCompPlayerController>();
        player->previous_state = "omnidash";
		player->y_speed_factor = 0;
		EngineTimer.setTimeSlower(1.f);
		EngineUI.setOmindash(false);

        CEntity* e_camera = EngineCameras.getOutputCamera();
        TCompRenderBlurRadial* c_render_blur_radial = e_camera->get< TCompRenderBlurRadial >();
        if (c_render_blur_radial)
        {
            c_render_blur_radial->setActive(false);
        }
       
        TCompTransform *c_my_transform = e_player->get<TCompTransform>();
		const Input::TInterface_Mouse& mouse = EngineInput.mouse();
		

		player->omnidash_arrow = mouse._position - VEC2(player->player_position.x, player->player_position.y);
		player->omnidash_arrow.Normalize();

		player->y_speed_factor = 0;

		TEntityParseContext ctx1;
		ctx1.entity_starting_the_parse = e_player;
		ctx1.root_transform = *(TCompTransform*)e_player->get<TCompTransform>();
		VEC3 omni_vector = c_my_transform->getFront();
		if (player->looking_left)
			omni_vector *= player->omnidash_arrow.x;
		else
			omni_vector *= player->omnidash_arrow.x * -1;
       
		omni_vector.y += player->omnidash_arrow.y;
		ctx1.front = -omni_vector;
		if (parseScene("data/prefabs/windstrike.prefab", ctx1)) {
			assert(!ctx1.entities_loaded.empty());			
		}
        int anim;
        if (player->omnidash_arrow.x >= 0.75) {
            anim = player->EAnimations::NajaOmniFr;
        }
        else if (player->omnidash_arrow.x <= -0.80) {
            anim = player->EAnimations::NajaOmniBk;
        }
        else if (player->omnidash_arrow.y <= -0.80) {
            anim = player->EAnimations::NajaOmniAr;
        }
        else if (player->omnidash_arrow.y >= 0.80) {
            anim = player->EAnimations::NajaOmniAb;
        }
        else if (player->omnidash_arrow.x >= 0.20 && player->omnidash_arrow.y >= 0.20) {
            anim = player->EAnimations::NajaOmniFrDn;
        }
        else if (player->omnidash_arrow.x <= -0.20 && player->omnidash_arrow.y <= -0.20) {
            anim = player->EAnimations::NajaOmniBkUp;
        }
        else if (player->omnidash_arrow.x <= -0.20 && player->omnidash_arrow.y >= 0.20) {
            anim = player->EAnimations::NajaOmniBkDn;
        }
        else {
            anim = player->EAnimations::NajaOmniFrUp;
        }
        player->change_animation(anim, true, 0, 0, true);
	}
}