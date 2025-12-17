#include "triangle.hpp"

#include "info_table.hpp"
#include "scene_manager.hpp"

#include <string>

static const float kStartSizeText = 100;
static const size_t kNumMaxLen = 30;

static std::string GetNormalStr(const Coordinates& normal) {
    char num[kNumMaxLen] = "";

    std::string res("");
    sprintf(num, "%lg", normal[0]);
    res.append(num);
    res.append(", ");
    sprintf(num, "%lg", normal[1]);
    res.append(num);
    res.append(", ");
    sprintf(num, "%lg", normal[2]);
    res.append(num);

    return res;
};

static Coordinates StrToCoord(const std::string& str) {
    float x = 0, y = 0, z = 0;

    sscanf(str.c_str(), "%f, %f, %f", &x, &y, &z);
    return Coordinates(3, x, y, z);
};

void Triangle::OnSelect(InfoTable* table) const {
    table->AddParam(
        new Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table, "A:",
            kFontFileNameInfoTable),
        new EditableText(Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table,
            GetNormalStr(Plane::GetCenterCoordinates()), kFontFileNameInfoTable)),
        [](EditableText* param_text, SceneManager* manager) {
            Triangle* triangle = dynamic_cast<Triangle*>(manager->GetCurObject());
            triangle->SetCenterCoordinates(StrToCoord(param_text->GetText()));
            triangle->UpdateNormal();
            manager->SetChanged(true);
            param_text->SetChanged(false);
        },
        [](const Object* obj) -> std::string {
            const Triangle* triangle = dynamic_cast<const Triangle*>(obj);
            return GetNormalStr(triangle->GetCenterCoordinates());
        });

    table->AddParam(
        new Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table, "B:",
            kFontFileNameInfoTable),
        new EditableText(Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table,
            GetNormalStr(point_b_), kFontFileNameInfoTable)),
        [](EditableText* param_text, SceneManager* manager) {
            Triangle* triangle = dynamic_cast<Triangle*>(manager->GetCurObject());
            triangle->point_b_ = StrToCoord(param_text->GetText());
            triangle->UpdateNormal();
            manager->SetChanged(true);
            param_text->SetChanged(false);
        },
        [](const Object* obj) -> std::string {
            const Triangle* triangle = dynamic_cast<const Triangle*>(obj);
            return GetNormalStr(triangle->point_b_);
        });

    table->AddParam(
        new Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table, "C:",
            kFontFileNameInfoTable),
        new EditableText(Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table,
            GetNormalStr(point_c_), kFontFileNameInfoTable)),
        [](EditableText* param_text, SceneManager* manager) {
            Triangle* triangle = dynamic_cast<Triangle*>(manager->GetCurObject());
            triangle->point_c_ = StrToCoord(param_text->GetText());
            triangle->UpdateNormal();
            manager->SetChanged(true);
            param_text->SetChanged(false);
        },
        [](const Object* obj) -> std::string {
            const Triangle* triangle = dynamic_cast<const Triangle*>(obj);
            return GetNormalStr(triangle->point_c_);
        });
}
