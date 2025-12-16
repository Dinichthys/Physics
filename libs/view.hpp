#ifndef VIEW_HPP
#define VIEW_HPP

#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>

#include "widget.hpp"
#include "text.hpp"
#include "button.hpp"

#include "scrollbar.hpp"

static const std::string kFontFileNameListObject = "data/font.ttf";

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

class ListWidgets : public WidgetContainer {
    private:
        int64_t start_index_;

        const std::vector<Widget*>& widgets_;

        std::function<void(size_t)> action_;

        dr4::Rectangle* rect_;
        float collected_delta_;

    public:
        explicit ListWidgets(const Coordinates& lt_corner, const std::vector<Widget*>& widgets,
                             std::function<void(size_t)> action, hui::State* state = NULL)
                    :WidgetContainer(lt_corner, kWidth, kHeight, state), widgets_(widgets), action_(action),
                    rect_((state == NULL) ? NULL : state->window_->CreateRectangle()) {
            if (rect_ != NULL) {
                rect_->SetPos({0, 0});
                rect_->SetSize({kWidth, kHeight});
                rect_->SetFillColor(kListColor);
                rect_->SetBorderColor(dr4::Color(0, 0, 0, 0));
                rect_->SetBorderThickness(0);
            }

            start_index_ = 0;
            for (size_t idx = 0; (idx < kMaxNumLines) && (idx < widgets.size()); idx++) {
                WidgetContainer::AddChild(new Button(Coordinates(2, 0, idx * kHeight / kMaxNumLines),
                                                     kWidth - kScrollBarWidth, kHeight / kMaxNumLines,
                                                     WidgetInList(idx), kFontFileNameListObject, state, this,
                                                     kElemInListColor, kElemInListColor,
                                                     kHeight / kMaxNumLines / kTextScaleButtons));
            }
            collected_delta_ = 0;

            WidgetContainer::AddChild(new ScrollBar(Coordinates(2, kWidth - kScrollBarWidth, 0),
                                        kScrollBarWidth, kHeight,
                                        (kHeight - 2 * kArrowScrollBarHeight)
                                        * WidgetContainer::GetChildrenNum() / widgets.size(), state,
                                        [this, widgets](float delta){
                                            float& collected_delta = this->GetCollectedDelta();
                                            int64_t idx_delta =
                                                    int64_t(float(widgets.size()) * (delta + collected_delta)
                                                    / 100);
                                            if ((idx_delta != 0) || (abs(delta) < kEpsilon)) {
                                                this->ShiftStartIndex(idx_delta);
                                                collected_delta = 0;
                                            } else {
                                                collected_delta += delta;
                                            }
                                        }));
        };

        ~ListWidgets() {
            delete rect_;
        }

        float& GetCollectedDelta() {return collected_delta_;};

        std::string WidgetInList(size_t index) {
            std::string str;

            char num[kNumLenListObjects] = "";
            sprintf(num, "%lu", index);
            str.append(num);

            str.append(" | ");

            str.append(TypeToStr(widgets_[index]->GetName()));

            return str;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (Widget::GetHidden()) {
                return false;
            }
            for (size_t idx = 0; idx < WidgetContainer::GetChildrenNum() - 1; idx++) {
                if (WidgetContainer::GetChild(idx)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                    action_(idx + start_index_);
                    return true;
                }
            }
            return WidgetContainer::GetChild
                    (WidgetContainer::GetChildrenNum() - 1)->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type);
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

            texture->Draw(*rect_);

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

            if (rect_ == NULL) {
                rect_ = state->window_->CreateRectangle();
                rect_->SetPos({0, 0});
                rect_->SetSize({kWidth, kHeight});
                rect_->SetFillColor(kListColor);
                rect_->SetBorderColor(dr4::Color(0, 0, 0, 0));
                rect_->SetBorderThickness(0);
            }

            WidgetContainer::SetState(state_);
        };
};

class ListObjectsTitle : public Widget {
    private:
        ListObjects* list_;
        Text text_;
        Button button_;
        dr4::Rectangle* rect_;

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
             rect_((state == NULL) ? NULL : state->window_->CreateRectangle()) {
            if (rect_ != NULL) {
                rect_->SetPos({0, 0});
                rect_->SetSize({kWidth, kListObjectsTitleHeight});
                rect_->SetFillColor(kListColor);
                rect_->SetBorderColor(dr4::Color(0, 0, 0, 0));
                rect_->SetBorderThickness(0);
            }

            list_->SetHidden(true);
        };

        ~ListObjectsTitle() {
            delete rect_;
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (button_.OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                if (list_->GetHidden()) {
                    list_->SetHidden(false);
                    button_.SetText(kListTitleHeightOpenedButtonText);
                } else {
                    list_->SetHidden(true);
                    button_.SetText(kListTitleHeightClosedButtonText);
                }
                return true;
            }

            return Widget::OnMousePress(mouse_pos, type);
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (button_.OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (button_.OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc(), delta)) {
                return true;
            }

            return false;
        };

        virtual void Move(float shift_x, float shift_y) override {
            Widget::Move(shift_x, shift_y);
            list_->Move(shift_x, shift_y);
        };

        virtual void Redraw() override {
            texture->Draw(*rect_);
            button_.Redraw();
            text_.Redraw();
            Widget::Redraw();
        };

        virtual void SetState(hui::State* state_) {
            state = state_;

            if (rect_ == NULL) {
                rect_ = state->window_->CreateRectangle();
                rect_->SetPos({0, 0});
                rect_->SetSize({kWidth, kListObjectsTitleHeight});
                rect_->SetFillColor(kListColor);
                rect_->SetBorderColor(dr4::Color(0, 0, 0, 0));
                rect_->SetBorderThickness(0);
            }

            text_.SetState(state);
            button_.SetState(state);
            Widget::SetState(state);
        };

};

#endif // VIEW_HPP
