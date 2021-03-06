//
// Created by richard on 2020-09-12.
//

#include <iostream>
#include "ImageDisplay.h"

bool sdlgui::ImageDisplay::mouseMotionEvent(const sdlgui::Vector2i &p, const sdlgui::Vector2i &rel, int button,
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

bool sdlgui::ImageDisplay::mouseButtonEvent(const sdlgui::Vector2i &p, int button, bool down, int modifiers) {
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

void sdlgui::ImageDisplay::draw(SDL_Renderer *renderer) {
    int ax = getAbsoluteLeft();
    int ay = getAbsoluteTop();

    PntRect clip = getAbsoluteCliprect();
    SDL_Rect clipRect = pntrect2srect(clip);

    if (!mImages.empty()) {
        mImageIndex = mImageIndex % (long)mImages.size();
        Vector2i p = Vector2i(mMargin, mMargin);
        p += Vector2i(ax, ay);
        int imgw = mImages[mImageIndex].w;
        int imgh = mImages[mImageIndex].h;

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

        SDL_RenderCopy(renderer, mImages[mImageIndex].tex, &imgSrcRect, &imgPaintRect);
    }

    Widget::draw(renderer);
}
