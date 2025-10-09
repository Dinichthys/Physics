#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "draw.hpp"
#include "scrollbar.hpp"

#include "scene_manager.hpp"
#include "object.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    Circle circles_arr[6] = {
        Circle(Coordinates(3, 100, 0, 0),       400, kSphere, Coordinates(3), 0),
        Circle(Coordinates(3, 1000, 1000, 0),   400, kSphere, Coordinates(3), 1),
        // Circle(Coordinates(3, 500, 500, 300),   100, kSphere, Coordinates(3), 1),
        Circle(Coordinates(3, 1000, 0, 0),      400, kSphere, Coordinates(3), 0.5),
        Circle(Coordinates(3, 500, 300, 100),   200, kSphere, Coordinates(3), 0),

        // Circle(Coordinates(3, 100, 450, 0),     50,  kLight,  Coordinates(3, 150, 150, 150)),
        Circle(Coordinates(3, 600, 600, 1000),  50,  kLight,  Coordinates(3, 255, 255, 255)),
        Circle(Coordinates(3, 1080, 720, -500), 50,  kLight,  Coordinates(3, 255, 255, 255)),
    };

    std::vector<Circle*> circles;
    size_t circles_num = sizeof(circles_arr) / sizeof(circles_arr[0]);
    for (size_t i = 0; i < circles_num; i++) {
        circles.push_back(&(circles_arr[i]));
    }

    std::vector<Widget*> desktop_children;
    desktop_children.push_back(new SceneManager(Coordinates(3, 100, 100), 800, 400, circles));
    UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics");

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
