//
// Created by richard on 2020-09-07.
//

#include <iostream>
#include <chrono>
#include <iomanip>
#include <array>
#include <string_view>
#include <PiApplication.h>
#include <sdlgui/nanovg.h>

using namespace sdlgui;
using namespace std;
using namespace std::chrono;

class TimeBox : public Widget {
private:
    static constexpr array<string_view, 7> mDays = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static constexpr array<string_view, 12> mMonths = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    time_point<system_clock> mEpoch;
    duration<double> mElapsedSeconds{};
    bool mIsLocalTime{false};
    Widget *mTimeDisplay;
    Widget *mDateDisplay;
    Label *mHoursMins;
    Label *mSeconds;
    Label *mDate;

public:
    explicit TimeBox(Widget *parent) : Widget(parent) {
        withLayout<BoxLayout>(Orientation::Vertical, Alignment::Minimum, 0, 5);
        mTimeDisplay = this->add<Widget>();
        mTimeDisplay->withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Minimum, 0, 5);
        mDateDisplay = this->add<Widget>();
        mDateDisplay->withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Maximum, 0, 5);
        mHoursMins = mTimeDisplay->add<Label>("", mTheme->mTimeBoxTimeFont);
        mHoursMins->setFontSize(mTheme->mTimeBoxHoursMinFontSize);
        mSeconds = mTimeDisplay->add<Label>("", mTheme->mTimeBoxTimeFont);
        mSeconds->setFontSize(mTheme->mTimeBoxSecFontSize);
        mDate = mDateDisplay->add<Label>("", mTheme->mTimeBoxDateFont);

        mEpoch = std::chrono::system_clock::now();
        renderTime(mEpoch);
    }

    void renderTime(const time_point<system_clock> &now) {
        using namespace std::chrono;
        auto tt = system_clock::to_time_t(now);
        auto tm = mIsLocalTime ? *localtime(&tt) : *gmtime(&tt);

        stringstream hm;
        hm << setw(2) << setfill('0') << tm.tm_hour << ':'
           << setw(2) << setfill('0') << tm.tm_min;
        mHoursMins->setCaption(hm.str());

        hm.str("");
        hm << setw(2) << setfill('0') << tm.tm_sec;
        mSeconds->setCaption(hm.str());

        hm.str("");
        hm << mDays[tm.tm_wday] << ' ' << mMonths[tm.tm_mon] << ' ' << tm.tm_mday << ", " << tm.tm_year + 1900;
        mDate->setCaption(hm.str());
    }

    void draw(SDL_Renderer *renderer) override {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - mEpoch;
        if ((elapsed_seconds - mElapsedSeconds).count() >= 1.0) {
            mElapsedSeconds = elapsed_seconds;
            renderTime(now);
        }
        Widget::draw(renderer);
    }
};


