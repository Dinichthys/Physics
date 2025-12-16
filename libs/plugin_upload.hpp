#ifndef PLUGIN_UPLOAD_HPP
#define PLUGIN_UPLOAD_HPP

#include "button.hpp"
#include "editable_text.hpp"

class PluginUpload : public Button {
    private:
        EditableText input_;

    public:
        PluginUpload(const Button& button)
            :Button(button) {

        };

};

#endif // PLUGIN_UPLOAD_HPP
