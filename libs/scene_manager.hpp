#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include <vector>
#include <stack>
#include <mutex>
#include <unistd.h>

#include "graphics.hpp"

#include "object.hpp"
#include "circle.hpp"
#include "triangle.hpp"
#include "plane.hpp"
#include "button.hpp"

#include "vector.hpp"
#include "info_table.hpp"
#include "list_objects.hpp"

#include "my_assert.h"

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

static const float kInfoTableWidth = 200;

static const size_t kListIdx = 1;

static const graphics::Color kFreeSpaceColor(graphics::kColorCyan);
static const graphics::Color kChoseObjectColor(graphics::kColorBrown);

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

class SceneManager : public WidgetContainer {
    private:
        graphics::VertexArray vertices_;

        std::vector<Object*> objects_;

        Eye eye_;

        Mueue<DrawTask> tasks_;

        size_t cur_object_idx_;
        graphics::Color cur_object_color_;

        InfoTable* table_;

    public:
        SceneManager(const Coordinates& lt_corner, float width, float height,
                     const std::vector<Object*> objects)
            :WidgetContainer(lt_corner, width, height), vertices_(width * height),
             eye_(Coordinates(start_eye_pos),
                  Coordinates(3,-(float)(width / 2),-(float)(height / 2), kWindowDistance),
                  Coordinates(3,-(float)(width / 2), (float)(height / 2), kWindowDistance),
                  Coordinates(3, (float)(width / 2),-(float)(height / 2), kWindowDistance),
                  Coordinates(3, (float)(width / 2), (float)(height / 2), kWindowDistance)), tasks_(std::stack<DrawTask>()) {
            size_t objects_num = objects.size();
            for (size_t i = 0; i < objects_num; i++) {
                objects_.push_back(objects[i]);
            }

            cur_object_idx_ = -1;

            table_ = NULL;
        };

        ~SceneManager() {
            size_t objects_num = objects_.size();
            for (size_t i = 0; i < objects_num; i++) {
                delete objects_.back();
                objects_.pop_back();
            }
            if (table_ != NULL) {
                delete table_;
                table_ = NULL;
            }
        };

        void SetPanelControl(PanelControl* panel) {
            panel->SetParent(this);
            WidgetContainer::AddChild(panel);
        }

        std::vector<Object*>& GetObjectsVec() {
            return objects_;
        }

        virtual bool OnLetterA() override;
        virtual bool OnLetterD() override;
        virtual bool OnLetterS() override;
        virtual bool OnLetterW() override;

        virtual bool OnArrowUp() override;
        virtual bool OnArrowDown() override;
        virtual bool OnArrowLeft() override;
        virtual bool OnArrowRight() override;

        virtual void Draw(graphics::RenderWindow* window) override;

        void MoveCurrentObject(const Coordinates& move_direction_) const {
            if (cur_object_idx_ >= objects_.size()) {
                return;
            }
            objects_[cur_object_idx_]->Move(move_direction_);
        };

        void DeleteCurrentObject() {
            if (cur_object_idx_ >= objects_.size()) {
                return;
            }
            delete objects_[cur_object_idx_];
            objects_.erase(objects_.begin() + cur_object_idx_);
            if (table_ != NULL) {
                delete table_;
                table_ = NULL;
            }
            cur_object_idx_ = -1;
        };

        void AddCopyCurrentObject() {
            if (cur_object_idx_ >= objects_.size()) {
                return;
            }
            switch (objects_[cur_object_idx_]->GetType()) {
                case kSphere : case kLight :
                    objects_.push_back(new Circle(*dynamic_cast<Circle*>(objects_[cur_object_idx_])));
                    break;

                case kPlane :
                    objects_.push_back(new Plane(*dynamic_cast<Plane*>(objects_[cur_object_idx_])));
                    break;

                case kTrianglesSet :
                    objects_.push_back(new TrianglesSet(*dynamic_cast<TrianglesSet*>(objects_[cur_object_idx_])));
                    break;

                case kAllTypes :
                    default:
                    return;
            }
            objects_.back()->SetCenterCoordinates(Coordinates(3, 0, 0, 0));
            objects_.back()->SetColor(cur_object_color_);
        };

        void ChooseObject(size_t idx) {
            if (cur_object_idx_ < objects_.size()) {
                objects_[cur_object_idx_]->SetColor(cur_object_color_);
                if (table_ != NULL) {
                    delete table_;
                    table_ = NULL;
                }
            }
            cur_object_idx_ = idx;
            table_ = new InfoTable(Coordinates(3, -kInfoTableWidth, 0),
                                    kInfoTableWidth, Widget::GetHeight(),
                                    objects_[cur_object_idx_], this);
            cur_object_color_ = graphics::Color(objects_[cur_object_idx_]->GetColor());
            objects_[cur_object_idx_]->SetColor(kChoseObjectColor);
        };

        void ChangeCurObjReflection(float delta) {
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
            float new_coeff = objects_[cur_object_idx_]->GetCoeffRefraction() + delta;
            if (new_coeff >= 0) {
                objects_[cur_object_idx_]->SetCoeffRefraction(new_coeff);
            }
        };

        void ChangeCurObjAbsorption(float delta) {
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

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            const Coordinates& widget_lt_corner = Widget::GetLTCornerLoc();
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            Coordinates loc_coordinates = mouse_pos - widget_lt_corner;

            if ((loc_coordinates[0] > 0)
                && (loc_coordinates[1] > 0)
                && (loc_coordinates[0] < width)
                && (loc_coordinates[1] < height)) {
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
                    return Widget::OnMousePress(mouse_pos, widget);;
                }

                if (coeff < 0) {
                    *widget = NULL;
                    if (cur_object_idx_ < objects_.size()) {
                        objects_[cur_object_idx_]->SetColor(cur_object_color_);
                    }
                    cur_object_idx_ = -1;
                    if (table_ != NULL) {
                        delete table_;
                        table_ = NULL;
                    }
                    return Widget::OnMousePress(mouse_pos, widget);
                } else {
                    if (cur_object_idx_ < objects_.size()) {
                        objects_[cur_object_idx_]->SetColor(cur_object_color_);
                        if (table_ != NULL) {
                            delete table_;
                            table_ = NULL;
                        }
                    }

                    cur_object_idx_ = tmp;
                    table_ = new InfoTable(Coordinates(3, -kInfoTableWidth, 0),
                                           kInfoTableWidth, Widget::GetHeight(),
                                           objects_[cur_object_idx_], this);
                    cur_object_color_ = graphics::Color(objects_[cur_object_idx_]->GetColor());
                    objects_[cur_object_idx_]->SetColor(kChoseObjectColor);
                    *widget = this;
                }

                return true;
            }

            return (table_ != NULL) ? WidgetContainer::OnMousePress(mouse_pos, widget)
                                    : WidgetContainer::GetChild(kListIdx)->OnMousePress(loc_coordinates, widget);
        };

    private:
        bool CameraRotationInTwoAxis(size_t ace_1, size_t ace_2, float direction);

        Object* GetPointIntersection(const Coordinates& pixel_pos, const Coordinates& vec,
               float& coeff, size_t& cur_object_idx);

        graphics::Color GetPointColor(const Coordinates& point, const Coordinates& eye_pos,
                                      size_t cur_object_idx, size_t depth_counting);
        graphics::Color GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
                                       size_t cur_object_idx);
        graphics::Color GetReflectionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_object_idx, size_t depth_counting);
        graphics::Color GetRefractionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_object_idx, size_t depth_counting);

        void DrawPart();
};

#endif // SCENE_MANAGER
