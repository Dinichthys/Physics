#ifndef DORISOVKA_HPP
#define DORISOVKA_HPP

#include <vector>
#include <dlfcn.h>
#include <stdexcept>

#include "widget.hpp"
#include "pp/canvas.hpp"
#include "pp/shape.hpp"
#include "pp/tool.hpp"
#include "cum/ifc/pp.hpp"

#include "dr4/math/rect.hpp"

#include "dorisovka_buttons.hpp"

static const size_t kRectangleID = 0;
static const size_t kCircleID = 1;
static const size_t kArrowID = 2;
static const float kGeomButtonWidth = 60;
static const float kGeomButtonHeight = 25;

static const std::string kFontFileNameGeomPrim = "data/FiraCodeNerdFont-Regular.ttf";
static const std::string kRectangleButtonName = "rect";
static const std::string kCircleButtonName = "circ";
static const std::string kArrowButtonName = "arrow";

static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libgeomprim.so";

static colors::Color kGeomPrimButtonColor = colors::Color(49, 49, 49);

class Dorisovka : public WidgetContainer, public pp::Canvas {
    private:
        cum::PPToolPlugin* backend_;
        std::unordered_map<pp::Shape*, pp::Shape*> prims_;
        std::vector<std::unique_ptr<pp::Tool>> tools_;

        pp::Shape* selected_shape_;
        pp::Tool* selected_tool_;

        const pp::ControlsTheme kTheme = {.shapeColor = colors::Color(0, 0, 0, 0),
                                          .lineColor = colors::kColorGreen,
                                          .textColor = colors::kColorBlack,
                                          .baseFontSize = 10,
                                          .handleColor = colors::kColorYellow,
                                          .handleHoverColor = colors::kColorBlue,
                                          .handleActiveColor = colors::kColorRed};

        PanelControl* panel_;

    public:
        explicit Dorisovka(const Coordinates& lt_corner, dr4::Vec2f size,
            const char* plugin_name, hui::State* state = NULL)
            :WidgetContainer(lt_corner,  size.x, size.y, state),
             backend_(dynamic_cast<cum::PPToolPlugin*>(state->manager.LoadFromFile(plugin_name))),
             tools_(backend_->CreateTools(this)) {
            std::vector<Widget*> buttons;

            for (size_t i = 0; i < tools_.size(); i++) {
            buttons.push_back(new GeomPrimCreationButton(Button(
                Coordinates(2,
                    (i + 1) * (size.x - kGeomButtonWidth * tools_.size()) / (tools_.size() + 1)
                    + kGeomButtonWidth * i,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight,
                std::string(tools_[i].get()->Icon()),
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                    this->CreatePrim(id);
                }, i));
            }

            panel_ = new PanelControl(
                Coordinates(2, 0, size.y - kGeomPrimPanelControlHeight),
                size.x, kGeomPrimPanelControlHeight, state, &buttons
            );
            WidgetContainer::AddChild(panel_);

            selected_shape_ = NULL;
            selected_tool_ = NULL;
        };

        ~Dorisovka() {
            for (auto prim : prims_) {
                delete prim.second;
            }
        };

        void CreatePrim(size_t id) {
            if (selected_shape_ != NULL) {
                delete selected_shape_;
                prims_.erase(selected_shape_);
            }
            tools_[id].get()->OnStart();
            selected_tool_ = tools_[id].get();
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if ((!Widget::OnMousePress(mouse_pos))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                bool res = WidgetContainer::OnMousePress(mouse_pos);
                if ((state->target_widget_ == this) || (state->target_widget_ == panel_)) {
                    state->target_widget_ = NULL;
                }
                return res;
            }
            if ((state->target_widget_ == this) || (state->target_widget_ == panel_)) {
                state->target_widget_ = NULL;
            }

            dr4::Event::MouseButton evt;
            evt.button = dr4::MouseButtonType::UNKNOWN;
            evt.pos = {mouse_pos[0] - Widget::GetLTCornerLoc()[0], mouse_pos[1] - Widget::GetLTCornerLoc()[1]};
            if (selected_tool_ != NULL) {
                selected_tool_->OnMouseDown(evt);
                return true;
            }

            if (selected_shape_ != NULL) {
                selected_shape_->OnMouseDown(evt);
                return true;
            }

            for (auto shape : prims_) {
                if (shape.second->OnMouseDown(evt)) {
                    return true;
                }
            }

            return true;
        };
        virtual bool OnMouseRelease(const Coordinates& mouse_pos) override {
            if (((selected_shape_ == NULL) && (selected_tool_ == NULL))
                || (!Widget::OnMouseRelease(mouse_pos))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return WidgetContainer::OnMouseRelease(mouse_pos);
            }

            dr4::Event::MouseButton evt;
            evt.button = dr4::MouseButtonType::UNKNOWN;
            evt.pos = {mouse_pos[0] - Widget::GetLTCornerLoc()[0], mouse_pos[1] - Widget::GetLTCornerLoc()[1]};
            if (selected_tool_ != NULL) {
                selected_tool_->OnMouseUp(evt);
                selected_tool_ = NULL;
                return true;
            }

            if (selected_shape_ != NULL) {
                selected_shape_->OnMouseUp(evt);
                return true;
            }

            for (auto shape : prims_) {
                if (shape.second->OnMouseUp(evt)) {
                    return true;
                }
            }

            return true;
        };
        virtual bool OnMouseEnter(const Coordinates& mouse_pos) override {
            if (((selected_shape_ == NULL) && (selected_tool_ == NULL))
                || (!Widget::OnMouseEnter(mouse_pos))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return WidgetContainer::OnMouseEnter(mouse_pos);
            }

            dr4::Event::MouseMove evt;
            evt.pos = {mouse_pos[0] - Widget::GetLTCornerLoc()[0], mouse_pos[1] - Widget::GetLTCornerLoc()[1]};
            if (selected_tool_ != NULL) {
                selected_tool_->OnMouseMove(evt);
                return true;
            }

            if (selected_shape_ != NULL) {
                selected_shape_->OnMouseMove(evt);
                return true;
            }

            for (auto shape : prims_) {
                if (shape.second->OnMouseMove(evt)) {
                    return true;
                }
            }

            return true;
        };

        virtual void Redraw() override {
            for (auto shape : prims_) {
                shape.second->DrawOn(*texture);
            }

            WidgetContainer::Redraw();
        };

        virtual pp::ControlsTheme GetControlsTheme() const override {
            return kTheme;
        };
        virtual void AddShape(pp::Shape *shape) override {
            prims_[shape] = shape;
        };
        virtual void DelShape(pp::Shape* shape) override {
            delete prims_[shape];
            prims_.erase(shape);
        };
        virtual void SetSelectedShape(pp::Shape *shape) override {
            selected_shape_ = shape;
        };
        virtual pp::Shape *GetSelectedShape() const override {
            return selected_shape_;
        };
        virtual void ShapeChanged(pp::Shape *) override {};
        virtual dr4::Window *GetWindow() override {
            return state->window_;
        };
};

#endif // DORISOVKA_HPP
