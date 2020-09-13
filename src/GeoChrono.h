//
// Created by richard on 2020-09-12.
//

#ifndef SDLGUI_GEOCHRONO_H
#define SDLGUI_GEOCHRONO_H

#include <sdlgui/common.h>
#include <sdlgui/widget.h>
#include <sdlgui/timebox.h>
#include <Adafruit_RA8875.h>

#include <utility>
#include <chrono>


#define USE_COMPILED_MAPS 0
#define USER_SET_CENTRE_LONG 1

namespace sdlgui {

    constexpr double deg2rad(double deg) { return deg * M_PI / 180.; }

    constexpr double rad2deg(double rad) { return rad * 180. / M_PI; }

    std::tuple<double, double> subSolar();

    /**
     * @class GeoChrono
     * A minimalist Image display widget capable of resizing and displaying one image on an ImageList
     */
    class GeoChrono : public Widget {
    public:
        enum EventType {
            UP_EVENT, LEFT_EVENT, DOWN_EVENT, RIGHT_EVENT, CLICK_EVENT
        };

        static constexpr double GrayLineCos = -0.208;
        static constexpr double GrayLinePow = 0.75;

        Timer<GeoChrono> mTimer;

    private:
        ImageInfo mForeground;      //< The foreground image
        ImageInfo mBackground;      //< The background image
        SDL_Surface *mDayMap;       //< The surface holding the day map
        SDL_Surface *mNightMap;     //< The surface holding the night map
        bool mTextureDirty{true};   //< True when the image needs to be re-drawn
        bool mMapsDirty{true};      //< True when the map surfaces need to be re-drawn

        bool mButton{false};        //< True when button 1 has been pressed
        bool mMotion{false};        //< True when the mouse has been in motion with button 1 pressed
        Vector2i mMotionStart{};    //< The starting point of the motion;
        Vector2i mMotionEnd{};      //< The ending point of the motion;

        double mCentreLongitude;    //< The longitude (in degrees, West negative) to center the map on.

        std::function<void(GeoChrono &, EventType)> mCallback;

        /**
         * The timer callback
         * @param interval
         * @return the new interval
         */
        Uint32 timerCallback(Uint32 interval);


        /**
         * Generate day and night surfaces from compiled in data.
         */
        void generateMapSurfaces(SDL_Renderer *renderer);

        auto computOffset() const {
#if USER_SET_CENTRE_LONG
            return ((unsigned long) round((((180.0 - mCentreLongitude) / 360.0)
                                           * (double) EARTH_BIG_W) + (double) EARTH_BIG_W / 2)) % EARTH_BIG_W;
#else
            return 0;
#endif
        }

    public:
        /**
         * (Constructor)
         * Construct a GeoChrono with no images
         * @param parent
         */
        explicit GeoChrono(Widget *parent) : Widget(parent), mTimer(*this, &GeoChrono::timerCallback, 60000),
                                             mDayMap{nullptr}, mNightMap{nullptr}, mCentreLongitude{0} {}

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
         * Building help, set the centre longitude.
         * @param centreLongitude in degrees, West is negative
         * @return a reference to this GeoChrono.
         */
        GeoChrono &withCentreLongitude(double centreLongitude) {
            mCentreLongitude = centreLongitude;
            mMapsDirty = true;
            mTextureDirty = true;
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
