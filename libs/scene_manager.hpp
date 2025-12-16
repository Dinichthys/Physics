#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include <vector>
#include <stack>
#include <mutex>
#include <unistd.h>

#include "colors.hpp"

#include "widget.hpp"
#include "widget_title.hpp"

#include "object.hpp"
#include "circle.hpp"
#include "triangle.hpp"
#include "plane.hpp"
#include "button.hpp"

#include "vector.hpp"
#include "info_table.hpp"
#include "list_objects.hpp"

#include "border.hpp"

#include "my_assert.h"

#include "dorisovka.hpp"

static const float kEyeHeight = 2000;
static const Coordinates start_eye_pos(3, 0, 0, -1000);
static const float kPowCosB = 32;
static const float kMaxColor = 255;
static const float kWindowDistance = 688;
static const float kStepEye = 10;
static const float kCosRotate = 0.9961947;
static const float kSinRotate = 0.08715574;
static const size_t kColorCountingDepth = 5;
static const size_t kSpinLockTimeOut = 100;

static const float kInfoTableWidth = 160;

static const size_t kListIdx = 1;

static const colors::Color kFreeSpaceColor(colors::Color(30, 28, 29));
static const colors::Color kChoseObjectColor(colors::kColorRed);

static const std::string kSceneManagerName = "Ray Tracing";

class Eye {
    private:
        Coordinates pos;
        Coordinates lt_corner;
        Coordinates lb_corner;
        Coordinates rt_corner;
        Coordinates rb_corner;

    public:
        explicit Eye(Coordinates pos_val,
                     Coordinates lt_val, Coordinates lb_val, Coordinates rt_val, Coordinates rb_val)
            :pos(pos_val), lt_corner(lt_val), lb_corner(lb_val), rt_corner(rt_val), rb_corner(rb_val) {};

        Coordinates GetEyePos() const {return Coordinates(pos);};
        Coordinates GetEyeLTCorner() const {return Coordinates(lt_corner);};
        Coordinates GetEyeLBCorner() const {return Coordinates(lb_corner);};
        Coordinates GetEyeRTCorner() const {return Coordinates(rt_corner);};
        Coordinates GetEyeRBCorner() const {return Coordinates(rb_corner);};

        void SetEyePos(Coordinates pos_val) {pos = pos_val;};
        void SetEyePosX(float x) {pos.SetCoordinate(0, x);};
        void SetEyePosY(float y) {pos.SetCoordinate(1, y);};
        void SetEyePosZ(float z) {pos.SetCoordinate(2, z);};

        void SetEyeLTCorner(Coordinates lt_corner_val) {lt_corner = lt_corner_val;};
        void SetEyeLBCorner(Coordinates lb_corner_val) {lb_corner = lb_corner_val;};
        void SetEyeRTCorner(Coordinates rt_corner_val) {rt_corner = rt_corner_val;};
        void SetEyeRBCorner(Coordinates rb_corner_val) {rb_corner = rb_corner_val;};
};

class DrawTask {
    private:
        size_t start_pixel_x_;
        size_t start_pixel_y_;
        float height_;
        float width_;
        size_t start_point_idx_;

    public:
        explicit DrawTask(size_t start_pixel_x, size_t start_pixel_y,
                          float width, float height,
                          size_t start_point_idx) {
            start_pixel_x_ = start_pixel_x;
            start_pixel_y_ = start_pixel_y;
            width_ = width;
            height_ = height;
            start_point_idx_ = start_point_idx;
        };

        size_t GetStartPixelX() const {return start_pixel_x_;};
        size_t GetStartPixelY() const {return start_pixel_y_;};
        size_t GetHeight() const {return height_;};
        size_t GetWidth() const {return width_;};
        size_t GetStartPointIdx() const {return start_point_idx_;};
};

template<typename T>
class Mueue : public std::stack<T> {
    private:
        std::mutex mu_;

