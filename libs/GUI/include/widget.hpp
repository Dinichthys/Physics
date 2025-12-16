#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "colors.hpp"

#include <vector>

#include "hui/widget.hpp"
#include "dr4/window.hpp"
#include "hui/container.hpp"

#include "cum/manager.hpp"

#include "vector.hpp"
#include "logging.h"

static const size_t kBorderThicknessWidget = 2;

static const std::string kWidgetDefaultName = "Widget";

namespace hui {

class State;

class MyWidget {

protected:

    dr4::Vec2f relPos; ///< Relative to parent
    MyWidget *parent = nullptr;
    State * state;
    dr4::Texture * texture;
    bool textureIsInvalid;

    /// Redraws widget's texture
    virtual void Redraw() = 0;

public:

    MyWidget(State *state_, MyWidget *parent_, dr4::Texture* texture_)
        :texture(texture_) {
        state = state_;
        parent = parent_;
        textureIsInvalid = false;
    };

    // REVIEW : move implementations to cpp files?

    dr4::Vec2f GetRelPos() const { return relPos; };
    virtual void SetRelPos(dr4::Vec2f pos) { relPos = pos; };

    dr4::Vec2f GetSize() const { return texture->GetSize(); }
    virtual void SetSize(dr4::Vec2f size) { texture->SetSize(size); }

    virtual MyWidget *GetParent() const { return parent; };
    virtual void SetParent(MyWidget *parent_) { parent = parent_; }
};

}; // namespace hui

class Widget;
class UI;

class hui::State {
    public:
        ::Widget* target_widget_;
        ::Widget* hovered_widget_;
        dr4::Window* window_;
        cum::Manager manager;
        ::UI* ui;

        State()
            :target_widget_(NULL), hovered_widget_(NULL), window_(NULL), manager(), ui(NULL){};
};

class Widget : public hui::MyWidget {
    private:
        Widget* parent_;

    protected:
        float width_;
        float height_;

        bool hovered_;

        bool hidden_;

    public:
        explicit Widget(const Coordinates& lt_corner, const float width, float height,
                        hui::State* const state, ::Widget* parent = NULL)
            :hui::MyWidget(state, parent,
                (state != NULL) ? state->window_->CreateTexture() : NULL) {
            hovered_ = false;
            hidden_ = false;
            width_ = width;
            height_ = height;
            parent_ = parent;

            if (texture != NULL) {
                texture->SetSize({width, height});
            }

            hui::MyWidget::SetRelPos({lt_corner[0], lt_corner[1]});
        };

        explicit Widget(const ::Widget& other)
            :hui::MyWidget(other) {
            width_ = other.width_;
            height_ = other.height_;
            parent_ = other.parent_;
            hovered_ = other.hovered_;
            hidden_ = other.hidden_;
            state = other.state;
            if (state != NULL) {
                texture = state->window_->CreateTexture();
                texture->SetSize({width_, height_});
                dr4::Vec2f old_pos = other.texture->GetPos();
                other.texture->SetPos({0, 0});
                texture->Draw(*(other.texture));
                other.texture->SetPos(old_pos);
            } else {
                texture = NULL;
            }
        };

        virtual ~Widget() {
            delete texture;
        };

        virtual void SetState(hui::State* state_) {
            state = state_;
            dr4::Texture* tmp = texture;
            texture = state->window_->CreateTexture();
            texture->SetSize({width_, height_});
            if (tmp != NULL) {
                tmp->SetPos({0, 0});
                texture->Draw(*tmp);
                delete tmp;
            }
        };

        void SetHidden(bool hidden) {hidden_ = hidden;};
        bool GetHidden() const {return hidden_;};

        dr4::Texture* GetTexture() const {return texture;};

        virtual Coordinates GetLTCornerLoc() const {
            return Coordinates(2, relPos.x, relPos.y);
        };
        virtual float GetWidth() const {return width_;};
        virtual float GetHeight() const {return height_;};
        virtual void SetSize(dr4::Vec2f size) {width_ = size.x; height_ = size.y; hui::MyWidget::SetSize(size);};
        virtual dr4::Vec2f GetSize() const {return {width_, height_};};

