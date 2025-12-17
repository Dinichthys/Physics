#ifndef PLUGIN_UPLOAD_BUTTON_HPP
#define PLUGIN_UPLOAD_BUTTON_HPP

#include <stdio.h>

#include "draw.hpp"
#include "button.hpp"
#include "editable_text.hpp"

static const std::string kPluginButtonName = "Plugin";

static const std::string kPluginNameOkButtonName = "Ok";

static const size_t kPluginNameWidth = 300;
static const size_t kPluginNameYOffset = 10;
static const size_t kPluginNameTextFieldWidth = 280;
static const size_t kPluginNameTextFieldHeight = 20;
static const size_t kPluginNameOkButtonHeight = 20;
static const size_t kPluginNameOkButtonWidth = 30;
static const size_t kPluginNameHeight = kPluginNameYOffset * 3 + kPluginNameTextFieldHeight + kPluginNameOkButtonHeight;

static const colors::Color kPluginNameOkButtonColor = colors::Color(49, 49, 49);

static const std::string kPluginNameTitle = "Plugin Name";
static const std::string kStartInputPluginName = "./plugins/build/ppMe.so";

class PluginName;

class PluginNameEditField : public EditableText {
    private:
        PluginName* plugin_name_;

    public:
        PluginNameEditField(const EditableText& other, PluginName* plugin_name)
            :EditableText(other), plugin_name_(plugin_name) {};

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) override;
};

class PluginName : public Widget {
    private:
        Title* title_;

        PluginNameEditField* plugin_name_;

        Button* ok_button_;

        dr4::Rectangle* background_;

    public:
        PluginName(hui::State* state, Widget* parent)
            :Widget(Coordinates((state->ui->main_field_->GetSize() - dr4::Vec2f{kPluginNameWidth, kPluginNameHeight + kTitleHeight}) / 2),
                    kPluginNameWidth, kPluginNameHeight + kTitleHeight, state, parent) {
            title_ = new Title(Coordinates(2), kPluginNameWidth, state, this, kPluginNameTitle, this);

            plugin_name_ = new PluginNameEditField(EditableText(Text(Coordinates(2, kPluginNameYOffset, kTitleHeight + kPluginNameYOffset),
                kPluginNameTextFieldWidth, kPluginNameTextFieldHeight,
                state, this, kStartInputPluginName, kWidgetDefaultFontFileName, kPluginNameTextFieldHeight)), this);

            background_ = state->window_->CreateRectangle();
            background_->SetBorderColor(kWidgetDefaultBorderColor);
            background_->SetBorderThickness(kBorderThicknessWidget);
            background_->SetFillColor(kWidgetDefaultFillColor);
            background_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget + kTitleHeight});
            background_->SetSize(dr4::Vec2f{kPluginNameWidth, kPluginNameHeight} - dr4::Vec2f{kBorderThicknessWidget, kBorderThicknessWidget} * 2);

            ok_button_ = new Button(Coordinates(2, kPluginNameWidth - kPluginNameYOffset - kPluginNameOkButtonWidth, kTitleHeight + kPluginNameYOffset + kPluginNameTextFieldHeight + kPluginNameYOffset),
                kPluginNameOkButtonWidth, kPluginNameOkButtonHeight, kPluginNameOkButtonName, kWidgetDefaultFontFileName, state, this, kPluginNameOkButtonColor, kPluginNameOkButtonColor);

        };

        virtual void Redraw() override {
            if (hidden_) {
                return;
            }

            background_->DrawOn(*texture);

            plugin_name_->Redraw();

            ok_button_->Redraw();

            title_->Redraw();
            Widget::Redraw();
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }

            Coordinates loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y);
            if (Widget::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }

                if (ok_button_->OnMousePress(loc_coors, type)) {
                    state->target_widget_ = plugin_name_;
                    plugin_name_->OnKeyPressed({dr4::KeyCode::KEYCODE_ENTER, 0});
                    return true;
                }

                if (plugin_name_->OnMousePress(loc_coors, type)) {
                    return true;
                }

                if (title_->OnMousePress(loc_coors, type)) {
                    return true;
                }

                return true;
            }
            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }

            Coordinates loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y);
            if (Widget::OnMouseRelease(mouse_pos, type)) {
                if (plugin_name_->OnMouseRelease(loc_coors, type)) {
                    return true;
                }

                if (ok_button_->OnMouseRelease(loc_coors, type)) {
                    return true;
                }

                if (title_->OnMouseRelease(loc_coors, type)) {
                    return true;
                }

                return true;
            }
            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (hidden_) {
                return false;
            }

            Coordinates loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y);
            if (Widget::OnMouseEnter(mouse_pos, delta)) {
                if (plugin_name_->OnMouseEnter(loc_coors, delta)) {
                    return true;
                }

                if (ok_button_->OnMouseEnter(loc_coors, delta)) {
                    return true;
                }

                if (title_->OnMouseEnter(loc_coors, delta)) {
                    return true;
                }
                return true;
            }
            return false;
        };

        void UploadPlugin(const std::string& plugin_name);
};

class PluginUploadButton : public Button {
    private:
        PluginName* plugin_name_;

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

                if (plugin_name_) {
                    plugin_name_->SetHidden(!(plugin_name_->GetHidden()));
                    return true;
                }

                plugin_name_ = new PluginName(state, state->ui->main_field_);
                state->ui->main_field_->AddChild(plugin_name_);

                return true;
            }
            return false;
        };
};

#endif // PLUGIN_UPLOAD_BUTTON_HPP
