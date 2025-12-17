#include "plane.hpp"

#include "info_table.hpp"
#include "scene_manager.hpp"
#include "border.hpp"

#include <string>

static const float kStartSizeText = 100;
static const size_t kNumMaxLen = 30;

Border* Plane::GetBorder() const {
    float size = 2 * (!normal_).GetModule();
    return new Border(Object::GetCenterCoordinates(), 2 * size, 2 * size, 2 * size);
};

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

void Plane::OnSelect(InfoTable* table) const {
    table->AddParam(
        new Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table, "Normal:",
            kFontFileNameInfoTable),
        new EditableText(Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table,
            GetNormalStr(normal_), kFontFileNameInfoTable)),
        [](EditableText* param_text, SceneManager* manager) {
            Plane* plane = dynamic_cast<Plane*>(manager->GetCurObject());
            plane->normal_ = StrToCoord(param_text->GetText());
            manager->SetChanged(true);
            param_text->SetChanged(false);
        },
        [](const Object* obj) -> std::string {
            const Plane* plane = dynamic_cast<const Plane*>(obj);
            return GetNormalStr(plane->normal_);
        });
}
