#include "mcv_platform.h"

static CVertexDeclManager vtx_decl_manager;

CVertexDeclManager& CVertexDeclManager::get() {
  return vtx_decl_manager;
}

// ----------------------------------------------------------
CVertexDecl* CVertexDeclManager::createNew(
  const std::string& name
  , D3D11_INPUT_ELEMENT_DESC* layout
  , UINT numElements
) {
  CVertexDecl* decl = new CVertexDecl();
  decl->name = name; 
  decl->cpu_layout = layout;
  decl->numElements = numElements;
  decls[name] = decl;

  // Automatically try to find the bytes per vertex based on the 
  // size of each element in the vertex layout
  UINT total_bytes = 0;
  for (UINT i = 0; i < numElements; ++i) {
    auto d = layout + i;
    switch (d->Format) {
    case DXGI_FORMAT_R32G32B32A32_FLOAT: total_bytes += 4 * 4; break;
    case DXGI_FORMAT_R32G32B32_FLOAT: total_bytes += 3 * 4; break;
    case DXGI_FORMAT_R32G32_FLOAT: total_bytes += 2 * 4; break;
    case DXGI_FORMAT_R8G8B8A8_UINT: total_bytes += 4; break;
    case DXGI_FORMAT_R8G8B8A8_UNORM: total_bytes += 4; break;
    default:
      fatal("Unknown size of vertex element %08x while declaring vtx decl %s.%s\n", d->Format, name.c_str(), d->SemanticName);
    }
  }
  decl->bytes_per_vertex = total_bytes;

  return decl;
}

// ----------------------------------------------------------
// Create & register each vertex declr
bool CVertexDeclManager::create() {

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createNew("Pos", layout, ARRAYSIZE(layout));
  }

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createNew("PosClr", layout, ARRAYSIZE(layout));
  }

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createNew("PosNUv", layout, ARRAYSIZE(layout));
  }

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createNew("PosNUvUv", layout, ARRAYSIZE(layout));
  }

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createNew("PosNUvUvT", layout, ARRAYSIZE(layout));
  }

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONES",    0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "WEIGHTS",  0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createNew("PosNUvSkin", layout, ARRAYSIZE(layout));
  }

  {
    static D3D11_INPUT_ELEMENT_DESC layout[] = {
      // Info of the instance. Sync with TRenderParticleDecal
    { "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },    // world0
    { "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },    // world1
    { "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },    // world2
    { "TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },    // world3
    { "TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },    // life, time_blending_out
    };
    createNew("DecalInstance", layout, ARRAYSIZE(layout));
  }

  return true;
}

const CVertexDecl* CVertexDeclManager::getByName(const std::string& name) {
  auto it = decls.find(name);
  if (it == decls.end()) {

    // Check if it's an instance vertex declaration
    auto idx = name.find('_');
    if (idx != std::string::npos) {
      std::string first = name.substr(0, idx);
      std::string second = name.substr(idx + 1);
      const CVertexDecl* a = getByName(first);
      const CVertexDecl* b = getByName(second);
      assert(a && b);
      auto num_elems = a->numElements + b->numElements;
      D3D11_INPUT_ELEMENT_DESC* layout = new D3D11_INPUT_ELEMENT_DESC[num_elems];
      memcpy(layout, a->cpu_layout, a->numElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
      memcpy(layout + a->numElements, b->cpu_layout, b->numElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
      return createNew(name, layout, num_elems);
    }

    fatal("Can't find vertex declaration named '%s'", name.c_str());
    return nullptr;
  }
  return it->second;
}


