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

    public:
        explicit UI(unsigned int width, unsigned int height,
                     const std::vector<Widget*>& children, const char* window_name = kWindowName)
            :WidgetContainer(Coordinates(2, 0, 0), width, height),
              window(width, height, window_name) {
            std::vector<Widget*>& children_ = WidgetContainer::GetChildren();

            size_t children_num = children.size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back(children[i]);
            }

            WidgetContainer::SetParentToChildren();
        };
        ~UI() {
            if (window.IsOpen()) {
                window.Close();
            }
        };

        RendererError ShowWindow();

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            ASSERT(widget != NULL, "");

            WidgetContainer::OnMousePress(mouse_pos, widget);

            *widget = (*widget == this) ? NULL : *widget;

            return false;
        };

    private:
        RendererError AnalyzeKey(const graphics::Event& event);
        void GetMousePosition(Coordinates& mouse_pos);
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
