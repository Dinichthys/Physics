#ifndef INFO_TABLE_HPP
#define INFO_TABLE_HPP

#include <string>
#include <functional>

#include "colors.hpp"

#include "widget.hpp"
#include "editable_text.hpp"
#include "object.hpp"
#include "button.hpp"
#include "widget_title.hpp"

static const size_t kFieldsNum = 6;
static const size_t kTextScale = 5;

static const std::string kInfoTableName = "InfoTable";

static const std::string kFontFileNameInfoTable = "data/font.ttf";

static const std::string kCoordinatesFieldStartStr = "Coors: ";
static const std::string kXCoorStartStr = "\n\t X : ";
static const std::string kYCoorStartStr = "\n\t Y : ";
static const std::string kZCoorStartStr = "\n\t Z : ";

static const std::string kColorFieldStartStr = "Color: #";

static const std::string kCoeffReflectionFieldStartStr = "Coeff Reflect: ";

static const std::string kCoeffRefractionFieldStartStr = "Coeff Refract: ";

static const std::string kCoeffAbsorptionFieldStartStr = "Coeff Absorb: ";

static const size_t kNumLenInfoTable = 30;
static const size_t kHexBase = 16;

static const colors::Color kInfoTableColor = kTitleBackgroundColor;

class SceneManager;

class InfoTable : public WidgetContainer {
    private:
        Text text_type_;

        Text text_center_;
        EditableText val_text_center_x_;
        EditableText val_text_center_y_;
        EditableText val_text_center_z_;

        Text text_color_;
        EditableText val_text_color_;

        Text text_coeff_reflection_;
        EditableText val_text_coeff_reflection_;

        Text text_coeff_refraction_;
        EditableText val_text_coeff_refraction_;

        Text text_coeff_absorption_;
        EditableText val_text_coeff_absorption_;

        const Object* object_;

        dr4::Rectangle* rect_;

        SceneManager* manager_;

        dr4::Texture* base_texture_;

        std::vector<Text*> additional_texts_;
        std::vector<EditableText*> additional_params_;
        std::vector<std::function<void(EditableText*, SceneManager*)>> call_if_change_;
        std::vector<std::function<std::string(const  Object*)>> call_if_nothing_;

        Title* title_;

