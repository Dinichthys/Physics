#ifndef PLUGIN_UPLOAD_BUTTON_HPP
#define PLUGIN_UPLOAD_BUTTON_HPP

#include "draw.hpp"
#include "button.hpp"
#include "editable_text.hpp"

static const std::string kPluginButtonName = "Plugin";

static const size_t kPluginNameWidth = 100;
static const size_t kPluginNameHeight = 20;

static const std::string kPluginNameTitle = "Plugin Name";
static const std::string kStartInputPluginName = "./plugins/build/ppMe.so";

class PluginName : public Widget {
    private:
        Title* title_;

        EditableText* plugin_name_;

        dr4::Rectangle* background_;

    public:
        PluginName(hui::State* state)
            :Widget(Coordinates((state->ui->main_field_->GetSize() - dr4::Vec2f{kPluginNameWidth, kPluginNameHeight + kTitleHeight}) / 2),
                    kPluginNameWidth, kPluginNameHeight + kTitleHeight, state, state->ui->main_field_) {
            title_ = new Title(Coordinates(2), kPluginNameWidth, state, this, kPluginNameTitle, this);

            plugin_name_ = new EditableText(Text(Coordinates(2, 0, kTitleHeight), kPluginNameWidth, kPluginNameHeight,
                state, this, kStartInputPluginName, kWidgetDefaultFontFileName, kPluginNameHeight));

            background_ = state->window_->CreateRectangle();
            background_->SetBorderColor(kWidgetDefaultBorderColor);
            background_->SetBorderThickness(kBorderThicknessWidget);
            background_->SetFillColor(kWidgetDefaultFillColor);
            background_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget});
            background_->SetSize(dr4::Vec2f{kPluginNameWidth, kPluginNameHeight} - dr4::Vec2f{kBorderThicknessWidget, kBorderThicknessWidget} * 2);
        };
};

class PluginUploadButton : public Button {
    private:
        EditableText* plugin_name_;

    public:
        PluginUploadButton(const Coordinates& lt_corner, hui::State* state, Widget* parent)
            :Button(lt_corner, kTopBarButtonWidth, kTitleHeight, kPluginButtonName, kFontFileNameTitle, state, parent,
                kTitleBackgroundColor, kTitleBackgroundColor) {plugin_name_ = NULL;};

        virtual void SetState(hui::State* state_) override {
            Button::SetState(state_);
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (WidgetContainer::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }

//                 if (plugin_name_) {
//                     plugin_name_->SetHidden(!(plugin_name_->GetHidden()));
//                     return true;
//                 }
//
//                 plugin_name_ = new ButtonDorisovka(Widget::GetLTCornerLoc() + Coordinates(2, 0, kTitleHeight), Widget::GetWidth(), state, state->ui);
//                 state->ui->AddChild(dorisovka_);

                return true;
            }
            return false;
        };
};

#endif // PLUGIN_UPLOAD_BUTTON_HPP
