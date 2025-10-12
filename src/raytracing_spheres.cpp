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
            vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));

            float coeff = -1;
            size_t cur_circle_idx = -1;
            Circle circle = GetPointIntersectionWithCircle(pixel_pos, pixel_pos - eye_pos, coeff, cur_circle_idx);
            if (coeff < 0) {
                vertices_.SetPixelColor(i * width + j, kFreeSpaceColor);
                continue;
            }
            if (circle.GetType() == kLight) {
                vertices_.SetPixelColor(i * width + j, graphics::Color(circle.GetColor()));
                continue;
            }

            Coordinates center(circle.GetCenterCoordinates());

            Coordinates point = pixel_pos + (pixel_pos - eye_pos) * coeff;

            vertices_.SetPixelColor(i * width + j, GetPointColor(point, eye_pos, cur_circle_idx, kColorCountingDepth));
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
                                            size_t cur_circle_idx, size_t depth_counting) {
    return GetLightEffect(point, eye_pos, cur_circle_idx) +
           GetReflectionEffect(point, eye_pos, cur_circle_idx, depth_counting) +
           GetRefractionEffect(point, eye_pos, cur_circle_idx, depth_counting);
}

graphics::Color SceneManager::GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
                                             size_t cur_circle_idx) {
    size_t circles_num = circles_.size();
    Coordinates center(circles_[cur_circle_idx]->GetCenterCoordinates());
    Coordinates base(circles_[cur_circle_idx]->GetColor());
    Coordinates color(graphics::kColorBlack);
    for (size_t light_index = 0; light_index < circles_num; light_index++) {
        if (circles_[light_index]->GetType() != kLight) {
            continue;
        }
        Circle light(*(circles_[light_index]));
        Coordinates light_coordinates(light.GetCenterCoordinates());
        Coordinates brightness(base * light.GetColor() / kMaxColor);

        bool drawable = true;
        for (size_t circle_index = 0; circle_index < circles_num; circle_index++) {
            if ((circle_index == cur_circle_idx) || (circles_[circle_index]->GetType() == kLight)) {
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

    float coeff_absorption = circles_[cur_circle_idx]->GetCoeffAbsorption();
    return graphics::Color(color * coeff_absorption);
}

graphics::Color SceneManager::GetReflectionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                                  size_t cur_circle_idx, size_t depth_counting) {
    if (depth_counting == 0) {
        return graphics::Color();
    }

    float coeff_reflection = circles_[cur_circle_idx]->GetCoeffReflection();
    if (coeff_reflection < kEpsilon) {
        return graphics::Color();
    }

    Coordinates new_point(point);
    Coordinates new_eye_pos(eye_pos);
    Coordinates base(circles_[cur_circle_idx]->GetColor());

    float coeff = -1;
    Coordinates radius_vec = !(new_point - circles_[cur_circle_idx]->GetCenterCoordinates());
    Coordinates before_ref = !(new_point - new_eye_pos);
    Coordinates after_ref = before_ref - radius_vec * (before_ref && radius_vec) * 2;
    Circle circle = GetPointIntersectionWithCircle(new_point, after_ref, coeff, cur_circle_idx);

    if (coeff < 0) {
        graphics::Color color = kFreeSpaceColor;
        Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
        return graphics::Color(base * color_coors / kMaxColor * coeff_reflection);
    }

    if (circle.GetType() == kLight) {
        return graphics::Color(base * circle.GetColor() / kMaxColor * coeff_reflection);
    }

    new_eye_pos = new_point;
    new_point = new_point + after_ref * coeff;

    graphics::Color color = GetPointColor(new_point, new_eye_pos, cur_circle_idx, depth_counting - 1) * coeff_reflection;
    Coordinates color_coors(3, color.GetRedPart(), color.GetGreenPart(), color.GetBluePart());
    return graphics::Color(base * color_coors / kMaxColor * coeff_reflection);
}

graphics::Color SceneManager::GetRefractionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                                  size_t cur_circle_idx, size_t depth_counting) {

    return graphics::Color();

    if (depth_counting == 0) {
        return graphics::Color();
    }

    float color_scale = 1 - (circles_[cur_circle_idx]->GetCoeffAbsorption()
                             + circles_[cur_circle_idx]->GetCoeffReflection());
    if (color_scale < kEpsilon) {
        return graphics::Color();
    }

    Coordinates new_point(point);
    Coordinates new_eye_pos(eye_pos);
    float new_coeff_refraction = circles_[cur_circle_idx]->GetCoeffReflection();

    float coeff = -1;
    Coordinates radius_vec = !(new_point - circles_[cur_circle_idx]->GetCenterCoordinates());
    Coordinates before_ref = !(new_point - new_eye_pos);
    Coordinates after_ref = before_ref - radius_vec * (before_ref && radius_vec) * 2;
    Circle circle = GetPointIntersectionWithCircle(new_point, after_ref, coeff, cur_circle_idx);
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
