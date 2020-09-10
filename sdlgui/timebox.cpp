//
// Created by richard on 2020-09-09.
//

#include <locale>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>
#include "timebox.h"

static Uint32 TimeBoxCallbackStub(Uint32 interval, void *param);

sdlgui::TimeBox::TimeBox(sdlgui::Widget *parent, bool localTime, const std::string &font, int fontSize)
        : Widget(parent),
          mTimer(*this, &TimeBox::timerCallback, 1000),
          mIsLocalTime(localTime),
          locale_time_put(use_facet<time_put<char>>(locale())) {
    if (fontSize < 0) {
        mTimeBoxHoursMinFontSize = mTheme->mTimeBoxHoursMinFontSize;
        mTimeBoxSecFontSize = mTheme->mTimeBoxSecFontSize;
        mTimeBoxDateFontSize = mTheme->mTimeBoxDateFontSize;
    } else {
        mTimeBoxHoursMinFontSize = fontSize;
        mTimeBoxDateFontSize = fontSize;
        mTimeBoxSecFontSize = (3 * fontSize) / 4;
    }

    if (font.empty()) {
        mTimeBoxDateFont = mTheme->mTimeBoxDateFont;
        mTimeBoxTimeFont = mTheme->mTimeBoxTimeFont;
    } else {
        mTimeBoxDateFont = font;
        mTimeBoxTimeFont = font;
    }

    // Compute an offset which will (hopefully) line hours, mins and secs up by their tops
    // TODO: Label widgets should set their size based on the font size used.
    auto timeSizeDiff = std::abs(mTimeBoxHoursMinFontSize - mTimeBoxSecFontSize) / 2;

    // Construct a time Box
    withLayout<BoxLayout>(Orientation::Vertical, Alignment::Minimum, 0, 5);
    mTimeDisplay = this->add<Widget>();
    mTimeDisplay->withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Minimum, 0, 5);
    mDateDisplay = this->add<Widget>();
    mDateDisplay->withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Maximum, 0, 5);
    mHoursMins = mTimeDisplay->add<Label>("", mTimeBoxTimeFont);
    mHoursMins->setFixedHeight(mTimeBoxHoursMinFontSize);
    mHoursMins->setFontSize(mTimeBoxHoursMinFontSize);
    mSeconds = mTimeDisplay->add<Label>("", mTimeBoxTimeFont);
    mSeconds->setFontSize(mTimeBoxSecFontSize);
    mSeconds->setFixedHeight(mTimeBoxSecFontSize + timeSizeDiff);
    mDate = mDateDisplay->add<Label>("", mTimeBoxDateFont);
    mDate->setFontSize(mTimeBoxDateFontSize);
    mDate->setFixedHeight(mTimeBoxDateFontSize);

    mEpoch = std::chrono::system_clock::now();
    renderTime(mEpoch);
}

void sdlgui::TimeBox::renderTime(const std::chrono::time_point<std::chrono::system_clock> &now) {
    using namespace std::chrono;
    auto tt = system_clock::to_time_t(now);
    auto tm = mIsLocalTime ? *localtime(&tt) : *gmtime(&tt);

    stringstream hm;
    hm.imbue(locale());

    put_locale_time( hm, ' ', &tm, mTheme->mTimeBoxHoursMinFmt);
    mHoursMins->setCaption(hm.str());

    hm.str("");
    put_locale_time( hm, ' ', &tm, mTheme->mTimeBoxSecFmt);
    mSeconds->setCaption(hm.str());

    hm.str("");
    put_locale_time( hm, ' ', &tm, mTheme->mTimeBoxDateFmt);
    mDate->setCaption(hm.str());
}

Uint32 TimeBoxCallbackStub(Uint32 interval, void *param) {
    interval = static_cast<sdlgui::TimeBox*>(param)->timerCallback(interval);
    std::cout << "New timer interval " << interval << '\n';
    return interval;
}

Uint32 sdlgui::TimeBox::timerCallback(Uint32 interval) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - mEpoch;
    auto delta_seconds = elapsed_seconds - mElapsedSeconds;
    if (delta_seconds.count() > 0.9) {
        mElapsedSeconds = elapsed_seconds;
        renderTime(now);
    }

    if (delta_seconds.count() > 1.0)
        fmod(delta_seconds.count(), 1.0) * 1000.0;
    else if (delta_seconds.count() < 0.990)
        return 1000.0 - (delta_seconds.count() * 1000.0);

    return 1000;
}
