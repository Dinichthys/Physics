#include "scene_manager.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "graphics.hpp"
#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

void SceneManager::Draw(graphics::RenderWindow* window) {
    size_t circles_num = circles_.size();

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
            Circle circle(Coordinates(0, 0, 0, 0), 0, kSphere);
            size_t cur_circle_idx = -1;
            for (size_t circle_index = 0; circle_index < circles_num; circle_index++) {
                Coordinates center(circles_[circle_index]->GetCenterCoordinates());
                float radius = circles_[circle_index]->GetRadius();
                float distance = ((center - pixel_pos) || (pixel_pos - eye_pos)).GetModule()
                                    / (pixel_pos - eye_pos).GetModule();
                if (distance > radius) {
                    continue;
                }
                float a = (pixel_pos - eye_pos).SqLength();
                float b = 2 * ((pixel_pos - eye_pos) && (pixel_pos - center));
                float c = (pixel_pos - center).SqLength() - radius * radius;
                float discrim = b * b - 4 * a * c;
                if (discrim < 0) {
                    continue;
                }
                discrim = sqrt(discrim);
                float res_minus = (-b - discrim) / (2 * a);
                float res_plus = (-b + discrim) / (2 * a);

                if (((res_plus < coeff) || (coeff < 0)) && (res_plus > 0)) {
                    coeff = res_plus;
                    circle = *(circles_[circle_index]);
                    cur_circle_idx = circle_index;
                }
                if (((res_minus < coeff) || (coeff < 0)) && (res_minus > 0)) {
                    coeff = res_minus;
                    circle = *(circles_[circle_index]);
                    cur_circle_idx = circle_index;
                }
            }
            if (coeff < 0) {
                vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));
                vertices_.SetPixelColor(i * width + j, graphics::kColorBrown);
                continue;
            }
            if (circle.GetObjectType() == kLight) {
                vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));
                vertices_.SetPixelColor(i * width + j, graphics::Color(kMaxColor, kMaxColor, kMaxColor));
                continue;
            }

            Coordinates center(circle.GetCenterCoordinates());

            Coordinates point = pixel_pos + (pixel_pos - eye_pos) * coeff;

            Coordinates color(kIBase);
            for (size_t light_index = 0; light_index < circles_num; light_index++) {
                if (circles_[light_index]->GetObjectType() != kLight) {
                    continue;
                }
                Circle light(*(circles_[light_index]));
                Coordinates light_coordinates(light.GetCenterCoordinates());
                Coordinates brightness(light.GetBrightness());

                bool drawable = true;
                for (size_t circle_index = 0; circle_index < circles_num; circle_index++) {
                    if ((circle_index == cur_circle_idx) || (circles_[circle_index]->GetObjectType() == kLight)) {
                        continue;
                    }
                    Coordinates checking_center(circles_[circle_index]->GetCenterCoordinates());
                    float distance = ((checking_center - point) || (point - light_coordinates)).GetModule()
                                     / (point - light_coordinates).GetModule();
                    float radius = circles_[circle_index]->GetRadius();
                    if (distance > radius) {
                        continue;
                    }

                    float a = (point - light_coordinates).SqLength();
                    float b = 2 * ((point - light_coordinates) && (point - checking_center));
                    float c = (point - checking_center).SqLength() - radius * radius;
                    float discrim = b * b - 4 * a * c;
                    if (discrim < 0) {
                        continue;
                    }
                    discrim = sqrt(discrim);
                    float res_minus = (-b - discrim) / (2 * a);
                    float res_plus = (-b + discrim) / (2 * a);

                    if (res_plus < kEpsilon) {
                        drawable = false;
                        break;
                    }
                    if (res_minus < kEpsilon) {
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
                        color = color + kMaxColor * powf32(cos_b, kPowCosB);
                    }
                }

                for (size_t i = 0; i < 3; i++)
                {
                    if (color[i] > kMaxColor) {
                        color.SetCoordinate(i, kMaxColor);
                    }
                }
            }

            vertices_.SetPixelPosition(i * width + j, Coordinates(2, abs_coors[0] + (float)j, abs_coors[1] + (float)i));
            vertices_.SetPixelColor(i * width + j, graphics::Color((uint8_t)color[0], (uint8_t)color[1], (uint8_t)color[2]));
        }
    }

    window->Draw(vertices_);
}