class HamChrono : public PiApplication {
private:
    std::vector<SDL_Texture *> mImagesData;
    sdlgui::ListImages mImages;
    int mCurrentImage{0};

public:
    explicit HamChrono(PiGraphicsContext &graphicsContext) : PiApplication(graphicsContext) {
        {
            {
                auto &nwindow = framelessWindow(/*"Button demo", graphicsContext.Position()*/)
                        .withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Minimum, 0, 0)
                        .withPosition(graphicsContext.Position())
                        .withFixedSize(graphicsContext.Size());

                auto &pannel1 = nwindow.add<Widget>()
                        ->withLayout<BoxLayout>(Orientation::Vertical, Alignment::Minimum, 10, 10)
                        .withFixedSize(Vector2i(200, 480));

                pannel1.button("VE3YSH", [] { cout << "QTH Edit\n"; })
                        .setFontSize(30);
                pannel1.wdg<TimeBox>();

//                nwindow.widget().withLayout<BoxLayout>(Orientation::Vertical, Alignment::Minimum)
//                        .withFixedSize(Vector2i( 200, 480 ))
//                        .label("VE3YSH", "FreeSansBold")._and()
//                        .button("Plain button", [] { cout << "pushed!" << endl; })
//                        .withTooltip("This is plain button tips");
//
//                nwindow.button("Styled", ENTYPO_ICON_ROCKET, [] { cout << "pushed!" << endl; })
//                        .withBackgroundColor(Color(0, 0, 255, 25));
//
//                nwindow.label("Toggle buttons", "sans-bold")._and()
//                        .button("Toggle me", [](bool state) { cout << "Toggle button state: " << state << endl; })
//                        .withFlags(Button::ToggleButton);
//
//                nwindow.label("Radio buttons", "sans-bold")._and()
//                        .button("Radio button 1")
//                        .withFlags(Button::RadioButton);
//
//                nwindow.button("Radio button 2")
//                        .withFlags(Button::RadioButton)._and()
//                        .label("A tool palette", "sans-bold");
//
//                auto &tools = nwindow.widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
//
//                tools.toolbutton(ENTYPO_ICON_CLOUD)._and()
//                        .toolbutton(ENTYPO_ICON_FF)._and()
//                        .toolbutton(ENTYPO_ICON_COMPASS)._and()
//                        .toolbutton(ENTYPO_ICON_INSTALL);
//
//                nwindow.label("Popup buttons", "sans-bold")._and()
//                        .popupbutton("Popup", ENTYPO_ICON_EXPORT)
//                        .popup()
//                        .withLayout<GroupLayout>()
//                        .label("Arbitrary widgets can be placed here")._and()
//                        .checkbox("A check box")._and()
//                        .popupbutton("Recursive popup", ENTYPO_ICON_FLASH).popup()
//                        .withLayout<GroupLayout>()
//                        .checkbox("Another check box");
            }

//            ListImages images = loadImageDirectory(SDL_GetRenderer(&graphicsContext.GetWindow()), "icons", 0);
//
//            {
//                auto &pwindow = window("Basic widgets", Vector2i{200, 15}).withLayout<GroupLayout>();
//
//                pwindow.label("Message dialog", "sans-bold")._and()
//                        .widget()
//                        .boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6)
//                        .button("Info", [&] {
//                            msgdialog(MessageDialog::Type::Information, "Title", "This is an information message",
//                                      [](int result) { cout << "Dialog result: " << result << endl; });
//                        })._and()
//                        .button("Warn", [&] {
//                            msgdialog(MessageDialog::Type::Warning, "Title", "This is a warning message",
//                                      [](int result) { cout << "Dialog result: " << result << endl; });
//                        })._and()
//                        .button("Ask", [&] {
//                            msgdialog(MessageDialog::Type::Warning, "Title", "This is a question message",
//                                      "Yes", "No", true,
//                                      [](int result) { cout << "Dialog result: " << result << endl; });
//                        });
//
//                pwindow.label("Image panel & scroll panel", "sans-bold");
//                auto &imagePanelBtn = pwindow.popupbutton("Image Panel", ENTYPO_ICON_FOLDER);
//
//                // Load all of the images by creating a GLTexture object and saving the pixel data.
//                mCurrentImage = 0;
//                for (auto &icon : images) mImagesData.emplace_back(icon.tex);
//
//                auto &img_window = window("Selected image", Vector2i(100, 0));
//
//                auto imageView = img_window.add<ImageView>(mImagesData[0]);
//
//                imagePanelBtn.popup(Vector2i(245, 150))
//                        .vscrollpanel()
//                        .imgpanel(images)
//                        .setCallback([this, imageView](int i) {
//                            if (i >= mImagesData.size())
//                                return;
//                            imageView->bindImage(mImagesData[i]);
//                            mCurrentImage = i;
//                            cout << "Selected item " << i << '\n';
//                        });
//
//
//                // Change the active textures.
//
//                imageView->setGridThreshold(20);
//                imageView->setPixelInfoThreshold(20);
//                imageView->setPixelInfoCallback(
//                        [this, imageView](const Vector2i &index) -> std::pair<std::string, Color> {
//                            void *pixels;
//                            int pitch, w, h;
//                            SDL_QueryTexture(mImagesData[mCurrentImage], nullptr, nullptr, &w, &h);
//
//                            SDL_LockTexture(mImagesData[mCurrentImage], nullptr, &pixels, &pitch);
//                            Uint32 *imageData = (Uint32 *) pixels;
//
//                            std::string stringData;
//                            uint16_t channelSum = 0;
//                            for (int i = 0; i != 4; ++i) {
//                                uint8_t *data = (uint8_t *) imageData;
//                                auto &channelData = data[4 * index.y * w + 4 * index.x + i];
//                                channelSum += channelData;
//                                stringData += (std::to_string(static_cast<int>(channelData)) + "\n");
//                            }
//                            float intensity = static_cast<float>(255 - (channelSum / 4)) / 255.0f;
//                            float colorScale = intensity > 0.5f ? (intensity + 1) / 2 : intensity / 2;
//                            Color textColor = Color(colorScale, 1.0f);
//                            SDL_UnlockTexture(mImagesData[mCurrentImage]);
//                            return {stringData, textColor};
//                        }
//                );
//
//                pwindow.label("File dialog", "sans-bold")._and()
//                        .widget()
//                        .boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6)
//                        .button("Open", [&] {
//                            cout << "File dialog result: " << file_dialog(
//                                    {{"png", "Portable Network Graphics"},
//                                     {"txt", "Text file"}}, false) << endl;
//                        })._and()
//                        .button("Save", [&] {
//                            cout << "File dialog result: " << file_dialog(
//                                    {{"png", "Portable Network Graphics"},
//                                     {"txt", "Text file"}}, true) << endl;
//                        });
//
//                pwindow.label("Combo box", "sans-bold")._and()
//                        .dropdownbox(std::vector<std::string>{"Dropdown item 1", "Dropdown item 2",
//                                                              "Dropdown item 3"})._and()
//                        .combobox(std::vector<std::string>{"Combo box item 1", "Combo box item 2",
//                                                           "Combo box item 3"})._and()
//                        .label("Check box", "sans-bold")._and()
//                        .checkbox("Flag 1", [](bool state) { cout << "Check box 1 state: " << state << endl; })
//                        .withChecked(true)._and()
//                        .checkbox("Flag 2", [](bool state) { cout << "Check box 2 state: " << state << endl; })._and()
//                        .label("Progress bar", "sans-bold")._and()
//                        .progressbar().withId("progressbar")._and()
//                        .label("Slider and text box", "sans-bold")._and()
//                        .widget().withLayout<BoxLayout>(Orientation::Horizontal, Alignment::Middle, 0, 20)
//                        .slider(0.5f, [](Slider *obj, float value) {
//                            if (auto *textBox = obj->gfind<TextBox>("slider-textbox"))
//                                textBox->setValue(std::to_string((int) (value * 100)));
//                        }, [](float value) { cout << "Final slider value: " << (int) (value * 100) << endl; })
//                        .withFixedWidth(80)._and()
//                        .textbox("50", "%").withAlignment(TextBox::Alignment::Right)
//                        .withId("slider-textbox")
//                        .withFixedSize(Vector2i(60, 25))
//                        .withFontSize(20);
//
//                pwindow.label("A switch boxes", "sans-bold");
//                Widget *swbx = new Widget(&pwindow);
//                swbx->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 2));
//
//                auto *swbh = new SwitchBox(swbx, SwitchBox::Alignment::Horizontal, "");
//                swbh->setFixedSize(Vector2i(84, 32));
//                new SwitchBox(swbx, SwitchBox::Alignment::Vertical, "");
//            }
//
//            {
//                auto &window = wdg<Window>("Misc. widgets");
//                window.setPosition(425, 15);
//                window.setFixedSize({400, 300});
//                window.setLayout(new GroupLayout());
//                TabWidget *tabWidget = window.add<TabWidget>();
//
//                Widget *layer = tabWidget->createTab("Color Wheel");
//                layer->setLayout(new GroupLayout());
//
//                // Use overloaded variadic add to fill the tab widget with Different tabs.
//                layer->add<Label>("Color wheel widget", "sans-bold");
//                layer->add<ColorWheel>();
//
//                layer = tabWidget->createTab("Function Graph");
//                layer->setLayout(new GroupLayout());
//
//                layer->add<Label>("Function graph widget", "sans-bold");
//
//                Graph *graph = layer->add<Graph>("Some Function");
//
//                graph->setHeader("E = 2.35e-3");
//                graph->setFooter("Iteration 89");
//                std::vector<float> &func = graph->values();
//                func.resize(100);
//                for (int i = 0; i < 100; ++i)
//                    func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
//                                      0.5f * std::cos(i / 23.f) + 1);
//
//                // Dummy tab used to represent the last tab button.
//                tabWidget->createTab("+");
//
//                // A simple counter.
//                int counter = 1;
//                tabWidget->setCallback([tabWidget, this, counter](int index) mutable {
//                    if (index == (tabWidget->tabCount() - 1)) {
//                        // When the "+" tab has been clicked, simply add a new tab.
//                        std::string tabName = "Dynamic " + std::to_string(counter);
//                        Widget *layerDyn = tabWidget->createTab(index, tabName);
//                        layerDyn->setLayout(new GroupLayout());
//                        layerDyn->add<Label>("Function graph widget", "sans-bold");
//                        Graph *graphDyn = layerDyn->add<Graph>("Dynamic function");
//
//                        graphDyn->setHeader("E = 2.35e-3");
//                        graphDyn->setFooter("Iteration " + std::to_string(index * counter));
//                        std::vector<float> &funcDyn = graphDyn->values();
//                        funcDyn.resize(100);
//                        for (int i = 0; i < 100; ++i)
//                            funcDyn[i] = 0.5f *
//                                         std::abs((0.5f * std::sin(i / 10.f + counter) +
//                                                   0.5f * std::cos(i / 23.f + 1 + counter)));
//                        ++counter;
//                        // We must invoke perform layout from the screen instance to keep everything in order.
//                        // This is essential when creating tabs dynamically.
//                        performLayout();
//                        // Ensure that the newly added header is visible on screen
//                        tabWidget->ensureTabVisible(index);
//
//                    }
//                });
//                tabWidget->setActiveTab(0);
//
//                // A button to go back to the first tab and scroll the window.
//                auto &panel = window.wdg<Widget>();
//                panel.add<Label>("Jump to tab: ");
//                panel.setLayout(new BoxLayout(Orientation::Horizontal,
//                                              Alignment::Middle, 0, 6));
//
//                auto ib = panel.add<IntBox<int>>();
//                ib->setEditable(true);
//
//                auto b = panel.add<Button>("", ENTYPO_ICON_FORWARD);
//                b->setFixedSize(Vector2i(22, 22));
//                ib->setFixedHeight(22);
//                b->setCallback([tabWidget, ib] {
//                    int value = ib->value();
//                    if (value >= 0 && value < tabWidget->tabCount()) {
//                        tabWidget->setActiveTab(value);
//                        tabWidget->ensureTabVisible(value);
//                    }
//                });
//            }
//
//            {
//                auto &window = wdg<Window>("Grid of small widgets");
//                window.withPosition({425, 288});
//                auto *layout = new GridLayout(Orientation::Horizontal, 2,
//                                              Alignment::Middle, 15, 5);
//                layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
//                layout->setSpacing(0, 10);
//                window.setLayout(layout);
//
//                window.add<Label>("Floating point :", "sans-bold");
//                auto &textBox = window.wdg<TextBox>();
//                textBox.setEditable(true);
//                textBox.setFixedSize(Vector2i(100, 20));
//                textBox.setValue("50");
//                textBox.setUnits("GiB");
//                textBox.setDefaultValue("0.0");
//                textBox.setFontSize(16);
//                textBox.setFormat("[-]?[0-9]*\\.?[0-9]+");
//
//                window.add<Label>("Positive integer :", "sans-bold");
//                auto &textBox2 = window.wdg<TextBox>();
//                textBox2.setEditable(true);
//                textBox2.setFixedSize(Vector2i(100, 20));
//                textBox2.setValue("50");
//                textBox2.setUnits("Mhz");
//                textBox2.setDefaultValue("0.0");
//                textBox2.setFontSize(16);
//                textBox2.setFormat("[1-9][0-9]*");
//
//                window.add<Label>("Checkbox :", "sans-bold");
//
//                window.wdg<CheckBox>("Check me")
//                        .withChecked(true)
//                        .withFontSize(16);
//
//                window.add<Label>("Combo box :", "sans-bold");
//                window.wdg<ComboBox>()
//                        .withItems(std::vector<std::string>{"Item 1", "Item 2", "Item 3"})
//                        .withFontSize(16)
//                        .withFixedSize(Vector2i(100, 20));
//
//                window.add<Label>("Color button :", "sans-bold");
//                auto &popupBtn = window.wdg<PopupButton>("", 0);
//                popupBtn.setBackgroundColor(Color(255, 120, 0, 255));
//                popupBtn.setFontSize(16);
//                popupBtn.setFixedSize(Vector2i(100, 20));
//                auto &popup = popupBtn.popup().withLayout<GroupLayout>();
//
//                ColorWheel &colorwheel = popup.wdg<ColorWheel>();
//                colorwheel.setColor(popupBtn.backgroundColor());
//
//                Button &colorBtn = popup.wdg<Button>("Pick");
//                colorBtn.setFixedSize(Vector2i(100, 25));
//                Color c = colorwheel.color();
//                colorBtn.setBackgroundColor(c);
//
//                colorwheel.setCallback([&colorBtn](const Color &value) {
//                    colorBtn.setBackgroundColor(value);
//                });
//
//                colorBtn.setChangeCallback([&colorBtn, &popupBtn](bool pushed) {
//                    if (pushed) {
//                        popupBtn.setBackgroundColor(colorBtn.backgroundColor());
//                        popupBtn.setPushed(false);
//                    }
//                });
//            }
//            performLayout(mRenderer);
        }
    }
};

int main(int, char **) {

    PiGraphicsContext piGraphicsContext(sdlgui::Vector2i{0, 0}, sdlgui::Vector2i{800, 480}, "Pi Application");
    HamChrono hamChrono(piGraphicsContext);
    hamChrono.performAppLayout();

    hamChrono.eventLoop();
}