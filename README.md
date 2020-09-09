# NanoGUI-SDL-Pi

NanoGUI-SDL-Pi is a RaspberryPi port/rewirte for [NanoGUI-SDL](https://github.com/dalerank/nanogui-sdl)
 which is an SDL port for [NanoGUI](https://github.com/wjakob/nanogui)

## Example screenshot
![Screenshot](https://github.com/dalerank/nanogui-sdl/blob/master/resources/screenshot1.png "Screenshot")

## Description
NanoGUI-SDL-Pi builds on [SDL2](http://www.libsdl.org/) for cross-platform context
creation and event handling, for basic vector types, and
[NanoVG/NanoVG-RT](https://github.com/memononen/NanoVG) to draw2D primitives.

NanoGUI-SDL-Pi is intended to work on the latest version of
[RaspberryPi OS](https://www.raspberrypi.org/downloads/raspberry-pi-os/)
, but is developed on Linux (specifically Linux Mint) so it should compile and run
on any Linux variant that supports C++17 and the SDL2 library set.

### Reasoning

I initially started with a fork of NanoGUI-SDL on github, but I found there were too many changes I wanted
to make:
    
* Support touch screen actions on Raspberry Pi 7 inch touch display;
* Write "kiosk" type applications that take over the screen and run without X11, a screen manager or
    similar;
* Add the ability to use installed font files;
* Fix as many compiler wanings as I can; and
* Upgrade to C++17 or even C++20 over time.

When ready this repository will be nested into my [HamChrono](git@github.com:pa28/HamChrono.git)
repository to provide frame buffer support. 

## Creating widgets
NanoGUI-SDL makes it easy to instantiate widgets, set layout constraints, and
register event callbacks using high-level C++11 code. For instance, the
following two lines from the included example application add a new button to
an existing window `window` and register an event callback.
```C++
auto& button = window.add<Button>("Plain button")
                     .withCallback([] { cout << "pushed!" << endl; });
```

The following lines from the example application create the coupled
slider and text box on the bottom of the second window (see the screenshot).
```C++
/* Create an empty panel with a horizontal layout */
auto& panel = window.add<Widget>()
                    .withLayout<BoxLayout>(BoxLayout::Horizontal, BoxLayout::Middle, 0, 20);

/* Add a slider and set defaults */
auto& slider = panel.add<Slider>()
                    .withValue(0.5f)
                    .withFixedWidth(80);

/* Add a textbox and set defaults */
auto& textBox = panel.add<TextBox>()
                     .withFixedSize(Vector2i(60, 25))
                     .withValue("50")
                     .withUnits("%");

/* Propagate slider changes to the text box */
slider.setCallback([textBox](float value) { textBox.setValue(std::to_string((int) (value * 100))); });


/* Create an empty panel with a horizontal layout */
window.widget()
        .boxlayout(BoxLayout::Horizontal, BoxLayout::Middle, 0, 20))
            /* Add a slider and set defaults */
            .slider(0.5f, [](Slider* obj, float value) {
                if (auto* textBox = obj->gfind<TextBox>("slider-textbox"))
                    textBox->setValue(std::to_string((int) (value * 100)) );
            }).withFixedWidth(80).and() 
            /* Add a textbox and set defaults */   
            .textbox("50", "%").withFixedSize(Vector2i(60, 25))    
   		        .withId("slider-textbox")

```

## "Simple mode"

Christian Schüller contributed a convenience class that makes it possible to
create AntTweakBar-style variable manipulators using just a few lines of code.
For instance, the source code below was used to create the following example
application.

```C++
/// dvar, bar, strvar, etc. are double/bool/string/.. variables

FormHelper *gui = new FormHelper(screen);
ref<Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
gui->addGroup("Basic types");
gui->addVariable("bool", bvar);
gui->addVariable("string", strvar);

gui->addGroup("Validating fields");
gui->addVariable("int", ivar);
gui->addVariable("float", fvar);
gui->addVariable("double", dvar);

gui->addGroup("Complex types");
gui->addVariable("Enumeration", enumval, enabled)
   ->setItems({"Item 1", "Item 2", "Item 3"});
gui->addVariable("Color", colval);

gui->addGroup("Other widgets");
gui->addButton("A button", [](){ std::cout << "Button pressed." << std::endl; });

screen->setVisible(true);
screen->performLayout();
window->center();
```


### start On Linux (Ubuntu)
```
#prerequirements:

$ sudo apt install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

#setup and run example:

$ git clone git@github.com:pa28/nanogui-sdl-pi.git
$ cd nanogui-sdl
$ mkdir -p build
$ cd build/
$ cmake ..
$ cmake --build .
$ ./example1
$ ./hamcrono

```

### License

nanogui-sdl is provided under a BSD-style license that can be found in the
``LICENSE.txt`` file. By using, distributing, or contributing to this project,
you agree to the terms and conditions of this license.
