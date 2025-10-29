#ifndef LIST_OBJECTS_HPP
#define LIST_OBJECTS_HPP

#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>

#include "widget.hpp"
#include "text.hpp"
#include "button.hpp"

#include "object.hpp"

#include "scrollbar.hpp"

static const std::string kFontFileNameListObject = "data/font.ttf";

static const graphics::Color kListColor = graphics::Color(170, 170, 170);

static const size_t kNumLenListObjects = 30;

static const size_t kMaxNumLines = 5;

static const size_t kWidth = 300;
static const size_t kHeight = 200;

class ListObjects : public WidgetContainer {
    private:
        int64_t start_index_;

        const std::vector<Object*>& objects_;
        std::vector<Text> texts_;

        std::function<void(size_t)> action_;

        graphics::RectangleShape rect_;

    public:
        explicit ListObjects(const Coordinates& lt_corner, const std::vector<Object*>& objects,
                             std::function<void(size_t)> action)
                    :WidgetContainer(lt_corner, kWidth, kHeight), objects_(objects),
                     action_(action), rect_(kWidth, kHeight) {
            rect_.SetFillColor(kListColor);
            start_index_ = 0;
            for (size_t idx = 0; (idx < kMaxNumLines) && (idx < objects.size()); idx++) {
                WidgetContainer::AddChild(new Button(Coordinates(2, 0, idx * kHeight / kMaxNumLines),
                                                     kWidth, kHeight / kMaxNumLines,
                                                     ObjectInList(idx), kFontFileNameListObject));
            }
        };

        std::string ObjectInList(size_t index) {
            std::string str;

            char num[kNumLenListObjects] = "";
            sprintf(num, "%lu", index);
            str.append(num);

            str.append(" | ");

            str.append(TypeToStr(objects_[index]->GetType()));

            return str;
        };

        const std::string TypeToStr(ObjectType type) {
        #define CASE_TYPE_TO_STRING_(type) \
            case k ## type : {             \
                return "Type: "#type;      \
            }

            switch(type) {
                CASE_TYPE_TO_STRING_(Sphere);
                CASE_TYPE_TO_STRING_(Light);
                CASE_TYPE_TO_STRING_(Plane);
                CASE_TYPE_TO_STRING_(TrianglesSet);

                case kAllTypes :
                default:
                    return "Type: Invalid";
            };
        #undef CASE_TYPE_STRING_
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            for (size_t idx = 0; idx < WidgetContainer::GetChildrenNum(); idx++) {
                if (WidgetContainer::GetChild(idx)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), widget)) {
                    action_(idx);
                    return true;
                }
            }
            return false;
        };

        void ShiftStartIndex(int64_t shift) {
            if ((start_index_ >= 0) && (start_index_ + kMaxNumLines < objects_.size())) {
                start_index_ += shift;
            }
            if (start_index_ + kMaxNumLines > objects_.size()) {
                start_index_ = std::max(0l, int64_t(objects_.size()) - int64_t(kMaxNumLines));
            }
            if (start_index_ < 0) {
                start_index_ = 0;
            }
        };

        virtual void Draw(graphics::RenderWindow* window) override {
            rect_.SetPosition(Widget::GetLTCornerAbs());
            window->Draw(rect_);
            WidgetContainer::Draw(window);
        };
};

#endif // LIST_OBJECTS_HPP
