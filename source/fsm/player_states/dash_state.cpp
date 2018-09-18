#include "mcv_platform.h"
#include "dash_state.h"
#include "fsm/context.h"
#include "components/player/comp_player_controller.h"
#include "components\comp_particles.h"

namespace FSM
{
	void DashState::onStart(CContext& ctx) const
	{
		ctx.setVariable("can_dash", false);
		CEntity* e = ctx.getOwner();
		TCompPlayerController* player = e->get<TCompPlayerController>();
		TCompTransform* c_my_transform = e->get<TCompTransform>();
		player->jumping_start_height = c_my_transform->getPosition().y;
        player->clear_animations(0.1f);
		player->change_animation(player->EAnimations::NajaDashStrike, _is_action, _delay_in, _delay_out, true);

        CEntity* particles_emiter;
        if (player->looking_left)
            particles_emiter = (CEntity*)getEntityByName("humo_dash_left");
        else
            particles_emiter = (CEntity*)getEntityByName("humo_dash_right");
        TCompParticles* c_particles = particles_emiter->get<TCompParticles>();
        c_particles->emit();
		CEntity* trail = (CEntity *)getEntityByName("player_trail");
		if (trail)
		{
			TCompParticles* particle = (TCompParticles*)trail->get<TCompParticles>();
			particle->_core->life.maxParticles = 200;
		}
	}

	bool DashState::load(const json& jData)
	{
		_x_speed = jData.value("x_speed", 10.f);
		_is_action = jData.value("is_action", false);
		_delay_out = jData.value("delay_out", 0.01f);
		_delay_in = jData.value("delay_in", 0.01f);
		return true;
	}

	bool DashState::update(float dt, CContext& ctx) const
	{
		CEntity* e = ctx.getOwner();
		TCompPlayerController* player = e->get<TCompPlayerController>();
		if (player->looking_left)
			player->move_player(false, false, dt, 0, _x_speed);
		else
			player->move_player(true, false, dt, 0, _x_speed);
		return false;
	}

	void DashState::onFinish(CContext& ctx) const {
		CEntity* trail = (CEntity *)getEntityByName("player_trail");
		if (trail)
		{
			TCompParticles* particle = (TCompParticles*)trail->get<TCompParticles>();
			particle->_core->life.maxParticles = 0;
		}

		ctx.setVariable("dash", false);
		CEntity* e = ctx.getOwner();
		TCompPlayerController* player = e->get<TCompPlayerController>();
        player->previous_state = "dash";
	}
}