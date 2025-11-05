#ifndef DRAW_HPP
#define DRAW_HPP

#include "graphics.hpp"

#include "widget.hpp"
#include "vector.hpp"

#include "my_assert.h"

static const unsigned int kStartHeight = 720;
static const unsigned int kStartWidth = 1080;
static const char* const kWindowName = "Window";
static const size_t kOneSceneUpdateTimeInMicro = 10000;
static const size_t kCharacterSize = 100;

static const graphics::Color kBackgroundColor = graphics::Color(42, 40, 41);

enum RendererError {
    kDoneRenderer = 0,
    kBadAllocReactionRenderer,
    kCantLoadFontRenderer,
};

class UI : public WidgetContainer {
    private:
        graphics::RenderWindow window;
        hui::State state_;

    public:
        explicit UI(unsigned int width, unsigned int height,
                     const std::vector<Widget*>& children, const char* window_name = kWindowName)
            :WidgetContainer(Coordinates(2, 0, 0), width, height, &state_),
              window(width, height, window_name) {
            std::vector<Widget*>& children_ = WidgetContainer::GetChildren();

            size_t children_num = children.size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back(children[i]);
                children_[i]->SetState(&state_);
            }

            state_.hovered_widget_ = NULL;
            state_.target_widget_ = NULL;
            state = &state_;

            WidgetContainer::SetParentToChildren();
        };

        ~UI() {
            if (window.IsOpen()) {
                window.Close();
            }
        };

        RendererError ShowWindow();

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            WidgetContainer::OnMousePress(mouse_pos);

            state->target_widget_ = (state->target_widget_ == this) ? NULL : state->target_widget_;

            return false;
        };

    private:
        RendererError AnalyzeKey(const dr4::Event& event);
        void GetMousePosition(Coordinates& mouse_pos);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
