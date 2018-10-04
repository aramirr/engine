#include "mcv_platform.h"
#include "entity.h"
#include "entity_parser.h"
#include "components/juan/comp_name.h"
#include "common_msgs.h"

DECL_OBJ_MANAGER("entity", CEntity);

std::unordered_multimap< uint32_t, TCallbackSlot > all_registered_msgs;
uint32_t getNextUniqueMsgID() {
  static uint32_t unique_msg_id = 0;
  ++unique_msg_id;
  return unique_msg_id;
}

CEntity::~CEntity() {
  sendMsg(TMsgEntityDestroyed());

  // Comp 0 is not valid
  for (uint32_t i = 1; i < CHandleManager::getNumDefinedTypes(); ++i) {
    CHandle h = comps[i];
    if (comps[i].isValid())
      comps[i].destroy();
  }
}


void CEntity::set(uint32_t comp_type, CHandle new_comp) {
  assert(comp_type < CHandle::max_types);
  assert(!comps[comp_type].isValid());
  comps[comp_type] = new_comp;
  new_comp.setOwner(CHandle(this));
}

const char* CEntity::getName() const {
  TCompName* comp_name = get<TCompName>();
  return comp_name->getName();
}

void CEntity::addNumberToName(int number) {
	TCompName* comp_name = get<TCompName>();
	
	std::string new_name = comp_name->getName();
	new_name += std::to_string(number);
	comp_name->setName(new_name.c_str());
}

void CEntity::renderDebug() {
  for (uint32_t i = 0; i < CHandleManager::getNumDefinedTypes(); ++i) {
    CHandle h = comps[i];
    if (h.isValid())
      h.renderDebug();
  }
}

void CEntity::debugInMenu() {
  if (ImGui::TreeNode(getName())) {
    renderDebug();

    // Give option to destroy the entity
    ImGui::SameLine();
    if (ImGui::SmallButton("Del")) {
      CHandle(this).destroy();
      // If destroyed, exit the menu asap
      ImGui::TreePop();
      return;
    }

    for (int i = 0; i < CHandle::max_types; ++i) {
      CHandle h = comps[i];
      if (h.isValid()) {

        // Open a tree using the name of the component
        if (ImGui::TreeNode(h.getTypeName())) {
          // Do the real show details of the component
          h.debugInMenu();
          ImGui::TreePop();
        }
      }
    }

    ImGui::TreePop();
  }
}

void CEntity::load(const json& j, TEntityParseContext& ctx) {

  ctx.current_entity = CHandle(this);

  for (auto it = j.begin(); it != j.end(); ++it) {

    auto& comp_name = it.key();
    auto& comp_json = it.value();

    auto om = CHandleManager::getByName(comp_name.c_str());
    if (!om) {
      if( comp_name != "prefab" )
      continue;
    }
    assert(om);
    
    int comp_type = om->getType();      // 7

    // This is my current handle of this type of component
    CHandle h_comp = comps[comp_type];
    if (h_comp.isValid()) {
      // Give an option to reconfigure the existing comp with the new json
      h_comp.load(comp_json, ctx);
    }
    else 
    {
      // Get a new fresh component of this type for me
      h_comp = om->createHandle();

      // Initialize the comp from the json. You still don't have an owner
      h_comp.load(comp_json, ctx);

      // Bind it to me
      set(comp_type, h_comp);
    }

  }

  // Send a msg to the entity components to let them know
  // the entity is fully loaded.
  if(!ctx.parsing_prefab)
    sendMsg(TMsgEntityCreated());

}
