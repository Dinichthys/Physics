#include "dr4_backend.hpp"
#include "graphics.hpp"

const std::string &graphics::Backend::Name() const {
    return graphics::kBackendName;
}

dr4::Window *graphics::Backend::CreateWindow() {
    return new graphics::RenderWindow();
}

    class Backend : dr4::DR4Backend {
        public:
            virtual const std::string &Name() const;
            virtual dr4::Window *CreateWindow();
