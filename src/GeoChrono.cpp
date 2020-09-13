//
// Created by richard on 2020-09-12.
//

#include <iostream>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <sdlgui/common.h>
#include <sdlgui/screen.h>
#include <iomanip>
#include "GeoChrono.h"

bool sdlgui::GeoChrono::mouseMotionEvent(const sdlgui::Vector2i &p, const sdlgui::Vector2i &rel, int button,
                                         int modifiers) {
    if (button) {
        if (!mMotion) {
            mMotionStart = p;
            mMotionEnd = p;
            mButton = mMotion = true;
        } else {
            mMotionEnd = p;
        }
    }
    return Widget::mouseMotionEvent(p, rel, button, modifiers);
}

bool sdlgui::GeoChrono::mouseButtonEvent(const sdlgui::Vector2i &p, int button, bool down, int modifiers) {
    if (button) {
        if (down) {
            if (!mButton) {
                mMotion = false;
            }
            mButton = true;
        } else {
            if (mCallback) {
                auto d = mMotionEnd - mMotionStart;
                if (mMotion && d.x * d.x + d.y * d.y > 25) {
                    if (abs(d.y) >= abs(d.x)) {
                        if (d.y > 0)
                            mCallback(*this, DOWN_EVENT);
                        else
                            mCallback(*this, UP_EVENT);
                    } else {
                        if (d.x > 0)
                            mCallback(*this, RIGHT_EVENT);
                        else
                            mCallback(*this, LEFT_EVENT);
                    }
                    mMotionStart = mMotionEnd = Vector2i(0, 0);
                    mMotion = mButton = false;
                } else {
                    mCallback(*this, CLICK_EVENT);
                }
            }
        }
    }
    return Widget::mouseButtonEvent(p, button, down, modifiers);
}

void sdlgui::GeoChrono::draw(SDL_Renderer *renderer) {
    int ax = getAbsoluteLeft();
    int ay = getAbsoluteTop();

    PntRect clip = getAbsoluteCliprect();
    SDL_Rect clipRect = pntrect2srect(clip);

    if (mForeground.tex) {
        Vector2i p = Vector2i(0, 0);
        p += Vector2i(ax, ay);
        int imgw = mForeground.w;
        int imgh = mForeground.h;

        float iw, ih, ix, iy;
        int thumbSize;
        if (imgw < imgh) {
            thumbSize = fixedWidth() != 0 ? fixedWidth() : width();
            iw = (float) thumbSize;
            ih = iw * (float) imgh / (float) imgw;
            ix = 0;
            iy = 0;
        } else {
            thumbSize = fixedHeight() != 0 ? fixedHeight() : height();
            ih = (float) thumbSize;
            iw = ih * (float) imgw / (float) imgh;
            ix = 0;
            iy = 0;
        }

        SDL_Rect imgPaintRect{p.x + (int) ix, p.y + (int) iy, (int) iw, (int) ih};
        SDL_Rect imgSrcRect{0, 0, imgw, imgh};
        PntRect imgrect = clip_rects(srect2pntrect(imgPaintRect), clip);
        imgPaintRect.w = imgrect.x2 - imgrect.x1;
        imgPaintRect.h = imgrect.y2 - imgrect.y1;
        if (imgPaintRect.y < clip.y1) {
            imgPaintRect.y = clip.y1;
            imgSrcRect.h = (int) (((float) imgPaintRect.h / (float) ih) * (float) imgh);
            imgSrcRect.y = (int) ((1 - ((float) imgPaintRect.h / (float) ih)) * (float) imgh);
        } else if (imgPaintRect.h < (int) ih) {
            imgSrcRect.h = (int) (((float) imgPaintRect.h / ih) * (float) imgh);
        }

        if (mTextureDirty) {
            auto day_map = IMG_Load("maps/day_earth.png");
            SDL_SetSurfaceBlendMode(day_map,
                                    SDL_BLENDMODE_BLEND);
            auto tran_day_map = SDL_CreateRGBSurface(0, mForeground.w, mForeground.h, 32, rmask, gmask, bmask, amask);
//        SDL_FillRect(tran_day_map, nullptr, SDL_MapRGBA(tran_day_map->format, 0x00,0x00,0x00,0x8));
            SDL_SetSurfaceBlendMode(day_map, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(day_map, nullptr, tran_day_map, nullptr);

//        std::cout << std::setbase(16) << SDL_MapRGBA(tran_day_map->format, 0x80,0xC0,0xE0,0xFF) << '\n';

            auto *pixels = (Uint32 *) tran_day_map->pixels;
            for (int x = 0; x < tran_day_map->w; ++x) {
                for (int y = 0; y < tran_day_map->h; ++y) {
                    pixels[(y * tran_day_map->w) + x] = set_a_value(pixels[(y * tran_day_map->w) + x], 0x0FU);
                }
            }

            mForeground.tex = SDL_CreateTextureFromSurface(renderer, tran_day_map);
            mTextureDirty = false;
        }

        SDL_BlendMode mode;
        SDL_GetTextureBlendMode(mForeground.tex, &mode);
        SDL_SetTextureBlendMode(mForeground.tex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(mBackground.tex, SDL_BLENDMODE_BLEND);

        SDL_RenderCopy(renderer, mBackground.tex, &imgSrcRect, &imgPaintRect);
        SDL_RenderCopy(renderer, mForeground.tex, &imgSrcRect, &imgPaintRect);
    }

    Widget::draw(renderer);
}
