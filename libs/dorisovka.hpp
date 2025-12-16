#ifndef DORISOVKA_HPP
#define DORISOVKA_HPP

#include <vector>
#include <dlfcn.h>
#include <stdexcept>

#include "widget.hpp"
#include "draw.hpp"
#include "pp/canvas.hpp"
#include "pp/shape.hpp"
#include "pp/tool.hpp"
#include "cum/ifc/pp.hpp"
#include "color_picker.hpp"
#include "plugin_upload.hpp"

#include "dr4/math/rect.hpp"

#include "dorisovka_buttons.hpp"

#include "toolbar.hpp"

static const size_t kRectangleID = 0;
static const size_t kCircleID = 1;
static const size_t kArrowID = 2;
static const float kGeomButtonWidth = 60;
static const float kGeomButtonHeight = 25;

static const std::string kFontFileNameGeomPrim = "/usr/share/fonts/TTF/CaskaydiaCoveNerdFontMono-Regular.ttf";
static const std::string kRectangleButtonName = "rect";
static const std::string kCircleButtonName = "circ";
static const std::string kArrowButtonName = "arrow";

static const char* const kDorisovkaPlugNames[3] = {
    "./plugins/build/ppMe.so",
    "./plugins/build/ppArtem.so",
    "./plugins/build/ppSeva.so",
    // "./plugins/build/ppVova.so",
    // "./plugins/build/ppEgor.so"
};
// static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libgeomprim.so";
// static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libgeomprim_image.so";
// static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libpiska.so";
// static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/Artem.so";
// static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/Fedya.so";
// static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libgeomprim_old.so";
// static const char* const kDorisovkaPlugName = "./plugins/optor/build/source/piska/libpiska.so";

static const size_t kDorisovkaBorderThickness = 5;

static colors::Color kGeomPrimButtonColor = colors::Color(49, 49, 49);

class Dorisovka : public Widget, public pp::Canvas {
    private:
        std::vector<cum::PPToolPlugin*> backends_;
        std::vector<pp::Shape*> prims_;
        std::vector<std::unique_ptr<pp::Tool>> tools_;

        pp::Shape* selected_shape_;
        pp::Tool* selected_tool_;

        dr4::Rectangle* border_;

        ColorPicker* color_picker_;

        ToolBar* toolbar_;

        PluginUploadButton* plugin_upload_button_;

    public:
        pp::ControlsTheme theme_ = {.shapeFillColor = colors::Color(0, 0, 0, 0),
                                          .shapeBorderColor = colors::kColorRed,
                                          .selectColor = colors::kColorGreen,
                                          .textColor = colors::kColorGreen,
                                          .baseFontSize = 20,
                                          .handleColor = colors::kColorYellow,
                                          .handleHoverColor = colors::kColorBlue,
                                          .handleActiveColor = colors::kColorRed};

        explicit Dorisovka(const Coordinates& lt_corner, dr4::Vec2f size, hui::State* state = NULL)
            :Widget(lt_corner,  size.x, size.y, state) {
            LoadPPBackends();

            toolbar_ = new ToolBar(Coordinates(2, 0, 0), this, state, this);
            state->ui->main_field_->AddChild(toolbar_);

            for (size_t i = 0; i < tools_.size(); i++) {
            toolbar_->AddButton(new GeomPrimCreationButton(Button(
                Coordinates(2,
                    (i + 1) * (size.x - kGeomButtonWidth * tools_.size()) / (tools_.size() + 1)
                    + kGeomButtonWidth * i,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight,
                std::string(std::string(" ") + std::string(tools_[i].get()->Icon())),
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                    this->CreatePrim(id);
                }, i));
            }

            plugin_upload_button_ = new PluginUploadButton(Coordinates(2, kTopBarButtonWidth, 0), state, NULL);
            state->ui->topbar_->AddChild(plugin_upload_button_);

            selected_shape_ = NULL;
            selected_tool_ = NULL;

            border_ = state->window_->CreateRectangle();
            border_->SetBorderColor(colors::kColorOrange);
            border_->SetPos({kDorisovkaBorderThickness, kDorisovkaBorderThickness});
            border_->SetFillColor(colors::Color(0, 0, 0, 0));
            border_->SetBorderThickness(kDorisovkaBorderThickness);
            border_->SetSize(size - dr4::Vec2f{kDorisovkaBorderThickness, kDorisovkaBorderThickness} * 2);
        };

        ~Dorisovka() {
            for (auto prim : prims_) {
                DelShape(prim);
            }

            std::vector<Widget*>& children_main_field = state->ui->main_field_->GetChildren();
            for (size_t i = 0; i < children_main_field.size(); i++) {
                if (children_main_field[i] == toolbar_) {
                    children_main_field.erase(children_main_field.begin() + i);
                    delete toolbar_;
                    toolbar_ = NULL;
                    break;
                }
            }

            std::vector<Widget*>& children_topbar = state->ui->topbar_->GetChildren();
            for (size_t i = 0; i < children_topbar.size(); i++) {
                if (children_topbar[i] == plugin_upload_button_) {
                    children_topbar.erase(children_topbar.begin() + i);
                    delete plugin_upload_button_;
                    plugin_upload_button_ = NULL;
                    break;
                }
            }
        };

        void CreatePrim(size_t id) {
            if (selected_shape_ != NULL) {
                DelShape(selected_shape_);
                selected_shape_ = NULL;
            }
            if (selected_tool_ != NULL) {
                selected_tool_->OnEnd();
            }
            if (selected_tool_ == tools_[id].get()) {
                selected_tool_ = NULL;
                return;
            }

            tools_[id].get()->OnStart();
            selected_tool_ = tools_[id].get();
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if ((!Widget::OnMousePress(mouse_pos, type))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                bool res = Widget::OnMousePress(mouse_pos, type);
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }
                return res;
            }
            if (state->target_widget_ == this) {
                state->target_widget_ = NULL;
            }

            dr4::Event::MouseButton evt;
            evt.button = type;
            evt.pos = {mouse_pos[0] - Widget::GetLTCornerLoc()[0], mouse_pos[1] - Widget::GetLTCornerLoc()[1]};
            if (selected_tool_ != NULL) {
                if (selected_tool_->OnMouseDown(evt)) {
                    return true;
                }
            }

            if (selected_shape_ != NULL) {
                if (selected_shape_->OnMouseDown(evt)) {
                    return true;
                }
            }

            for (auto shape : prims_) {
                if (shape->OnMouseDown(evt)) {
                    return true;
                }
            }

            return true;
        };
        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (((selected_shape_ == NULL) && (selected_tool_ == NULL))
                || (!Widget::OnMouseRelease(mouse_pos, type))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return Widget::OnMouseRelease(mouse_pos, type);
            }

