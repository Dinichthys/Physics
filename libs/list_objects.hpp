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
static const std::string kListTitleHeightClosedButtonText = " \\/";
static const std::string kListTitleHeightOpenedButtonText = " /\\";

static const colors::Color kElemInListColor = colors::Color(90, 75, 60);

static const colors::Color kListColor = colors::Color(62, 62, 66);

static const colors::Color kListArrowColor = colors::Color(180, 180, 180);

static const size_t kNumLenListObjects = 30;

static const size_t kMaxNumLines = 5;

static const size_t kWidth = 500;
static const size_t kHeight = 200;
static const size_t kListObjectsTitleHeight = 25;
static const float kTextScaleTitleHeight = 1.2;
static const float kTextScaleButtons = 1.4;

static const size_t kScrollBarWidth = 10;

class ListObjects : public WidgetContainer {
    private:
        int64_t start_index_;

        const std::vector<Object*>& objects_;
        std::vector<Text> texts_;

        std::function<void(size_t)> action_;

        dr4::Rectangle rect_;
        float collected_delta_;

    public:
        explicit ListObjects(const Coordinates& lt_corner, const std::vector<Object*>& objects,
                             std::function<void(size_t)> action, hui::State* state = NULL)
                    :WidgetContainer(lt_corner, kWidth, kHeight, state), objects_(objects),
                     action_(action), rect_(dr4::Rect2f({0, 0}, {kWidth, kHeight}), kListColor) {
            start_index_ = 0;
            for (size_t idx = 0; (idx < kMaxNumLines) && (idx < objects.size()); idx++) {
                WidgetContainer::AddChild(new Button(Coordinates(2, 0, idx * kHeight / kMaxNumLines),
                                                     kWidth - kScrollBarWidth, kHeight / kMaxNumLines,
                                                     ObjectInList(idx), kFontFileNameListObject, state, this,
                                                     kElemInListColor, kElemInListColor,
                                                     kHeight / kMaxNumLines / kTextScaleButtons));
            }
            collected_delta_ = 0;

            WidgetContainer::AddChild(new ScrollBar(Coordinates(2, kWidth - kScrollBarWidth, 0),
                                        kScrollBarWidth, kHeight,
                                        (kHeight - 2 * kArrowScrollBarHeight)
                                        * WidgetContainer::GetChildrenNum() / objects.size(), state,
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

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if (Widget::GetHidden()) {
                return false;
            }
            for (size_t idx = 0; idx < WidgetContainer::GetChildrenNum() - 1; idx++) {
                if (WidgetContainer::GetChild(idx)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc())) {
                    action_(idx + start_index_);
                    return true;
                }
            }
            return WidgetContainer::GetChild
                    (WidgetContainer::GetChildrenNum() - 1)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc());
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

        virtual void Redraw() override {
            if (Widget::GetHidden()) {
                return;
            }

            texture->Draw(rect_);

            for (size_t idx = 0; (idx < kMaxNumLines) && (idx < objects_.size()); idx++) {
                (dynamic_cast<Button*>(WidgetContainer::GetChild(idx)))->SetText(ObjectInList(start_index_ + idx));
            }
            for (size_t idx = objects_.size(); idx < kMaxNumLines; idx++) {
                (dynamic_cast<Button*>(WidgetContainer::GetChild(idx)))->SetText("");
            }

            WidgetContainer::Redraw();
        };

        virtual void SetState(hui::State* state_) {
            state = state_;
            size_t texts_num = texts_.size();
            for (size_t i = 0; i < texts_num; i++) {
                texts_[i].SetState(state_);
            }
            WidgetContainer::SetState(state_);
        };
};

class ListObjectsTitle : public Widget {
    private:
        ListObjects* list_;
        Text text_;
        Button button_;
        dr4::Rectangle rect_;

    public:
        explicit ListObjectsTitle(const Coordinates& lt_corner, ListObjects* list_objects, hui::State* state = NULL)
            :Widget(lt_corner, kWidth, kListObjectsTitleHeight, state),
             list_(list_objects),
             text_(Coordinates(2, 0, 0), kWidth, kListObjectsTitleHeight,
                   state, this, kListObjectsTitleStr, kFontFileNameListObject,
                   kListObjectsTitleHeight / kTextScaleTitleHeight),
             button_(Coordinates(2, kWidth - kListObjectsTitleHeight), kListObjectsTitleHeight,
                     kListObjectsTitleHeight, kListTitleHeightClosedButtonText,
                     kFontFileNameListObject, state, this, kListArrowColor, kListArrowColor),
             rect_(dr4::Rect2f({0, 0}, {kWidth, kListObjectsTitleHeight}), kListColor) {
            list_->SetHidden(true);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if (button_.OnMousePress(mouse_pos - Widget::GetLTCornerLoc())) {
                if (list_->GetHidden()) {
                    list_->SetHidden(false);
                    button_.SetText(kListTitleHeightOpenedButtonText);
                } else {
                    list_->SetHidden(true);
                    button_.SetText(kListTitleHeightClosedButtonText);
                }
                return true;
            }

            return Widget::OnMousePress(mouse_pos);
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) {
            if (button_.OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc())) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos) {
            if (button_.OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc())) {
                return true;
            }

            return false;
        };

        virtual void Redraw() override {
            texture->Draw(rect_);
            button_.Redraw();
            text_.Redraw();
            Widget::Redraw();
        };

        virtual void SetState(hui::State* state_) {
            state = state_;
            text_.SetState(state);
            button_.SetState(state);
            Widget::SetState(state);
        };

};

#endif // LIST_OBJECTS_HPP
