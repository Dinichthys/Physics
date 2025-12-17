#include "info_table.hpp"

#include "scene_manager.hpp"

void InfoTable::Redraw() {
    if (Widget::GetHidden() || (object_ == NULL)) {
        return;
    }

    texture->Draw(*rect_);

    text_type_.Redraw();

    text_center_.Redraw();
    if (!val_text_center_x_.GetPrintingInfo()) {
        if (val_text_center_x_.GetChanged()) {
            auto old_coors(object_->GetCenterCoordinates());
            manager_->MoveCurrentObject(Coordinates(3, atof(val_text_center_x_.GetText().c_str()) - old_coors[0], 0, 0));
            val_text_center_x_.SetChanged(false);
        } else {
            val_text_center_x_.SetText(CoordinatesToStrX(object_->GetCenterCoordinates()));
        }
    }
    val_text_center_x_.Redraw();
    if (!val_text_center_y_.GetPrintingInfo()) {
        if (val_text_center_y_.GetChanged()) {
            auto old_coors(object_->GetCenterCoordinates());
            manager_->MoveCurrentObject(Coordinates(3, 0, atof(val_text_center_y_.GetText().c_str()) - old_coors[1], 0));
            val_text_center_y_.SetChanged(false);
        } else {
            val_text_center_y_.SetText(CoordinatesToStrY(object_->GetCenterCoordinates()));
        }
    }
    val_text_center_y_.Redraw();
    if (!val_text_center_z_.GetPrintingInfo()) {
        if (val_text_center_z_.GetChanged()) {
            auto old_coors(object_->GetCenterCoordinates());
            manager_->MoveCurrentObject(Coordinates(3, 0, 0, atof(val_text_center_z_.GetText().c_str()) - old_coors[2]));
            val_text_center_z_.SetChanged(false);
        } else {
            val_text_center_z_.SetText(CoordinatesToStrZ(object_->GetCenterCoordinates()));
        }
    }
    val_text_center_z_.Redraw();

    text_color_.Redraw();
    if (!val_text_color_.GetPrintingInfo()) {
        if (val_text_color_.GetChanged()) {
            int32_t color = 0;
            int32_t brightness = 0;
            sscanf(val_text_color_.GetText().c_str(), "%x %d", &color, &brightness);
            manager_->ChangeCurObjColor(colors::Color((color >> 16) & 0xFF, (color >> 8 & 0xFF), color & 0xFF, brightness));
            val_text_color_.SetChanged(false);
        } else {
            val_text_color_.SetText(ColorToStr(manager_->GetCurObjColor()));
        }
    }
    val_text_color_.Redraw();

    text_coeff_reflection_.Redraw();
    if (!val_text_coeff_reflection_.GetPrintingInfo()) {
        if (val_text_coeff_reflection_.GetChanged()) {
            manager_->ChangeCurObjReflection(atof(val_text_coeff_reflection_.GetText().c_str()) - object_->GetCoeffReflection());
            val_text_coeff_reflection_.SetChanged(false);
        } else {
            val_text_coeff_reflection_.SetText(CoeffReflectionToStr(object_->GetCoeffReflection()));
        }
    }
    val_text_coeff_reflection_.Redraw();

    text_coeff_refraction_.Redraw();
    if (!val_text_coeff_refraction_.GetPrintingInfo()) {
        if (val_text_coeff_refraction_.GetChanged()) {
            manager_->ChangeCurObjRefraction(atof(val_text_coeff_refraction_.GetText().c_str()) - object_->GetCoeffRefraction());
            val_text_coeff_refraction_.SetChanged(false);
        } else {
            val_text_coeff_refraction_.SetText(CoeffRefractionToStr(object_->GetCoeffRefraction()));
        }
    }
    val_text_coeff_refraction_.Redraw();

    text_coeff_absorption_.Redraw();
    if (!val_text_coeff_absorption_.GetPrintingInfo()) {
        if (val_text_coeff_absorption_.GetChanged()) {
            manager_->ChangeCurObjAbsorption(atof(val_text_coeff_absorption_.GetText().c_str()) - object_->GetCoeffAbsorption());
            val_text_coeff_absorption_.SetChanged(false);
        } else {
            val_text_coeff_absorption_.SetText(CoeffAbsorptionToStr(object_->GetCoeffAbsorption()));
        }
    }
    val_text_coeff_absorption_.Redraw();

    for (auto text : additional_texts_) {
        text->Redraw();
    }

    for (size_t i = 0; i < additional_params_.size(); i++) {
        if (!additional_params_[i]->GetPrintingInfo()) {
            if (additional_params_[i]->GetChanged()) {
                call_if_change_[i](additional_params_[i], manager_);
            } else {
                additional_params_[i]->SetText(call_if_nothing_[i](object_));
            }
        }
        additional_params_[i]->Redraw();
    }

    auto tmp = texture->GetPos();
    texture->SetPos({});
    texture->DrawOn(*base_texture_);
    texture->SetPos(tmp);
    texture->Clear(colors::Color(0, 0, 0, 0));
    base_texture_->DrawOn(*texture);
    title_->Redraw();
    WidgetContainer::Redraw();
}
