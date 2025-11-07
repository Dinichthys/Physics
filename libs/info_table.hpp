#ifndef INFO_TABLE_HPP
#define INFO_TABLE_HPP

#include <string>

#include "colors.hpp"

#include "widget.hpp"
#include "text.hpp"
#include "object.hpp"

static const size_t kFieldsNum = 6;
static const size_t kTextScale = 5;
static const std::string kFontFileNameInfoTable = "data/font.ttf";

static const std::string kCoordinatesFieldStartStr = "Coors: ";
static const std::string kXCoorStartStr = "\n\t X = ";
static const std::string kYCoorStartStr = "\n\t Y = ";
static const std::string kZCoorStartStr = "\n\t Z = ";

static const std::string kColorFieldStartStr = "Color: #";

static const std::string kCoeffReflectionFieldStartStr = "Coeff Reflect: ";

static const std::string kCoeffRefractionFieldStartStr = "Coeff Refract: ";

static const std::string kCoeffAbsorptionFieldStartStr = "Coeff Absorb: ";

static const size_t kNumLenInfoTable = 30;
static const size_t kHexBase = 16;

static const colors::Color kInfoTableColor = colors::Color(30, 30, 32);

class InfoTable : public Widget {
    private:
        Text text_type_;
        Text text_center_;
        Text text_color_;
        Text text_coeff_reflection_;
        Text text_coeff_refraction_;
        Text text_coeff_absorption_;

        const Object* object_;

        dr4::Rectangle rect_;

    public:
        explicit InfoTable(const Coordinates& lt_corner, float width, float height, const Object* object,
                           hui::State* state,
                           Widget* parent = NULL)
            :Widget(lt_corner, width, height, state, parent),
             text_type_            (Coordinates(2, 0, 0),
                                    width, height / kFieldsNum, state,
                                    this, TypeToStr(object->GetType()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             text_center_          (Coordinates(2, 0, height / kFieldsNum),
                                    width, height / kFieldsNum, state,
                                    this, CoordinatesToStr(object->GetCenterCoordinates()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             text_color_           (Coordinates(2, 0, height / kFieldsNum * 2),
                                    width, height / kFieldsNum, state,
                                    this, ColorToStr(colors::Color(object->GetColor())),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             text_coeff_reflection_(Coordinates(2, 0, height / kFieldsNum * 3),
                                    width, height / kFieldsNum, state,
                                    this, CoeffReflectionToStr(object->GetCoeffReflection()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             text_coeff_refraction_(Coordinates(2, 0, height / kFieldsNum * 4),
                                    width, height / kFieldsNum, state,
                                    this, CoeffRefractionToStr(object->GetCoeffRefraction()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale),
             text_coeff_absorption_(Coordinates(2, 0, height / kFieldsNum * 5),
                                    width, height / kFieldsNum, state,
                                    this, CoeffAbsorptionToStr(object->GetCoeffAbsorption()),
                                    kFontFileNameInfoTable, height / kFieldsNum / kTextScale) {
            rect_.rect.pos = {0, 0}; rect_.rect.size = {width, height};
            rect_.fill = kInfoTableColor;
            rect_.borderColor = dr4::Color(0, 0, 0, 0); rect_.borderThickness = 0;

            object_ = object;
        };

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

        const std::string CoordinatesToStr(const Coordinates& center) {
            std::string str;
            str.append(kCoordinatesFieldStartStr);
            str.append(kXCoorStartStr);
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%10.2f", center[0]);
            str.append(num);
            str.append(kYCoorStartStr);
            sprintf(num, "%10.2f", center[1]);
            str.append(num);
            str.append(kZCoorStartStr);
            sprintf(num, "%10.2f", center[2]);
            str.append(num);

            return str;
        };

        const std::string ColorToStr(const colors::Color& color) {
            std::string str;
            str.append(kColorFieldStartStr);
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%x%x", uint8_t(color.r / kHexBase), uint8_t(uint8_t(color.r) % kHexBase));
            str.append(num);
            sprintf(num, "%x%x", uint8_t(color.g / kHexBase), uint8_t(uint8_t(color.g) % kHexBase));
            str.append(num);
            sprintf(num, "%x%x", uint8_t(color.b / kHexBase), uint8_t(uint8_t(color.b) % kHexBase));
            str.append(num);

            return str;
        };

        const std::string CoeffReflectionToStr(float coeff_reflection) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            str.append(kCoeffReflectionFieldStartStr);
            sprintf(num, "%10.2f", coeff_reflection);
            str.append(num);

            return str;
        };

        const std::string CoeffRefractionToStr(float coeff_refraction) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            str.append(kCoeffRefractionFieldStartStr);
            sprintf(num, "%10.2f", coeff_refraction);
            str.append(num);

            return str;
        };

        const std::string CoeffAbsorptionToStr(float coeff_absorption) {
            std::string str;
            char num[kNumLenInfoTable] = "";
            str.append(kCoeffAbsorptionFieldStartStr);
            sprintf(num, "%10.2f", coeff_absorption);
            str.append(num);

            return str;
        };

        void SetObject(const Object* object) {
            object_ = object;
            if (object == NULL) {
                return;
            }
            text_color_.SetText(ColorToStr(colors::Color(object_->GetColor())));
            text_type_.SetText(TypeToStr(object_->GetType()));
        };

        void SetState(hui::State* state_) {
            state = state_;
            text_type_.SetState(state);
            text_color_.SetState(state);
            text_center_.SetState(state);
            text_coeff_absorption_.SetState(state);
            text_coeff_reflection_.SetState(state);
            text_coeff_refraction_.SetState(state);
            Widget::SetState(state);
        };

        virtual void Redraw() override {
            if (Widget::GetHidden()) {
                return;
            }
            texture->Draw(rect_);
            text_type_.Redraw();
            text_center_.SetText(CoordinatesToStr(object_->GetCenterCoordinates()));
            text_center_.Redraw();
            text_color_.Redraw();
            text_coeff_reflection_.SetText(CoeffReflectionToStr(object_->GetCoeffReflection()));
            text_coeff_reflection_.Redraw();
            text_coeff_refraction_.SetText(CoeffRefractionToStr(object_->GetCoeffRefraction()));
            text_coeff_refraction_.Redraw();
            text_coeff_absorption_.SetText(CoeffAbsorptionToStr(object_->GetCoeffAbsorption()));
            text_coeff_absorption_.Redraw();
            Widget::Redraw();
        };
};

#endif // INFO_TABLE_HPP
