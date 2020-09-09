/*
    hamutilites.cpp 
    how to use the various widget classes. 

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

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
#include <memory>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

#undef main

using namespace sdlgui;

class TestWindow : public Screen
{
public:
    TestWindow( SDL_Window* pwindow, int rwidth, int rheight )
      : Screen( pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
      {
        ListImages images = loadImageDirectory(SDL_GetRenderer(pwindow), "images", 400);

        {
          auto& swindow = window("Basic widgets", Vector2i{0, 0 }).withLayout<GroupLayout>();

          swindow.label("Image panel & scroll panel", "sans-bold");
          auto& imagePanelBtn = swindow.popupbutton("Image Panel", ENTYPO_ICON_FOLDER);

          // Load all of the images by creating a GLTexture object and saving the pixel data.
          mCurrentImage = 0;
          for (auto& icon : images) mImagesData.emplace_back(icon.tex);

          auto& img_window = window("Selected image", Vector2i(100, 50));
          img_window.withLayout<GroupLayout>();

          auto imageView = img_window.add<ImageView>(mImagesData[0]);

          imagePanelBtn.popup(Vector2i(245, 150))
                         .vscrollpanel()
                           .imgpanel(images)
                             .setCallback([this, imageView](int i)
                             {
                               if (i >= mImagesData.size())
                                 return;
                               imageView->bindImage(mImagesData[i]);
                               mCurrentImage = i;
                               cout << "Selected item " << i << '\n';
                             });


          // Change the active textures.
          
          imageView->setGridThreshold(20);
          imageView->setPixelInfoThreshold(20);
          imageView->setPixelInfoCallback(
              [this, imageView](const Vector2i& index) -> std::pair<std::string, Color>
              {
                void *pixels;
                int pitch, w, h;
                SDL_QueryTexture(mImagesData[mCurrentImage], nullptr, nullptr, &w, &h);

                SDL_LockTexture(mImagesData[mCurrentImage], nullptr, &pixels, &pitch);
                auto *imageData = (Uint32*)pixels;

                std::string stringData;
                uint16_t channelSum = 0;
                for (int i = 0; i != 4; ++i) 
                {
                    auto *data = (uint8_t*)imageData;
                    auto& channelData = data[4*index.y*w + 4*index.x + i];
                    channelSum += channelData;
                    stringData += (std::to_string(static_cast<int>(channelData)) + "\n");
                }
                float intensity = static_cast<float>(255 - (channelSum / 4)) / 255.0f;
                float colorScale = intensity > 0.5f ? (intensity + 1) / 2 : intensity / 2;
                Color textColor = Color(colorScale, 1.0f);
                SDL_UnlockTexture(mImagesData[mCurrentImage]);
                return { stringData, textColor };
              }
          );

        }

        performLayout(mRenderer);
    }

    ~TestWindow() override = default;

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override
    {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;

        //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        // {
        //    setVisible(false);
        //    return true;
        //}
        return false;
    }

    void draw(SDL_Renderer* renderer) override
    {
      if (auto pbar = gfind<ProgressBar>("progressbar"))
      {
        pbar->setValue(pbar->value() + 0.001f);
        if (pbar->value() >= 1.f)
          pbar->setValue(0.f);
      }

      Screen::draw(renderer);
    }

    void drawContents() override
    {
    }
private:
    std::vector<SDL_Texture*> mImagesData;
    int mCurrentImage;
};


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


std::ostream &printEvent( std::ostream &os, const SDL_Event &e) {
    if (e.type == SDL_MOUSEBUTTONDOWN ||
    	e.type == SDL_MOUSEBUTTONUP) {
	    os << "type:     " << e.type << '\n'
               << "windowID: " << e.button.windowID << '\n'
	       << "which:    " << e.button.which << '\n'
	       << "button:   " << (Uint32)e.button.button << '\n'
	       << "state:    " << (Uint32)e.button.state << '\n'
	       << "clicks:   " << (Uint32)e.button.clicks << '\n'
	       << "x:        " << e.button.x << '\n'
	       << "y:        " << e.button.y << '\n'
	       << std::endl;
    } else if ( e.type == SDL_FINGERMOTION ||
        e.type == SDL_FINGERDOWN ||
        e.type == SDL_FINGERUP ) {
	    os << "type:     " << e.type << '\n'
	       << "touchId:  " << e.tfinger.touchId << '\n'
	       << "fingerId: " << e.tfinger.fingerId << '\n'
	       << "x:        " << e.tfinger.x << '\n'
	       << "y:        " << e.tfinger.y << '\n'
	       << "dx:       " << e.tfinger.dx << '\n'
	       << "dy:       " << e.tfinger.dy << '\n'
	       << std::endl;
    }

    return os;
}

int main(int /* argc */, char ** /* argv */)
{
    char rendername[256] = {0};
    SDL_RendererInfo info;

    SDL_Init(SDL_INIT_VIDEO);   // Initialize SDL2

    SDL_Window *window;        // Declare a pointer to an SDL_Window

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int winWidth = 800;
    int winHeight = 480;

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
      "An SDL2 window",         //    const char* title
      SDL_WINDOWPOS_UNDEFINED,  //    int x: initial x position
      SDL_WINDOWPOS_UNDEFINED,  //    int y: initial y position
      winWidth,                      //    int w: width, in pixels
      winHeight,                      //    int h: height, in pixels
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN  | SDL_WINDOW_ALLOW_HIGHDPI        //    Uint32 flags: window options, see docs
    );

    // Check that the window was successfully made
    if(window==nullptr){
      // In the event that the window could not be made...
      std::cout << "Could not create window: " << SDL_GetError() << '\n';
      SDL_Quit();
      return 1;
    }

    auto context = SDL_GL_CreateContext(window);

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++) {
        SDL_GetRenderDriverInfo(it, &info);
        strcat(rendername, info.name);
        strcat(rendername, " ");
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    auto *screen = new TestWindow(window, winWidth, winHeight);

    Fps fps;

    bool quit = false;
    try
    {
        SDL_Event e;
        while( !quit )
        {
            //Handle events on queue
            while( SDL_PollEvent( &e ) != 0 )
            {
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }


                if ( e.type == SDL_FINGERDOWN || e.type == SDL_FINGERUP ) {
		    //printEvent( std::cout, e );
		    //std::cout << "SDL_FINGERDOWN: " << e.type << std::endl;
		    screen->onEvent( e );
		    SDL_Event mbe;

		    mbe.type = ( e.type == SDL_FINGERDOWN ) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
		    mbe.button.timestamp = e.tfinger.timestamp;
		    mbe.button.windowID = SDL_GetWindowID(window);
		    mbe.button.which = SDL_TOUCH_MOUSEID;
		    mbe.button.button = SDL_BUTTON_LEFT;
		    mbe.button.state = ( e.type == SDL_FINGERDOWN ) ? SDL_PRESSED : SDL_RELEASED;
		    mbe.button.clicks = 1;
		    mbe.button.x = (Sint32)(e.tfinger.x * (float)winWidth);
		    mbe.button.y = (Sint32)(e.tfinger.y * (float)winHeight);
		    SDL_WarpMouseGlobal(mbe.button.x, mbe.button.y);
		    //std::cout << "Replaced with: SDL_MOUSEBUTTONDOWN: " << e.type << ' ' << winWidth << 'x' << winHeight <<std::endl;
		    //printEvent( std::cout, mbe );
		    SDL_PushEvent( &mbe );
		} else if ( e.type == SDL_FINGERMOTION ) {
		    //printEvent( std::cout, e );
		    SDL_WarpMouseGlobal((Sint32)(e.tfinger.x * (float)winWidth), (Sint32)(e.tfinger.y * (float)winHeight));
		} else
                    screen->onEvent( e );
            }
            
            SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff );
            SDL_RenderClear( renderer );

            screen->drawAll();

            // Render the rect to the screen
            SDL_RenderPresent(renderer);

            fps.next();
        }
    }
    catch (const std::runtime_error &e)
    {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }
    return 0;
}
