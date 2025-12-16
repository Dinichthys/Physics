#include "plugin_upload.hpp"
#include "scene_manager.hpp"

void PluginName::UploadPlugin(const std::string& plugin_name) {
    SceneManager* manager = NULL;

    for (auto child : state->ui->main_field_->GetChildren()) {
        manager = dynamic_cast<SceneManager*>(child);
        if (manager) {
            manager->dorisovka_->UploadPlugin(plugin_name);
            return;
        }
    }
};

bool PluginNameEditField::OnKeyPressed(const dr4::Event::KeyEvent& evt) {
    if (hidden_) {
        return false;
    }
    if (EditableText::OnKeyPressed(evt)) {
        if (evt.sym == dr4::KeyCode::KEYCODE_ENTER) {
            plugin_name_->UploadPlugin(EditableText::GetText());
            plugin_name_->SetHidden(true);
        }

        return true;
    }

    return false;
};
