#ifndef DRAW_HPP
#define DRAW_HPP

#include <stdexcept>
#include <iostream>

#include "colors.hpp"

#include "cum/manager.hpp"
#include "cum/ifc/dr4.hpp"

#include "widget.hpp"
#include "vector.hpp"

#include "my_assert.h"

static const unsigned int kStartHeight = 1080;
static const unsigned int kStartWidth = 1720;
static const char* const kWindowName = "Window";
static const char* const kBackendFileName = "./plugins/MyDR4Backend/build/libbackend.so";
static const size_t kOneSceneUpdateTimeInMicro = 10000;
static const size_t kCharacterSize = 100;

static const colors::Color kBackgroundColor = colors::Color(42, 40, 41);

enum RendererError {
    kDoneRenderer = 0,
    kBadAllocReactionRenderer,
    kCantLoadFontRenderer,
};

class UI : public WidgetContainer {
    private:
        dr4::Window* const window_;
        hui::State state_;
        cum::DR4BackendPlugin* backend_;

    public:
        explicit UI(float width, float height,
                     const std::vector<Widget*>& children, const char* window_name = kWindowName,
                     const char* const dll_backend_name = kBackendFileName)
            :WidgetContainer(Coordinates(2, 0, 0), width, height, NULL),
             window_(CreateWindow({width, height}, window_name, dll_backend_name)),
             state_() {
            state_.hovered_widget_ = NULL;
            state_.target_widget_ = NULL;
            state_.window_ = window_;
            state = &state_;

            Widget::SetState(&state_);

            std::vector<Widget*>& children_ = WidgetContainer::GetChildren();
            size_t children_num = children.size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back(children[i]);
                children_[i]->SetState(&state_);
            }

            WidgetContainer::SetParentToChildren();
        };

        ~UI() {
            if (window_->IsOpen()) {
                window_->Close();
            }
            delete window_;
        };

        RendererError ShowWindow();

        virtual bool OnMousePress(const Coordinates& mouse_pos, dr4::MouseButtonType type) override {
            WidgetContainer::OnMousePress(mouse_pos, type);

            state->target_widget_ = (state->target_widget_ == this) ? NULL : state->target_widget_;

            return false;
        };

    private:
        RendererError AnalyzeKey(const dr4::Event& event);

        dr4::Window* CreateWindow(const dr4::Vec2f& size, const char* name, const char* const dll_backend_name) {
            backend_ = dynamic_cast<cum::DR4BackendPlugin*>(state_.manager.LoadFromFile(dll_backend_name));
            if (backend_ == NULL) {
                throw std::runtime_error("Can't load from file dr4backend\n");
            }

            dr4::Window* window = backend_->CreateWindow();
            window->Open();
            window->SetSize(size);
            window->SetTitle(name);
            return window;
        }
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
