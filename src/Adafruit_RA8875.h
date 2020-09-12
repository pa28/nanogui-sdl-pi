//
// Created by richard on 2020-09-11.
//

#ifndef ESPHAMCLOCK_ADAFRUIT_RA8875_H
#define ESPHAMCLOCK_ADAFRUIT_RA8875_H

#include <cstdint>
#include <tuple>

#define	PROGMEM

#if defined(_CLOCK_1600x960)

static constexpr std::size_t FB_XRES = 1600;
static constexpr std::size_t FB_YRES = 960;
static constexpr std::size_t EARTH_BIG_W = 1320;
static constexpr std::size_t EARTH_BIG_H = 660;

#elif defined(_CLOCK_2400x1440)

static constexpr std::size_t FB_XRES = 2400;
static constexpr std::size_t FB_YRES = 1440;
static constexpr std::size_t EARTH_BIG_W = 1980;
static constexpr std::size_t EARTH_BIG_H = 990;

#elif defined(_CLOCK_3200x1920)

static constexpr std::size_t FB_XRES = 3200;
static constexpr std::size_t FB_YRES = 1920;
static constexpr std::size_t EARTH_BIG_W = 2640;
static constexpr std::size_t EARTH_BIG_H = 1320;

#else   // original size

static constexpr std::size_t FB_XRES = 800;
static constexpr std::size_t FB_YRES = 480;
static constexpr std::size_t EARTH_BIG_W = 660;
static constexpr std::size_t EARTH_BIG_H = 330;

#endif


class Adafruit_RA8875 {
public:
    enum Map {
        DAY_MAP,
        NIGHT_MAP,
    };

static constexpr auto  RGB565_R(uint16_t c) {return  (((c) & 0xF800U) >> 8U); }
static constexpr auto  RGB565_G(uint16_t c) {return  (((c) & 0x07E0U) >> 3U); }
static constexpr auto  RGB565_B(uint16_t c) {return  (((c) & 0x001FU) << 3U); }


    static auto get_RGB_separate( std::size_t x, std::size_t y, Map map) {
        auto c = map == DAY_MAP ? DEARTH_BIG[x][y] : NEARTH_BIG[x][y];
        return std::make_tuple( RGB565_R(c), RGB565_G(c), RGB565_B(c));
    }

    static auto get_ARGB( std::size_t x, std::size_t y, Map map, uint a = 0xFFU) {
        auto [r, g, b] = get_RGB_separate(x, y, map);
        return uint32_t{ ((a & 0xFFU) << 24U) | ((r & 0xFFU) << 16U) | ((g & 0xFFU) << 8U) | (b & 0xFFU)};
    }

    static auto get_RGBA( std::size_t x, std::size_t y, Map map, uint a = 0xFFU) {
        auto [r, g, b] = get_RGB_separate(x, y, map);
        return uint32_t{ ((r & 0xFFU) << 24U) | ((g & 0xFFU) << 16U) | ((b & 0xFFU) << 8U) | (a & 0xFFU)};
    }

private:
    static const uint16_t DEARTH_BIG[EARTH_BIG_H][EARTH_BIG_W];
    static const uint16_t NEARTH_BIG[EARTH_BIG_H][EARTH_BIG_W];

};

#endif //ESPHAMCLOCK_ADAFRUIT_RA8875_H
