#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "graphics.hpp"

#include <vector>

#include "hui/widget.hpp"
#include "hui/container.hpp"

#include "vector.hpp"
#include "logging.h"

class Widget;

class hui::State {
    public:
        Widget* target_widget_;
        Widget* hovered_widget_;
};

class Widget : public hui::Widget {
    private:
        float width_;
        float height_;
        Widget* parent_;

        bool hovered_;

    public:
        explicit Widget(const Coordinates& lt_corner, const float width, float height,
                        hui::State* const state, ::Widget* parent = NULL)
            :hui::Widget(state, parent, new graphics::Texture(width, height)) {
            hovered_ = false;
            width_ = width;
            height_ = height;
            parent_ = parent;

            hui::Widget::SetRelPos({lt_corner[0], lt_corner[1]});
        };

        explicit Widget(const ::Widget& other)
            :hui::Widget(other) {
            width_ = other.width_;
            height_ = other.height_;
            parent_ = other.parent_;
            hovered_ = other.hovered_;
            hui::Widget::texture = new graphics::Texture(*(dynamic_cast<graphics::Texture*>(other.texture)));
        };

        virtual ~Widget() {
            delete texture;
        };

        virtual void SetState(hui::State* state_) {state = state_;};

        dr4::Texture* GetTexture() const {return texture;};

        virtual Coordinates GetLTCornerLoc() const {
            return Coordinates(2, relPos.x, relPos.y);
        };
        virtual Coordinates GetLTCornerAbs() const {
            return (parent_ == NULL) ? Coordinates(2, relPos.x, relPos.y)
                                     : Coordinates(2, relPos.x, relPos.y) + parent_->GetLTCornerAbs();
        };
        virtual Coordinates GetRBCornerLoc() const {return GetLTCornerLoc() + Coordinates(2, width_, height_);};
        virtual Coordinates GetRBCornerAbs() const {return GetLTCornerAbs() + Coordinates(2, width_, height_);};
        virtual float GetWidth() const {return width_;};
        virtual float GetHeight() const {return height_;};
        virtual void SetSize(dr4::Vec2f size) {width_ = size.x; height_ = size.y; hui::Widget::SetSize(size);};

        virtual ::Widget* GetParent() const {return parent_;};

        virtual void SetLTCorner(const Coordinates& coors) {relPos = {coors[0], coors[1]};};
        void SetParent(::Widget* parent) {parent_ = parent; ::Widget::Widget::parent = parent;};

        bool GetHovered() const {return hovered_;};
        void SetHovered(bool hovered) {hovered_ = hovered;};

        virtual void Redraw() override {
            if (parent != NULL) {
                (dynamic_cast<::Widget*>(parent))->GetTexture()->Draw(*texture, relPos);
            }
        };

        virtual void Move(float shift_x, float shift_y) {
            relPos.x += shift_x;
            relPos.y += shift_y;
        };

        bool EventInside(dr4::Vec2f& pos) {
            return (pos.x > relPos.x)
                && (pos.y > relPos.y)
                && (pos.x < relPos.x + width_)
                && (pos.y < relPos.y + height_);
        };

        virtual bool OnMouseMove(float shift_x, float shift_y) {
            Move(shift_x, shift_y);

            return true;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) {
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                state->target_widget_ = this;
                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) {
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos) {
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                hovered_ = true;
                state->hovered_widget_ = this;
                return true;
            }

            state->hovered_widget_ = (state->hovered_widget_ == this) ? NULL : state->hovered_widget_;
            hovered_ = false;
            return false;
        };

        virtual bool OnIdle() {
            return false;
        };

        virtual bool OnLetterA() {
            return false;
        };
        virtual bool OnLetterD() {
            return false;
        };
        virtual bool OnLetterS() {
            return false;
        };
        virtual bool OnLetterW() {
            return false;
        };

        virtual bool OnArrowRight() {
            return false;
        };
        virtual bool OnArrowLeft() {
            return false;
        };
        virtual bool OnArrowUp() {
            return false;
        };
        virtual bool OnArrowDown() {
            return false;
        };
};

class WidgetContainer : public ::Widget {
    private:
        std::vector<::Widget*> children_;

    public:
        explicit WidgetContainer(const Coordinates& lt_corner, const float width, float height,
                                 hui::State* state,
                                 const std::vector<::Widget*>* children = NULL, ::Widget* parent = NULL)
            : ::Widget(lt_corner, width, height, state, parent) {
            if (children == NULL) {
                return;
            }

            size_t children_num = children->size();
            for (size_t i = 0; i < children_num; i++) {
                children_.push_back((*children)[i]);
            }
        };

        ~WidgetContainer() {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                delete children_.back();
                children_.pop_back();
            }
        };

        virtual void SetState(hui::State* state_) override {
            state = state_;
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->SetState(state_);
            }
        };

        std::vector<Widget*>& GetChildren() {return children_;};
        size_t GetChildrenNum() const {return children_.size();};
        void AddChild(Widget* child) {children_.push_back(child); child->SetParent(this);};
        Widget* GetChild(size_t index) const {return children_[index];};
        void SetChild(size_t index, Widget* child) {
            children_[index] = child;
            child->SetParent(this);
        };

        void ResetChildren() {children_ = {};};

        void SetParentToChildren() {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->SetParent(this);
            }
        }

        virtual void Redraw() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->Redraw();
            }
            Widget::Redraw();
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            int64_t children_num = children_.size();
            for (int64_t i = children_num - 1; i > -1; i--) {
                if (children_[i]->OnMousePress(mouse_pos - lt_corner)) {
                    return true;
                }
            }

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                state->target_widget_ = this;
                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            int64_t children_num = children_.size();
            for (int64_t i = children_num - 1; i > -1; i--) {
                if (children_[i]->OnMouseRelease(mouse_pos - lt_corner)) {
                    return true;
                }
            }

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            int64_t children_num = children_.size();
            for (int64_t i = children_num - 1; i > -1; i--) {
                children_[i]->OnMouseEnter(mouse_pos - lt_corner);
            }

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                state->hovered_widget_ = (state->hovered_widget_ == this) ? NULL : state->hovered_widget_;
                SetHovered(true);
                return true;
            }

            state->hovered_widget_ = (state->hovered_widget_ == this) ? NULL : state->hovered_widget_;
            SetHovered(false);
            return false;
        };

        virtual bool OnIdle() override {
            bool res = false;
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnIdle()) {
                    res = true;
                }
            }
            return res;
        };

        virtual bool OnLetterA() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnLetterA()) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnLetterD() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnLetterD()) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnLetterS() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnLetterS()) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnLetterW() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnLetterW()) {
                    return true;
                }
            }
            return false;
        };

        virtual bool OnArrowRight() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnArrowRight()) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnArrowLeft() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnArrowLeft()) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnArrowUp() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnArrowUp()) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnArrowDown() override {
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnArrowDown()) {
                    return true;
                }
            }
            return false;
        };
};

#endif // WIDGET_HPP
