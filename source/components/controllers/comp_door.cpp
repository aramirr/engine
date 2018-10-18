#include "mcv_platform.h"
#include "entity/entity_parser.h"
#include "comp_door.h"
#include "components/juan/comp_transform.h"
#include "components/physics/controller_filter.h"
#include "components/physics/query_filter.h"
#include "components/juan/comp_father.h"
#include "components/sound/comp_sound.h"

using namespace physx;

DECL_OBJ_MANAGER("door", TCompDoor);

void TCompDoor::debugInMenu() {
}

void TCompDoor::load(const json& j, TEntityParseContext& ctx) {
  setEntity(ctx.current_entity);
  speed = j.value("speed", 1.0f);
	left = j.value("left", false);
	up = j.value("up", false);
	height = j.value("height", 4.0f);
  is_open = false;
  opening = false;
	radiants = 90.f;
	current_height = 0.f;
  current_radiants = 0.f;
}

void TCompDoor::setEntity(CHandle new_entity) {
	h_entity = new_entity;
	assert(h_entity.isValid());
}

void TCompDoor::update(float DT) {
    if (opening && !is_open) {
        CEntity *e = h_entity;
        if (!h_entity.isValid()) return;
        TCompTransform *my_transform = e->get<TCompTransform>();
        TCompCollider *my_collider = e->get<TCompCollider>();
        VEC3 my_pos = my_transform->getPosition();
				if (!up) {
					float y, p, r;
					my_transform->getYawPitchRoll(&y, &p, &r);
					float current_rotation = speed * DT;
					y += left ? current_rotation : -current_rotation;
					current_radiants += current_rotation;
					my_transform->setYawPitchRoll(y, p, r);
					if (current_radiants >= deg2rad(radiants)) {
						is_open = true;
					}
				}
				else {
					current_height += DT;
					if (current_height >= height)
						is_open = true;
					my_pos.y += DT;					
					my_transform->setPosition(my_pos);
				}
        
        QUAT newRot = my_transform->getRotation();

        PxRigidActor* rigidActor = ((PxRigidActor*)my_collider->actor);
        PxTransform tr = rigidActor->getGlobalPose();
        tr.p = PxVec3(my_pos.x, my_pos.y, my_pos.z);
        tr.q = PxQuat(newRot.x, newRot.y, newRot.z, newRot.w);
        rigidActor->setGlobalPose(tr);        
    } else	
    if (closing && !is_closed) {
        CEntity *e = h_entity;
        if (!h_entity.isValid()) return;
        TCompTransform *my_transform = e->get<TCompTransform>();
        TCompCollider *my_collider = e->get<TCompCollider>();
        VEC3 my_pos = my_transform->getPosition();
        
        current_height -= DT;
        if (current_height <= -height/8)
            is_closed= true;
        my_pos.y -= DT*8;
        my_transform->setPosition(my_pos);

        QUAT newRot = my_transform->getRotation();

        PxRigidActor* rigidActor = ((PxRigidActor*)my_collider->actor);
        PxTransform tr = rigidActor->getGlobalPose();
        tr.p = PxVec3(my_pos.x, my_pos.y, my_pos.z);
        tr.q = PxQuat(newRot.x, newRot.y, newRot.z, newRot.w);
        rigidActor->setGlobalPose(tr);
    }
}


void TCompDoor::destroy()
{
	if (!CHandle(this).getOwner().isValid()) {
		return;
	}
	CEntity * e = h_entity;
	/*TCompFather *father_comp = e->get<TCompFather>();
	if (father_comp) {
		for (auto h : father_comp->sons) {
			CEntity* e = h;
			e->~CEntity();
		}
	}*/

	CHandle h_col = e->get<TCompCollider>();
	TCompCollider *my_col = e->get<TCompCollider>();
	if (my_col) {
		my_col->actor->getScene()->removeActor(*my_col->actor);
		my_col->actor = nullptr;

		CEntity* e = CHandle(this).getOwner();
		assert(e);
		my_col->controller->release();
		my_col->controller = nullptr;
	}
	CEntity *e_creator = h_parent;
	TMsgWindstrike msg;
	e_creator->sendMsg(msg);	
	CHandle(this).getOwner().destroy();
}

void TCompDoor::open(const TMsgOpenDoor& msg)
{
    opening = true;
}

void TCompDoor::close(const TMsgCloseDoor& msg)
{
    if (!closing && !is_closed) {
        TCompSound* sound = get<TCompSound>();
        sound->playSound("sound");
    }
    closing = true;
}

void TCompDoor::registerMsgs() {
    DECL_MSG(TCompDoor, TMsgOpenDoor, open);
    DECL_MSG(TCompDoor, TMsgCloseDoor, close);
}