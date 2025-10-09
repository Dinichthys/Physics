#include "scene_manager.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "graphics.hpp"
#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

void SceneManager::Draw(graphics::RenderWindow* window) {
    Coordinates eye_pos = eye_.GetEyePos();
    Coordinates lt_corner = eye_.GetEyeLTCorner();
    Coordinates lb_corner = eye_.GetEyeLBCorner();
    Coordinates rt_corner = eye_.GetEyeRTCorner();

    Coordinates hor_vec = !(rt_corner - lt_corner);
    Coordinates ver_vec = !(lb_corner - lt_corner);

    float width = Widget::GetWidth();
    float height = Widget::GetHeight();
    Coordinates abs_coors = Widget::GetLTCornerAbs();

    Coordinates pixel_pos(eye_pos + lt_corner);
    for (unsigned int i = 0; i < height; i++) {
        pixel_pos = eye_pos + lt_corner + ver_vec * i;
        for (unsigned int j = 0; j < width; j++) {
            pixel_pos = pixel_pos + hor_vec;

            float coeff = -1;
            size_t cur_circle_idx = -1;
            Circle circle = GetPointIntersectionWithCircle(pixel_pos, pixel_pos - eye_pos, coeff, cur_circle_idx);
            if (coeff < 0) {
                vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));
                vertices_.SetPixelColor(i * width + j, graphics::kColorBrown);
                continue;
            }
            if (circle.GetObjectType() == kLight) {
                vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));
                vertices_.SetPixelColor(i * width + j, graphics::Color(circle.GetBrightness()));
                continue;
            }

            Coordinates center(circle.GetCenterCoordinates());

            Coordinates point = pixel_pos + (pixel_pos - eye_pos) * coeff;

            vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));
            vertices_.SetPixelColor(i * width + j, GetPointColor(point, eye_pos, center, cur_circle_idx, circle.GetReflection()));
        }
    }

    window->Draw(vertices_);
}

Circle SceneManager::GetPointIntersectionWithCircle(const Coordinates& pixel_pos, const Coordinates& vec,
                                                    float& coeff, size_t& cur_circle_idx) {
    Circle circle(Coordinates(0, 0, 0, 0), 0, kSphere);
    size_t circles_num = circles_.size();
    size_t new_cur_circle_idx = -1;
    for (size_t circle_index = 0; circle_index < circles_num; circle_index++) {
        if (circle_index == cur_circle_idx) {
            continue;
        }
        float res_plus = 0;
        float res_minus = 0;
        if (!GetIntersectionResultQuadraticEquation(circles_[circle_index], pixel_pos, vec, res_plus, res_minus)) {
            continue;
        }
        if (((res_plus < coeff) || (coeff < 0)) && (res_plus > 0)) {
            coeff = res_plus;
            circle = *(circles_[circle_index]);
            new_cur_circle_idx = circle_index;
        }
        if (((res_minus < coeff) || (coeff < 0)) && (res_minus > 0)) {
            coeff = res_minus;
            circle = *(circles_[circle_index]);
            new_cur_circle_idx = circle_index;
        }
    }
    cur_circle_idx = new_cur_circle_idx;

    return circle;
}

graphics::Color SceneManager::GetPointColor(const Coordinates& point, const Coordinates& eye_pos,
                                            const Coordinates& center, size_t cur_circle_idx,
                                            float coeff_reflection) {
    graphics::Color color(GetLightEffect(point, eye_pos, center, cur_circle_idx));
    Coordinates new_point(point);
    Coordinates new_eye_pos(eye_pos);
    Coordinates new_center(center);
    size_t new_cur_circle_idx = cur_circle_idx;
    float new_coeff_reflection = coeff_reflection;
    for (size_t i = 0; i < kReflectionTimes; i++) {
        if (new_coeff_reflection < kEpsilon) {
            return color;
        }
        float coeff = -1;
        Coordinates radius_vec = !(new_point - new_center);
        Coordinates before_ref = !(new_point - new_eye_pos);
        Coordinates after_ref = before_ref - radius_vec * (before_ref && radius_vec) * 2;
        Circle circle = GetPointIntersectionWithCircle(new_point, after_ref, coeff, new_cur_circle_idx);

        if (coeff < 0) {
            break;
        }

        if (circle.GetObjectType() == kLight) {
            color = color + graphics::Color(circle.GetBrightness()) * coeff_reflection;
            break;
        }

        new_center = circle.GetCenterCoordinates();

        new_eye_pos = new_point;
        new_point = new_point + after_ref * coeff;
        color = color + GetLightEffect(new_point, new_eye_pos, new_center, new_cur_circle_idx) * new_coeff_reflection;
        new_coeff_reflection *= circle.GetReflection();
    }

    return color;
}

graphics::Color SceneManager::GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
                                             const Coordinates& center, size_t cur_circle_idx) {
    size_t circles_num = circles_.size();
    Coordinates color(kIBase);
    for (size_t light_index = 0; light_index < circles_num; light_index++) {
        if (circles_[light_index]->GetObjectType() != kLight) {
            continue;
        }
        Circle light(*(circles_[light_index]));
        Coordinates light_coordinates(light.GetCenterCoordinates());
        Coordinates brightness(light.GetBrightness());
        float mid_color = (brightness[0] + brightness[1] + brightness[2]) / 3;

        bool drawable = true;
        for (size_t circle_index = 0; circle_index < circles_num; circle_index++) {
            if ((circle_index == cur_circle_idx) || (circles_[circle_index]->GetObjectType() == kLight)) {
                continue;
            }
            float res_plus = 0;
            float res_minus = 0;
            if (!GetIntersectionResultQuadraticEquation(circles_[circle_index], light_coordinates, point - light_coordinates, res_plus, res_minus)) {
                continue;
            }
            if ((res_plus > kEpsilon) && (res_plus < 1)) {
                drawable = false;
                break;
            }
            if ((res_minus > kEpsilon) && (res_plus < 1)) {
                drawable = false;
                break;
            }
        }
        if (!drawable) {
            continue;
        }

        float cos_a = (!(point - center)) && (!(light_coordinates - point));

        if (cos_a > 0) {
            color = color + brightness * cos_a;

            float cos_b = ((!(point - light_coordinates))
                            + (!(point - center)) * ((!(point - center)) && (!(light_coordinates - point))) * 2)
                            && (!(eye_pos - point));

            if (cos_b > 0) {
                color = color + mid_color * powf32(cos_b, kPowCosB);
            }
        }

        for (size_t i = 0; i < 3; i++)
        {
            if (color[i] > kMaxColor) {
                color.SetCoordinate(i, kMaxColor);
            }
        }
    }

    return graphics::Color(color[0], color[1], color[2]);
}

bool SceneManager::GetIntersectionResultQuadraticEquation(const Circle* circle,
                                                          const Coordinates& pixel_pos,
                                                          const Coordinates& vec,
                                                          float& res_plus, float& res_minus) {
    Coordinates center(circle->GetCenterCoordinates());
    float radius = circle->GetRadius();
    float distance = ((center - pixel_pos) || vec).GetModule()
                        / vec.GetModule();
    if (distance > radius) {
        return false;
    }
    float a = vec.SqLength();
    float b = 2 * (vec && (pixel_pos - center));
    float c = (pixel_pos - center).SqLength() - radius * radius;
    float discrim = b * b - 4 * a * c;
    if (discrim < 0) {
        return false;
    }
    discrim = sqrt(discrim);
    res_minus = (-b - discrim) / (2 * a);
    res_plus = (-b + discrim) / (2 * a);

    return true;
}
