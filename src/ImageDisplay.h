//
// Created by richard on 2020-09-12.
//

#ifndef SDLGUI_IMAGEDISPLAY_H
#define SDLGUI_IMAGEDISPLAY_H

#include <sdlgui/common.h>
#include <sdlgui/widget.h>

namespace sdlgui {


    /**
     * @class ImageDisplay
     * A minimalist Image display widget capable of resizing and displaying one image on an ImageList
     */
    class ImageDisplay : public Widget {
    public:
        enum EventType {
            UP_EVENT, LEFT_EVENT, DOWN_EVENT, RIGHT_EVENT, CLICK_EVENT
        };

    private:
        ListImages mImages;         //< This list of images
        long mImageIndex{0};        //< The index to the image displayed
        int mMargin{0};             //< The margin around the image
        bool mTextureDirty{true};   //< True when the image needs to be re-drawn

        bool mButton{false};        //< True when button 1 has been pressed
        bool mMotion{false};        //< True when the mouse has been in motion with button 1 pressed
        Vector2i mMotionStart{};    //< The starting point of the motion;
        Vector2i mMotionEnd{};      //< The ending point of the motion;

        std::function<void(ImageDisplay &, EventType)> mCallback;

    public:
        /**
         * (Constructor)
         * Construct an empty ImageDisplay
         * @param parent the parent widget
         */
        explicit ImageDisplay(Widget *parent) : Widget(parent) {}

        /**
         * (Constructor)
         * Construct an ImageDisplay with an ListImages
         * @param parent the parent widget
         * @param data the ListImages
         */
        ImageDisplay(Widget *parent, const ListImages &data)
                : ImageDisplay(parent) { setImages(data); }

        /**
         * Set the ListImages
         * @param listImages
         */
        void setImages(const ListImages &listImages) {
            mImages = listImages;
            setImageIndex(0);
        }

        /**
         * Get the ImageList
         * @return a const reference to the ListImages.
         */
        const ListImages &images() const { return mImages; }

        /**
         * Get the current image index
         * @return the image index
         */
        auto getImageIndex() const { return mImageIndex; }

        /**
         * Set the image index. The result set is modulo the size of the ListImages so -1 is allowed.
         * @param index the index to set
         */
        void setImageIndex(long index) {
            mImageIndex = ((long) mImages.size() + index) % (long) mImages.size();
        }

        bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;

        bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

        /**
         * Override the Widget draw method.
         * @param renderer the renderer to draw to.
         */
        void draw(SDL_Renderer *renderer) override;

        /**
         * Building help, add a List of Images
         * @param listImages
         * @return a reference to this ImageDisplay
         */
        ImageDisplay &withImages(const ListImages &listImages) {
            setImages(listImages);
            return *this;
        }

        /**
         * Building help, set the index to the desired image.
         * @param idx the requested index
         * @return a reference to this ImageDisplay
         */
        ImageDisplay &withImageIndex(const long idx) {
            mImageIndex = idx % (long)mImages.size();
            return *this;
        }

        /**
         * Get the currently set callback function.
         * @return the callback function.
         */
        std::function<void(ImageDisplay &, EventType)> callback() const { return mCallback; }

        /**
         * Set the call back function
         * @param callback the desired callback function, a simple but usefule lambda is:
         * [](ImageDisplay &w, ImageDisplay::EventType e) {
                            switch(e) {
                                case ImageDisplay::RIGHT_EVENT:
                                case ImageDisplay::DOWN_EVENT:
                                    w.setImageIndex(w.getImageIndex()+1);
                                    break;
                                case ImageDisplay::LEFT_EVENT:
                                case ImageDisplay::UP_EVENT:
                                    w.setImageIndex(w.getImageIndex()-1);
                                    break;
                            }
                        }
         * @return a reference to this ImageDisplay
         */
        ImageDisplay &setCallback(const std::function<void(ImageDisplay &, EventType)> &callback) {
            mCallback = callback;
            return *this;
        }

    };

}


#endif //SDLGUI_IMAGEDISPLAY_H
