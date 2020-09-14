//
// Created by richard on 2020-09-13.
//

#ifndef SDLGUI_BRIGHNESSCONTROL_H
#define SDLGUI_BRIGHNESSCONTROL_H

#include <string_view>
#include <fstream>
#include <sdlgui/widget.h>
#include <sdlgui/slider.h>
#include <sdlgui/theme.h>
#include <iostream>

namespace sdlgui {
    class BrightnessControl {
    private:
        bool mHasBrightnessControl{true};
        Theme *mTheme{nullptr};

        static constexpr std::string_view brightnessDevice = "/sys/class/backlight/rpi_backlight/brightness";

    public:
        explicit BrightnessControl()  {
            std::ofstream ofs;
            ofs.open(std::string(brightnessDevice), std::ofstream::out);
            if (ofs) {
                ofs.close();
            } else {
                // TODO: Better error reporting.
                mHasBrightnessControl = false;
                std::cerr << "Can not open " << brightnessDevice << std::endl;
            }
        }

        void setTheme( Theme &theme) {

        }

        void setBrightness(float brightness) {
            if (mHasBrightnessControl) {
                auto min = 16;
                auto max = 255;
                auto bright = (float) (max - min) * brightness + (float) min;

                std::ofstream ofs;
                ofs.open(std::string(brightnessDevice), std::ofstream::out);
                if (ofs) {
                    ofs << (int)bright << '\n';
                    ofs.close();
                } else {
                    // TODO: Better error reporting.
                    mHasBrightnessControl = false;
                    std::cerr << "Can not open " << brightnessDevice << std::endl;
                }
            }
        }
    };
}

#endif //SDLGUI_BRIGHNESSCONTROL_H
