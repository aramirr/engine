#pragma once

#include "comp_base.h"

class CRenderMesh;
class CTexture;
class CRenderTechnique;
class CMaterial;

class TCompRender : public TCompBase {
  void loadMesh(const json& j, TEntityParseContext& ctx);
  void onDefineLocalAABB(const TMsgDefineLocalAABB& msg);

public:

  ~TCompRender();

  VEC4               color = VEC4(1, 1, 1, 1);
  AABB               aabb;
  // This represents a single object mesh with several materials. Not multiples meshes
  struct CMeshWithMaterials {
    bool               enabled = true;
    const CRenderMesh* mesh = nullptr;
    std::vector<const CMaterial*> materials;
  };
  std::vector<CMeshWithMaterials> meshes;
  
  void refreshMeshesInRenderManager();
  void debugInMenu();
  void renderDebug();
  void load(const json& j, TEntityParseContext& ctx);

  static void registerMsgs();

  DECL_SIBLING_ACCESS();
};