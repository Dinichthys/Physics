#include "circle.hpp"
#include "info_table.hpp"
#include "scene_manager.hpp"

#include <string>

static const float kStartSizeText = 100;

void Circle::OnSelect(InfoTable* table) const {
    table->AddParam(
        new Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table, "Radius:",
            kFontFileNameInfoTable),
        new EditableText(Text(Coordinates(2), kStartSizeText, kStartSizeText, table->GetState(), table,
            std::to_string(radius_), kFontFileNameInfoTable)),
        [](EditableText* param_text, SceneManager* manager) {
            Circle* circle = dynamic_cast<Circle*>(manager->GetCurObject());
            circle->radius_ = atof(param_text->GetText().c_str());
            manager->SetChanged(true);
            param_text->SetChanged(false);
        },
        [](const Object* obj) -> std::string {
            const Circle* circle = dynamic_cast<const Circle*>(obj);
            return std::to_string(circle->radius_);
        });
}
