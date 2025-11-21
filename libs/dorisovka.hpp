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

static const std::string kFontFileNameGeomPrim = "data/font.ttf";
static const std::string kRectangleButtonName = "rect";
static const std::string kCircleButtonName = "circ";
static const std::string kArrowButtonName = "arrow";

static const char* const kDorisovkaPlugName = "./plugins/MyGeomPrimBackend/build/libgeomprim.so";

static colors::Color kGeomPrimButtonColor = colors::Color(49, 49, 49);

class Dorisovka : public WidgetContainer, public pp::Canvas {
    private:
        cum::PPToolPlugin* backend_;
        std::unordered_map<size_t, pp::Shape*> prims_;
        std::vector<std::unique_ptr<pp::Tool>> tools_;
        pp::State pp_state_;
        size_t unique_id_in_map_;

        const pp::ControlsTheme kTheme = {.shapeColor = colors::Color(0, 0, 0, 0),
                                          .lineColor = colors::kColorGreen,
                                          .textColor = colors::kColorBlack,
                                          .baseFontSize = 10,
                                          .handleColor = colors::kColorYellow};

    public:
        explicit Dorisovka(const Coordinates& lt_corner, dr4::Vec2f size,
            const char* plugin_name, hui::State* state = NULL)
            :WidgetContainer(lt_corner,  size.x, size.y, state),
             backend_(dynamic_cast<cum::PPToolPlugin*>(state->manager.LoadFromFile(plugin_name))),
             tools_(backend_->CreateTools(this)) {
            std::vector<Widget*> buttons;

            buttons.push_back(new GeomPrimCreationButton(Button(
                lt_corner + Coordinates(2, (size.x - kGeomButtonWidth * 3) / 4,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight,
                std::string(tools_[kRectangleID].get()->Name()),
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                this->CreatePrim(id);
              }, kRectangleID));
            buttons.push_back(new GeomPrimCreationButton(Button(
                lt_corner + Coordinates(2, 2 * (size.x - kGeomButtonWidth * 3) / 4 + kGeomButtonWidth,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight,
                std::string(tools_[kCircleID].get()->Name()),
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                this->CreatePrim(id);
              }, kCircleID));
            buttons.push_back(new GeomPrimCreationButton(Button(
                lt_corner + Coordinates(2, 3 * (size.x - kGeomButtonWidth * 3) / 4 + kGeomButtonWidth * 2,
                (kGeomPrimPanelControlHeight - kGeomButtonHeight) / 2),
                kGeomButtonWidth, kGeomButtonHeight,
                std::string(tools_[kArrowID].get()->Name()),
                kFontFileNameGeomPrim, state, this, kGeomPrimButtonColor, kGeomPrimButtonColor),
                [this] (size_t id) {
                this->CreatePrim(id);
              }, kArrowID));
            WidgetContainer::AddChild(
                new PanelControl(lt_corner + Coordinates(2, 0, size.y - kGeomPrimPanelControlHeight),
                size.x, kGeomPrimPanelControlHeight, state, &buttons
            ));

            unique_id_in_map_ = 0;
            pp_state_.selectedShape = NULL;
            pp_state_.selectedTool = NULL;
        };

        ~Dorisovka() {
            while (!(prims_.empty())) {
                delete prims_.begin()->second;
                prims_.erase(prims_.begin());
            }
        };

        void CreatePrim(size_t id) {
            if (pp_state_.selectedShape != NULL) {
                delete pp_state_.selectedShape;
            }
            if (pp_state_.selectedTool != NULL) {
                delete pp_state_.selectedTool;
            }
            tools_[id].get()->OnStart();
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if (((pp_state_.selectedShape == NULL) && (pp_state_.selectedTool == NULL))
                || (!Widget::OnMousePress(mouse_pos))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                bool res = WidgetContainer::OnMousePress(mouse_pos);
                if (state->target_widget_ == this) {state->target_widget_ = NULL;}
                return res;
            }
            if (state->target_widget_ == this) {state->target_widget_ = NULL;}

            dr4::Event::MouseButton evt;
            evt.button = dr4::MouseButtonType::UNKNOWN;
            evt.pos = {mouse_pos[0], mouse_pos[1]};
            if (pp_state_.selectedTool != NULL) {
                pp_state_.selectedTool->OnMouseDown(evt);
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
            if (((pp_state_.selectedShape == NULL) && (pp_state_.selectedTool == NULL))
                || (!Widget::OnMouseRelease(mouse_pos))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return WidgetContainer::OnMouseRelease(mouse_pos);
            }

            dr4::Event::MouseButton evt;
            evt.button = dr4::MouseButtonType::UNKNOWN;
            evt.pos = {mouse_pos[0], mouse_pos[1]};
            if (pp_state_.selectedTool != NULL) {
                pp_state_.selectedTool->OnMouseUp(evt);
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
            if (((pp_state_.selectedShape == NULL) && (pp_state_.selectedTool == NULL))
                || (!Widget::OnMouseEnter(mouse_pos))
                || (mouse_pos[1] + kGeomPrimPanelControlHeight > Widget::GetHeight())) {
                return WidgetContainer::OnMouseEnter(mouse_pos);
            }

            dr4::Event::MouseMove evt;
            evt.pos = {mouse_pos[0], mouse_pos[1]};
            if (pp_state_.selectedTool != NULL) {
                pp_state_.selectedTool->OnMouseMove(evt);
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
        virtual pp::State* GetState() override {
            return &pp_state_;
        };
        virtual size_t AddShape(pp::Shape *shape) override {
            prims_[unique_id_in_map_++] = shape;
            return unique_id_in_map_ - 1;
        };
        virtual void DelShape(size_t ind) override {
            delete prims_[ind];
            prims_.erase(ind);
        };
        virtual dr4::Window *GetWindow() override {
            return state->window_;
        };
};

#endif // DORISOVKA_HPP
