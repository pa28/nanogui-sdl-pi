//
// Created by richard on 2020-09-07.
//

#ifndef SDLGUI_PIAPPLICATION_H
#define SDLGUI_PIAPPLICATION_H

#include <string>
#include <string_view>
#include <SDL_render.h>
#include <sdlgui/common.h>
#include <sdlgui/screen.h>
#include <sdlgui/screen.h>
#include <sdlgui/window.h>
#include <sdlgui/layout.h>
#include <sdlgui/label.h>
#include <sdlgui/checkbox.h>
#include <sdlgui/button.h>
#include <sdlgui/toolbutton.h>
#include <sdlgui/popupbutton.h>
#include <sdlgui/combobox.h>
#include <sdlgui/dropdownbox.h>
#include <sdlgui/progressbar.h>
#include <sdlgui/entypo.h>
#include <sdlgui/messagedialog.h>
#include <sdlgui/textbox.h>
#include <sdlgui/slider.h>
#include <sdlgui/imagepanel.h>
#include <sdlgui/imageview.h>
#include <sdlgui/vscrollpanel.h>
#include <sdlgui/colorwheel.h>
#include <sdlgui/graph.h>
#include <sdlgui/tabwidget.h>
#include <sdlgui/switchbox.h>
#include <sdlgui/formhelper.h>
#include <sdlgui/widget.h>

class PiApplication;

class Fps
{
public:
    explicit Fps(int tickInterval = 30)
            : m_tickInterval(tickInterval)
            , m_nextTime(SDL_GetTicks() + tickInterval)
    {
    }

    void next()
    {
        SDL_Delay(getTicksToNextFrame());

        m_nextTime += m_tickInterval;
    }

private:
    const int m_tickInterval;
    Uint32 m_nextTime;

    Uint32 getTicksToNextFrame() const
    {
        Uint32 now = SDL_GetTicks();

        return (m_nextTime <= now) ? 0 : m_nextTime - now;
    }
};

class PiGraphicsContext {
protected:
    friend class PiApplication;

    std::string mCaption;

    sdlgui::Vector2i mPosition{};
    sdlgui::Vector2i mSize;

    SDL_Renderer *mRenderer{};
    SDL_Window *mWindow{};

    std::ostringstream mRendererName;

public:
    PiGraphicsContext(const sdlgui::Vector2i &position, const sdlgui::Vector2i &size,
                      const std::string &caption);

    SDL_Window &GetWindow() { return *mWindow; }

    auto Position() const { return mPosition; }

    auto Size() const { return mSize; }

    ~PiGraphicsContext();
};

class PiApplication : public sdlgui::Screen {
protected:
    bool mRunEventLoop{true};
    PiGraphicsContext &mGraphicsContext;

public:
    explicit PiApplication(PiGraphicsContext &graphicsContext);

    void eventLoop();

    void performAppLayout() {
        performLayout(mGraphicsContext.mRenderer);
    }
};


#endif //SDLGUI_PIAPPLICATION_H
