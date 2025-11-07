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
#include "list_objects.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    Circle circles_arr[8] = {
        // Circle(Coordinates(3, 100, -400, 100),       400, kSphere, colors::kColorBlue, 0, 1),
        Circle(Coordinates(3, 1000, 600, 100),   400, kSphere, colors::Color(255, 0, 120), 0.5, 0.5),
        Circle(Coordinates(3, 1000, -400, 100),      400, kSphere, colors::kColorWhite, 0.5, 0.5),
        Circle(Coordinates(3, 500, -100, 200),   200, kSphere, colors::kColorRed, 0, 1),
        Circle(Coordinates(3, 400, -200, -2100),   50, kSphere, colors::kColorWhite, 0, 0.2, 2),
        Circle(Coordinates(3, 500, -2000, -7000),  1000, kSphere, colors::kColorRed, 0, 1),
        Circle(Coordinates(3, 500, 0, -7000),  1000, kSphere, colors::kColorBlue, 0, 1),

        // Circle(Coordinates(3, 100, 450, 0),     50,  kLight,  Coordinates(3, 150, 150, 150)),

        Circle(Coordinates(3, 600, -1000, 2000),  50,  kLight,  colors::kColorWhite),
        Circle(Coordinates(3, 1080, -1000, -500), 50,  kLight,  colors::kColorWhite),
    };

    Plane planes_arr[1] = {
        Plane(Coordinates(3, 500, 1000, 0), Coordinates(3, 0, -1, 0), colors::kColorWhite, 0, 1),
        // Plane(Coordinates(3, 500, 0, 0), Coordinates(3, 0, -1, 0), colors::kColorWhite, 0, 0.2, 2)
    };

    Triangle triangles_arr[1] = {
        Triangle(Plane(Coordinates(3, 500, 0, 500), Coordinates(3, 0, 1, 1), colors::kColorYellow),
                 Coordinates(3, -400, 500, -500), Coordinates(3, -300, 0, 0)),
    };

    Triangle triangles_pyramid[6] {
        Triangle (Plane(Coordinates(3, 0, 0, 0), Coordinates(3, 0, 1, 0), colors::kColorYellow),
                  Coordinates(3, 200, 0, 0), Coordinates(3, 200, 0, 200)),
        Triangle (Plane(Coordinates(3, 0, 0, 0), Coordinates(3, 0, 1, 0), colors::kColorYellow),
                  Coordinates(3, 0, 0, 200), Coordinates(3, 200, 0, 200)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 0, -1, -5), colors::kColorYellow),
                  Coordinates(3, -100, 500, -100), Coordinates(3, 100, 500, -100)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 5, -1, 0), colors::kColorYellow),
                  Coordinates(3, 100, 500, -100), Coordinates(3, 100, 500, 100)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 0, -1, 5), colors::kColorYellow),
                  Coordinates(3, 100, 500, 100), Coordinates(3, -100, 500, 100)),
        Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, -5, -1, 0), colors::kColorYellow),
                  Coordinates(3, -100, 500, 100), Coordinates(3, -100, 500, -100)),
    };

    // std::vector<Triangle> triangles_pyramid_vec;
    // size_t triangles_pyramid_num = sizeof(triangles_pyramid) / sizeof(triangles_pyramid[0]);
    // for (size_t i = 0; i < triangles_pyramid_num; i++) {
    //     triangles_pyramid_vec.push_back(triangles_pyramid[i]);
    // }

    std::vector<Object*> objects;
    // objects.push_back(new Border(Coordinates(3, 0, 0, 0), 100, 100, 100));
    // objects.push_back(new Circle(Coordinates(3, 0, 0, 0), 10, kSphere, colors::kColorWhite));
    // objects.push_back(new Circle(Coordinates(3, 100, -200, 0), 10, kLight, colors::kColorWhite));

    // objects.push_back(new TrianglesSet(Coordinates(3, 100, 0, 100), triangles_pyramid_vec,
    //                                    colors::kColorWhite, 0, 1));

    size_t circles_num = sizeof(circles_arr) / sizeof(circles_arr[0]);
    for (size_t i = 0; i < circles_num; i++) {
        objects.push_back(new Circle(circles_arr[i]));
    }

