/*
    sglgio/theme.cpp -- Storage class for basic theme-related properties
    
    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/theme.h>
#include "resources.h"
#include <map>
#include <string>

#if defined(_WIN32)
#include <SDL_ttf.h>
#else

#include <SDL2/SDL_ttf.h>

#endif

NAMESPACE_BEGIN(sdlgui)

    namespace internal {
        std::map<std::string, TTF_Font *> fonts;
    }

    Theme::Theme([[maybe_unused]] SDL_Renderer *ctx) {
        mStandardFontSize = 16;
        mButtonFontSize = 20;
        mTextBoxFontSize = 20;
        mWindowCornerRadius = 2;
        mWindowHeaderHeight = 30;
        mWindowDropShadowSize = 10;
        mButtonCornerRadius = 2;
        mTabBorderWidth = 0.75f;
        mTabInnerMargin = 5;
        mTabMinButtonWidth = 30;
        mTabMaxButtonWidth = 180;
        mTabControlWidth = 20;
        mTabButtonHorizontalPadding = 10;
        mTabButtonVerticalPadding = 2;
        mScrollBarWidth = 20;
        mScrollBarGutter = 2;

        mDropShadow = Color(32, 32, 32, 255);
        mTransparent = Color(0, 0);
        mBorderDark = Color(29, 255);
        mBorderLight = Color(92, 255);
        mBorderMedium = Color(35, 255);
        mTextColor = Color(255, 160);
        mDisabledTextColor = Color(255, 80);
        mTextColorShadow = Color(0, 160);
        mIconColor = mTextColor;

        mButtonGradientTopFocused = Color(64, 255);
        mButtonGradientBotFocused = Color(48, 255);
        mButtonGradientTopUnfocused = Color(74, 255);
        mButtonGradientBotUnfocused = Color(58, 255);
        mButtonGradientTopPushed = Color(41, 255);
        mButtonGradientBotPushed = Color(29, 255);

        /* Window-related */
        mWindowFillUnfocused = Color(43, 255);
        mWindowFillFocused = Color(45, 255);
        mWindowTitleUnfocused = Color(220, 160);
        mWindowTitleFocused = Color(255, 190);

        /* Slider */
        mSliderKnobOuter = Color(92, 255);
        mSliderKnobInner = Color(220, 255);


        mWindowHeaderGradientTop = mButtonGradientTopUnfocused;
        mWindowHeaderGradientBot = mButtonGradientBotUnfocused;
        mWindowHeaderSepTop = mBorderLight;
        mWindowHeaderSepBot = mBorderDark;

        mWindowPopup = Color(50, 255);
        mWindowPopupTransparent = Color(50, 0);

        mFontPath = "/usr/share/fonts/truetype/freefont";

        TTF_Init();
    }

    /**
     * getFont -- Attempt to find the requested font in the file system or built-in. Return
     * the found font or a fallback if none is found.
     * @param theme Theme data for the user font path.
     * @param fontname The user supplied font name.
     * @param ptsize The user supplied point size.
     * @return a TrueType font, possibly a built in as fallback.
     */
    TTF_Font *getFont(const Theme &theme, const std::string &fontname, size_t ptsize) {
        // Compose a font name including the size as a key for caching
        std::string fullFontName = fontname;
        fullFontName += "_";
        fullFontName += std::to_string(ptsize);

        auto fontIt = internal::fonts.find(fullFontName);
        if (fontIt == internal::fonts.end()) {
            // Compose a full pathname to the requested font and try to open it.
            std::string tmpFontname = theme.mFontPath;
            tmpFontname += "/";
            tmpFontname += fontname;
            tmpFontname += ".ttf";

            auto *fileFont = TTF_OpenFont(tmpFontname.c_str(), ptsize);
            if (fileFont != nullptr) {
                internal::fonts[fullFontName] = fileFont;
                return fileFont;
            } else {
                // Use one of the "built in" fonts.
                SDL_RWops *rw;
                if (fontname == "sans")
                    rw = SDL_RWFromMem(roboto_regular_ttf, roboto_regular_ttf_size);
                else if (fontname == "sans-bold")
                    rw = SDL_RWFromMem(roboto_bold_ttf, roboto_bold_ttf_size);
                else if (fontname == "icons")
                    rw = SDL_RWFromMem(entypo_ttf, entypo_ttf_size);
                else // Provide a fall back font
                    rw = SDL_RWFromMem(roboto_regular_ttf, roboto_regular_ttf_size);

                TTF_Font *newFont = TTF_OpenFontRW(rw, false, ptsize);
                internal::fonts[fullFontName] = newFont;
                return newFont;
            }
        } else {
            return fontIt->second;
        }
    }

    int Theme::getTextBounds(const char *fontname, size_t ptsize, const char *text, int *w, int *h) const {
        TTF_Font *font = getFont(*this, fontname, ptsize);

        if (!font)
            return -1;

        TTF_SizeText(font, text, w, h);
        return 0;
    }

    int Theme::getUtf8Bounds(const char *fontname, size_t ptsize, const char *text, int *w, int *h) const {
        TTF_Font *font = getFont(*this, fontname, ptsize);

        if (!font)
            return -1;

        TTF_SizeUTF8(font, text, w, h);
        return 0;
    }

    int Theme::getTextWidth(const char *fontname, size_t ptsize, const char *text) const {
        int w, h;
        getTextBounds(fontname, ptsize, text, &w, &h);
        return w;
    }

    int Theme::getUtf8Width(const char *fontname, size_t ptsize, const char *text) const {
        TTF_Font *font = getFont(*this, fontname, ptsize);

        if (!font)
            return -1;

        int w, h;
        TTF_SizeUTF8(font, text, &w, &h);
        return w;
    }


    [[maybe_unused]] void Theme::getTexAndRect(SDL_Renderer *renderer, int x, int y, const char *text,
                              const char *fontname, size_t ptsize, SDL_Texture **texture, SDL_Rect *rect,
                              SDL_Color *textColor) const {
        int text_width;
        int text_height;

        if (*texture != nullptr)
            SDL_DestroyTexture(*texture);

        SDL_Color defColor{255, 255, 255, 0};

        TTF_Font *font = getFont(*this, fontname, ptsize);

        if (!font)
            return;

        SDL_Surface *surface = TTF_RenderText_Blended(font, text, textColor ? *textColor : defColor);
        if (!surface) {
            rect->x = x;
            rect->y = y;
            rect->w = 0;
            rect->h = 0;
            *texture = nullptr;
            return;
        }

        *texture = SDL_CreateTextureFromSurface(renderer, surface);
        text_width = surface->w;
        text_height = surface->h;
        SDL_FreeSurface(surface);
        rect->x = x;
        rect->y = y;
        rect->w = text_width;
        rect->h = text_height;
    }


    void Theme::getTexAndRectUtf8(SDL_Renderer *renderer, int x, int y, const char *text,
                                  const char *fontname, size_t ptsize, SDL_Texture **texture, SDL_Rect *rect,
                                  SDL_Color *textColor) const {
        int text_width;
        int text_height;

        if (*texture != nullptr)
            SDL_DestroyTexture(*texture);

        SDL_Color defColor{255, 255, 255, 0};

        TTF_Font *font = getFont(*this, fontname, ptsize);

        if (!font)
            return;

        SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, textColor ? *textColor : defColor);
        if (!surface) {
            rect->x = x;
            rect->y = y;
            rect->w = 0;
            rect->h = 0;
            *texture = nullptr;
            return;
        }

        *texture = SDL_CreateTextureFromSurface(renderer, surface);
        text_width = surface->w;
        text_height = surface->h;
        SDL_FreeSurface(surface);
        rect->x = x;
        rect->y = y;
        rect->w = text_width;
        rect->h = text_height;
    }

    std::string Theme::breakText([[maybe_unused]] SDL_Renderer *renderer, const char *string, const char *fontname, int ptsize,
                                 float breakRowWidth) const {
        std::string _string(string);
        for (int i = 0; i < _string.size(); i++) {
            int slen = getTextWidth(fontname, ptsize, _string.substr(0, i).c_str());
            if ((float)slen >= breakRowWidth)
                return _string.substr(0, i);
        }

        return string;
    }

    void Theme::getTexAndRectUtf8(SDL_Renderer *renderer, Texture &tx, int x, int y, const char *text,
                                  const char *fontname, size_t ptsize, const Color &textColor) const {
        tx.dirty = false;
        SDL_Color tColor = textColor.toSdlColor();
        getTexAndRectUtf8(renderer, 0, 0, text, fontname, ptsize, &tx.tex, &tx.rrect, &tColor);
    }

    void SDL_RenderCopy(SDL_Renderer *renderer, Texture &tx, const Vector2i &pos) {
        if (!tx.tex)
            return;

        SDL_Rect rect{pos.x, pos.y, tx.rrect.w, tx.rrect.h};
        SDL_RenderCopy(renderer, tx.tex, nullptr, &rect);
    }

NAMESPACE_END(sdlgui)
