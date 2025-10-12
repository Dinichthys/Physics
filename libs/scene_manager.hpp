#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include <vector>

#include "graphics.hpp"

#include "object.hpp"
#include "vector.hpp"

#include "my_assert.h"

static const Coordinates kIBase(3, 19, 19, 19);
static const float kEyeHeight = 1000;
static const float kPowCosB = 32;
static const float kMaxColor = 255;
static const float kWindowDistance = 688;
static const float kStepEye = 10;
static const float kCosRotate = 0.9961947;
static const float kSinRotate = 0.08715574;
static const size_t kColorCountingDepth = 5;

static const graphics::Color kFreeSpaceColor(graphics::kColorBrown);

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

class SceneManager : public Widget {
    private:
        graphics::VertexArray vertices_;
        std::vector<Circle*> circles_;
        Eye eye_;

    public:
        SceneManager(const Coordinates& lt_corner, float width, float height,
                     const std::vector<Circle*> circles)
            :Widget(lt_corner, width, height), vertices_(width * height),
             eye_(Coordinates(3, (float)(width / 2), (float)(height / 2), - kEyeHeight),
                  Coordinates(3,-(float)(width / 2),-(float)(height / 2), kWindowDistance),
                  Coordinates(3,-(float)(width / 2), (float)(height / 2), kWindowDistance),
                  Coordinates(3, (float)(width / 2),-(float)(height / 2), kWindowDistance),
                  Coordinates(3, (float)(width / 2), (float)(height / 2), kWindowDistance)) {
            size_t circles_num = circles.size();
            for (size_t i = 0; i < circles_num; i++) {
                circles_.push_back(new Circle(*(circles[i])));
            }
        };

        ~SceneManager() {
            size_t circles_num = circles_.size();
            for (size_t i = 0; i < circles_num; i++) {
                delete circles_.back();
                circles_.pop_back();
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

    private:
        Circle GetPointIntersectionWithCircle(const Coordinates& pixel_pos, const Coordinates& eye_pos,
                                              float& coeff, size_t& cur_circle_index);
        graphics::Color GetPointColor(const Coordinates& point, const Coordinates& eye_pos,
                                      size_t cur_circle_idx, size_t depth_counting);
        graphics::Color GetLightEffect(const Coordinates& point, const Coordinates& eye_pos,
                                       size_t cur_circle_idx);
        graphics::Color GetReflectionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_circle_idx, size_t depth_counting);
        graphics::Color GetRefractionEffect(const Coordinates& point, const Coordinates& eye_pos,
                                            size_t cur_circle_idx, size_t depth_counting);
        bool GetIntersectionResultQuadraticEquation(const Circle* circle, const Coordinates& pixel_pos,
                                                    const Coordinates& eye_pos, float& res_plus, float& res_minus);
};

#endif // SCENE_MANAGER
