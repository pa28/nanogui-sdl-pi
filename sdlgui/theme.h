/*
    sdlgui/theme.h -- Storage class for basic theme-related properties

    The text box widget was contributed by Christian Schueller.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <sdlgui/common.h>
#include <mutex>
#include <string_view>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

NAMESPACE_BEGIN(sdlgui)

struct Texture
{
  SDL_Texture* tex = nullptr;
  SDL_Rect rrect{};
  bool dirty = false;

  inline int w() const { return rrect.w; }
  inline int h() const { return rrect.h; }
};

void SDL_RenderCopy(SDL_Renderer* renderer, Texture& tex, const Vector2i& pos);
/**
 * \class Theme theme.h sdlgui/theme.h
 *
 * \brief Storage class for basic theme-related properties.
 */

class  Theme : public Object 
{
public:
    explicit Theme(SDL_Renderer *ctx = nullptr);

    /* Spacing-related parameters */
    int mStandardFontSize;
    int mButtonFontSize;
    int mTextBoxFontSize;
    int mTimeBoxHoursMinFontSize;
    int mTimeBoxSecFontSize;
    int mTimeBoxDateFontSize;
    int mTimeBoxSmallHoursMinFontSize;
    int mTimeBoxSmallSecFontSize;
    int mTimeBoxSmallDateFontSize;
    int mWindowCornerRadius;
    int mWindowHeaderHeight;
    int mWindowDropShadowSize;
    int mButtonCornerRadius;
    float mTabBorderWidth;
    int mTabInnerMargin;
    int mTabMinButtonWidth;
    int mTabMaxButtonWidth;
    int mTabControlWidth;
    int mTabButtonHorizontalPadding;
    int mTabButtonVerticalPadding;
    int mScrollBarWidth;
    int mScrollBarGutter;
    int mMinBrightness;
    int mMaxBrightness;

    std::mutex loadMutex;

    /* Generic colors */
    Color mDropShadow;
    Color mTransparent;
    Color mBorderDark;
    Color mBorderLight;
    Color mBorderMedium;
    Color mTextColor;
    Color mDisabledTextColor;
    Color mTextColorShadow;
    Color mIconColor;

    /* Button colors */
    Color mButtonGradientTopFocused;
    Color mButtonGradientBotFocused;
    Color mButtonGradientTopUnfocused;
    Color mButtonGradientBotUnfocused;
    Color mButtonGradientTopPushed;
    Color mButtonGradientBotPushed;

    /* Window colors */
    Color mWindowFillUnfocused;
    Color mWindowFillFocused;
    Color mWindowTitleUnfocused;
    Color mWindowTitleFocused;

    /* Slider coloes */
    Color mSliderKnobOuter;
    Color mSliderKnobInner;

    Color mWindowHeaderGradientTop;
    Color mWindowHeaderGradientBot;
    Color mWindowHeaderSepTop;
    Color mWindowHeaderSepBot;

    Color mWindowPopup;
    Color mWindowPopupTransparent;

    std::string mFontPath;
    std::string mTimeBoxTimeFont;
    std::string mTimeBoxDateFont;
    std::string mTimeBoxSmallTimeFont;
    std::string mTimeBoxSmallDateFont;

    std::string mTimeBoxHoursMinFmt;
    std::string mTimeBoxSecFmt;
    std::string mTimeBoxDateFmt;

    std::string mTimeBoxSmallHoursMinFmt;
    std::string mTimeBoxSmallSecFmt;
    std::string mTimeBoxSmallDateFmt;

    [[maybe_unused]] void getTexAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
      const char* fontname, size_t ptsize, SDL_Texture **texture, SDL_Rect *rect, SDL_Color *textColor) const;

    void getTexAndRectUtf8(SDL_Renderer *renderer, int x, int y, const char *text,
      const char* fontname, size_t ptsize, SDL_Texture **texture, SDL_Rect *rect, SDL_Color *textColor) const;

    std::string breakText(SDL_Renderer* renderer, const char* string, const char* fontname, int ptsize,
                       float breakRowWidth) const;

    int getTextWidth(const char* fontname, size_t ptsize, const char* text) const;
    int getUtf8Width(const char* fontname, size_t ptsize, const char* text) const;
    int getTextBounds(const char* fontname, size_t ptsize, const char* text, int *w, int *h) const;
    int getUtf8Bounds(const char* fontname, size_t ptsize, const char* text, int *w, int *h) const;

    void getTexAndRectUtf8(SDL_Renderer *renderer, Texture& tx, int x, int y, const char *text,
                           const char* fontname, size_t ptsize, const Color& textColor) const;

//protected:
    virtual ~Theme() { };
};

NAMESPACE_END(sdlgui)
