#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "draw.hpp"
#include "scrollbar.hpp"

#include "scene_manager.hpp"
#include "circle.hpp"
#include "plane.hpp"
#include "triangle.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    Circle circles_arr[9] = {
        Circle(Coordinates(3, 100, -400, 100),       400, kSphere, graphics::kColorBlue, 0, 1),
        Circle(Coordinates(3, 1000, 600, 100),   400, kSphere, graphics::Color(255, 0, 120), 0.5, 0.5),
        // Circle(Coordinates(3, 500, 500, 300),   100, kSphere, kIBase, 1, 0),
        Circle(Coordinates(3, 1000, -400, 100),      400, kSphere, graphics::kColorWhite, 0.5, 0.5),
        Circle(Coordinates(3, 500, -100, 200),   200, kSphere, graphics::kColorRed, 0, 1),
        Circle(Coordinates(3, 400, -200, -2100),   50, kSphere, graphics::kColorWhite, 0, 0.2, 2),
        Circle(Coordinates(3, 500, -2000, -7000),  1000, kSphere, graphics::kColorRed, 0, 1),
        Circle(Coordinates(3, 500, 0, -7000),  1000, kSphere, graphics::kColorBlue, 0, 1),

        // Circle(Coordinates(3, 100, 450, 0),     50,  kLight,  Coordinates(3, 150, 150, 150)),
        Circle(Coordinates(3, 600, -1000, 2000),  50,  kLight,  graphics::kColorWhite),
        Circle(Coordinates(3, 1080, -1000, -500), 50,  kLight,  graphics::kColorWhite),
    };

    Plane planes_arr[1] = {
        Plane(Coordinates(3, 500, 1000, 0), Coordinates(3, 0, -1, 0), graphics::kColorWhite, 0, 1),
        // Plane(Coordinates(3, 500, 0, 0), Coordinates(3, 0, -1, 0), graphics::kColorWhite, 0, 0.2, 2)
    };

    Triangle triangles_arr[1] = {
        Triangle(Plane(Coordinates(3, 500, 0, 500), Coordinates(3, 0, 1, 1), graphics::kColorYellow),
                 Coordinates(3, 0, 500, 0), Coordinates(3, 100, 500, 0), Coordinates(3, 200, 0, 500)),
    };

    std::vector<Object*> objects;

    size_t circles_num = sizeof(circles_arr) / sizeof(circles_arr[0]);
    for (size_t i = 0; i < circles_num; i++) {
        objects.push_back(new Circle(circles_arr[i]));
    }

    size_t planes_num = sizeof(planes_arr) / sizeof(planes_arr[0]);
    for (size_t i = 0; i < planes_num; i++) {
        objects.push_back(new Plane(planes_arr[i]));
    }

    size_t triangles_num = sizeof(triangles_arr) / sizeof(triangles_arr[0]);
    for (size_t i = 0; i < triangles_num; i++) {
        objects.push_back(new Triangle(triangles_arr[i]));
    }

    std::vector<Widget*> desktop_children;
    desktop_children.push_back(new SceneManager(Coordinates(3, 100, 100), 800, 400, objects));
    UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics");

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
