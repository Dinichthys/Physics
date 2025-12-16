#include "toolbar.hpp"
#include "dorisovka.hpp"

ToolBar::ToolBar(const Coordinates& lt_corner, Dorisovka* cvs, hui::State* state, Widget* parent)
    :Widget(lt_corner, kWidthToolBar, kTitleHeight + kColorBoxSize + kSpaceBetweenTools + (kWidthToolBar - kColorBoxSize) / 2, state, parent) {
    cvs_ = cvs;

    title_ = new Title(Coordinates(2), kWidthToolBar, state, this, kToolBarName, this);

    color_box_ = state->window_->CreateTexture();
    color_box_->SetPos({(kWidthToolBar - kColorBoxSize) / 2, kSpaceBetweenTools + kTitleHeight});
    color_box_->SetSize({kColorBoxSize, kColorBoxSize});
    color_box_->Clear(cvs_->theme_.shapeBorderColor);

    background_ = state->window_->CreateRectangle();
    background_->SetBorderColor(kTitleBorderColor);
    background_->SetBorderThickness(kBorderThicknessWidget);
    background_->SetFillColor(kTitleBackgroundColor);
    background_->SetSize(dr4::Vec2f{kWidthToolBar, kTitleHeight + kColorBoxSize + 2 * kSpaceBetweenTools} - dr4::Vec2f{kBorderThicknessWidget, kBorderThicknessWidget} * 2 - dr4::Vec2f{0, kTitleHeight});
    background_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget + kTitleHeight});
};