    public:
        explicit Mueue(const std::stack<T>& stk)
            :std::stack<T>(stk), mu_() {}

        DrawTask GetElem() {
            while(!mu_.try_lock()) {usleep(kSpinLockTimeOut);}
            DrawTask elem = std::stack<T>::top();
            std::stack<T>::pop();
            mu_.unlock();
            return elem;
        };

        void push(const DrawTask elem) {
            while(!mu_.try_lock()) {usleep(kSpinLockTimeOut);}
            std::stack<T>::push(elem);
            mu_.unlock();
            return;
        };
};

class SceneManager : public Widget {
    private:
        dr4::Image* image_;

        std::vector<Object*> objects_;

        Eye eye_;

        Mueue<DrawTask> tasks_;

        size_t cur_object_idx_;
        size_t border_idx_;
        colors::Color cur_object_color_;
        ListObjectsTitle* list_objects_;
        InfoTable* table_;
        PanelControl* panel_;

        Dorisovka* dorisovka_;

        Title* title_;

        bool changed_;

        dr4::Rectangle* border_;

    public:
        SceneManager(const Coordinates& lt_corner, float width, float height,
                     const std::vector<Object*> objects, hui::State* state)
            :Widget(lt_corner, width, height, state),
             image_((state != NULL) ? state->window_->CreateImage() : NULL),
             eye_(Coordinates(start_eye_pos),
                  Coordinates(3,-(float)(width / 2),-(float)(height / 2), kWindowDistance),
                  Coordinates(3,-(float)(width / 2), (float)(height / 2), kWindowDistance),
                  Coordinates(3, (float)(width / 2),-(float)(height / 2), kWindowDistance),
                  Coordinates(3, (float)(width / 2), (float)(height / 2), kWindowDistance)), tasks_(std::stack<DrawTask>()),
             list_objects_(NULL), table_(NULL), panel_(NULL),
             border_((state != NULL) ? state->window_->CreateRectangle() : NULL) {
            size_t objects_num = objects.size();
            for (size_t i = 0; i < objects_num; i++) {
                objects_.push_back(objects[i]);
            }

            if (image_ != NULL) {
                image_->SetSize({width, height});
                image_->SetPos({0, 0});
            }

            if (border_ != NULL) {
                border_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget});
                border_->SetSize({width - 2 * kBorderThicknessWidget, height - 2 * kBorderThicknessWidget});
                border_->SetFillColor(colors::Color(0, 0, 0, 0));
                border_->SetBorderColor(kTitleBorderColor);
                border_->SetBorderThickness(kBorderThicknessWidget);
            }

            cur_object_idx_ = -1;
            border_idx_ = -1;

            table_ = NULL;
            dorisovka_ = NULL;

            title_ = new Title(Coordinates(2), width, state, this, kSceneManagerName, this);

            changed_ = true;
        };

        ~SceneManager() {
            size_t objects_num = objects_.size();
            for (size_t i = 0; i < objects_num; i++) {
                delete objects_.back();
                objects_.pop_back();
            }
            delete image_;
            if (dorisovka_ != NULL) {
                delete dorisovka_;
                dorisovka_ = NULL;
            }
            delete title_;
        };

        virtual void SetState(hui::State* state_) {
            title_->SetState(state_);

            dr4::Image* tmp = image_;
            image_ = state_->window_->CreateImage();
            image_->SetSize(::Widget::GetSize());
            if (tmp != NULL) {
                float width = Widget::GetWidth();
                float height = Widget::GetHeight();
                for (float i = 0; i < height; i++) {
                    for (float j = 0; j < width; j++) {
                        image_->SetPixel(j, i, tmp->GetPixel(j, i));
                    }
                }
                image_->SetPos({0, 0});
                delete tmp;
            }

            border_ = state_->window_->CreateRectangle();
            border_->SetPos({kBorderThickness, kBorderThickness});
            border_->SetSize({Widget::GetWidth() - 2 * kBorderThickness, Widget::GetHeight() - 2 * kBorderThickness});
            border_->SetFillColor(colors::Color(0, 0, 0, 0));
            border_->SetBorderColor(kTitleBackgroundColor);
            border_->SetBorderThickness(kBorderThickness);

            Widget::SetState(state_);
        };

        void SetPanelControl(PanelControl* panel) {
            panel_ = panel;
            panel_->SetHidden(true);
        };
        void SetListObjects(ListObjectsTitle* list_objects) {
            list_objects_ = list_objects;
            list_objects_->SetHidden(false);
        };
        void SetInfoTable(InfoTable* table) {
            table_ = table;
            table_->SetManager(this);
            table_->SetHidden(true);
        };

        std::vector<Object*>& GetObjectsVec() {
            return objects_;
        };

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) {
            if (dorisovka_ != NULL) {
                if (dorisovka_->OnKeyPressed(evt)) {
                    return true;
                }
            }

            bool old_changed = changed_;
            changed_ = true;
            switch(evt.sym) {
                case dr4::KeyCode::KEYCODE_A :
                    return OnLetterA();
                case dr4::KeyCode::KEYCODE_D :
                    return OnLetterD();
                case dr4::KeyCode::KEYCODE_S :
                    return OnLetterS();
                case dr4::KeyCode::KEYCODE_W :
                    return OnLetterW();

                case dr4::KeyCode::KEYCODE_G :
                    changed_ = old_changed;
                    return OnLetterG();

                case dr4::KeyCode::KEYCODE_ESCAPE :
                    changed_ = old_changed;
                    return OnESC();

                case dr4::KeyCode::KEYCODE_UP :
                    return OnArrowUp();
                case dr4::KeyCode::KEYCODE_DOWN :
                    return OnArrowDown();
                case dr4::KeyCode::KEYCODE_LEFT :
                    return OnArrowLeft();
                case dr4::KeyCode::KEYCODE_RIGHT :
                    return OnArrowRight();

                default:
                    changed_ = old_changed;
                    return false;
            };
        }

        bool OnLetterA();
        bool OnLetterD();
        bool OnLetterS();
        bool OnLetterW();

        bool OnArrowUp();
        bool OnArrowDown();
        bool OnArrowLeft();
        bool OnArrowRight();

        virtual bool OnKeyUp(const dr4::Event::KeyEvent& evt) {
            if (dorisovka_ != NULL) {
                return dorisovka_->OnKeyUp(evt);
            }
            return false;
        }
        virtual bool OnText(const dr4::Event::TextEvent& evt) {
            if (dorisovka_ != NULL) {
                return dorisovka_->OnText(evt);
            }
            return false;
        }

        virtual void Redraw() override;

        void MoveCurrentObject(const Coordinates& move_direction_) {
            if (cur_object_idx_ >= objects_.size()) {
                return;
            }
            changed_ = true;
            objects_[cur_object_idx_]->Move(move_direction_);
            if (objects_[border_idx_] != NULL) {
                objects_[border_idx_]->Move(move_direction_);
            }
        };

        void DeleteCurrentObject() {
            if (cur_object_idx_ >= objects_.size()) {
                return;
            }
            changed_ = true;
            delete objects_[cur_object_idx_];
            objects_.erase(objects_.begin() + cur_object_idx_);
            table_->SetHidden(true);
            if (panel_) panel_->SetHidden(true);
            table_->SetObject(NULL);
            cur_object_idx_ = -1;
            if (objects_[border_idx_] != NULL) {
                delete objects_[border_idx_];
            }
            objects_.erase(objects_.begin() + border_idx_);
        };

        void AddCopyCurrentObject() {
            if (cur_object_idx_ >= objects_.size()) {
                return;
            }
            changed_ = true;
            objects_.push_back(objects_[cur_object_idx_]->GetCopy());
            objects_.back()->SetCenterCoordinates(Coordinates(3, 0, 0, 0));
            objects_.back()->SetColor(cur_object_color_);
        };

        void ChooseObject(size_t idx) {
            changed_ = true;
            if (cur_object_idx_ < objects_.size()) {
                objects_[cur_object_idx_]->SetColor(cur_object_color_);
                if (objects_[border_idx_] != NULL) {
                    delete objects_[border_idx_];
                }
                objects_.erase(objects_.begin() + border_idx_);
            }
            cur_object_idx_ = idx;
            table_->SetObject(objects_[cur_object_idx_]);
            table_->SetHidden(false);
            if (panel_) panel_->SetHidden(false);
            cur_object_color_ = colors::Color(objects_[cur_object_idx_]->GetColor());
            // objects_[cur_object_idx_]->SetColor(kChoseObjectColor);
            objects_.push_back(objects_[cur_object_idx_]->GetBorder());
            border_idx_ = objects_.size() - 1;
        };

        void ChangeCurObjReflection(float delta) {
            changed_ = true;
            float new_reflection = objects_[cur_object_idx_]->GetCoeffReflection() + delta;
            if ((new_reflection > -kEpsilon) && (new_reflection < 1 + kEpsilon)) {
                if (new_reflection > 1) {
                    new_reflection = 1;
                }
                if (new_reflection < 0) {
                    new_reflection = 0;
                }

                float old_reflection = objects_[cur_object_idx_]->GetCoeffReflection();
                float old_absorption = objects_[cur_object_idx_]->GetCoeffAbsorption();
                objects_[cur_object_idx_]->SetCoeffReflection(new_reflection);

                if ((old_absorption > kEpsilon) || (1 - old_reflection > kEpsilon)) {
                    objects_[cur_object_idx_]->SetCoeffAbsorption(
                        old_absorption - (new_reflection - old_reflection) * old_absorption / (1 - old_reflection)
                    );
                } else {
                    objects_[cur_object_idx_]->SetCoeffAbsorption(old_reflection - new_reflection);
                }
            }
        };

        void ChangeCurObjRefraction(float delta) {
            changed_ = true;
            float new_coeff = objects_[cur_object_idx_]->GetCoeffRefraction() + delta;
            if (new_coeff >= 0) {
                objects_[cur_object_idx_]->SetCoeffRefraction(new_coeff);
            }
        };

        void ChangeCurObjAbsorption(float delta) {
            changed_ = true;
            float new_absorption = objects_[cur_object_idx_]->GetCoeffAbsorption() + delta;
            if ((new_absorption > -kEpsilon) && (new_absorption < 1 + kEpsilon)) {
                if (new_absorption > 1) {
                    new_absorption = 1;
                }
                if (new_absorption < 0) {
                    new_absorption = 0;
                }

                float old_reflection = objects_[cur_object_idx_]->GetCoeffReflection();
                float old_absorption = objects_[cur_object_idx_]->GetCoeffAbsorption();
                objects_[cur_object_idx_]->SetCoeffAbsorption(new_absorption);

                if ((old_reflection > kEpsilon) || (1 - old_absorption > kEpsilon)) {
                    objects_[cur_object_idx_]->SetCoeffReflection(
                        old_reflection - (new_absorption - old_absorption) * old_reflection / (1 - old_absorption)
                    );
                } else {
                    objects_[cur_object_idx_]->SetCoeffReflection(old_absorption - new_absorption);
                }
            }
        };

        void ChangeCurObjColor(const colors::Color& color) {
            changed_ = true;
            cur_object_color_ = color;
            objects_[cur_object_idx_]->SetColor(color);
        };

        colors::Color GetCurObjColor() {
            return cur_object_color_;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (title_->OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc(), delta)) {
                return true;
            }
            if (dorisovka_ != NULL) {
                return dorisovka_->OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc(), delta);
            }
            return Widget::OnMouseEnter(mouse_pos, delta);
        }

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (title_->OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }
            if (dorisovka_ != NULL) {
                return dorisovka_->OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc(), type);
            }
            return Widget::OnMouseRelease(mouse_pos, type);
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (title_->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }
            if (dorisovka_ != NULL) {
                return dorisovka_->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type);
            }

            const Coordinates& widget_lt_corner = Widget::GetLTCornerLoc();
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            Coordinates loc_coordinates = mouse_pos - widget_lt_corner;

            if ((loc_coordinates[0] > 0)
                && (loc_coordinates[1] > 0)
                && (loc_coordinates[0] < width)
                && (loc_coordinates[1] < height)) {
                state->target_widget_ = NULL;

                float coeff = -1;

                Coordinates lt_corner = eye_.GetEyeLTCorner();
                Coordinates lb_corner = eye_.GetEyeLBCorner();
                Coordinates rt_corner = eye_.GetEyeRTCorner();

                Coordinates hor_vec = !(rt_corner - lt_corner);
                Coordinates ver_vec = !(lb_corner - lt_corner);

                size_t tmp = -1;
                GetPointIntersection(eye_.GetEyePos(),
                                     lt_corner + hor_vec * loc_coordinates[0] + ver_vec * loc_coordinates[1],
                                     coeff, tmp);

                if (cur_object_idx_ == tmp) {
                    return Widget::OnMousePress(mouse_pos, type);;
                }

                changed_ = true;
                if (coeff < 0) {
                    if (cur_object_idx_ < objects_.size()) {
                        objects_[cur_object_idx_]->SetColor(cur_object_color_);
                    }
                    cur_object_idx_ = -1;
                    table_->SetHidden(true);
                    if (panel_) panel_->SetHidden(true);
                    table_->SetObject(NULL);
                    delete objects_[border_idx_];
                    objects_.erase(objects_.begin() + border_idx_);
                    return Widget::OnMousePress(mouse_pos, type);
                } else {
                    if (cur_object_idx_ < objects_.size()) {
                        objects_[cur_object_idx_]->SetColor(cur_object_color_);
                        if (objects_[border_idx_] != NULL) {
                            delete objects_[border_idx_];
                            objects_.erase(objects_.begin() + border_idx_);
                            border_idx_ = -1;
                        }
                    }

                    cur_object_idx_ = tmp;
                    table_->SetObject(objects_[cur_object_idx_]);
                    table_->SetHidden(false);
                    if (panel_) panel_->SetHidden(false);
                    cur_object_color_ = colors::Color(objects_[cur_object_idx_]->GetColor());
                    // objects_[cur_object_idx_]->SetColor(kChoseObjectColor);
                    objects_.push_back(objects_[cur_object_idx_]->GetBorder());
                    border_idx_ = objects_.size() - 1;
                }

                return true;
            }

            return false;
        };

        bool OnLetterG() {
            if (dorisovka_ == NULL) {
                dorisovka_ = new Dorisovka(
                    Coordinates(2, 0, kTitleHeight),
                    {Widget::GetSize().x, Widget::GetSize().y - kTitleHeight},
                    state
                );
                dorisovka_->SetParent(this);
            }
            return true;
        };
        bool OnESC() {
            if (dorisovka_ != NULL) {
                delete dorisovka_;
                dorisovka_ = NULL;
            }
            return true;
        };

    private:
        bool CameraRotationInTwoAxis(size_t ace_1, size_t ace_2, float direction);

        Object* GetPointIntersection(const Coordinates& pixel_pos, const Coordinates& vec,
               float& coeff, size_t& cur_object_idx);

        colors::Color GetPointColor(const Coordinates& point, const Coordinates& eye_pos,
                                      size_t cur_object_idx, size_t depth_counting);
        colors::Color GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
                                       size_t cur_object_idx);
        colors::Color GetReflectionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_object_idx, size_t depth_counting);
        colors::Color GetRefractionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_object_idx, size_t depth_counting);

        void DrawPart();
};

#endif // SCENE_MANAGER
