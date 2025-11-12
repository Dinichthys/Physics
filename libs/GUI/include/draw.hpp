#ifndef DRAW_HPP
#define DRAW_HPP

#include <dlfcn.h>
#include <stdexcept>
#include <iostream>

#include "colors.hpp"

#include "misc/dr4_ifc.hpp"

#include "widget.hpp"
#include "vector.hpp"

#include "my_assert.h"

static const unsigned int kStartHeight = 720;
static const unsigned int kStartWidth = 1080;
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
        dr4::DR4Backend* backend_;

    public:
        explicit UI(float width, float height,
                     const std::vector<Widget*>& children, const char* window_name = kWindowName,
                     const char* const dll_backend_name = kBackendFileName)
            :WidgetContainer(Coordinates(2, 0, 0), width, height, NULL),
             window_(CreateWindow({width, height}, window_name, dll_backend_name)) {
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
            delete backend_;
        };

        RendererError ShowWindow();

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            WidgetContainer::OnMousePress(mouse_pos);

            state->target_widget_ = (state->target_widget_ == this) ? NULL : state->target_widget_;

            return false;
        };

    private:
        RendererError AnalyzeKey(const dr4::Event& event);

        dr4::Window* CreateWindow(const dr4::Vec2f& size, const char* name, const char* const dll_backend_name) {
            void* dll = dlopen(dll_backend_name, RTLD_NOW | RTLD_NODELETE);
            if (dll == NULL) {
                fprintf(stderr, "%s\n", dlerror());
                throw std::runtime_error("Can't upload dll with backend\n");
            }

            typedef dr4::DR4Backend* (*CreateBackend_t) ();

            CreateBackend_t CreateBackend = (CreateBackend_t) dlsym(dll, dr4::DR4BackendFunctionName);
            if (CreateBackend == NULL) {
                throw std::runtime_error("Can't find function for backend creation in dll\n");
            }
            dlclose(dll);

            backend_ = CreateBackend();

            dr4::Window* window = backend_->CreateWindow();
            window->Open();
            window->SetSize(size);
            window->SetTitle(name);
            return window;
        }
};

const char* ErrorHandler(enum RendererError error);

#endif // DRAW_HPP
