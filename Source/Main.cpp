#include <fstream>
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class SubMaskApplication : public JUCEApplication {
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
    class MainWindow : public DocumentWindow, public ChangeListener {
    public:
        MainWindow() : DocumentWindow ("MainWindow", Colours::transparentBlack, 0)
		, opacity (0.80f), mouseIn (false) {
            centreWithSize (1280, 55);
            setResizable (true, false);
            setResizeLimits (100, 50, 4096, 200);

            setDropShadowEnabled (false);
            setTitleBarHeight (0);
            setVisible (true);

            setAlwaysOnTop (true);
        }

        //==============================================================================
        // 绘制函数：绘制图形。
        void paint (Graphics& g) {
            g.fillAll (Colour(background.withAlpha (opacity)));

            if (mouseIn) {
                g.setColour (Colours::cyan);
                g.drawRect (0, 0, getWidth(), getHeight());
            }
        }

        void closeButtonPressed() { }

        //==============================================================================
        // 左键在窗口内拖拽：移动窗口。右键弹出菜单：更改透明度，退出。
		void mouseDown (const MouseEvent& e) {
            // 此处需先调用父类的同名函数，否则鼠标左键在窗口内拖拽移动窗口时，窗口的左上角自动跳到鼠标指针处
            DocumentWindow::mouseDown (e);
			// DBG(e.mods.isCtrlDown);

			if (e.mods.isRightButtonDown()) {
				PopupMenu menu;
				
				menu.addItem (1, "Display Border", true, mouseIn);
				menu.addItem (2, "Always on Top", true, isAlwaysOnTop());
				menu.addSeparator();

				menu.addItem (3, "Opacity + 0.05");
				menu.addItem (4, "Opacity - 0.05");
				menu.addSeparator();
				
				menu.addItem (5, "Background Picker");
				menu.addItem (6, "Quit Application");
				menu.addItem (7, "About UnderwaySoft...");

				const int selectID = menu.show();
				ColourSelector* colourSelector = new ColourSelector();

				switch (selectID) {
					case 1:
						mouseIn = !mouseIn;
						break;

					case 2:
						setAlwaysOnTop(!isAlwaysOnTop());
						break;

					case 3:
						opacity = jmin (opacity + 0.05f, 1.00f);
						//DBG (opacity);
						break;

					case 4:
						opacity = jmax (opacity - 0.05f, 0.10f);
						//DBG (opacity);
						break;

					case 5:
						colourSelector->setName("Background Picker");
						colourSelector->setCurrentColour(background.withAlpha(opacity));
						colourSelector->addChangeListener(this);
						colourSelector->setSize(300, 400);

						CallOutBox::launchAsynchronously(colourSelector, getScreenBounds(), nullptr);
						break;

					case 6:
						JUCEApplication::getInstance()->systemRequestedQuit();
						break;

					case 7:
						Process::openDocument("http://underwaySoft.com", String());
						break;

					default:
						break;
				}
			}
			repaint();
		}

		//==============================================================================
		// 颜色选择器回调事件：更改背景颜色以及透明度
		void changeListenerCallback(ChangeBroadcaster* source) override {
			if (ColourSelector *cs = dynamic_cast<ColourSelector*> (source)) {
				background = Colour(cs->getCurrentColour());
				opacity = int((float(cs->getCurrentColour().getAlpha()) / 0xFF) * 100) / float(100);
				if (opacity < 0.1f) {
					opacity = 0.1f;
					cs->setCurrentColour(background.withAlpha(float(0.1)));
				}
				//log("info", "cs->getCurrentColour().getAlpha() - #" + String(cs->getCurrentColour().getAlpha()));
			}
			repaint();
		}

		//==============================================================================
		// 滚轮在垂直方向滚动：更改透明度。
		void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& w) {
			if (e.mods.getNumMouseButtonsDown() == 0) {
				if (w.deltaY > 0) {
					opacity = jmin(opacity + 0.02f, 1.00f);
				}
				else {
					opacity = jmax(opacity - 0.02f, 0.10f);
				}
				repaint();
			}
		}

		//==============================================================================
		// 方向键上下：更改透明度。方向键左右：调节音量。退格键：静音。
		bool keyPressed(const KeyPress& k) override {
			if (k.getKeyCode() == k.upKey) {
				opacity = jmin(opacity + 0.05f, 1.00f);
				repaint();
			}
			else if (k.getKeyCode() == k.downKey) {
				opacity = jmax(opacity - 0.05f, 0.10f);
				repaint();
			}
			else if (k.getKeyCode() == k.leftKey) {
				float gain = SystemAudioVolume::getGain();
				gain = jmin(gain - 0.05f, 1.00f);
				SystemAudioVolume::setGain(gain);
			}
			else if (k.getKeyCode() == k.rightKey) {
				float gain = SystemAudioVolume::getGain();
				gain = jmax(gain + 0.05f, 0.00f);
				SystemAudioVolume::setGain(gain);
			}
			else if (k.getKeyCode() == k.backspaceKey) {
				SystemAudioVolume::setMuted(!SystemAudioVolume::isMuted());
			}
			else { }
			return k.getKeyCode() >= 0 ? true : false;
		}

	private:
		Colour background = Colours::black;
		float opacity;
		bool mouseIn;
		ChangeListener *changeListenerColourPtr;

		//==============================================================================
		// 返回当前时间
		String getTime() {
			time_t timep;
			time(&timep); //获取time_t类型的当前时间
			char tmp[64];
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));//对日期和时间进行格式化
			return tmp;
		}

		//==============================================================================
		// 写入日志文件
		bool log(String lvl = "info", String msg = "null") {
			std::ofstream myfile("./log.txt", std::ios::app);
			if (myfile.is_open()) {
				myfile << "[" << lvl << "] " << getTime() << ": " << msg << std::endl;
				myfile.clear();
				myfile.close();
				return true;
			}

			return false;
		}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
	};

private:
	ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================

START_JUCE_APPLICATION (SubMaskApplication);
