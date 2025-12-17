#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "draw.hpp"
#include "scrollbar.hpp"
#include "button.hpp"
#include "object_buttons.hpp"
#include "camera_control.hpp"
#include "topbar.hpp"
#include "file_button.hpp"
#include "view.hpp"

#include "scene_manager.hpp"

#include "circle.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "list_objects.hpp"

#include "logging.h"

int main() {
    set_log_lvl(kError);

    Circle circles_arr[6] = {
        // Circle(Coordinates(3, 100, -400, 100),       400, kSphere, colors::kColorBlue, 0, 1),
        Circle(Coordinates(3, 400, 800, 1000),  200, kSphere, colors::kColorBlue, 0.5, 0.5),
        Circle(Coordinates(3, 0,   800, 1500),  200, kSphere, colors::kColorRed, 0, 1),
        Circle(Coordinates(3,-400, 800, 1000),  200, kSphere, colors::kColorGreen, 0, 0.2, 2),
        Circle(Coordinates(3, 0, 1000, 7000),  4000, kSphere, colors::Color(100, 100, 100), 0, 1),
        // Circle(Coordinates(3, 500, 0, -7000),  1000, kSphere, colors::kColorBlue, 0, 1),

        // Circle(Coordinates(3, 100, 450, 0),     50,  kLight,  Coordinates(3, 150, 150, 150)),

        Circle(Coordinates(3, 600, -1000, 2000),  50,  kLight,  colors::kColorWhite),
        Circle(Coordinates(3, 1080, -1000, -500), 50,  kLight,  colors::kColorWhite),
        // Circle(Coordinates(3, -40000, -40000, 100000), 10000,  kLight,  colors::kColorYellow),
    };

    Plane planes_arr[1] = {
        Plane(Coordinates(3, -500, 1000, 0), Coordinates(3, 0, -1, 0), colors::Color(90, 90, 90), 0.5, 0.5),
        // Plane(Coordinates(3, 500, 0, 0), Coordinates(3, 0, -1, 0), colors::kColorWhite, 0, 0.2, 2)
    };

    Triangle triangles_arr[1] = {
        Triangle(Plane(Coordinates(3, 0, 800, 1500), Coordinates(3, 0, -1, 0), colors::kColorYellow),
                 Coordinates(3, -400, 800, 1000), Coordinates(3, 400, 800, 1000)),
    };

    // Triangle triangles_pyramid[6] {
    //     Triangle (Plane(Coordinates(3, 0, 0, 0), Coordinates(3, 0, 1, 0), colors::kColorYellow),
    //               Coordinates(3, 200, 0, 0), Coordinates(3, 200, 0, 200)),
    //     Triangle (Plane(Coordinates(3, 0, 0, 0), Coordinates(3, 0, 1, 0), colors::kColorYellow),
    //               Coordinates(3, 0, 0, 200), Coordinates(3, 200, 0, 200)),
    //     Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 0, -1, -5), colors::kColorYellow),
    //               Coordinates(3, -100, 500, -100), Coordinates(3, 100, 500, -100)),
    //     Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 5, -1, 0), colors::kColorYellow),
    //               Coordinates(3, 100, 500, -100), Coordinates(3, 100, 500, 100)),
    //     Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, 0, -1, 5), colors::kColorYellow),
    //               Coordinates(3, 100, 500, 100), Coordinates(3, -100, 500, 100)),
    //     Triangle (Plane(Coordinates(3, 100, -500, 100), Coordinates(3, -5, -1, 0), colors::kColorYellow),
    //               Coordinates(3, -100, 500, 100), Coordinates(3, -100, 500, -100)),
    // };

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

    size_t planes_num = sizeof(planes_arr) / sizeof(planes_arr[0]);
    for (size_t i = 0; i < planes_num; i++) {
        objects.push_back(new Plane(planes_arr[i]));
    }

    size_t triangles_num = sizeof(triangles_arr) / sizeof(triangles_arr[0]);
    for (size_t i = 0; i < triangles_num; i++) {
        objects.push_back(new Triangle(triangles_arr[i]));
    }

    std::vector<Widget*> desktop_children;
    desktop_children.push_back(new SceneManager(Coordinates(2, 120, 0), 800, 800, objects, NULL));
    SceneManager* scene_manager = dynamic_cast<SceneManager*>(desktop_children.back());
    MoveButton buttons[6] = {
        MoveButton(Button(Coordinates(2, 5, 107), 7, 7, "/\\", kFontFileNameScrollBar, NULL,  NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                   Coordinates(3, 100, 0, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 115), 7, 7, "\\/", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                   Coordinates(3, -100, 0, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 123), 7, 7, "/\\", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                   Coordinates(3, 0, 100, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 131), 7, 7, "\\/", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                   Coordinates(3, 0, -100, 0),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 139), 7, 7, "/\\", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                   Coordinates(3, 0, 0, 100),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
        MoveButton(Button(Coordinates(2, 5, 147), 7, 7, "\\/", kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                   Coordinates(3, 0, 0, -100),
                   [scene_manager](const Coordinates& move_direction){scene_manager->MoveCurrentObject(move_direction);}),
    };

    CoeffChangingButton coeff_changing_buttons[6] = {
        CoeffChangingButton(Button(Coordinates(2, 80, 225), 7, 7, "/\\",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                        [scene_manager](float delta){scene_manager->ChangeCurObjReflection(delta);}, 0.1),
        CoeffChangingButton(Button(Coordinates(2, 80, 233), 7, 7, "\\/",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                        [scene_manager](float delta){scene_manager->ChangeCurObjReflection(delta);}, -0.1),
        CoeffChangingButton(Button(Coordinates(2, 80, 295), 7, 7, "/\\",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                        [scene_manager](float delta){scene_manager->ChangeCurObjRefraction(delta);}, 0.1),
        CoeffChangingButton(Button(Coordinates(2, 80, 303), 7, 7, "\\/",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                        [scene_manager](float delta){scene_manager->ChangeCurObjRefraction(delta);}, -0.1),
        CoeffChangingButton(Button(Coordinates(2, 80, 363), 7, 7, "/\\",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                        [scene_manager](float delta){scene_manager->ChangeCurObjAbsorption(delta);}, 0.1),
        CoeffChangingButton(Button(Coordinates(2, 80, 371), 7, 7, "\\/",
                        kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 8, colors::kColorWhite),
                        [scene_manager](float delta){scene_manager->ChangeCurObjAbsorption(delta);}, -0.1),
    };

    std::vector<Widget*> buttons_on_panel;

    size_t buttons_num = sizeof(buttons) / sizeof(buttons[0]);
    for (size_t i = 0; i < buttons_num; i++) {
        buttons_on_panel.push_back(new MoveButton(buttons[i]));
    }

    buttons_on_panel.push_back(new ExistenceButton(Button(Coordinates(2, 5, 40), 70, 40, "   -",
                               kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 0, colors::kColorWhite),
                               [scene_manager](){scene_manager->DeleteCurrentObject();}));
    buttons_on_panel.push_back(new ExistenceButton(Button(Coordinates(2, 85, 40), 70, 40, "   +",
                               kFontFileNameScrollBar, NULL, NULL, kColorButtons, kColorButtons, 0, colors::kColorWhite),
                               [scene_manager](){scene_manager->AddCopyCurrentObject();}));

    size_t buttons_coeff_num = sizeof(coeff_changing_buttons) / sizeof(coeff_changing_buttons[0]);
    for (size_t i = 0; i < buttons_coeff_num; i++) {
        buttons_on_panel.push_back(new CoeffChangingButton(coeff_changing_buttons[i]));
    }
//
//     desktop_children.push_back(new PanelControl(Coordinates(2), 120, 410, NULL, NULL));
//     scene_manager->SetPanelControl(dynamic_cast<PanelControl*>(desktop_children.back()));
    desktop_children.push_back(new ListObjects(Coordinates(2, 1000, 410 + kListObjectsTitleHeight),
                                scene_manager->GetObjectsVec(),
                                [scene_manager](size_t idx){scene_manager->ChooseObject(idx);}));
    desktop_children.push_back(new ListObjectsTitle(Coordinates(2, 1000, 410),
                                                    dynamic_cast<ListObjects*>(desktop_children.back())));
    scene_manager->SetListObjects(dynamic_cast<ListObjectsTitle*>(desktop_children.back()));
    desktop_children.push_back(new InfoTable(Coordinates(2, 920, 0), 200, 410, circles_arr + 0, NULL, NULL, &buttons_on_panel));
    scene_manager->SetInfoTable(dynamic_cast<InfoTable*>(desktop_children.back()));

    CameraControlButton camera_buttons[8] = {
        CameraControlButton(Button(Coordinates(2, 5, 25), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnLetterA();}),
        CameraControlButton(Button(Coordinates(2, 5, 60), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnLetterD();}),
        CameraControlButton(Button(Coordinates(2, 40, 25), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnLetterW();}),
        CameraControlButton(Button(Coordinates(2, 40, 60), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnLetterS();}),
        CameraControlButton(Button(Coordinates(2, 75, 25), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnArrowLeft();}),
        CameraControlButton(Button(Coordinates(2, 75, 60), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnArrowRight();}),
        CameraControlButton(Button(Coordinates(2, 110, 25), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnArrowUp();}),
        CameraControlButton(Button(Coordinates(2, 110, 60), 30, 30, " ",
                                    kFontFileNameGeomPrim, NULL, NULL, kColorButtons, kColorButtons, 20, colors::kColorWhite),
                                    [scene_manager](){scene_manager->OnArrowDown();}),
    };

    std::vector<Widget*> camera_buttons_vec;
    size_t camera_buttons_num = sizeof(camera_buttons) / sizeof(camera_buttons[0]);
    for (size_t i = 0; i < camera_buttons_num; i++) {
        camera_buttons_vec.push_back(new CameraControlButton(camera_buttons[i]));
    }

    desktop_children.push_back(new CameraController(Coordinates(2, 1000, 500), 145, 95 - kTitleHeight, NULL, NULL, &camera_buttons_vec));

    MainField* main_field = new MainField(Coordinates(2, 0, kTitleHeight), kStartWidth, kStartHeight, NULL, &desktop_children, NULL);

    std::vector<Widget*> topbar_buttons;

    topbar_buttons.push_back(new ViewButton(Coordinates(2, kTopBarButtonWidth, 0), NULL, NULL));
    topbar_buttons.push_back(new FileButton(Coordinates(2, 0, 0), kTopBarButtonWidth, NULL, NULL));

    TopBar* topbar = new TopBar(Coordinates(2), kStartWidth, NULL, &topbar_buttons, NULL);

    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/build/libdr4_sdl3.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/build/libdr4_sdl3.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/Artem/plugin/libswuix_sdl3.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/build/libdr4.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/Egor_dr4_backend/build/dr4_sfml_backend.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/Artem/plugin_new/libswuix_sdl3.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/MyDR4Backend/build/IvanAbrutin.so");
    // UI renderer(kStartWidth, kStartHeight, desktop_children, "Physics", "./plugins/build/libplugin_dr4.so");
    UI renderer(kStartWidth, kStartHeight, main_field, topbar, "Physics");

    enum RendererError result = renderer.ShowWindow();
    if (result != kDoneRenderer) {
        fprintf(stderr, "Runtime error in ShowWindow(): %s\n", ErrorHandler(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
