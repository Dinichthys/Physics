#include "scene_manager.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stack>
#include <thread>

#include "graphics.hpp"
#include "object.hpp"
#include "circle.hpp"

#include "logging.h"
#include "my_assert.h"

void SceneManager::Draw(graphics::RenderWindow* window) {
    if (table_ != NULL) {
        table_->Draw(window);
        WidgetContainer::Draw(window);
    }

    size_t width = Widget::GetWidth();
    size_t height = Widget::GetHeight();

    tasks_.push(DrawTask(        0,          0, width / 2, height / 2,                      0));
    tasks_.push(DrawTask(        0, height / 2, width / 2, height    ,     width * height / 4));
    tasks_.push(DrawTask(width / 2,          0, width,     height / 2,     width * height / 2));
    tasks_.push(DrawTask(width / 2, height / 2, width,     height    , 3 * width * height / 4));

    std::thread t1([this](){this->DrawPart();});
    std::thread t2([this](){this->DrawPart();});
    std::thread t3([this](){this->DrawPart();});
    std::thread t4([this](){this->DrawPart();});

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    window->Draw(vertices_);
}

void SceneManager::DrawPart() {
    if (tasks_.empty()) {
        return;
    }

    Coordinates eye_pos = eye_.GetEyePos();
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();

    Coordinates hor_vec = !(rt_corner - lt_corner);
    Coordinates ver_vec = !(lb_corner - lt_corner);

    Coordinates abs_coors = Widget::GetLTCornerAbs();

    DrawTask task = tasks_.GetElem();

    Coordinates pixel_pos(eye_pos + lt_corner);
    float height = task.GetHeight();
    float width = task.GetWidth();
    size_t point_idx = task.GetStartPointIdx();
    for (size_t i = task.GetStartPixelY(); i < height; i++) {
        pixel_pos = eye_pos + lt_corner + ver_vec * i + hor_vec * task.GetStartPixelX();
        for (size_t j = task.GetStartPixelX(); j < width; j++) {
            pixel_pos = pixel_pos + hor_vec;
            vertices_.SetPixelPosition(point_idx,
                                       Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));

            float coeff = -1;
            size_t cur_object_idx = -1;
            Object* object = GetPointIntersection(
                pixel_pos, pixel_pos - eye_pos, coeff, cur_object_idx
            );
            if (coeff < 0) {
                vertices_.SetPixelColor(point_idx++, kFreeSpaceColor);
                continue;
            }
            if (object->GetType() == kLight) {
                vertices_.SetPixelColor(point_idx++, graphics::Color(object->GetColor()));
                continue;
            }

            Coordinates point = pixel_pos + (pixel_pos - eye_pos) * coeff;

            vertices_.SetPixelColor(point_idx++,
                                    GetPointColor(
                                        point, eye_pos, cur_object_idx, kColorCountingDepth
                                    ));
        }
    }
}

Object* SceneManager::GetPointIntersection(const Coordinates& pixel_pos, const Coordinates& vec,
                                          float& coeff, size_t& cur_object_idx) {
    Object* object = NULL;
    size_t objects_num = objects_.size();
    size_t new_cur_object_idx = -1;
    for (size_t object_index = 0; object_index < objects_num; object_index++) {
        if (object_index == cur_object_idx) {
            continue;
        }
        if (objects_[object_index]->GetIntersection(pixel_pos, vec, coeff)) {
            object = objects_[object_index];
            new_cur_object_idx = object_index;
        }
    }
    cur_object_idx = new_cur_object_idx;

    return object;
}

graphics::Color SceneManager::GetPointColor(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_object_idx, size_t depth_counting) {
    return GetLightEffect(point, eye_pos, cur_object_idx)
           + GetReflectionEffect(point, eye_pos, cur_object_idx, depth_counting)
           + GetRefractionEffect(point, eye_pos, cur_object_idx, depth_counting);
}

