#include <JuceHeader.h>
#include "MainComponent_Test.h"

class ToneGeneratorApplication : public juce::JUCEApplication
{
public:
    ToneGeneratorApplication() = default;

    const juce::String getApplicationName() override { return "Tone Generator Test"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(const juce::String& name)
            : DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour(juce::ResizableWindow::backgroundColourId),
                           juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent_Test(), true);

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
#endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ToneGeneratorApplication)
