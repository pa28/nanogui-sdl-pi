#include <sdlgui/common.h>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#else
#include <dirent.h>
#endif

#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

NAMESPACE_BEGIN(sdlgui)

    SDL_Texture * get_texture(
            SDL_Renderer * pRenderer,
            const std::string& image_filename,
            const int limit) {
        SDL_Texture * result = nullptr;

        SDL_Surface * pSurface = IMG_Load(image_filename.c_str());

        if (pSurface == nullptr) {
            printf("Error image load: %s\n", IMG_GetError());
        }
        else {
            int width = pSurface->w;
            int height = pSurface->h;

            if ((width > limit) ||
                (height > limit)) {
                SDL_Rect sourceDimensions;
                sourceDimensions.x = 0;
                sourceDimensions.y = 0;
                sourceDimensions.w = width;
                sourceDimensions.h = height;

                float scale = (float)limit / (float)width;
                float scaleH = (float)limit / (float)height;

                if (scaleH < scale) {
                    scale = scaleH;
                }

                SDL_Rect targetDimensions;
                targetDimensions.x = 0;
                targetDimensions.y = 0;
                targetDimensions.w = (int)((float)width * scale);
                targetDimensions.h = (int)((float)height * scale);

                // create a 32 bits per pixel surface to Blit the image to first, before BlitScaled
                // https://stackoverflow.com/questions/33850453/sdl2-blit-scaled-from-a-palettized-8bpp-surface-gives-error-blit-combination/33944312
                SDL_Surface *p32BPPSurface = SDL_CreateRGBSurface(
                        pSurface->flags,
                        sourceDimensions.w,
                        sourceDimensions.h,
                        32,
                        pSurface->format->Rmask,
                        pSurface->format->Gmask,
                        pSurface->format->Bmask,
                        pSurface->format->Amask);

                if (SDL_BlitSurface(pSurface, nullptr, p32BPPSurface, nullptr) < 0) {
                    printf("Error did not blit surface: %s\n", SDL_GetError());
                }
                else {
                    // create another 32 bits per pixel surface are the desired scale
                    SDL_Surface *pScaleSurface = SDL_CreateRGBSurface(
                            p32BPPSurface->flags,
                            targetDimensions.w,
                            targetDimensions.h,
                            p32BPPSurface->format->BitsPerPixel,
                            p32BPPSurface->format->Rmask,
                            p32BPPSurface->format->Gmask,
                            p32BPPSurface->format->Bmask,
                            p32BPPSurface->format->Amask);

                    // 32 bit per pixel surfaces (loaded from the original file) won't scale down with BlitScaled, suggestion to first fill the surface
                    // 8 and 24 bit depth pngs did not require this
                    // https://stackoverflow.com/questions/20587999/sdl-blitscaled-doesnt-work
                    SDL_FillRect(pScaleSurface, &targetDimensions, SDL_MapRGBA(pScaleSurface->format, 255, 0, 0, 255));

                    if (SDL_BlitScaled(p32BPPSurface, nullptr, pScaleSurface, nullptr) < 0) {
                        printf("Error did not scale surface: %s\n", SDL_GetError());

                        SDL_FreeSurface(pScaleSurface);
                        pScaleSurface = nullptr;
                    }
                    else {
                        SDL_FreeSurface(pSurface);

                        pSurface = pScaleSurface;
                        width = pSurface->w;
                        height = pSurface->h;
                    }
                }

                SDL_FreeSurface(p32BPPSurface);
                p32BPPSurface = nullptr;
            }

            SDL_Texture * pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

            if (pTexture == nullptr) {
                printf("Error image load: %s\n", SDL_GetError());
            }
            else {
                SDL_SetTextureBlendMode(
                        pTexture,
                        SDL_BLENDMODE_BLEND);

                result = pTexture;
            }

            SDL_FreeSurface(pSurface);
            pSurface = nullptr;
        }

        return result;
    }

ListImages loadImageDirectory(SDL_Renderer *renderer, const std::string &path, const int limit)
{
  ListImages result;
#if !defined(_WIN32)
    DIR *dp = opendir(path.c_str());
    if (!dp)
        throw std::runtime_error("Could not open image directory!");
    struct dirent *ep;
    while ((ep = readdir(dp))) {
        const char *fname = ep->d_name;
#else
    WIN32_FIND_DATA ffd;
    std::string searchPath = path + "/*.*";
    HANDLE handle = FindFirstFileA(searchPath.c_str(), &ffd);
    if (handle == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Could not open image directory!");
    do {
        const char *fname = ffd.cFileName;
#endif
        if (strstr(fname, "png") == nullptr && strstr(fname, "jpg") == nullptr)
            continue;
        std::string fullName = path + "/" + std::string(fname);
        SDL_Texture *tex;

        if (limit == 0)
            tex = IMG_LoadTexture(renderer, fullName.c_str());
        else
            tex = get_texture(renderer, fullName, limit);

        if (tex == nullptr)
            throw std::runtime_error("Could not open image data!");
        ImageInfo iminfo;
        iminfo.tex = tex;
        iminfo.path = fullName;
        SDL_QueryTexture(tex, nullptr, nullptr, &iminfo.w, &iminfo.h);
        
        result.push_back(iminfo);
#if !defined(_WIN32)
    }
    closedir(dp);
#else
    } while (FindNextFileA(handle, &ffd) != 0);
    FindClose(handle);
#endif
    return result;
}

NAMESPACE_END(sdlgui)