            dr4::Event::MouseButton evt;
            evt.button = type;
            evt.pos = {mouse_pos[0] - Widget::GetLTCornerLoc()[0], mouse_pos[1] - Widget::GetLTCornerLoc()[1]};
            if (selected_tool_ != NULL) {
                if (selected_tool_->OnMouseUp(evt)) {
                    return true;
                }
            }

            if (selected_shape_ != NULL) {
                if (selected_shape_->OnMouseUp(evt)) {
                    return true;
                }
            }

            for (auto shape : prims_) {
                if (shape->OnMouseUp(evt)) {
                    return true;
                }
            }

            return true;
        };
        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (((selected_shape_ == NULL) && (selected_tool_ == NULL))
                || (!Widget::OnMouseEnter(mouse_pos, delta))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return Widget::OnMouseEnter(mouse_pos, delta);
            }

            dr4::Event::MouseMove evt;
            evt.pos = {mouse_pos[0] - Widget::GetLTCornerLoc()[0], mouse_pos[1] - Widget::GetLTCornerLoc()[1]};
            evt.rel = {delta[0], delta[1]};
            if (selected_tool_ != NULL) {
                if (selected_tool_->OnMouseMove(evt)) {
                    return true;
                }
            }

            if (selected_shape_ != NULL) {
                if (selected_shape_->OnMouseMove(evt)) {
                    return true;
                }
            }

            for (auto shape : prims_) {
                if (shape->OnMouseMove(evt)) {
                    return true;
                }
            }

            return true;
        };

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) override {
            if (selected_tool_) {
                return selected_tool_->OnKeyDown(evt);
            }
            if (selected_shape_) {
                return selected_shape_->OnKeyDown(evt);
            }
            return false;
        }
        virtual bool OnKeyUp(const dr4::Event::KeyEvent& evt) override {
            if (selected_tool_) {
                return selected_tool_->OnKeyUp(evt);
            }
            if (selected_shape_) {
                return selected_shape_->OnKeyUp(evt);
            }
            return false;
        }
        virtual bool OnText(const dr4::Event::TextEvent& evt) override {
            if (selected_tool_) {
                return selected_tool_->OnText(evt);
            }
            if (selected_shape_) {
                return selected_shape_->OnText(evt);
            }
            return false;
        }
        virtual bool OnIdle(const hui::IdleEvent &idle_evt) {
            pp::IdleEvent evt;
            evt.absTime = idle_evt.absTime;
            evt.deltaTime = idle_evt.deltaTime;
            if (selected_tool_) {
                return selected_tool_->OnIdle(evt);
            }
            if (selected_shape_) {
                return selected_shape_->OnIdle(evt);
            }
            return false;
        }

        virtual void Redraw() override {
            for (auto shape : prims_) {
                shape->DrawOn(*texture);
            }

            border_->DrawOn(*texture);

            Widget::Redraw();
        };

        virtual pp::ControlsTheme GetControlsTheme() const override {
            return theme_;
        };

        virtual void AddShape(pp::Shape *shape) override {
            prims_.push_back(shape);
        };
        virtual void DelShape(pp::Shape* shape) override {
            if (selected_shape_ == shape) {
                selected_shape_ = NULL;
            }

            for (size_t i = 0; i < prims_.size(); i++) {
                if (prims_[i] == shape) {
                    delete prims_[i];
                    prims_.erase(prims_.begin() + i);
                    return;
                }
            }
        };
        virtual void SetSelectedShape(pp::Shape *shape) override {
            if (selected_shape_ && (shape != selected_shape_)) {
                selected_shape_->OnDeselect();
            }
            selected_shape_ = shape;
            if (shape) {
                shape->OnSelect();
            }
        };
        virtual pp::Shape *GetSelectedShape() const override {
            return selected_shape_;
        };
        virtual void ShapeChanged(pp::Shape *) override {};
        virtual dr4::Window *GetWindow() override {
            return state->window_;
        };

    private:
        void LoadPPBackends() {
            for (auto& plug_name : kDorisovkaPlugNames) {
                backends_.push_back(dynamic_cast<cum::PPToolPlugin*>(state->manager.LoadFromFile(plug_name)));
            }

            for (auto& back : backends_) {
                auto tools = back->CreateTools(this);
                for (auto& tool : tools) {
                    tools_.push_back(std::move(tool));
                }
            }
        }
};

#endif // DORISOVKA_HPP