graphics::Color SceneManager::GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
                                             size_t cur_object_idx) {
    size_t objects_num = objects_.size();
    Coordinates base(objects_[cur_object_idx]->GetColor());
    Coordinates color(base * (kIBase / kMaxColor));
    for (size_t light_index = 0; light_index < objects_num; light_index++) {
        if (objects_[light_index]->GetType() != kLight) {
            continue;
        }
        Circle light = *(dynamic_cast<Circle*>(objects_[light_index]));
        Coordinates light_coordinates(light.GetCenterCoordinates());
        Coordinates brightness(base * light.GetColor() / kMaxColor);

        bool drawable = true;
        float coeff = 1;
        for (size_t object_index = 0; object_index < objects_num; object_index++) {
            if ((object_index == cur_object_idx) || (objects_[object_index]->GetType() == kLight)) {
                continue;
            }
            if (objects_[object_index]->GetIntersection(light_coordinates, point - light_coordinates, coeff)) {
                drawable = false;
                break;
            }
        }
        if (!drawable) {
            continue;
        }

        Coordinates normal(!(objects_[cur_object_idx]->GetNormal(point, point - eye_pos)));
        float cos_a = (normal) && (!(light_coordinates - point));

        if (cos_a > 0) {
            color = color + brightness * cos_a;

            float cos_b = ((!(point - light_coordinates))
                            + normal * (normal && (!(light_coordinates - point))) * 2)
                            && (!(eye_pos - point));

            if (cos_b > 0) {
                color = color + brightness * powf32(cos_b, kPowCosB);
            }
        }

        for (size_t i = 0; i < 3; i++)
        {
            if (color[i] > kMaxColor) {
                color.SetCoordinate(i, kMaxColor);
            }
        }
    }

    float coeff_absorption = objects_[cur_object_idx]->GetCoeffAbsorption();
    return graphics::Color(color * coeff_absorption);
}

graphics::Color SceneManager::GetReflectionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                                  size_t cur_object_idx, size_t depth_counting) {
    if (depth_counting == 0) {
        return graphics::Color();
    }

    float coeff_reflection = objects_[cur_object_idx]->GetCoeffReflection();
    if (coeff_reflection < kEpsilon) {
        return graphics::Color();
    }

    Coordinates new_point(point);
    Coordinates new_eye_pos(eye_pos);
    Coordinates base(objects_[cur_object_idx]->GetColor());

    float coeff = -1;
    Coordinates after_ref = objects_[cur_object_idx]->GetReflectionVec(point, point - eye_pos);
    Object* object = GetPointIntersection(new_point, after_ref, coeff, cur_object_idx);

    if (coeff < 0) {
        graphics::Color color = kFreeSpaceColor;
        Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
        return graphics::Color(base * color_coors / kMaxColor * coeff_reflection);
    }

    if (object->GetType() == kLight) {
        return graphics::Color(base * object->GetColor() / kMaxColor * coeff_reflection);
    }

    new_eye_pos = new_point;
    new_point = new_point + after_ref * coeff;

    graphics::Color color = GetPointColor(
                                new_point, new_eye_pos, cur_object_idx, depth_counting - 1
                            );
    Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
    return graphics::Color(base * color_coors / kMaxColor * coeff_reflection);
}

graphics::Color SceneManager::GetRefractionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                                  size_t cur_object_idx, size_t depth_counting) {
    static thread_local std::stack<float> refractions;

    if (depth_counting == 0) {
        while (!refractions.empty()) {
            refractions.pop();
        }
        return graphics::Color();
    }

    float color_scale = 1 - (objects_[cur_object_idx]->GetCoeffAbsorption()
                             + objects_[cur_object_idx]->GetCoeffReflection());
    if (color_scale < kEpsilon) {
        while (!refractions.empty()) {
            refractions.pop();
        }
        return graphics::Color();
    }

    Coordinates new_point(point);
    Coordinates new_eye_pos(eye_pos);
    Coordinates base(objects_[cur_object_idx]->GetColor());
    float old_coeff_refraction = (refractions.empty()) ? kAirCoeffRefraction
                                                       : refractions.top();

    bool enter = false;
    bool stop = false;
    Coordinates after_ref = objects_[cur_object_idx]->GetRefractionVec(
                                                        point, point - eye_pos, old_coeff_refraction, enter, stop
                                                      );

    if (stop) {
        return graphics::Color();
    }

    if (enter) {
        refractions.push(objects_[cur_object_idx]->GetCoeffRefraction());
    } else {
        if (refractions.size() == 0) {
            while (!refractions.empty()) {
                refractions.pop();
            }
            return graphics::Color();
        } else {
            refractions.pop();
        }
    }

    cur_object_idx = -1;
    float coeff = -1;
    Object* object = GetPointIntersection(new_point, after_ref, coeff, cur_object_idx);

    if (coeff < 0) {
        while (refractions.size() != 0) {
            refractions.pop();
        }
        graphics::Color color = kFreeSpaceColor;
        Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
        return graphics::Color(base * color_coors / kMaxColor * color_scale);
    }

    if (object->GetType() == kLight) {
        while (!refractions.empty()) {
            refractions.pop();
        }
        return graphics::Color(base * object->GetColor() / kMaxColor * color_scale);
    }

    new_eye_pos = new_point;
    new_point = new_point + after_ref * coeff;

    graphics::Color color = GetPointColor(
                                new_point, new_eye_pos, cur_object_idx, depth_counting - 1
                            );
    while (!refractions.empty()) {
        refractions.pop();
    }
    Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
    return graphics::Color(base * color_coors / kMaxColor * color_scale);
}
