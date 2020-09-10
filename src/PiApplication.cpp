//
// Created by richard on 2020-09-07.
//

#include <locale>
#include "PiApplication.h"

PiGraphicsContext::PiGraphicsContext(const sdlgui::Vector2i &position, const sdlgui::Vector2i &size,
                                     const std::string &caption) :
        mSize(size),
        mPosition(position),
        mCaption(caption) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);   // Initialize SDL2

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    std::locale::global(std::locale(""));

#if 0
    SDL_ShowCursor(SDL_DISABLE);    // ToDo: Create a timeout system to SDL_ENABLE/SDL_DISABLE the cursor.
#endif

    // Create an application window with the following settings:
    mWindow = SDL_CreateWindow(
            "An SDL2 window",         //    const char* title
            mPosition.x,  //    int x: initial x position
            mPosition.y,  //    int y: initial y position
            mSize.x,                      //    int w: width, in pixels
            mSize.y,                      //    int h: height, in pixels
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
            SDL_WINDOW_ALLOW_HIGHDPI        //    Uint32 flags: window options, see docs
    );

    // Check that the window was successfully made
    if (mWindow == nullptr) {
        // In the event that the window could not be made...
        throw std::runtime_error(SDL_GetError());
    }

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(it, &info);
        mRendererName << info.name << ' ';
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
}

PiGraphicsContext::~PiGraphicsContext() {
    SDL_Quit();
}

PiApplication::PiApplication(PiGraphicsContext &graphicsContext) : Screen(graphicsContext.mWindow,
                                                                          graphicsContext.mSize,
                                                                          graphicsContext.mCaption, false, true),
                                                                   mGraphicsContext(graphicsContext) {

}

void PiApplication::eventLoop() {
    try {
        SDL_Event e;
        Fps fps;

        while (mRunEventLoop) {
            //Handle events on queue
            while (SDL_PollEvent(&e) != 0) {
                //User requests quit
                if (e.type == SDL_QUIT) {
                    mRunEventLoop = false;
                    continue;
                }

                if (e.type == SDL_FINGERDOWN || e.type == SDL_FINGERUP) {
//                    onEvent(e);
                    SDL_Event mbe;

                    // Translate finger events to mouse evnets.
                    mbe.type = (e.type == SDL_FINGERDOWN) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
                    mbe.button.timestamp = e.tfinger.timestamp;
                    mbe.button.windowID = SDL_GetWindowID(mGraphicsContext.mWindow);
                    mbe.button.which = SDL_TOUCH_MOUSEID;
                    mbe.button.button = SDL_BUTTON_LEFT;
                    mbe.button.state = (e.type == SDL_FINGERDOWN) ? SDL_PRESSED : SDL_RELEASED;
                    mbe.button.clicks = 1;
                    mbe.button.x = (Sint32) (e.tfinger.x * (float) mGraphicsContext.mSize.x);
                    mbe.button.y = (Sint32) (e.tfinger.y * (float) mGraphicsContext.mSize.y);
                    SDL_WarpMouseGlobal(mbe.button.x, mbe.button.y);
                    SDL_PushEvent(&mbe);
                } else if (e.type == SDL_FINGERMOTION) {
                    //printEvent( std::cout, e );
                    SDL_WarpMouseGlobal((Sint32) (e.tfinger.x * (float) mGraphicsContext.mSize.x),
                                        (Sint32) (e.tfinger.y * (float) mGraphicsContext.mSize.y));
                } else
                    onEvent(e);
            }

            SDL_SetRenderDrawColor(mRenderer, 0x0, 0x0, 0x0, 0xff);
            SDL_RenderClear(mRenderer);

            drawAll();

            // Render the rect to the screen
            SDL_RenderPresent(mRenderer);

            fps.next();
        }
    }

    catch (const std::runtime_error &e) {
        throw e;
    }
}
