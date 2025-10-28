#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "draw.hpp"
#include "scrollbar.hpp"
#include "button.hpp"
#include "object_buttons.hpp"

#include "scene_manager.hpp"

#include "circle.hpp"
#include "plane.hpp"
#include "triangle.hpp"
// #include "square.hpp"
// #include "pyramid.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    Circle circles_arr[2] = {
        // Circle(Coordinates(3, 100, -400, 100),       400, kSphere, graphics::kColorBlue, 0, 1),
        // Circle(Coordinates(3, 1000, 600, 100),   400, kSphere, graphics::Color(255, 0, 120), 0.5, 0.5),
        // Circle(Coordinates(3, 1000, -400, 100),      400, kSphere, graphics::kColorWhite, 0.5, 0.5),
        // Circle(Coordinates(3, 500, -100, 200),   200, kSphere, graphics::kColorRed, 0, 1),
        // Circle(Coordinates(3, 400, -200, -2100),   50, kSphere, graphics::kColorWhite, 0, 0.2, 2),
        // Circle(Coordinates(3, 500, -2000, -7000),  1000, kSphere, graphics::kColorRed, 0, 1),
        Circle(Coordinates(3, 500, 0, -7000),  1000, kSphere, graphics::kColorBlue, 0, 1),

        // Circle(Coordinates(3, 100, 450, 0),     50,  kLight,  Coordinates(3, 150, 150, 150)),

        // Circle(Coordinates(3, 600, -1000, 2000),  50,  kLight,  graphics::kColorWhite),
        Circle(Coordinates(3, 1080, -1000, -500), 50,  kLight,  graphics::kColorWhite),
    };

    Plane planes_arr[1] = {
        Plane(Coordinates(3, 500, 1000, 0), Coordinates(3, 0, -1, 0), graphics::kColorWhite, 0, 1),
        // Plane(Coordinates(3, 500, 0, 0), Coordinates(3, 0, -1, 0), graphics::kColorWhite, 0, 0.2, 2)
    };

    Triangle triangles_arr[1] = {
        Triangle(Plane(Coordinates(3, 500, 0, 500), Coordinates(3, 0, 1, 1), graphics::kColorYellow),
                 Coordinates(3, -400, 500, -500), Coordinates(3, -300, 0, 0)),
    };

    Triangle triangles_pyramid[6] {
        Triangle (Plane(Coordinates(3, 0, 0, 0), Coordinates(3, 0, 1, 0), graphics::kColorYellow),
                  Coordinates(3, 200, 0, 0), Coordinates(3, 200, 0, 200)),
        Triangle (Plane(Coordinates(3, 0, 0, 0), Coordinates(3, 0, 1, 0), graphics::kColorYellow),
                  Coordinates(3, 0, 0, 200), Coordinates(3, 200, 0, 200)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 0, -1, -5), graphics::kColorYellow),
                  Coordinates(3, -100, 500, -100), Coordinates(3, 100, 500, -100)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 5, -1, 0), graphics::kColorYellow),
                  Coordinates(3, 100, 500, -100), Coordinates(3, 100, 500, 100)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 0, -1, 5), graphics::kColorYellow),
                  Coordinates(3, 100, 500, 100), Coordinates(3, -100, 500, 100)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, -5, -1, 0), graphics::kColorYellow),
                  Coordinates(3, -100, 500, 100), Coordinates(3, -100, 500, -100)),
    };

    std::vector<Triangle> triangles_pyramid_vec;
    size_t triangles_pyramid_num = sizeof(triangles_pyramid) / sizeof(triangles_pyramid[0]);
    for (size_t i = 0; i < triangles_pyramid_num; i++) {
        triangles_pyramid_vec.push_back(triangles_pyramid[i]);
    }

    std::vector<Object*> objects;
    // objects.push_back(new TrianglesSet(Coordinates(3, 100, 0, 100), triangles_pyramid_vec,
    //                                    graphics::kColorWhite, 0, 1));

    size_t circles_num = sizeof(circles_arr) / sizeof(circles_arr[0]);
    for (size_t i = 0; i < circles_num; i++) {
        objects.push_back(new Circle(circles_arr[i]));
    }

    // size_t planes_num = sizeof(planes_arr) / sizeof(planes_arr[0]);
    // for (size_t i = 0; i < planes_num; i++) {
    //     objects.push_back(new Plane(planes_arr[i]));
    // }

    // size_t triangles_num = sizeof(triangles_arr) / sizeof(triangles_arr[0]);
    // for (size_t i = 0; i < triangles_num; i++) {
    //     objects.push_back(new Triangle(triangles_arr[i]));
    // }

    std::vector<Widget*> desktop_children;
    desktop_children.push_back(new SceneManager(Coordinates(2, 100, 100), 800, 400, objects));
    SceneManager* scene_manager = dynamic_cast<SceneManager*>(desktop_children.back());
    MoveButton buttons[6] = {
        MoveButton(Button(Coordinates(2, 5, 5), 50, 40, "X+", kFontFileNameScrollBar), Coordinates(3, 100, 0, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 65, 5), 50, 40, "X-", kFontFileNameScrollBar), Coordinates(3, -100, 0, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 50), 50, 40, "Y+", kFontFileNameScrollBar), Coordinates(3, 0, 100, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 65, 50), 50, 40, "Y-", kFontFileNameScrollBar), Coordinates(3, 0, -100, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 95), 50, 40, "Z+", kFontFileNameScrollBar), Coordinates(3, 0, 0, 100),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 65, 95), 50, 40, "Z-", kFontFileNameScrollBar), Coordinates(3, 0, 0, -100),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
    };

    std::vector<Widget*> buttons_on_panel;
    size_t buttons_num = sizeof(buttons) / sizeof(buttons[0]);
    for (size_t i = 0; i < buttons_num; i++) {
        buttons_on_panel.push_back(new MoveButton(buttons[i]));
    }
    buttons_on_panel.push_back(new ExistenceButton(Button(Coordinates(2, 5, 140), 110, 40, "Del",
                               kFontFileNameScrollBar),
                               [scene_manager](){scene_manager->DeleteCurrentObject();}));
    buttons_on_panel.push_back(new ExistenceButton(Button(Coordinates(2, 5, 185), 110, 40, "Add",
                               kFontFileNameScrollBar),
                               [scene_manager](){scene_manager->AddCopyCurrentObject();}));
    scene_manager->SetPanelControl(new PanelControl(Coordinates(2, 800, 0), 120, 230, &buttons_on_panel));

    UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics");

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
