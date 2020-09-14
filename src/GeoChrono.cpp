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
#include <cmath>
#include <tuple>
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

    if (mMapsDirty)
        generateMapSurfaces(renderer);

    if (mDayMap) {
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

        if (mTextureDirty) {
            SDL_SetSurfaceBlendMode(mDayMap,
                                    SDL_BLENDMODE_BLEND);
            auto tran_day_map = SDL_CreateRGBSurface(0, mDayMap->w, mDayMap->h, 32, rmask, gmask, bmask, amask);
            SDL_SetSurfaceBlendMode(mDayMap, SDL_BLENDMODE_BLEND);
            SDL_BlitSurface(mDayMap, nullptr, tran_day_map, nullptr);

            auto [latS, lonS] = subSolar();
            auto *pixels = (Uint32 *) tran_day_map->pixels;
            for (int x = 0; x < tran_day_map->w; x+=1) {
                for (int y = 0; y < tran_day_map->h; y+=1) {
                    auto lonE = (double)(x - tran_day_map->w/2) * M_PI / (double)(tran_day_map->w/2)
//                                #if USER_SET_CENTRE_LONG
//                            + deg2rad(mCentreLongitude)
//                                #endif
                                    ;
                    auto latE = (double)(tran_day_map->h/2 - y) * M_PI_2 / (double)(tran_day_map->h/2);
                    auto cosDeltaSigma = sin(latS)*sin(latE) + cos(latS)*cos(latE)*cos(abs(lonS-lonE));
                    uint32_t alpha = 255;
                    double fract_day;
                    if (cosDeltaSigma < 0) {
                        if (cosDeltaSigma > GrayLineCos) {
                            fract_day = 1.0 - pow(cosDeltaSigma / GrayLineCos, GrayLinePow);
                            alpha = (uint32_t) (fract_day * 247.0) + 8;
                        } else
                            alpha = 8;
                    }
                    auto pixel = set_a_value(pixels[(y * tran_day_map->w) + x], alpha);
                    pixels[(y * tran_day_map->w) + x] = pixel;
//                    std::cout << latE << ' ' << lonE << ' '
//                        << acos(cosDeltaSigma) << ' '
//                        << fract_day << ' '
//                        << alpha << '\n';
                }
//                std::cout << '\n';
            }

            if (mForeground.tex)
                SDL_DestroyTexture(mForeground.tex);
            mForeground.tex = SDL_CreateTextureFromSurface(renderer, tran_day_map);
            mForeground.h = tran_day_map->h;
            mForeground.w = tran_day_map->w;
            mForeground.path = "*autogen*";
            SDL_FreeSurface(tran_day_map);
            mTextureDirty = false;
        }

//        SDL_Rect imgPaintRect{p.x + (int) ix, p.y + (int) iy, (int) iw, (int) ih};
//        SDL_Rect imgSrcRect{0, 0, imgw, imgh};
//        PntRect imgrect = clip_rects(srect2pntrect(imgPaintRect), clip);
//        imgPaintRect.w = imgrect.x2 - imgrect.x1;
//        imgPaintRect.h = imgrect.y2 - imgrect.y1;
//        if (imgPaintRect.y < clip.y1) {
//            imgPaintRect.y = clip.y1;
//            imgSrcRect.h = (int) (((float) imgPaintRect.h / (float) ih) * (float) imgh);
//            imgSrcRect.y = (int) ((1 - ((float) imgPaintRect.h / (float) ih)) * (float) imgh);
//        } else if (imgPaintRect.h < (int) ih) {
//            imgSrcRect.h = (int) (((float) imgPaintRect.h / ih) * (float) imgh);
//        }

        auto offset = computOffset();

        SDL_BlendMode mode;
        SDL_GetTextureBlendMode(mForeground.tex, &mode);
        SDL_SetTextureBlendMode(mForeground.tex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(mBackground.tex, SDL_BLENDMODE_BLEND);

        if (offset == 0) {
            SDL_Rect src{0, 0, mForeground.w, mForeground.h};
            SDL_Rect dst{p.x + (int) ix, p.y + (int) iy, mForeground.w, mForeground.h};
            SDL_RenderCopy(renderer, mBackground.tex, &src, &dst);
            SDL_RenderCopy(renderer, mForeground.tex, &src, &dst);
        } else {
            SDL_Rect src0{mForeground.w - (int)offset, 0, (int)offset, imgh};
            SDL_Rect dst0{p.x + (int) ix, p.y + (int) iy, (int)offset, imgh};
            SDL_RenderCopy(renderer, mBackground.tex, &src0, &dst0);
            SDL_RenderCopy(renderer, mForeground.tex, &src0, &dst0);

            SDL_Rect src1{ 0, 0, mForeground.w - (int)offset, imgh};
            SDL_Rect dst1{ p.x + (int) ix + (int)offset, p.y + (int) iy, mForeground.w - (int)offset, imgh};
            SDL_RenderCopy(renderer, mBackground.tex, &src1, &dst1);
            SDL_RenderCopy(renderer, mForeground.tex, &src1, &dst1);
        }
    }

    Widget::draw(renderer);
}

