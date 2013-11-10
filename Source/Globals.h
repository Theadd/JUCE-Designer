/*
  ==============================================================================

    Globals.h
    Created: 13 Oct 2013 2:59:53am
    Author:  admin

  ==============================================================================
*/

#ifndef __GLOBALS_H_D2823AC0__
#define __GLOBALS_H_D2823AC0__



#define RESIZABLEEDGESIZE 5
#define TOOLBARSIZE 32
#define MINIMUMPANELSIZE 90
#define MENUBARHEIGHT 24

namespace Globals
{
    enum Position
	{
		top,
		right,
		bottom,
		left,
		center
	};

	enum Orientation
	{
		none,
		vertical,
		horizontal
	};
}

enum ColourIds
{
    mainBackgroundColourId          = 0x2340000,
    treeviewHighlightColourId       = 0x2340002,
};


String T (const String& text);
String T (const char* text);
String T (CharPointer_UTF8 text);

/*WIP ~= RIP
class Literal
{
public:

	struct Reference
	{
		Value value;	//its localised value (translated)
		String text;	//string keyword to translate from
	};

	OwnedArray<Reference> references;

	Value& getValueFor (String& text);

};*/


/*
const char* T_UTF8 (const String& text);
const char* T_UTF8 (const char* text);

const char* T_UTF8 (CharPointer_UTF8 text);
*/





/*
namespace LayoutColourIds
{
	const Identifier headerBackgroundColour		("headerBackgroundColour");
	const Identifier headerTextColour	("headerTextColour");
}

namespace Layout
{
	inline Colour getColourFor (Identifier c)
	{
		if (c == LayoutColourIds::headerBackgroundColour)
			return Colour(Colour((juce::uint8) 83, (juce::uint8) 94, (juce::uint8) 104, (juce::uint8) 255));

		else if (c == LayoutColourIds::headerBackgroundColour)
			return Colour(Colour((juce::uint8) 83, (juce::uint8) 94, (juce::uint8) 104, (juce::uint8) 255));

		else
			return Colours::transparentBlack;
	}
}
*/

#endif  // __GLOBALS_H_D2823AC0__
