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
static const std::string kListObjectsTitleStr = "Objects";
static const std::string kListTitleHeightClosedButtonText = "\\/";
static const std::string kListTitleHeightOpenedButtonText = "/\\";

static const graphics::Color kElemInListColor = graphics::Color(90, 75, 60);

static const graphics::Color kListColor = graphics::Color(62, 62, 66);

static const graphics::Color kListArrowColor = graphics::Color(180, 180, 180);

static const size_t kNumLenListObjects = 30;

static const size_t kMaxNumLines = 5;

static const size_t kWidth = 500;
static const size_t kHeight = 200;
static const size_t kListObjectsTitleHeight = 25;
static const float kTextScaleTitleHeight = 1.2;

static const size_t kScrollBarWidth = 10;

class ListObjects : public WidgetContainer {
    private:
        int64_t start_index_;

        const std::vector<Object*>& objects_;
        std::vector<Text> texts_;

        std::function<void(size_t)> action_;

        graphics::RectangleShape rect_;
        float collected_delta_;

    public:
        explicit ListObjects(const Coordinates& lt_corner, const std::vector<Object*>& objects,
                             std::function<void(size_t)> action)
                    :WidgetContainer(lt_corner, kWidth, kHeight), objects_(objects),
                     action_(action), rect_(kWidth, kHeight) {
            rect_.SetFillColor(kListColor);
            start_index_ = 0;
            for (size_t idx = 0; (idx < kMaxNumLines) && (idx < objects.size()); idx++) {
                WidgetContainer::AddChild(new Button(Coordinates(2, 0, idx * kHeight / kMaxNumLines),
                                                     kWidth - kScrollBarWidth, kHeight / kMaxNumLines,
                                                     ObjectInList(idx), kFontFileNameListObject, this,
                                                     kElemInListColor, kElemInListColor));
            }
            collected_delta_ = 0;

            WidgetContainer::AddChild(new ScrollBar(Coordinates(2, lt_corner[0] + kWidth - kScrollBarWidth, 0),
                                        kScrollBarWidth, kHeight,
                                        (kHeight - 2 * kArrowScrollBarHeight)
                                        * WidgetContainer::GetChildrenNum() / objects.size(),
                                        [this, objects](float delta){
                                            float& collected_delta = this->GetCollectedDelta();
                                            int64_t idx_delta =
                                                    int64_t(float(objects.size()) * (delta + collected_delta)
                                                    / 100);
                                            if ((idx_delta != 0) || (abs(delta) < kEpsilon)) {
                                                this->ShiftStartIndex(idx_delta);
                                                collected_delta = 0;
                                            } else {
                                                collected_delta += delta;
                                            }
                                        }));
        };

        float& GetCollectedDelta() {return collected_delta_;};

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
                CASE_TYPE_TO_STRING_(Border);

                case kAllTypes :
                default:
                    return "Type: Invalid";
            };
        #undef CASE_TYPE_STRING_
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            for (size_t idx = 0; idx < WidgetContainer::GetChildrenNum() - 1; idx++) {
                if (WidgetContainer::GetChild(idx)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), widget)) {
                    action_(idx + start_index_);
                    return true;
                }
            }
            return WidgetContainer::GetChild
                    (WidgetContainer::GetChildrenNum() - 1)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), widget);
        };

        void ShiftStartIndex(int64_t shift) {
            start_index_ += shift;

            if (start_index_ + kMaxNumLines > objects_.size()) {
                start_index_ = std::max(0l, int64_t(objects_.size()) - int64_t(kMaxNumLines));
            }

            if (start_index_ < 0) {
                start_index_ = 0;
            }

            float percentage = dynamic_cast<ScrollBar*>(
                                    WidgetContainer::GetChild(WidgetContainer::GetChildrenNum() - 1)
                                )->GetPercentage();
            if (percentage < kEpsilon) {
                start_index_ = 0;
            } else if (shift == 0) {
                start_index_ = std::max(0l, int64_t(objects_.size()) - int64_t(kMaxNumLines));
            }
        };

        virtual void Draw(graphics::RenderWindow* window) override {
            rect_.SetPosition(Widget::GetLTCornerAbs());
            window->Draw(rect_);

            for (size_t idx = 0; (idx < kMaxNumLines) && (idx < objects_.size()); idx++) {
                (dynamic_cast<Button*>(WidgetContainer::GetChild(idx)))->SetText(ObjectInList(start_index_ + idx));
            }
            for (size_t idx = objects_.size(); idx < kMaxNumLines; idx++) {
                (dynamic_cast<Button*>(WidgetContainer::GetChild(idx)))->SetText("");
            }

            WidgetContainer::Draw(window);
        };
};

class ListObjectsTitle : public Widget {
    private:
        ListObjects list_;
        Text text_;
        Button button_;
        graphics::RectangleShape rect_;
        bool opened_;

    public:
        explicit ListObjectsTitle(const Coordinates& lt_corner, const std::vector<Object*>& objects,
                                  std::function<void(size_t)> action)
            :Widget(lt_corner, kWidth, kListObjectsTitleHeight),
             list_(Coordinates(2, 0, kListObjectsTitleHeight), objects, action),
             text_(Coordinates(2, 0, 0), kWidth, kListObjectsTitleHeight / kTextScaleTitleHeight, this, kListObjectsTitleStr, kFontFileNameListObject),
             button_(Coordinates(2, kWidth - kListObjectsTitleHeight), kListObjectsTitleHeight, kListObjectsTitleHeight,
             kListTitleHeightClosedButtonText, kFontFileNameListObject, this, kListArrowColor, kListArrowColor),
             rect_(kWidth, kListObjectsTitleHeight) {
            opened_ = false;
            rect_.SetFillColor(kListColor);
            list_.SetParent(this);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            if (button_.OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), widget)) {
                if (opened_) {
                    opened_ = false;
                    button_.SetText(kListTitleHeightClosedButtonText);
                } else {
                    opened_ = true;
                    button_.SetText(kListTitleHeightOpenedButtonText);
                }
                return true;
            }

            if ((opened_) && (list_.OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), widget))) {
                return true;
            }

            return Widget::OnMousePress(mouse_pos, widget);
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) {
            if ((opened_) && (list_.OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc()))) {
                return true;
            }
            if (button_.OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc())) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos) {
            if ((opened_) && (list_.OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc()))) {
                return true;
            }
            if (button_.OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc())) {
                return true;
            }

            return false;
        };

        virtual void Draw(graphics::RenderWindow* window) override {
            rect_.SetPosition(Widget::GetLTCornerAbs());
            window->Draw(rect_);

            button_.Draw(window);
            text_.Draw(window);
            if(opened_) {
                list_.Draw(window);
            }
        };
};

#endif // LIST_OBJECTS_HPP