void sdlgui::GeoChrono::generateMapSurfaces(SDL_Renderer *renderer) {
    if (mDayMap)
        SDL_FreeSurface(mDayMap);
    if (mNightMap)
        SDL_FreeSurface(mNightMap);

    mDayMap = SDL_CreateRGBSurface(0, EARTH_BIG_W, EARTH_BIG_H, 32, rmask, gmask, bmask, amask);
    mNightMap = SDL_CreateRGBSurface(0, EARTH_BIG_W, EARTH_BIG_H, 32, rmask, gmask, bmask, amask);

#if USE_COMPILED_MAPS

    auto offx = computOffset();

    for (int x = 0; x < EARTH_BIG_W; ++x) {
        for (int y = 0; y < EARTH_BIG_H; ++y) {
            auto xMap = (x + offx) % EARTH_BIG_W;
            {
                auto pixels = (uint32_t *) mDayMap->pixels;
                auto[r, g, b] = Adafruit_RA8875::get_RGB_separate(x, y, Adafruit_RA8875::DAY_MAP);
                pixels[(y * EARTH_BIG_W) + xMap] = SDL_MapRGBA(mDayMap->format, r, g, b, 255);
            }
            {
                auto pixels = (uint32_t *) mNightMap->pixels;
                auto[r, g, b] = Adafruit_RA8875::get_RGB_separate(x, y, Adafruit_RA8875::NIGHT_MAP);
                pixels[(y * EARTH_BIG_W) + xMap] = SDL_MapRGBA(mNightMap->format, r, g, b, 255);
            }
        }

    }
#else
    auto dayPNG = IMG_Load("maps/day_earth.png");
    auto nightPNG = IMG_Load( "maps/night_earth.png");

    SDL_BlitSurface(dayPNG, nullptr, mDayMap, nullptr);
    SDL_BlitSurface(nightPNG, nullptr, mNightMap, nullptr);

    SDL_FreeSurface(dayPNG);
    SDL_FreeSurface(nightPNG);
#endif

    if (mBackground.tex)
        SDL_DestroyTexture(mBackground.tex);

    mBackground.tex = SDL_CreateTextureFromSurface(renderer, mNightMap);
    mBackground.w = EARTH_BIG_W;
    mBackground.h = EARTH_BIG_H;
    mBackground.path = "*auto_gen*";

    mMapsDirty = false;
}

Uint32 sdlgui::GeoChrono::timerCallback(Uint32 interval) {
    mMapsDirty = true;
    mTextureDirty = true;
    return interval;
}

std::tuple<double, double> sdlgui::subSolar() {
    using namespace std::chrono;
    auto epoch = system_clock::now();
    time_t tt = system_clock::to_time_t(epoch);

    double JD = (tt/86400.0) + 2440587.5;
    double D = JD - 2451545.0;
    double g = 357.529 + 0.98560028*D;
    double q = 280.459 + 0.98564736*D;
    double L = q + 1.915*sin(M_PI/180*g) + 0.020*sin(M_PI/180*2*g);
    double e = 23.439 - 0.00000036*D;
    double RA = 180/M_PI*atan2 (cos(M_PI/180*e)*sin(M_PI/180*L), cos(M_PI/180*L));
    auto lat = asin(sin(M_PI/180*e)*sin(M_PI/180*L));
    auto lat_d = rad2deg(lat);
#ifdef LINIX_STANDALONE_TEST
    printf ("Solar RA %g hours, Dec %g degrees\n", RA/15, 180/M_PI*ll.lat);
#endif // LINIX_STANDALONE_TEST
    double GMST = fmod(15*(18.697374558 + 24.06570982441908*D), 360.0);
    auto lng_d = fmod(RA-GMST+36000.0+180.0, 360.0) - 180.0;
    auto lng = deg2rad(lng_d);

    return std::make_tuple(lat, lng);
}
