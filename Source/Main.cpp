/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
//#include "Styles/Default.h"
#include "Layouts/MainLayout.cpp"


//==============================================================================
class JUCEDesignerApplication  : public JUCEApplication
{
public:
    //==============================================================================
    JUCEDesignerApplication() {}

    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return true; }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        // This method is where you should put your application's initialisation code..

        mainWindow = new MainWindow();
    }

    void shutdown()
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    /*class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow()  : DocumentWindow ("MainWindow",
                                        Colours::lightgrey,
                                        DocumentWindow::allButtons)
        {
            setContentOwned (new MainContentComponent(), true);

            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed()
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

         Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };*/
    class MainWindow    : public DocumentWindow
{
public:
    MainWindow()  : DocumentWindow ("JUCE GUI Designer",
                                        Colours::lightgrey,
                                        DocumentWindow::allButtons)
    {
		//Look And Feel
        /*setLookAndFeel(lookAndFeel = new JUCEDesignerLookAndFeel());
		lookAndFeel->setColour(TextButton::buttonColourId, Colour(Colour((juce::uint8) 83, (juce::uint8) 94, (juce::uint8) 104, (juce::uint8) 255)));
		lookAndFeel->setColour(TextButton::textColourOffId, Colours::transparentWhite.withAlpha(0.9f));
		lookAndFeel->setColour(TextButton::textColourOnId, Colours::blue.withAlpha(0.9f));*/

        setBounds(55, 35, 490, 440);
        setColour(DocumentWindow::backgroundColourId, Colour::fromString("FF202A32"));
        setName("JUCE GUI Designer");
		DBG("dbg: MainWindow()");
        setTitleBarHeight(26);
        setTitleBarButtonsRequired(7, 0);
        setTitleBarTextCentred(0);
        setUsingNativeTitleBar(1);
        setContentOwned (&mainlayout, true);

        setResizable(true, true);
        setVisible (true);
    }

    void closeButtonPressed()
    {
        JUCEApplication::getInstance()->systemRequestedQuit();
    }

    void mouseUp (const MouseEvent& event)
    {
        if (event.eventComponent == this)
            DocumentWindow::mouseUp(event);
    }

    void mouseDrag (const MouseEvent& event)
    {
        if (event.eventComponent == this)
            DocumentWindow::mouseDrag(event);
    }

    void mouseDoubleClick (const MouseEvent& event)
    {
        if (event.eventComponent == this)
            DocumentWindow::mouseDoubleClick(event);
    }

private:
	ScopedPointer <LookAndFeel> lookAndFeel;
    MainLayout mainlayout;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};

private:
    ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (JUCEDesignerApplication)