    public:
        explicit InfoTable(const Coordinates& lt_corner, float width, float height, const Object* object,
                           hui::State* state,
                           Widget* parent = NULL, const std::vector<Widget*>* buttons = NULL)
            :WidgetContainer(lt_corner, width, height, state, buttons, parent),
             text_type_            (Coordinates(2, 0, 0),
                                    width, height / kFieldsNum, state,
                                    this, TypeToStr(object->GetType()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             text_center_          (Coordinates(2, 0, height / kFieldsNum),
                                    width / 2, height / kFieldsNum, state,
                                    this, kCoordinatesFieldStartStr
                                    + kXCoorStartStr + kYCoorStartStr + kZCoorStartStr,
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             val_text_center_x_    (Text(Coordinates(2, width / 2, height / kFieldsNum + height / kFieldsNum / 4),
                                    width / 2, height / kFieldsNum / 4, state,
                                    this, CoordinatesToStrX(object->GetCenterCoordinates()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
             val_text_center_y_    (Text(Coordinates(2, width / 2, height / kFieldsNum + height / kFieldsNum * 2 / 4),
                                    width / 2, height / kFieldsNum / 4, state,
                                    this, CoordinatesToStrY(object->GetCenterCoordinates()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
             val_text_center_z_    (Text(Coordinates(2, width / 2, height / kFieldsNum + height / kFieldsNum * 3 / 4),
                                    width / 2, height / kFieldsNum / 4, state,
                                    this, CoordinatesToStrZ(object->GetCenterCoordinates()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
             text_color_           (Coordinates(2, 0, height / kFieldsNum * 2),
                                    width / 2, height / kFieldsNum, state,
                                    this, kColorFieldStartStr,
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             val_text_color_       (Text(Coordinates(2, width / 2, height / kFieldsNum * 2),
                                    width / 2, height / kFieldsNum, state,
                                    this, ColorToStr(colors::Color(object->GetColor())),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
             text_coeff_reflection_(Coordinates(2, 0, height / kFieldsNum * 3),
                                    width / 2, height / kFieldsNum, state,
                                    this, kCoeffReflectionFieldStartStr,
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             val_text_coeff_reflection_ (Text(Coordinates(2, width / 2, height / kFieldsNum * 3),
                                    width / 2, height / kFieldsNum, state,
                                    this, CoeffReflectionToStr(object->GetCoeffReflection()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
             text_coeff_refraction_(Coordinates(2, 0, height / kFieldsNum * 4),
                                    width / 2, height / kFieldsNum, state,
                                    this, kCoeffRefractionFieldStartStr,
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             val_text_coeff_refraction_ (Text(Coordinates(2, width / 2, height / kFieldsNum * 4),
                                    width / 2, height / kFieldsNum, state,
                                    this, CoeffRefractionToStr(object->GetCoeffRefraction()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
             text_coeff_absorption_(Coordinates(2, 0, height / kFieldsNum * 5),
                                    width / 2, height / kFieldsNum, state,
                                    this, kCoeffAbsorptionFieldStartStr,
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             val_text_coeff_absorption_(Text(Coordinates(2, width / 2, height / kFieldsNum * 5),
                                    width / 2, height / kFieldsNum, state,
                                    this, CoeffAbsorptionToStr(object->GetCoeffAbsorption()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale)),
            rect_((state == NULL) ? NULL : state->window_->CreateRectangle()),
            base_texture_((state == NULL) ? NULL : state->window_->CreateTexture())  {
            Widget::SetLTCorner(Coordinates(2, relPos.x, relPos.y + kTitleHeight));
            if (rect_ != NULL) {
                rect_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget});
                rect_->SetSize(dr4::Vec2f{width, height} - dr4::Vec2f(kBorderThicknessWidget, kBorderThicknessWidget) * 2);
                rect_->SetFillColor(kInfoTableColor);
                rect_->SetBorderColor(kTitleBorderColor);
                rect_->SetBorderThickness(kBorderThicknessWidget);
            }

            if (base_texture_ != NULL) {
                base_texture_->SetPos({});
                base_texture_->SetSize(width, height);
                texture->SetSize({width, height + kTitleHeight});
                texture->SetPos({relPos.x, relPos.y - kTitleHeight});
            }

            object_ = object;

            manager_ = NULL;

            title_ = new Title(Coordinates(2), width, state, this, kInfoTableName, this);

            SetParentToChildren();
        };

        ~InfoTable() {
            delete rect_;
        }

        void SetManager(SceneManager* manager) {
            manager_ = manager;
        }

        const std::string TypeToStr(ObjectType type) {
        #define CASE_TYPE_TO_STRING_(type) \
            case k ## type : {             \
                return "Type: "#type;      \
            }

            switch(type) {
                CASE_TYPE_TO_STRING_(Sphere);
                CASE_TYPE_TO_STRING_(Light);
                CASE_TYPE_TO_STRING_(Plane);
                CASE_TYPE_TO_STRING_(TrianglesSet);
                CASE_TYPE_TO_STRING_(Border);

                case kAllTypes :
                default:
                    return "Type: Invalid";
            };
        #undef CASE_TYPE_STRING_
        };

        const std::string CoordinatesToStrX(const Coordinates& center) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", center[0]);
            str.append(num);

            return str;
        };
        const std::string CoordinatesToStrY(const Coordinates& center) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", center[1]);
            str.append(num);

            return str;
        };
        const std::string CoordinatesToStrZ(const Coordinates& center) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", center[2]);
            str.append(num);

            return str;
        };

        const std::string ColorToStr(const colors::Color& color) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%x%x", uint8_t(color.r / kHexBase), uint8_t(uint8_t(color.r) % kHexBase));
            str.append(num);
            sprintf(num, "%x%x", uint8_t(color.g / kHexBase), uint8_t(uint8_t(color.g) % kHexBase));
            str.append(num);
            sprintf(num, "%x%x", uint8_t(color.b / kHexBase), uint8_t(uint8_t(color.b) % kHexBase));
            str.append(num);
            sprintf(num, " %d", uint8_t(color.a));
            str.append(num);

            return str;
        };

        const std::string CoeffReflectionToStr(float coeff_reflection) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", coeff_reflection);
            str.append(num);

            return str;
        };

        const std::string CoeffRefractionToStr(float coeff_refraction) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", coeff_refraction);
            str.append(num);

            return str;
        };

        const std::string CoeffAbsorptionToStr(float coeff_absorption) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", coeff_absorption);
            str.append(num);

            return str;
        };

        void SetObject(const Object* object) {
            object_ = object;
            if (object == NULL) {
                ResetParam();
                return;
            }
            text_type_.SetText(TypeToStr(object_->GetType()));

            ResetParam();

            object->OnSelect(this);
        };

        void SetState(hui::State* state_) {
            WidgetContainer::SetState(state_);

            if (rect_ == NULL) {
                delete rect_;
            }

            rect_ = state->window_->CreateRectangle();

            rect_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget});
            rect_->SetSize(Widget::GetSize() - dr4::Vec2f(kBorderThicknessWidget, kBorderThicknessWidget) * 2);
            rect_->SetFillColor(kInfoTableColor);
            rect_->SetBorderColor(kTitleBorderColor);
            rect_->SetBorderThickness(kBorderThicknessWidget);

            if (base_texture_ != NULL) {
                delete base_texture_;
            }

            base_texture_ = state->window_->CreateTexture();

            base_texture_->SetPos({0, kTitleHeight});
            base_texture_->SetSize(texture->GetSize());
            texture->SetSize(Widget::GetWidth(), Widget::GetHeight() + kTitleHeight);
            texture->SetPos({relPos.x, relPos.y - kTitleHeight});

            text_type_.SetState(state);

            text_center_.SetState(state);
            val_text_center_x_.SetState(state);
            val_text_center_y_.SetState(state);
            val_text_center_z_.SetState(state);

            text_color_.SetState(state);
            val_text_color_.SetState(state);

            text_coeff_reflection_.SetState(state);
            val_text_coeff_reflection_.SetState(state);

            text_coeff_refraction_.SetState(state);
            val_text_coeff_refraction_.SetState(state);

            text_coeff_absorption_.SetState(state);
            val_text_coeff_absorption_.SetState(state);

            title_->SetState(state);
        };

        virtual void Redraw() override;

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) {
            if (hidden_) {
                return false;
            }
            if (title_->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }
            Coordinates loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y - kTitleHeight);
            Coordinates button_loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y);
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                for (auto child : WidgetContainer::GetChildren()) {
                    if (child->OnMousePress(button_loc_coors, type)) {
                        return true;
                    }
                }

                if (val_text_center_x_.OnMousePress(loc_coors, type)) {
                    return true;
                }
                if (val_text_center_y_.OnMousePress(loc_coors, type)) {
                    return true;
                }
                if (val_text_center_z_.OnMousePress(loc_coors, type)) {
                    return true;
                }
                if (val_text_color_.OnMousePress(loc_coors, type)) {
                    return true;
                }
                if (val_text_coeff_reflection_.OnMousePress(loc_coors, type)) {
                    return true;
                }
                if (val_text_coeff_refraction_.OnMousePress(loc_coors, type)) {
                    return true;
                }
                if (val_text_coeff_absorption_.OnMousePress(loc_coors, type)) {
                    return true;
                }

                for (auto param : additional_params_) {
                    if (param->OnMousePress(loc_coors, type)) {
                        return true;
                    }
                }

                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (title_->OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc(), delta)) {
                return true;
            }
            return WidgetContainer::OnMouseEnter(mouse_pos, delta);
        }

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (title_->OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }
            return WidgetContainer::OnMouseRelease(mouse_pos, type);
        }

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) {
            if (hidden_) {
                return false;
            }

            if (val_text_center_x_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_text_center_y_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_text_center_z_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_text_color_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_text_coeff_reflection_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_text_coeff_refraction_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_text_coeff_absorption_.OnKeyPressed(evt)) {
                return true;
            }

            for (auto param : additional_params_) {
                if (param->OnKeyPressed(evt)) {
                    return true;
                }
            }

            return false;
        };

        virtual bool OnText(const dr4::Event::TextEvent& evt) {
            if (hidden_) {
                return false;
            }

            if (val_text_center_x_.OnText(evt)) {
                return true;
            }
            if (val_text_center_y_.OnText(evt)) {
                return true;
            }
            if (val_text_center_z_.OnText(evt)) {
                return true;
            }
            if (val_text_color_.OnText(evt)) {
                return true;
            }
            if (val_text_coeff_reflection_.OnText(evt)) {
                return true;
            }
            if (val_text_coeff_refraction_.OnText(evt)) {
                return true;
            }
            if (val_text_coeff_absorption_.OnText(evt)) {
                return true;
            }

            for (auto param : additional_params_) {
                if (param->OnText(evt)) {
                    return true;
                }
            }

            return true;
        };

        virtual const std::string& GetName() const override {
            return kInfoTableName;
        };

        void AddParam(Text* intro, EditableText* changing_param,
                std::function<void(EditableText*, SceneManager*)> call_if_change,
                std::function<std::string(const Object*)> call_if_nothing) {
            additional_texts_.push_back(intro);
            additional_params_.push_back(changing_param);
            call_if_change_.push_back(call_if_change);
            call_if_nothing_.push_back(call_if_nothing);

            intro->SetSize(text_center_.GetSize());
            intro->SetRelPos({0, height_});
            intro->text_->SetFontSize(text_center_.text_->GetFontSize());
            if (changing_param != NULL) {
                changing_param->SetSize(text_center_.GetSize());
                changing_param->SetRelPos({width_ / 2, height_});
                changing_param->text_->SetFontSize(text_center_.text_->GetFontSize());
                changing_param->SetCaretHeight(text_center_.text_->GetFontSize());
            }

            Widget::SetSize(Widget::GetSize() + dr4::Vec2f{0, intro->GetSize().y});

            base_texture_->SetSize(base_texture_->GetSize() + dr4::Vec2f{0, intro->GetSize().y});
            rect_->SetSize(rect_->GetSize() + dr4::Vec2f{0, intro->GetSize().y});
            texture->SetSize(Widget::GetSize() + dr4::Vec2f({0, kTitleHeight}));
        };

        void ResetParam() {
            Widget::SetSize({width_, height_ - additional_params_.size() * text_center_.GetSize().y});
            base_texture_->SetSize(base_texture_->GetSize() - dr4::Vec2f{0, text_center_.GetSize().y} * additional_params_.size());
            rect_->SetSize(rect_->GetSize() - dr4::Vec2f{0, text_center_.GetSize().y} * additional_params_.size());
            texture->SetSize(Widget::GetSize() + dr4::Vec2f({0, kTitleHeight}));

            additional_params_.clear();
            additional_texts_.clear();
            call_if_change_.clear();
            call_if_nothing_.clear();
        };
};

#endif // INFO_TABLE_HPP
