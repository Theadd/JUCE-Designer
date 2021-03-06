/*
  ==============================================================================

    Default.h
    Created: 11 Oct 2013 9:22:22pm
    Author:  admin

  ==============================================================================
*/

#ifndef __DUSKMAP_H_4A610CFE__
#define __DUSKMAP_H_4A610CFE__

#include "../ExtendedLookAndFeel.h"


class DuskMapLookAndFeel   : public ExtendedLookAndFeel
{
public:
    DuskMapLookAndFeel();
	~DuskMapLookAndFeel();

    void fillWithBackgroundTexture (Graphics&);
    static void fillWithBackgroundTexture (Component&, Graphics&);

    //int getTabButtonOverlap (int tabDepth) override;
    //int getTabButtonSpaceAroundImage() override;
    //int getTabButtonBestWidth (TabBarButton&, int tabDepth) override;
    //static Colour getTabBackgroundColour (TabBarButton&);
	void createTabButtonShape (TabBarButton& button, Path& p, bool isMouseOver, bool isMouseDown);
	int getTabButtonOverlap (int tabDepth) override;
	int getTabButtonSpaceAroundImage() override;
    void drawTabButton (TabBarButton& button, Graphics&, bool isMouseOver, bool isMouseDown) override;
	void fillTabButtonShape (TabBarButton&, Graphics&, const Path& path, bool isMouseOver, bool isMouseDown) override;

    //Rectangle<int> getTabButtonExtraComponentBounds (const TabBarButton&, Rectangle<int>& textArea, Component&) override;
    //void drawTabAreaBehindFrontButton (TabbedButtonBar&, Graphics&, int, int) override {}

    void drawStretchableLayoutResizerBar (Graphics&, int w, int h, bool isVerticalBar, bool isMouseOver, bool isMouseDragging) override;
    //Rectangle<int> getPropertyComponentContentPosition (PropertyComponent&) override;

    bool areScrollbarButtonsVisible() override   { return false; }

    void drawScrollbar (Graphics&, ScrollBar&, int x, int y, int width, int height, bool isScrollbarVertical,
                        int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override;

    void drawConcertinaPanelHeader (Graphics&, const Rectangle<int>& area, bool isMouseOver, bool isMouseDown,
                                    ConcertinaPanel&, Component&) override;

    void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;

    //void drawTableHeaderBackground (Graphics&, TableHeaderComponent&) override;

    static Colour getScrollbarColourForBackground (Colour background) override;

	void drawPopupMenuBackground (Graphics &, int width, int height) override;
	void drawMenuBarBackground (Graphics &, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar) override;
	
	void drawTreeviewPlusMinusBox (Graphics &, int x, int y, int w, int h, bool isPlus, bool isMouseOver) override;
	void drawTreeviewPlusMinusBox (Graphics &, const Rectangle< float > &area, Colour backgroundColour, bool isOpen, bool isMouseOver) override;
	bool areLinesDrawnForTreeView (TreeView&) override;
	int getTreeViewIndentSize (TreeView&) override;

	int getDefaultScrollbarWidth () override;

private:
    Image backgroundTexture;
    Colour backgroundTextureBaseColour;

};


#endif  // __DUSKMAP_H_4A610CFE__
