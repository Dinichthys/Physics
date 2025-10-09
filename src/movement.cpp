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
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();
    Coordinates rb_corner = eye_.GetEyeRBCorner();

    Coordinates ver_vec = (lt_corner - lb_corner) / 2;
    Coordinates vec = lt_corner - ver_vec;
    lt_corner.SetCoordinate(0, lt_corner[0] * kCosRotate + lt_corner[2] * kSinRotate);
    lt_corner.SetCoordinate(2, lt_corner[2] * kCosRotate - lt_corner[0] * kSinRotate);
    eye_.SetEyeLTCorner(Coordinates(lt_corner));

    vec = lb_corner - ver_vec;
    lb_corner.SetCoordinate(0, lb_corner[0] * kCosRotate + lb_corner[2] * kSinRotate);
    lb_corner.SetCoordinate(2, lb_corner[2] * kCosRotate - lb_corner[0] * kSinRotate);
    eye_.SetEyeLBCorner(Coordinates(lb_corner));

    vec = rt_corner + ver_vec;
    rt_corner.SetCoordinate(0, rt_corner[0] * kCosRotate + rt_corner[2] * kSinRotate);
    rt_corner.SetCoordinate(2, rt_corner[2] * kCosRotate - rt_corner[0] * kSinRotate);
    eye_.SetEyeRTCorner(Coordinates(rt_corner));

    vec = rb_corner + ver_vec;
    rb_corner.SetCoordinate(0, rb_corner[0] * kCosRotate + rb_corner[2] * kSinRotate);
    rb_corner.SetCoordinate(2, rb_corner[2] * kCosRotate - rb_corner[0] * kSinRotate);
    eye_.SetEyeRBCorner(Coordinates(rb_corner));

    return true;
}

bool SceneManager::OnArrowLeft() {
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();
    Coordinates rb_corner = eye_.GetEyeRBCorner();

    Coordinates ver_vec = (lt_corner - lb_corner) / 2;
    Coordinates vec = lt_corner - ver_vec;
    lt_corner.SetCoordinate(0, lt_corner[0] * kCosRotate - lt_corner[2] * kSinRotate);
    lt_corner.SetCoordinate(2, lt_corner[2] * kCosRotate + lt_corner[0] * kSinRotate);
    eye_.SetEyeLTCorner(Coordinates(lt_corner));

    vec = lb_corner - ver_vec;
    lb_corner.SetCoordinate(0, lb_corner[0] * kCosRotate - lb_corner[2] * kSinRotate);
    lb_corner.SetCoordinate(2, lb_corner[2] * kCosRotate + lb_corner[0] * kSinRotate);
    eye_.SetEyeLBCorner(Coordinates(lb_corner));

    vec = rt_corner + ver_vec;
    rt_corner.SetCoordinate(0, rt_corner[0] * kCosRotate - rt_corner[2] * kSinRotate);
    rt_corner.SetCoordinate(2, rt_corner[2] * kCosRotate + rt_corner[0] * kSinRotate);
    eye_.SetEyeRTCorner(Coordinates(rt_corner));

    vec = rb_corner + ver_vec;
    rb_corner.SetCoordinate(0, rb_corner[0] * kCosRotate - rb_corner[2] * kSinRotate);
    rb_corner.SetCoordinate(2, rb_corner[2] * kCosRotate + rb_corner[0] * kSinRotate);
    eye_.SetEyeRBCorner(Coordinates(rb_corner));

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
