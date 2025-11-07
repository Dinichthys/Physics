#include "scrollbar.hpp"

extern "C" ScrollBar* GetScrollBar() {
    return new ScrollBar();
}
