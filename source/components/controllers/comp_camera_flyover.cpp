#include "mcv_platform.h"
#include "comp_camera_flyover.h"
#include "components/juan/comp_transform.h"

DECL_OBJ_MANAGER("camera_flyover", TCompCameraFlyover);

void TCompCameraFlyover::debugInMenu() {
  ImGui::DragFloat("Speed", &_speed, 10.f, 1.f, 100.f);
  ImGui::DragFloat("Sensitivity", &_sensitivity, 0.01f, 0.001f, 0.1f);
}

void TCompCameraFlyover::load(const json& j, TEntityParseContext& ctx) {
  _speed = j.value("speed", _speed);
  _sensitivity = j.value("sensitivity", _sensitivity);
}

void TCompCameraFlyover::update(float dt)
{
  TCompTransform* c_transform = get<TCompTransform>();
  if (!c_transform)
    return;
  VEC3 pos = c_transform->getPosition();
  VEC3 front = c_transform->getFront();
  VEC3 left = c_transform->getLeft();
  VEC3 up = VEC3::UnitY;

  // movement
  float deltaSpeed = _speed * dt;
  if (EngineInput["turbo"])
    deltaSpeed *= 2.f;
  VEC3 off;
  off += front * EngineInput["front"].value * deltaSpeed;
  off += -front * EngineInput["back"].value * deltaSpeed;
  off += left * EngineInput["left"].value * deltaSpeed;
  off += -left * EngineInput["right"].value * deltaSpeed;
  off += up * EngineInput["up"].value * deltaSpeed;
  off += -up * EngineInput["down"].value * deltaSpeed;

  // rotation
  float yaw, pitch;
  getYawPitchFromVector(front, &yaw, &pitch);

  auto& mouse = EngineInput.mouse();
  if (mouse.button(Input::MOUSE_RIGHT).isPressed())
  {
    VEC2 mOff = mouse._position_delta;
    yaw += -mOff.x * _sensitivity;
    pitch += -mOff.y * _sensitivity;
    pitch = clamp(pitch, -_maxPitch, _maxPitch);
  }

  // final values
  VEC3 newPos = pos + off;
  VEC3 newFront = getVectorFromYawPitch(yaw, pitch);

  c_transform->lookAt(newPos, newPos + newFront);
}
