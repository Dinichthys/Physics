#include "scene_manager.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "logging.h"
#include "my_assert.h"

bool SceneManager::OnLetterA() {
    eye_.SetEyePos(eye_.GetEyePos() + (!(eye_.GetEyeLTCorner() - eye_.GetEyeRTCorner()))
     * kStepEye);
    return true;
}

bool SceneManager::OnLetterD() {
    eye_.SetEyePos(eye_.GetEyePos() - (!(eye_.GetEyeLTCorner() - eye_.GetEyeRTCorner()))
     * kStepEye);
    return true;
}

bool SceneManager::OnLetterS() {
    eye_.SetEyePos(eye_.GetEyePos() - (!(eye_.GetEyeLTCorner() + eye_.GetEyeRBCorner()))
     * kStepEye);
    return true;
}

bool SceneManager::OnLetterW() {
    eye_.SetEyePos(eye_.GetEyePos() + (!(eye_.GetEyeLTCorner() + eye_.GetEyeRBCorner()))
     * kStepEye);
    return true;
}

bool SceneManager::OnArrowRight() {
    return CameraRotationInTwoAxis(0, 2, 1);
}

bool SceneManager::OnArrowLeft() {
    return CameraRotationInTwoAxis(0, 2, -1);
}

bool SceneManager::CameraRotationInTwoAxis(size_t axis_1, size_t axis_2, float direction) {
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();
    Coordinates rb_corner = eye_.GetEyeRBCorner();

    Coordinates vec(lt_corner);
    vec.SetCoordinate(axis_1, lt_corner[axis_1] * kCosRotate + direction * lt_corner[axis_2] * kSinRotate);
    vec.SetCoordinate(axis_2, lt_corner[axis_2] * kCosRotate - direction * lt_corner[axis_1] * kSinRotate);
    eye_.SetEyeLTCorner(Coordinates(vec));

    vec = lb_corner;
    vec.SetCoordinate(axis_1, lb_corner[axis_1] * kCosRotate + direction * lb_corner[axis_2] * kSinRotate);
    vec.SetCoordinate(axis_2, lb_corner[axis_2] * kCosRotate - direction * lb_corner[axis_1] * kSinRotate);
    eye_.SetEyeLBCorner(Coordinates(vec));

    vec = rt_corner;
    vec.SetCoordinate(axis_1, rt_corner[axis_1] * kCosRotate + direction * rt_corner[axis_2] * kSinRotate);
    vec.SetCoordinate(axis_2, rt_corner[axis_2] * kCosRotate - direction * rt_corner[axis_1] * kSinRotate);
    eye_.SetEyeRTCorner(Coordinates(vec));

    vec = rb_corner;
    vec.SetCoordinate(axis_1, rb_corner[axis_1] * kCosRotate + direction * rb_corner[axis_2] * kSinRotate);
    vec.SetCoordinate(axis_2, rb_corner[axis_2] * kCosRotate - direction * rb_corner[axis_1] * kSinRotate);
    eye_.SetEyeRBCorner(Coordinates(vec));

    return true;
}

bool SceneManager::OnArrowUp() {
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();
    Coordinates rb_corner = eye_.GetEyeRBCorner();

    Coordinates hor_vec = (lt_corner - rt_corner) / 2;
    Coordinates vec = lt_corner - hor_vec;
    float hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    lt_corner.SetCoordinate(0, hor_vec[0] + vec[0] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    lt_corner.SetCoordinate(1, hor_vec[1] + vec[1] * kCosRotate - kSinRotate * hor_length);
    lt_corner.SetCoordinate(2, hor_vec[2] + vec[2] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    eye_.SetEyeLTCorner(Coordinates(lt_corner));

    vec = lb_corner - hor_vec;
    hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    lb_corner.SetCoordinate(0, hor_vec[0] + vec[0] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    lb_corner.SetCoordinate(1, hor_vec[1] + vec[1] * kCosRotate - kSinRotate * hor_length);
    lb_corner.SetCoordinate(2, hor_vec[2] + vec[2] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    eye_.SetEyeLBCorner(Coordinates(lb_corner));

    vec = rt_corner + hor_vec;
    hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    rt_corner.SetCoordinate(0, -hor_vec[0] + vec[0] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    rt_corner.SetCoordinate(1, -hor_vec[1] + vec[1] * kCosRotate - kSinRotate * hor_length);
    rt_corner.SetCoordinate(2, -hor_vec[2] + vec[2] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    eye_.SetEyeRTCorner(Coordinates(rt_corner));

    vec = rb_corner + hor_vec;
    hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    rb_corner.SetCoordinate(0, -hor_vec[0] + vec[0] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    rb_corner.SetCoordinate(1, -hor_vec[1] + vec[1] * kCosRotate - kSinRotate * hor_length);
    rb_corner.SetCoordinate(2, -hor_vec[2] + vec[2] * (kCosRotate + vec[1] * kSinRotate / hor_length));
    eye_.SetEyeRBCorner(Coordinates(rb_corner));

    return true;
}

bool SceneManager::OnArrowDown() {
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();
    Coordinates rb_corner = eye_.GetEyeRBCorner();

    Coordinates hor_vec = (lt_corner - rt_corner) / 2;
    Coordinates vec = lt_corner - hor_vec;
    float hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    lt_corner.SetCoordinate(0, hor_vec[0] + vec[0] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    lt_corner.SetCoordinate(1, hor_vec[1] + vec[1] * kCosRotate + kSinRotate * hor_length);
    lt_corner.SetCoordinate(2, hor_vec[2] + vec[2] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    eye_.SetEyeLTCorner(Coordinates(lt_corner));

    vec = lb_corner - hor_vec;
    hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    lb_corner.SetCoordinate(0, hor_vec[0] + vec[0] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    lb_corner.SetCoordinate(1, hor_vec[1] + vec[1] * kCosRotate + kSinRotate * hor_length);
    lb_corner.SetCoordinate(2, hor_vec[2] + vec[2] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    eye_.SetEyeLBCorner(Coordinates(lb_corner));

    vec = rt_corner + hor_vec;
    hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    rt_corner.SetCoordinate(0, -hor_vec[0] + vec[0] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    rt_corner.SetCoordinate(1, -hor_vec[1] + vec[1] * kCosRotate + kSinRotate * hor_length);
    rt_corner.SetCoordinate(2, -hor_vec[2] + vec[2] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    eye_.SetEyeRTCorner(Coordinates(rt_corner));

    vec = rb_corner + hor_vec;
    hor_length = sqrt(vec[0] * vec[0] + vec[2] * vec[2]);
    rb_corner.SetCoordinate(0, -hor_vec[0] + vec[0] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    rb_corner.SetCoordinate(1, -hor_vec[1] + vec[1] * kCosRotate + kSinRotate * hor_length);
    rb_corner.SetCoordinate(2, -hor_vec[2] + vec[2] * (kCosRotate - vec[1] * kSinRotate / hor_length));
    eye_.SetEyeRBCorner(Coordinates(rb_corner));

    return true;
}