//     size_t planes_num = sizeof(planes_arr) / sizeof(planes_arr[0]);
//     for (size_t i = 0; i < planes_num; i++) {
//         objects.push_back(new Plane(planes_arr[i]));
//     }
//
//     size_t triangles_num = sizeof(triangles_arr) / sizeof(triangles_arr[0]);
//     for (size_t i = 0; i < triangles_num; i++) {
//         objects.push_back(new Triangle(triangles_arr[i]));
//     }

    std::vector<Widget*> desktop_children;
    desktop_children.push_back(new SceneManager(Coordinates(2, 120, 0), 800, 410, objects, NULL));
    SceneManager* scene_manager = dynamic_cast<SceneManager*>(desktop_children.back());
    MoveButton buttons[6] = {
        MoveButton(Button(Coordinates(2, 5, 5), 50, 40, "X+", kFontFileNameScrollBar, NULL,  NULL, kColorButtons, kColorButtons),
                   Coordinates(3, 100, 0, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 65, 5), 50, 40, "X-", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                   Coordinates(3, -100, 0, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 50), 50, 40, "Y+", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                   Coordinates(3, 0, 100, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 65, 50), 50, 40, "Y-", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                   Coordinates(3, 0, -100, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 95), 50, 40, "Z+", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                   Coordinates(3, 0, 0, 100),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 65, 95), 50, 40, "Z-", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                   Coordinates(3, 0, 0, -100),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
    };

    CoeffChangingButton coeff_changing_buttons[6] = {
        CoeffChangingButton(Button(Coordinates(2, 5, 230), 110, 25, "Reflect +",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                        [scene_manager](float delta){scene_manager->ChangeCurObjReflection(delta);}, 0.1),
        CoeffChangingButton(Button(Coordinates(2, 5, 260), 110, 25, "Reflect -",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                        [scene_manager](float delta){scene_manager->ChangeCurObjReflection(delta);}, -0.1),
        CoeffChangingButton(Button(Coordinates(2, 5, 290), 110, 25, "Refract +",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                        [scene_manager](float delta){scene_manager->ChangeCurObjRefraction(delta);}, 0.1),
        CoeffChangingButton(Button(Coordinates(2, 5, 320), 110, 25, "Refract -",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                        [scene_manager](float delta){scene_manager->ChangeCurObjRefraction(delta);}, -0.1),
        CoeffChangingButton(Button(Coordinates(2, 5, 350), 110, 25, "Absorb +",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                        [scene_manager](float delta){scene_manager->ChangeCurObjAbsorption(delta);}, 0.1),
        CoeffChangingButton(Button(Coordinates(2, 5, 380), 110, 25, "Absorb -",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                        [scene_manager](float delta){scene_manager->ChangeCurObjAbsorption(delta);}, -0.1),
    };

    std::vector<Widget*> buttons_on_panel;

    size_t buttons_num = sizeof(buttons) / sizeof(buttons[0]);
    for (size_t i = 0; i < buttons_num; i++) {
        buttons_on_panel.push_back(new MoveButton(buttons[i]));
    }

    buttons_on_panel.push_back(new ExistenceButton(Button(Coordinates(2, 5, 140), 110, 40, "Del",
                               kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                               [scene_manager](){scene_manager->DeleteCurrentObject();}));
    buttons_on_panel.push_back(new ExistenceButton(Button(Coordinates(2, 5, 185), 110, 40, "Add",
                               kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons),
                               [scene_manager](){scene_manager->AddCopyCurrentObject();}));

    size_t buttons_coeff_num = sizeof(coeff_changing_buttons) / sizeof(coeff_changing_buttons[0]);
    for (size_t i = 0; i < buttons_coeff_num; i++) {
        buttons_on_panel.push_back(new CoeffChangingButton(coeff_changing_buttons[i]));
    }

    desktop_children.push_back(new PanelControl(Coordinates(2), 120, 410, NULL, &buttons_on_panel));
    scene_manager->SetPanelControl(dynamic_cast<PanelControl*>(desktop_children.back()));
    desktop_children.push_back(new ListObjects(Coordinates(2, 120, 410 + kListObjectsTitleHeight),
                                scene_manager->GetObjectsVec(),
                                [scene_manager](size_t idx){scene_manager->ChooseObject(idx);}));
    desktop_children.push_back(new ListObjectsTitle(Coordinates(2, 120, 410),
                                                    dynamic_cast<ListObjects*>(desktop_children.back())));
    scene_manager->SetListObjects(dynamic_cast<ListObjectsTitle*>(desktop_children.back()));
    desktop_children.push_back(new InfoTable(Coordinates(2, 920, 0), 160, 410, circles_arr + 0, NULL));
    scene_manager->SetInfoTable(dynamic_cast<InfoTable*>(desktop_children.back()));

    UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/build/libdr4.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics");

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
