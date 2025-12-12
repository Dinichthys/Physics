#include "scene_manager.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stack>
#include <thread>

#include "colors.hpp"
#include "object.hpp"
#include "circle.hpp"

#include "logging.h"
#include "my_assert.h"

void SceneManager::Redraw() {
    if (dorisovka_ != NULL) {
        texture->Draw(*image_);
        title_->Redraw();
        dorisovka_->Redraw();
        Widget::Redraw();
        return;
    }

    if (!changed_) {
        texture->Draw(*image_);
        title_->Redraw();
        Widget::Redraw();
        return;
    }

    size_t width = Widget::GetWidth();
    size_t height = Widget::GetHeight();

    tasks_.push(DrawTask(        0,          0, width / 4, height / 4,                      0 ));
    tasks_.push(DrawTask(        0, height / 4, width / 4, height / 2,     width * height / 16));
    tasks_.push(DrawTask(width / 4,          0, width / 2, height / 4,     width * height / 8 ));
    tasks_.push(DrawTask(width / 4, height / 4, width / 2, height / 2, 3 * width * height / 16));

    tasks_.push(DrawTask(        0,     height / 2,    width / 4, 3 * height / 4,     width * height / 4 ));
    tasks_.push(DrawTask(width / 4,     height / 2,    width / 2, 3 * height / 4, 5 * width * height / 16));
    tasks_.push(DrawTask(        0, 3 * height / 4,    width / 4,     height,     3 * width * height / 8 ));
    tasks_.push(DrawTask(width / 4, 3 * height / 4,    width / 2,     height,     7 * width * height / 16));

    tasks_.push(DrawTask(    width / 2,          0, 3 * width / 4, height / 4,      width * height / 2 ));
    tasks_.push(DrawTask(    width / 2, height / 4, 3 * width / 4, height / 2,  9 * width * height / 16));
    tasks_.push(DrawTask(3 * width / 4,          0,     width,     height / 4,  5 * width * height / 8 ));
    tasks_.push(DrawTask(3 * width / 4, height / 4,     width,     height / 2, 11 * width * height / 16));

    tasks_.push(DrawTask(    width / 2,     height / 2, 3 * width / 4, 3 * height / 4,  3 * width * height / 4 ));
    tasks_.push(DrawTask(3 * width / 4,     height / 2,     width,     3 * height / 4, 13 * width * height / 16));
    tasks_.push(DrawTask(    width / 2, 3 * height / 4, 3 * width / 4,     height,      7 * width * height / 8 ));
    tasks_.push(DrawTask(3 * width / 4, 3 * height / 4,     width,         height,     15 * width * height / 16));

    std::thread t1([this](){this->DrawPart();});
    std::thread t2([this](){this->DrawPart();});
    std::thread t3([this](){this->DrawPart();});
    std::thread t4([this](){this->DrawPart();});
    std::thread t5([this](){this->DrawPart();});
    std::thread t6([this](){this->DrawPart();});
    std::thread t7([this](){this->DrawPart();});
    std::thread t8([this](){this->DrawPart();});
    std::thread t9([this](){this->DrawPart();});
    std::thread t10([this](){this->DrawPart();});
    std::thread t11([this](){this->DrawPart();});
    std::thread t12([this](){this->DrawPart();});
    std::thread t13([this](){this->DrawPart();});
    std::thread t14([this](){this->DrawPart();});
    std::thread t15([this](){this->DrawPart();});
    std::thread t16([this](){this->DrawPart();});

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    t11.join();
    t12.join();
    t13.join();
    t14.join();
    t15.join();
    t16.join();

    texture->Draw(*image_);
    title_->Redraw();
    Widget::Redraw();
    changed_ = false;
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

    DrawTask task = tasks_.GetElem();

    Coordinates pixel_pos(eye_pos + lt_corner);
    float height = task.GetHeight();
    float width = task.GetWidth();
    for (size_t i = task.GetStartPixelY(); i < height; i++) {
        pixel_pos = eye_pos + lt_corner + ver_vec * i + hor_vec * task.GetStartPixelX();
        for (size_t j = task.GetStartPixelX(); j < width; j++) {
            pixel_pos = pixel_pos + hor_vec;
            dr4::Vec2f pos((float)j, (float)i);

            float coeff = -1;
            size_t cur_object_idx = -1;
            Object* object = GetPointIntersection(
                pixel_pos, pixel_pos - eye_pos, coeff, cur_object_idx
            );
            if (coeff < 0) {
                image_->SetPixel(pos.x, pos.y, kFreeSpaceColor);
                continue;
            }
            if (object->GetType() == kLight) {
                image_->SetPixel(pos.x, pos.y, colors::Color(object->GetColor()));
                continue;
            }

            Coordinates point = pixel_pos + (pixel_pos - eye_pos) * coeff;

            image_->SetPixel(pos.x, pos.y,
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

colors::Color SceneManager::GetPointColor(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_object_idx, size_t depth_counting) {
    return GetLightEffect(point, eye_pos, cur_object_idx)
           + GetReflectionEffect(point, eye_pos, cur_object_idx, depth_counting)
           + GetRefractionEffect(point, eye_pos, cur_object_idx, depth_counting);
}

colors::Color SceneManager::GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
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
    return colors::Color(color * coeff_absorption);
}

colors::Color SceneManager::GetReflectionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                                  size_t cur_object_idx, size_t depth_counting) {
    if (depth_counting == 0) {
        return colors::Color();
    }

    float coeff_reflection = objects_[cur_object_idx]->GetCoeffReflection();
    if (coeff_reflection < kEpsilon) {
        return colors::Color();
    }

    Coordinates new_point(point);
    Coordinates new_eye_pos(eye_pos);
    Coordinates base(objects_[cur_object_idx]->GetColor());

    float coeff = -1;
    Coordinates after_ref = objects_[cur_object_idx]->GetReflectionVec(point, point - eye_pos);
    Object* object = GetPointIntersection(new_point, after_ref, coeff, cur_object_idx);

    if (coeff < 0) {
        colors::Color color = kFreeSpaceColor;
        Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
        return colors::Color(base * color_coors / kMaxColor * coeff_reflection);
    }

    if (object->GetType() == kLight) {
        return colors::Color(base * object->GetColor() / kMaxColor * coeff_reflection);
    }

    new_eye_pos = new_point;
    new_point = new_point + after_ref * coeff;

    colors::Color color = GetPointColor(
                                new_point, new_eye_pos, cur_object_idx, depth_counting - 1
                            );
    Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
    return colors::Color(base * color_coors / kMaxColor * coeff_reflection);
}

colors::Color SceneManager::GetRefractionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                                  size_t cur_object_idx, size_t depth_counting) {
    static thread_local std::stack<float> refractions;

    if (depth_counting == 0) {
        while (!refractions.empty()) {
            refractions.pop();
        }
        return colors::Color();
    }

    float color_scale = 1 - (objects_[cur_object_idx]->GetCoeffAbsorption()
                             + objects_[cur_object_idx]->GetCoeffReflection());
    if (color_scale < kEpsilon) {
        while (!refractions.empty()) {
            refractions.pop();
        }
        return colors::Color();
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
        return colors::Color();
    }

    if (enter) {
        refractions.push(objects_[cur_object_idx]->GetCoeffRefraction());
    } else {
        if (refractions.size() == 0) {
            while (!refractions.empty()) {
                refractions.pop();
            }
            return colors::Color();
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
        colors::Color color = kFreeSpaceColor;
        Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
        return colors::Color(base * color_coors / kMaxColor * color_scale);
    }

    if (object->GetType() == kLight) {
        while (!refractions.empty()) {
            refractions.pop();
        }
        return colors::Color(base * object->GetColor() / kMaxColor * color_scale);
    }

    new_eye_pos = new_point;
    new_point = new_point + after_ref * coeff;

    colors::Color color = GetPointColor(
                                new_point, new_eye_pos, cur_object_idx, depth_counting - 1
                            );
    while (!refractions.empty()) {
        refractions.pop();
    }
    Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
    return colors::Color(base * color_coors / kMaxColor * color_scale);
}
