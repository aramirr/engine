#include "mcv_platform.h"
#include "camera.h"

CCamera::CCamera() {
  setPerspective(deg2rad(60.f), 1.0f, 1000.f);
  lookAt(VEC3(1, 0, 0), VEC3(0, 0, 0), VEC3(0, 1, 0));
}

void CCamera::updateViewProj() {
  view_proj = view * proj;
}

// 
void CCamera::lookAt(VEC3 new_pos, VEC3 new_target, VEC3 new_up_aux) {
  
  pos = new_pos;
  target = new_target;
  up_aux = new_up_aux;

  front = (target - pos);
  front.Normalize();
  left = front.Cross(new_up_aux);
  left.Normalize();
  up = front.Cross(left);
  up.Normalize();

  left = new_up_aux.Cross(front);
  left.Normalize();

  up = front.Cross(left);

  view = MAT44::CreateLookAt(pos, target, up_aux);
  updateViewProj();
}

void CCamera::setPerspective(float new_fov_vertical, float new_z_near, float new_z_far) {
  fov_vertical = new_fov_vertical;
  z_near = new_z_near;
  z_far = new_z_far;
  proj = MAT44::CreatePerspectiveFieldOfView(new_fov_vertical, aspect_ratio, new_z_near, new_z_far);
  updateViewProj();
  is_ortho = false;
}

MAT44 loadOrtho(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) {
  MAT44 m = MAT44::Identity;

  // (RH) l=x_min; r=x_max; b=y_min; t=y_max; zn=z_min; zf=z_max; 
  //
  // 2/(r-l)      0            0           0
  // 0            2/(t-b)      0           0
  // 0            0            1/(zf-zn)   0
  // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

  assert(x_max != x_min);
  assert(y_max != y_min);
  assert(z_max != z_min);

  m._11 = 2.0f / (x_max - x_min);
  m._22 = 2.0f / (y_max - y_min);
  m._33 = 1.0f / (z_max - z_min);
  m._41 = (x_min + x_max) / (x_min - x_max);
  m._42 = (y_max + y_min) / (y_min - y_max);
  m._43 = z_min / (z_min - z_max);
  return m;
}

void CCamera::setOrtho(float orto_width, float orto_height, float new_z_near, float new_z_far) {

	is_ortho = true;

	z_near = new_z_near;
	z_far = new_z_far;
	this->orto_width = orto_width;
	this->orto_height = orto_height;

	proj = MAT44::CreateOrthographic(orto_width, orto_width, new_z_near, new_z_far);
	updateViewProj();
}

void CCamera::setOrthographic(float width, float height)
{
  is_ortho = true;

  aspect_ratio = fabsf(width / height);
  z_far = 1.f;
  z_near = -1.f;

  MAT44 custom_ortho = loadOrtho(0.f, width, height, 0.f, z_near, z_far);
  proj = custom_ortho;

  // Position and target and unset!
  pos = VEC3(width * 0.5f, height * 0.5f, z_near);
  front.x = 0;
  front.y = 0;
  front.z = 1;
  left.x = 1;
  left.y = 0;
  left.z = 0;
  up.x = 0;
  up.y = 1;
  up.z = 0;
  view = MAT44::Identity;
  view_proj = proj;
}

void CCamera::setViewport(int x0, int y0, int width, int height) {
  // save params
  viewport.x0 = x0;
  viewport.y0 = y0;
  viewport.width = width;
  viewport.height = height;

  aspect_ratio = (float)width / (float)height;

  if(!is_ortho)
    setPerspective(fov_vertical, z_near, z_far);
}

bool CCamera::getScreenCoordsOfWorldCoord(VEC3 world_pos, VEC3* result) const {

  // It's also dividing by w  -> [-1..1]
  VEC3 pos_in_homo_space = VEC3::Transform(world_pos, getViewProjection());

  // Convert to 0..1 and then to viewport coordinates
  VEC3 pos_in_screen_space(
    viewport.x0 + (pos_in_homo_space.x + 1.0f) * 0.5f * viewport.width,
    viewport.y0 + (1.0f - pos_in_homo_space.y) * 0.5f * viewport.height,
    pos_in_homo_space.z
  );

  assert(result);
  *result = pos_in_screen_space;

  // Return true if the coord is inside the frustum
  return pos_in_homo_space.x >= -1.0f && pos_in_homo_space.x <= 1.0f
    && pos_in_homo_space.y >= -1.0f && pos_in_homo_space.y <= 1.0f
    && pos_in_homo_space.z >= 0.0f && pos_in_homo_space.z <= 1.0f
    ;
}
