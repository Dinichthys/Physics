#ifndef DR4_BACKEND
#define DR4_BACKEND

#include "misc/dr4_ifc.hpp"

namespace graphics {

    class Backend : dr4::DR4Backend {
        public:
            virtual const std::string &Name() const;
            virtual dr4::Window *CreateWindow();
    };

    std::string const kBackendName = "DenDR4Backend";

};

#endif // DR4_BACKEND
