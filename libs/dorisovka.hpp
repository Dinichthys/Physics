#ifndef DORISOVKA_HPP
#define DORISOVKA_HPP

#include <vector>
#include <dlfcn.h>
#include <stdexcept>

#include "widget.hpp"
#include "cum/geomprim_ifc.hpp"

#include "dr4/math/rect.hpp"

#include "dorisovka_buttons.hpp"

static const size_t kRectangleID = 0;
static const size_t kCircleID = 1;
static const size_t kArrowID = 2;
static const float kGeomButtonWidth = 50;
static const float kGeomButtonHeight = 25;

static const std::string kFontFileNameGeomPrim = "data/font.ttf";
static const std::string kRectangleButtonName = "rect";
static const std::string kCircleButtonName = "circ";
static const std::string kArrowButtonName = "arrow";

static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libgeomprim.so";

static colors::Color kGeomPrimButtonColor = colors::Color(49, 49, 49);

class Dorisovka : public WidgetContainer {
    private:
        hui::GeomPrimBackend* backend_;
        std::vector<hui::GeomPrim*> prims_;
        hui::GeomPrim* target_;

        bool is_changing_;

    public:
        explicit Dorisovka(const Coordinates& lt_corner, dr4::Vec2f size,
            const char* plugin_name, hui::State* state = NULL)
            :WidgetContainer(lt_corner,  size.x, size.y, state),
             backend_(CreateBackendMethod(plugin_name)) {

            std::vector<Widget*> buttons;
            buttons.push_back(new GeomPrimCreationButton(Button(
                lt_corner + Coordinates(2, (size.x - kGeomButtonWidth * 3) / 4,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight, kRectangleButtonName,
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                this->CreatePrim(id);
              }, kRectangleID));
            buttons.push_back(new GeomPrimCreationButton(Button(
                lt_corner + Coordinates(2, 2 * (size.x - kGeomButtonWidth * 3) / 4 + kGeomButtonWidth,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight, kCircleButtonName,
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                this->CreatePrim(id);
              }, kCircleID));
            buttons.push_back(new GeomPrimCreationButton(Button(
                lt_corner + Coordinates(2, 3 * (size.x - kGeomButtonWidth * 3) / 4 + kGeomButtonWidth * 2,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight, kArrowButtonName,
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                this->CreatePrim(id);
              }, kArrowID));
            WidgetContainer::AddChild(
                new PanelControl(lt_corner + Coordinates(2, 0, size.y - kGeomPrimPanelControlHeight),
                size.x, kGeomPrimPanelControlHeight, state, &buttons
            ));

            is_changing_ = false;
            target_ = NULL;
        };

        ~Dorisovka() {
            for (size_t i = 0; i < prims_.size(); i++) {
                delete prims_[i];
            }

            delete backend_;
            if (target_ != NULL) {
                delete target_;
            }
        };

        void CreatePrim(size_t id) {
            if (target_ != NULL) {
                delete target_;
            }
            target_ = backend_->CreateGeomPrim(id, state->window_);

            dr4::Line* line = state->window_->CreateLine();
            line->SetThickness(100);
            line->SetStart({0, 0});
            line->SetEnd(Widget::GetSize());
            line->SetColor({255, 0, 0});
            line->DrawOn(*texture);
            delete line;

            fprintf(stderr, "geom prim - %lu\ntarget_ = %p\n\n", id, target_);
            is_changing_ = false;
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if ((target_ == NULL) || (!Widget::OnMousePress(mouse_pos))
               || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                bool res = WidgetContainer::OnMousePress(mouse_pos);
                if (state->target_widget_ == this) {state->target_widget_ = NULL;}
                return res;
            }
            if (state->target_widget_ == this) {state->target_widget_ = NULL;}

            hui::MouseDownEvent evt;
            evt.relPos = {mouse_pos[0], mouse_pos[1]};
            target_->OnMouseDown(evt);
            prims_.push_back(target_);
            is_changing_ = true;
            return true;
        };
        virtual bool OnMouseRelease(const Coordinates& mouse_pos) override {
            if ((target_ == NULL) || (!Widget::OnMouseRelease(mouse_pos))
               || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return WidgetContainer::OnMouseRelease(mouse_pos);
            }

            if (!is_changing_) {
                return false;
            }

            hui::MouseUpEvent evt;
            evt.relPos = {mouse_pos[0], mouse_pos[1]};
            target_->OnMouseRelease(evt);
            target_ = NULL;
            is_changing_ = false;
            return true;
        };
        virtual bool OnMouseEnter(const Coordinates& mouse_pos) override {
            if ((target_ == NULL) || (!Widget::OnMouseEnter(mouse_pos))
               || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return WidgetContainer::OnMouseEnter(mouse_pos);
            }

            if (!is_changing_) {
                return false;
            }

            hui::MouseMoveEvent evt;
            evt.rel = {mouse_pos[0], mouse_pos[1]};
            target_->OnMouseMove(evt);
            return true;
        };

        virtual void Redraw() override {
            for (size_t i = 0; i < prims_.size(); i++) {
                prims_[i]->DrawOn(*texture);
            }

            WidgetContainer::Redraw();
        };

    private:
        hui::GeomPrimBackend* CreateBackendMethod(const char* const dll_backend_name) {
            void* dll = dlopen(dll_backend_name, RTLD_NOW | RTLD_NODELETE);
            if (dll == NULL) {
                fprintf(stderr, "%s\n", dlerror());
                throw std::runtime_error("Can't upload dll with backend\n");
            }

            typedef hui::GeomPrimBackend* (*CreateBackend_t) ();

            CreateBackend_t CreateBackend = (CreateBackend_t) dlsym(dll, hui::GeomPrimBackendFunctionName);
            if (CreateBackend == NULL) {
                throw std::runtime_error("Can't find function for backend creation in dll\n");
            }
            dlclose(dll);

            return CreateBackend();
        };
};

#endif // DORISOVKA_HPP
