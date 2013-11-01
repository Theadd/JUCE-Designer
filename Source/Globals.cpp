/*
  ==============================================================================

    Globals.cpp
    Created: 28 Oct 2013 9:10:18pm
    Author:  admin

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "Globals.h"

String T (const String& text)       { return juce::translate (text, text); }
String T (const char* text)
{
	String a(text);
	return juce::translate (a, a);
}

String T (CharPointer_UTF8 text)
{
	String a(text);
	return juce::translate (a, a);
}

const char* T_UTF8 (const String& text)
{
	char* t = new char;
	strcpy(t, juce::translate (text, text).toRawUTF8());
	return t;
}
const char* T_UTF8 (const char* text)
{
	String a = String::fromUTF8(text);
	char* t = new char;
	strcpy(t, juce::translate (a, a).toRawUTF8());
	return t;
}

const char* T_UTF8 (CharPointer_UTF8 text)
{
	String a(text);
	char* t = new char;
	strcpy(t, juce::translate (a, a).toRawUTF8());
	return t;
}