        virtual ::Widget* GetParent() const {return parent_;};

        virtual void SetLTCorner(const Coordinates& coors) {relPos = {coors[0], coors[1]};};
        virtual void SetParent(hui::MyWidget* parent) override {
            parent_ = dynamic_cast<::Widget*>(parent);
            ::Widget::MyWidget::parent = parent;
        };

        bool GetHovered() const {return hovered_;};
        void SetHovered(bool hovered) {hovered_ = hovered;};

        hui::State* GetState() const {return state;};

        virtual void Redraw() override {
            if (hidden_) {
                return;
            }
            // texture->Display();
            if (parent != NULL) {
                texture->SetPos(relPos);
                (dynamic_cast<::Widget*>(parent))->GetTexture()->Draw(*texture);
                texture->Clear(dr4::Color(0, 0, 0, 0));
            }
        };

        virtual void Move(float shift_x, float shift_y) {
            relPos.x += shift_x;
            relPos.y += shift_y;
        };

        bool EventInside(dr4::Vec2f pos) {
            return (pos.x > relPos.x)
                && (pos.y > relPos.y)
                && (pos.x < relPos.x + width_)
                && (pos.y < relPos.y + height_);
        };

        virtual bool OnMouseMove(float shift_x, float shift_y) {
            Move(shift_x, shift_y);

            return true;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType) {
            if (hidden_) {
                return false;
            }
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                state->target_widget_ = this;
                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType) {
            if (hidden_) {
                return false;
            }
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates&) {
            if (hidden_) {
                return false;
            }
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

        virtual bool OnIdle(const hui::IdleEvent &) {
            return false;
        };

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent&) {
            return false;
        };
        virtual bool OnKeyUp(const dr4::Event::KeyEvent&) {
            return false;
        };
        virtual bool OnText(const dr4::Event::TextEvent&) {
            return false;
        };

        virtual const std::string& GetName() const {
            return kWidgetDefaultName;
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
            Widget::SetState(state_);
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
            if (hidden_) {
                return;
            }
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                children_[i]->Redraw();
            }
            Widget::Redraw();
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {

                int64_t children_num = children_.size();
                for (int64_t i = children_num - 1; i > -1; i--) {
                    if (children_[i]->OnMousePress(mouse_pos - lt_corner, type)) {
                        return true;
                    }
                }

                state->target_widget_ = this;
                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {

                int64_t children_num = children_.size();
                for (int64_t i = children_num - 1; i > -1; i--) {
                    if (children_[i]->OnMouseRelease(mouse_pos - lt_corner, type)) {
                        return true;
                    }
                }

                return true;
            }

            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (hidden_) {
                return false;
            }
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {

                int64_t children_num = children_.size();
                for (int64_t i = children_num - 1; i > -1; i--) {
                    children_[i]->OnMouseEnter(mouse_pos - lt_corner, delta);
                }

                state->hovered_widget_ = (state->hovered_widget_ == this) ? NULL : state->hovered_widget_;
                SetHovered(true);
                return true;
            }

            state->hovered_widget_ = (state->hovered_widget_ == this) ? NULL : state->hovered_widget_;
            SetHovered(false);
            return false;
        };

        virtual bool OnIdle(const hui::IdleEvent &evt) override {
            bool res = false;
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnIdle(evt)) {
                    res = true;
                }
            }
            return res;
        };

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) override {
            if (hidden_) {
                return false;
            }
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnKeyPressed(evt)) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnKeyUp(const dr4::Event::KeyEvent& evt) override {
            if (hidden_) {
                return false;
            }
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnKeyUp(evt)) {
                    return true;
                }
            }
            return false;
        };
        virtual bool OnText(const dr4::Event::TextEvent& evt) override {
            if (hidden_) {
                return false;
            }
            size_t children_num = children_.size();
            for (size_t i = 0; i < children_num; i++) {
                if (children_[i]->OnText(evt)) {
                    return true;
                }
            }
            return false;
        };
};

#endif // WIDGET_HPP
