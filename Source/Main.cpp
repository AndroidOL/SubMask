/*
  ==============================================================================

    电影字幕遮盖器，鼠标右键弹出菜单：显示边框，增减透明度，退出，等。

	作者：SwingCoder
	http://underwaySoft.com

  ==============================================================================
*/

#include "JuceHeader.h"

//==============================================================================
class SubMaskApplication : public JUCEApplication
{
public:
    //==============================================================================
    SubMaskApplication() {}

    const String getApplicationName() { return ProjectInfo::projectName; }
    const String getApplicationVersion() { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() { return true; }

    //==============================================================================
    void initialise (const String&) { mainWindow = new MainWindow(); }
    void shutdown() { mainWindow = nullptr; }

    //==============================================================================
    void systemRequestedQuit() { quit(); }

    void anotherInstanceStarted (const String&) {}

    //==============================================================================
    /*  */
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow() : DocumentWindow ("MainWindow", Colour (0x0), 0),
            opacity (0.90f),
            mouseIn (true)
        {
            centreWithSize (1024, 55);
            setResizable (true, false);
            setResizeLimits (100, 50, 4096, 200);

            setDropShadowEnabled (false);
            setTitleBarHeight (0);
            setVisible (true);

            setAlwaysOnTop (true);
        }

        //==============================================================================
        void paint (Graphics& g)
        {
            g.fillAll (Colours::black.withAlpha (opacity));

            if (mouseIn)
            {
                g.setColour (Colours::cyan);
                g.drawRect (0, 0, getWidth(), getHeight());
            }
        }

        void closeButtonPressed() { }

        //==============================================================================
        // 左键在窗口内拖拽：移动窗口。右键弹出菜单：更改透明度，退出。
        void mouseDown (const MouseEvent& e)
        {
            // 此处需先调用父类的同名函数，否则鼠标左键在窗口内拖拽移动窗口时，窗口的左上角自动跳到鼠标指针处
            DocumentWindow::mouseDown (e);

            if (e.mods.isRightButtonDown())
            {
                PopupMenu menu;

                menu.addItem (1, "Display Border", true, mouseIn);
                menu.addSeparator();

                menu.addItem (2, "Opacity + 0.05");
                menu.addItem (3, "Opacity - 0.05");
                menu.addSeparator();

                menu.addItem (5, "About UnderwaySoft...");
                menu.addItem (4, "Quit");

                const int selectID = menu.show();

                switch (selectID)
                {
                case 1:
                    mouseIn = !mouseIn;
                    repaint();
                    break;

                case 2:
                    opacity = jmin (opacity + 0.05f, 1.0f);
                    repaint();
                    //DBG (opacity);
                    break;

                case 3:
                    opacity = jmax (opacity - 0.05f, 0.0f);
                    repaint();
                    //DBG (opacity);
                    break;

                case 4:
                    JUCEApplication::getInstance()->systemRequestedQuit();
                    break;

                case 5:
                    Process::openDocument("http://underwaySoft.com", String());
                    break;

                default:
                    break;
                }
            }
        }


    private:
        float opacity;
        bool mouseIn;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================

START_JUCE_APPLICATION (SubMaskApplication)
