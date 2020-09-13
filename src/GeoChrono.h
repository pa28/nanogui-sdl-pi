//
// Created by richard on 2020-09-12.
//

#ifndef SDLGUI_GEOCHRONO_H
#define SDLGUI_GEOCHRONO_H

#include <sdlgui/common.h>
#include <sdlgui/widget.h>

#include <utility>

namespace sdlgui {

    /**
     * @class GeoChrono
     * A minimalist Image display widget capable of resizing and displaying one image on an ImageList
     */
    class GeoChrono : public Widget {
    public:
        enum EventType {
            UP_EVENT, LEFT_EVENT, DOWN_EVENT, RIGHT_EVENT, CLICK_EVENT
        };

    private:
        ImageInfo mForeground;      //< The foreground image
        ImageInfo mBackground;      //< The background image
        bool mTextureDirty{true};   //< True when the image needs to be re-drawn

        bool mButton{false};        //< True when button 1 has been pressed
        bool mMotion{false};        //< True when the mouse has been in motion with button 1 pressed
        Vector2i mMotionStart{};    //< The starting point of the motion;
        Vector2i mMotionEnd{};      //< The ending point of the motion;

        std::function<void(GeoChrono &, EventType)> mCallback;

    public:
        /**
         * (Constructor)
         * Construct a GeoChrono with no images
         * @param parent
         */
        explicit GeoChrono(Widget *parent) : Widget(parent) {}

        /**
         * (Constructor)
         * Construct a GeoChrono with images
         * @param parent
         * @param foreground the foreground image
         * @param background the background image
         */
        GeoChrono(Widget *parent, ImageInfo foreground, ImageInfo background)
                : Widget(parent), mForeground(std::move(foreground)), mBackground(std::move(background)) {}

        bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;

        bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

        /**
         * Override the Widget draw method.
         * @param renderer the renderer to draw to.
         */
        void draw(SDL_Renderer *renderer) override;

        /**
         * Building help, add a List of Images
         * @param foreground
         * @return a reference to this GeoChrono
         */
        GeoChrono &withForeground(ImageInfo &foreground) {
            mForeground = foreground;
            return *this;
        }

        /**
         * Building help, add a List of Images
         * @param background
         * @return a reference to this GeoChrono
         */
        GeoChrono &withBackground(ImageInfo &background) {
            mBackground = background;
            return *this;
        }

        /**
         * Get the currently set callback function.
         * @return the callback function.
         */
        std::function<void(GeoChrono &, EventType)> callback() const { return mCallback; }

        /**
         * Set the call back function
         * @param callback the desired callback function, a simple but usefule lambda is:
         * [](GeoChrono &w, GeoChrono::EventType e) {
                            switch(e) {
                                case GeoChrono::RIGHT_EVENT:
                                case GeoChrono::DOWN_EVENT:
                                    w.setImageIndex(w.getImageIndex()+1);
                                    break;
                                case GeoChrono::LEFT_EVENT:
                                case GeoChrono::UP_EVENT:
                                    w.setImageIndex(w.getImageIndex()-1);
                                    break;
                            }
                        }
         * @return a reference to this GeoChrono
         */
        GeoChrono &setCallback(const std::function<void(GeoChrono &, EventType)> &callback) {
            mCallback = callback;
            return *this;
        }

    };

}


#endif //SDLGUI_GEOCHRONO_H
