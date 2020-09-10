//
// Created by richard on 2020-09-09.
//

#ifndef SDLGUI_TIMEBOX_H
#define SDLGUI_TIMEBOX_H

#include <iomanip>
#include <string_view>
#include <sdlgui/label.h>
#include <sdlgui/widget.h>

namespace sdlgui {
    using namespace std;
    using namespace std::chrono;

    class TimeBox : public Widget {
    private:
        static constexpr array<string_view, 7> mDays = {
                "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        static constexpr array<string_view, 12> mMonths = {
                "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        const time_put<char> &locale_time_put;
        time_point <system_clock> mEpoch;
        duration<double> mElapsedSeconds{};
        bool mIsLocalTime{true};
        Widget *mTimeDisplay;
        Widget *mDateDisplay;
        Label *mHoursMins;
        Label *mSeconds;
        Label *mDate;

        int mTimeBoxHoursMinFontSize;
        int mTimeBoxSecFontSize;
        int mTimeBoxDateFontSize;

        std::string mTimeBoxTimeFont;
        std::string mTimeBoxDateFont;

    public:
        explicit TimeBox(Widget *parent,
                         const std::string &font = "", int fontSize = -1);

        void renderTime(const time_point <system_clock> &now);

        void draw(SDL_Renderer *renderer) override;

        auto put_locale_time( stringstream& strm, char fill, const std::tm *tm , const std::string &fmt) {
            if (fmt.empty()) {
                throw std::runtime_error("Format must not be empty");
            } else {
                const char *fmtbeg = fmt.c_str();
                const char *fmtend = fmtbeg + fmt.size();
                return locale_time_put.put(strm, strm, fill, tm, fmtbeg, fmtend);
            }
        }
    };
}

#endif //SDLGUI_TIMEBOX_H
