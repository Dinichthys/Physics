#ifndef INFO_TABLE_HPP
#define INFO_TABLE_HPP

#include <string>

#include "graphics.hpp"
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

static const graphics::Color kInfoTableColor = graphics::Color(30, 30, 32);

class InfoTable : public Widget {
    private:
        Text text_type_;
        Text text_center_;
        Text text_color_;
        Text text_coeff_reflection_;
        Text text_coeff_refraction_;
        Text text_coeff_absorption_;

        const Object* object_;

        graphics::RectangleShape rect_;

    public:
        explicit InfoTable(const Coordinates& lt_corner, float width, float height, const Object* object,
                           Widget* parent = NULL)
            :Widget(lt_corner, width, height, parent),
             text_type_            (Coordinates(2, 0, 0),
                                    width, height / kFieldsNum / kTextScale,
                                    this, TypeToStr(object->GetType()),
                                    kFontFileNameInfoTable),
             text_center_          (Coordinates(2, 0, height / kFieldsNum),
                                    width, height / kFieldsNum / kTextScale,
                                    this, CoordinatesToStr(object->GetCenterCoordinates()),
                                    kFontFileNameInfoTable),
             text_color_           (Coordinates(2, 0, height / kFieldsNum * 2),
                                    width, height / kFieldsNum / kTextScale,
                                    this, ColorToStr(graphics::Color(object->GetColor())),
                                    kFontFileNameInfoTable),
             text_coeff_reflection_(Coordinates(2, 0, height / kFieldsNum * 3),
                                    width, height / kFieldsNum / kTextScale,
                                    this, CoeffReflectionToStr(object->GetCoeffReflection()),
                                    kFontFileNameInfoTable),
             text_coeff_refraction_(Coordinates(2, 0, height / kFieldsNum * 4),
                                    width, height / kFieldsNum / kTextScale,
                                    this, CoeffRefractionToStr(object->GetCoeffRefraction()),
                                    kFontFileNameInfoTable),
             text_coeff_absorption_(Coordinates(2, 0, height / kFieldsNum * 5),
                                    width, height / kFieldsNum / kTextScale,
                                    this, CoeffAbsorptionToStr(object->GetCoeffAbsorption()),
                                    kFontFileNameInfoTable),
             rect_(width, height) {
            rect_.SetFillColor(kInfoTableColor);
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

        const std::string ColorToStr(const graphics::Color& color) {
            std::string str;
            str.append(kColorFieldStartStr);
            char num[kNumLenInfoTable] = "";
            sprintf(num, "%x%x", uint8_t(color[0] / kHexBase), uint8_t(uint8_t(color[0]) % kHexBase));
            str.append(num);
            sprintf(num, "%x%x", uint8_t(color[1] / kHexBase), uint8_t(uint8_t(color[1]) % kHexBase));
            str.append(num);
            sprintf(num, "%x%x", uint8_t(color[2] / kHexBase), uint8_t(uint8_t(color[2]) % kHexBase));
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

        virtual void Draw(graphics::RenderWindow* window) override {
            rect_.SetPosition(Widget::GetLTCornerAbs());
            window->Draw(rect_);
            text_type_.Draw(window);
            text_center_.SetText(CoordinatesToStr(object_->GetCenterCoordinates()));
            text_center_.Draw(window);
            text_color_.Draw(window);
            text_coeff_reflection_.SetText(CoeffReflectionToStr(object_->GetCoeffReflection()));
            text_coeff_reflection_.Draw(window);
            text_coeff_refraction_.SetText(CoeffRefractionToStr(object_->GetCoeffRefraction()));
            text_coeff_refraction_.Draw(window);
            text_coeff_absorption_.SetText(CoeffAbsorptionToStr(object_->GetCoeffAbsorption()));
            text_coeff_absorption_.Draw(window);
        };
};

#endif // INFO_TABLE_HPP
