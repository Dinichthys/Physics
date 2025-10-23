#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include <vector>
#include <stack>
#include <mutex>
#include <unistd.h>

#include "graphics.hpp"

#include "object.hpp"
#include "vector.hpp"

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

static const graphics::Color kFreeSpaceColor(graphics::kColorCyan);

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

    public:
        SceneManager(const Coordinates& lt_corner, float width, float height,
                     const std::vector<Object*> objects, const std::vector<Widget*>* children = NULL)
            :WidgetContainer(lt_corner, width, height, children), vertices_(width * height),
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

            WidgetContainer::SetParentToChildren();
        };

        ~SceneManager() {
            size_t objects_num = objects_.size();
            for (size_t i = 0; i < objects_num; i++) {
                delete objects_.back();
                objects_.pop_back();
            }
        };

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

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            const Coordinates& widget_lt_corner = Widget::GetLTCornerLoc();
            float width = WidgetContainer::GetWidth();
            float height = WidgetContainer::GetHeight();

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

                GetPointIntersection(eye_.GetEyePos(),
                                     lt_corner + hor_vec * loc_coordinates[0] + ver_vec * loc_coordinates[1],
                                     coeff, cur_object_idx_);

                if (coeff < 0) {
                    *widget = NULL;
                    cur_object_idx_ = -1;
                } else {
                    *widget = this;
                }

                return true;
            }

            return WidgetContainer::OnMousePress(mouse_pos, widget);
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
