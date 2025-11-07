#include "draw.hpp"

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>

#include "colors.hpp"

#include "object.hpp"

#include "logging.h"
#include "my_assert.h"

RendererError UI::ShowWindow() {
    dr4::Rectangle background(dr4::Rect2f({0, 0}, {(float)GetWidth(), (float)GetHeight()}), kBackgroundColor);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto start_time = end_time;
    size_t duration = 0;

    dr4::Event event;
    std::optional<dr4::Event> event_opt;
    while (window->IsOpen()) {
        if ((event_opt = window->PollEvent()).has_value()) {
            event = event_opt.value();
            if (event.type == dr4::Event::Type::QUIT) {
                window->Close();
                break;
            }
            AnalyzeKey(event);
        }


        WidgetContainer::OnIdle();

        start_time = end_time;
        end_time = std::chrono::high_resolution_clock::now();
        duration += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        if (kOneSceneUpdateTimeInMicro <= duration) {
            duration = 0;

            texture->Draw(background);

            WidgetContainer::Redraw();

            window->Draw(*texture, {0, 0});

            texture->Clear(dr4::Color(0, 0, 0, 255));

            window->Display();
        }
    }

    return kDoneRenderer;
}

RendererError UI::AnalyzeKey(const dr4::Event& event) {
    static Coordinates mouse_pos(2, 0, 0);

    switch(event.type) {
        case(dr4::Event::Type::MOUSE_DOWN) : {
            hui::MouseDownEvent hui_event;
            hui_event.relPos = event.mouseButton.pos;
            hui_event.Apply(*this);

            break;
        }
        case(dr4::Event::Type::MOUSE_UP) : {
            state_.target_widget_ = NULL;

            hui::MouseUpEvent hui_event;
            hui_event.relPos = event.mouseButton.pos;
            hui_event.Apply(*this);

            break;
        }
        case(dr4::Event::Type::MOUSE_MOVE) : {
            float old_x = mouse_pos[0];
            float old_y = mouse_pos[1];

            hui::MouseMoveEvent hui_event;
            hui_event.rel = event.mouseMove.pos;
            hui_event.Apply(*this);

            mouse_pos.SetCoordinate(0, event.mouseMove.pos.x);
            mouse_pos.SetCoordinate(1, event.mouseMove.pos.y);

            if (state_.target_widget_ == NULL) {
                break;
            }

            dynamic_cast<::Widget*>(state_.target_widget_)->OnMouseMove(mouse_pos[0] - old_x, mouse_pos[1] - old_y);

            break;
        }
        case(dr4::Event::Type::KEY_DOWN) : {
            hui::KeyPressed hui_event;
            hui_event.mod = event.key.mod;
            hui_event.sym = event.key.sym;
            hui_event.Apply(*this);

            break;
        }
        default:
            break;
    }

    return kDoneRenderer;
}

const char* ErrorHandler(enum RendererError error) {
    switch (error) {
        case kDoneRenderer:
            return "All were done";
        case kBadAllocReactionRenderer:
            return "Can't allocate memory in reaction";
        default:
            return "Invalid enum RendererError identifier";
    }
}
