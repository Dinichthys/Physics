#include "scene_manager.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "logging.h"
#include "my_assert.h"

bool SceneManager::OnLetterA() {
    eye_.SetEyePos(eye_.GetEyePos() + !(eye_.GetEyeLTCorner() - eye_.GetEyeRTCorner()) * kStepEye);
    return true;
}

bool SceneManager::OnLetterD() {
    eye_.SetEyePos(eye_.GetEyePos() - !(eye_.GetEyeLTCorner() - eye_.GetEyeRTCorner()) * kStepEye);
    return true;
}

bool SceneManager::OnLetterS() {
    eye_.SetEyePos(eye_.GetEyePos() - !(eye_.GetEyeLTCorner() + eye_.GetEyeRBCorner()) * kStepEye);
    return true;
}

bool SceneManager::OnLetterW() {
    eye_.SetEyePos(eye_.GetEyePos() + !(eye_.GetEyeLTCorner() + eye_.GetEyeRBCorner()) * kStepEye);
    return true;
}

bool SceneManager::OnArrowRight() {
    return CameraRotation(0, 2, 1);
}

bool SceneManager::OnArrowLeft() {
    return CameraRotation(0, 2, -1);
}

bool SceneManager::OnArrowUp() {
    return CameraRotation(1, 2, -1);
}

bool SceneManager::OnArrowDown() {
    return CameraRotation(1, 2, 1);
}

bool SceneManager::CameraRotation(size_t ace_1, size_t ace_2, float direction) {
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();
    Coordinates rb_corner = eye_.GetEyeRBCorner();

    Coordinates vec(lt_corner);
    vec.SetCoordinate(ace_1, lt_corner[ace_1] * kCosRotate + direction * lt_corner[ace_2] * kSinRotate);
    vec.SetCoordinate(ace_2, lt_corner[ace_2] * kCosRotate - direction * lt_corner[ace_1] * kSinRotate);
    eye_.SetEyeLTCorner(Coordinates(vec));

    vec = lb_corner;
    vec.SetCoordinate(ace_1, lb_corner[ace_1] * kCosRotate + direction * lb_corner[ace_2] * kSinRotate);
    vec.SetCoordinate(ace_2, lb_corner[ace_2] * kCosRotate - direction * lb_corner[ace_1] * kSinRotate);
    eye_.SetEyeLBCorner(Coordinates(vec));

    vec = rt_corner;
    vec.SetCoordinate(ace_1, rt_corner[ace_1] * kCosRotate + direction * rt_corner[ace_2] * kSinRotate);
    vec.SetCoordinate(ace_2, rt_corner[ace_2] * kCosRotate - direction * rt_corner[ace_1] * kSinRotate);
    eye_.SetEyeRTCorner(Coordinates(vec));

    vec = rb_corner;
    vec.SetCoordinate(ace_1, rb_corner[ace_1] * kCosRotate + direction * rb_corner[ace_2] * kSinRotate);
    vec.SetCoordinate(ace_2, rb_corner[ace_2] * kCosRotate - direction * rb_corner[ace_1] * kSinRotate);
    eye_.SetEyeRBCorner(Coordinates(vec));

    return true;
}
