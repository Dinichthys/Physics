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
    dr4::Rectangle* background = window_->CreateRectangle();
    background->SetPos({0, 0});
    background->SetSize({(float)GetWidth(), (float)GetHeight()});
    background->SetFillColor(kBackgroundColor);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto start_time = end_time;
    size_t duration = 0;

    double abs_time_win = window_->GetTime();
    double delta_time_win = 0;

    dr4::Event event;
    std::optional<dr4::Event> event_opt;
    while (window_->IsOpen()) {
        if ((event_opt = window_->PollEvent()).has_value()) {
            event = event_opt.value();
            if (event.type == dr4::Event::Type::QUIT) {
                if (window_->IsOpen()) {
                    window_->Close();
                }
                break;
            }
            AnalyzeKey(event);
        }

        double cur_time = window_->GetTime();
        delta_time_win = cur_time - abs_time_win;
        abs_time_win = cur_time;
        hui::IdleEvent idle_evt;
        idle_evt.absTime = abs_time_win;
        idle_evt.deltaTime = delta_time_win;
        WidgetContainer::OnIdle(idle_evt);

        start_time = end_time;
        end_time = std::chrono::high_resolution_clock::now();
        duration += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        if (kOneSceneUpdateTimeInMicro <= duration) {
            duration = 0;

            texture->Draw(*background);

            WidgetContainer::Redraw();

            window_->Draw(*texture);

            texture->Clear(dr4::Color(0, 0, 0, 255));

            window_->Display();
        }
    }

    delete background;

    return kDoneRenderer;
}

RendererError UI::AnalyzeKey(const dr4::Event& event) {
    static Coordinates mouse_pos(2, 0, 0);

    switch(event.type) {
        case(dr4::Event::Type::MOUSE_DOWN) : {
            this->OnMousePress(Coordinates(2, event.mouseButton.pos.x, event.mouseButton.pos.y), event.mouseButton.button);

            break;
        }
        case(dr4::Event::Type::MOUSE_UP) : {
            state_.target_widget_ = NULL;

            this->OnMouseRelease(Coordinates(2, event.mouseButton.pos.x, event.mouseButton.pos.y), event.mouseButton.button);

            break;
        }
        case(dr4::Event::Type::MOUSE_MOVE) : {
            float old_x = mouse_pos[0];
            float old_y = mouse_pos[1];

            this->OnMouseEnter(Coordinates(2, event.mouseMove.pos.x, event.mouseMove.pos.y),
                               Coordinates(2, mouse_pos[0] - old_x, mouse_pos[1] - old_y));

            mouse_pos.SetCoordinate(0, event.mouseMove.pos.x);
            mouse_pos.SetCoordinate(1, event.mouseMove.pos.y);

            if (state_.target_widget_ == NULL) {
                break;
            }

            dynamic_cast<::Widget*>(state_.target_widget_)->OnMouseMove(mouse_pos[0] - old_x, mouse_pos[1] - old_y);

            break;
        }
        case(dr4::Event::Type::KEY_DOWN) : {
            this->OnKeyPressed(event.key);
            break;
        }
        case(dr4::Event::Type::KEY_UP) : {
            this->OnKeyUp(event.key);
            break;
        }
        case(dr4::Event::Type::TEXT_EVENT) : {
            this->OnText(event.text);
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
