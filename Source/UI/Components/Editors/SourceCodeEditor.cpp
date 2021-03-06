/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#include "../../../Headers.h"
#include "SourceCodeEditor.h"
#include "../../../Application.h"
#include "../../../Core/DocumentManager.h"
#include "../../Windows/MainWindow/MainLayout.h"
#include "../../Misc/CodeHelpers.h"


//==============================================================================
SourceCodeDocument::SourceCodeDocument (Project* p, const File& f)
    : modDetector (f), project (p)
{
}

CodeDocument& SourceCodeDocument::getCodeDocument()
{
    if (codeDoc == nullptr)
    {
        codeDoc = new CodeDocument();
        reloadInternal();
        codeDoc->clearUndoHistory();
    }

    return *codeDoc;
}

Component* SourceCodeDocument::createEditor()
{
    SourceCodeEditor* e = new SourceCodeEditor (this, getCodeDocument());
    applyLastState (*(e->editor));
    return e;
}

void SourceCodeDocument::reloadFromFile()
{
    getCodeDocument();
    reloadInternal();
}

void SourceCodeDocument::reloadInternal()
{
    jassert (codeDoc != nullptr);
    modDetector.updateHash();
    codeDoc->applyChanges (getFile().loadFileAsString());
    codeDoc->setSavePoint();
}

static bool writeCodeDocToFile (const File& file, CodeDocument& doc)
{
    TemporaryFile temp (file);

    {
        FileOutputStream fo (temp.getFile());

        if (! (fo.openedOk() && doc.writeToStream (fo)))
            return false;
    }

    return temp.overwriteTargetFileWithTemporary();
}

bool SourceCodeDocument::save()
{
    if (writeCodeDocToFile (getFile(), getCodeDocument()))
    {
        getCodeDocument().setSavePoint();
        modDetector.updateHash();
        return true;
    }

    return false;
}

bool SourceCodeDocument::saveAs()
{
    FileChooser fc (TRANS("Save As..."), getFile(), "*");

    if (! fc.browseForFileToSave (true))
        return true;

    return writeCodeDocToFile (fc.getResult(), getCodeDocument());
}

void SourceCodeDocument::updateLastState (CodeEditorComponent& editor)
{
    lastState = new CodeEditorComponent::State (editor);
}

void SourceCodeDocument::applyLastState (CodeEditorComponent& editor) const
{
    if (lastState != nullptr)
        lastState->restoreState (editor);
}

//==============================================================================
SourceCodeEditor::SourceCodeEditor (OpenDocumentManager::Document* doc, CodeDocument& codeDocument)
    : DocumentEditorComponent (doc)
{
	setName(document->getFile().getFileName());
    if (document->getFile().hasFileExtension ("cpp;mm;m;c;cc;cxx;h;hpp;hxx;hh;inl"))
        setEditor (new CppCodeEditorComponent (document->getFile(), codeDocument));
    else
        setEditor (new GenericCodeEditorComponent (document->getFile(), codeDocument, nullptr));
}

SourceCodeEditor::SourceCodeEditor (OpenDocumentManager::Document* doc, CodeEditorComponent* ed)
    : DocumentEditorComponent (doc)
{
	setName(doc->getFile().getFileName());
    setEditor (ed);
}

SourceCodeEditor::~SourceCodeEditor()
{
    if (editor != nullptr)
        editor->getDocument().removeListener (this);

	//TOFIX
    //getAppSettings().appearance.settings.removeListener (this);

    if (SourceCodeDocument* doc = dynamic_cast <SourceCodeDocument*> (getDocument()))
        doc->updateLastState (*editor);
}

void SourceCodeEditor::setEditor (CodeEditorComponent* newEditor)
{
    if (editor != nullptr)
        editor->getDocument().removeListener (this);

    addAndMakeVisible (editor = newEditor);

    //editor->setFont (AppearanceSettings::getDefaultCodeFont());	//TOFIX
    editor->setTabSize (4, true);

    updateColourScheme();
    //getAppSettings().appearance.settings.addListener (this);	//TOFIX

    editor->getDocument().addListener (this);
}

void SourceCodeEditor::scrollToKeepRangeOnScreen (Range<int> range)
{
    const int space = jmin (10, editor->getNumLinesOnScreen() / 3);
    const CodeDocument::Position start (editor->getDocument(), range.getStart());
    const CodeDocument::Position end   (editor->getDocument(), range.getEnd());

    editor->scrollToKeepLinesOnScreen (Range<int> (start.getLineNumber() - space, end.getLineNumber() + space));
}

void SourceCodeEditor::highlight (Range<int> range, bool cursorAtStart)
{
    scrollToKeepRangeOnScreen (range);

    if (cursorAtStart)
    {
        editor->moveCaretTo (CodeDocument::Position (editor->getDocument(), range.getEnd()),   false);
        editor->moveCaretTo (CodeDocument::Position (editor->getDocument(), range.getStart()), true);
    }
    else
    {
        editor->setHighlightedRegion (range);
    }
}

void SourceCodeEditor::scrollToLine (int lineNumber, bool firstLineOnScreen, bool alsoMoveCaretToLine)
{
	int rawLineNumber = lineNumber;
	int numLinesOnScreen = editor->getNumLinesOnScreen();

	if (!firstLineOnScreen)
	{
		if ((numLinesOnScreen / 2) - 1 > lineNumber)
		{
			lineNumber = 0;
		}
		else
		{
			lineNumber -= (numLinesOnScreen / 2) - 1;
		}
	}
	//stop at end of screen
	int lineMod = lineNumber + numLinesOnScreen - editor->getDocument().getNumLines() + 1;
	if (lineMod > 0)
		lineNumber -= lineMod;

	editor->scrollToLine(lineNumber);
	if (alsoMoveCaretToLine)
	{
		editor->moveCaretTo (CodeDocument::Position (editor->getDocument(), rawLineNumber, 0), false);
		editor->grabKeyboardFocus();
	}
}

ValueTree SourceCodeEditor::getNavigatorTree ()
{
	return navigatorTree;
}

CodeDocument& SourceCodeEditor::getCodeDocument ()
{
	return editor->getDocument();
}

void SourceCodeEditor::focusGained (FocusChangeType cause)
{
	if (JUCEDesignerApp::getActiveInnerPanel() != this)
	{
		JUCEDesignerApp::setActiveInnerPanel(this);
		JUCEDesignerApp::setActiveDocument(getDocument());
	}
}

void SourceCodeEditor::resized()
{
    editor->setBounds (getLocalBounds());
}

//TOFIX
void SourceCodeEditor::updateColourScheme()     { };	//getAppSettings().appearance.applyToCodeEditor (*editor); }

void SourceCodeEditor::checkSaveState()
{
	DBG("void SourceCodeEditor::checkSaveState()");
	ValueTree tree (getNavigatorTree());

    setEditedState (getDocument()->needsSaving());
}

void SourceCodeEditor::valueTreePropertyChanged (ValueTree&, const Identifier&)   { updateColourScheme(); }
void SourceCodeEditor::valueTreeChildAdded (ValueTree&, ValueTree&)               { updateColourScheme(); }
void SourceCodeEditor::valueTreeChildRemoved (ValueTree&, ValueTree&)             { updateColourScheme(); }
void SourceCodeEditor::valueTreeChildOrderChanged (ValueTree&)                    { updateColourScheme(); }
void SourceCodeEditor::valueTreeParentChanged (ValueTree&)                        { updateColourScheme(); }
void SourceCodeEditor::valueTreeRedirected (ValueTree&)                           { updateColourScheme(); }

void SourceCodeEditor::codeDocumentTextInserted (const String&, int)              { checkSaveState(); }
void SourceCodeEditor::codeDocumentTextDeleted (int, int)                         { checkSaveState(); }

//==============================================================================
GenericCodeEditorComponent::GenericCodeEditorComponent (const File& f, CodeDocument& codeDocument,
                                                        CodeTokeniser* tokeniser)
   : CodeEditorComponent (codeDocument, tokeniser), file (f)
{
    setCommandManager (&JUCEDesignerApp::getCommandManager());
}

GenericCodeEditorComponent::~GenericCodeEditorComponent() {}

enum
{
    showInFinderID = 0x2fe821e3,
    insertComponentID = 0x2fe821e4
};

void GenericCodeEditorComponent::addPopupMenuItems (PopupMenu& menu, const MouseEvent* e)
{
    menu.addItem (showInFinderID,
                 #if JUCE_MAC
                  "Reveal " + file.getFileName() + " in Finder");
                 #else
                  "Reveal " + file.getFileName() + " in Explorer");
                 #endif
    menu.addSeparator();

    CodeEditorComponent::addPopupMenuItems (menu, e);
}

void GenericCodeEditorComponent::performPopupMenuAction (int menuItemID)
{
    if (menuItemID == showInFinderID)
        file.revealToUser();
    else
        CodeEditorComponent::performPopupMenuAction (menuItemID);
}

void GenericCodeEditorComponent::getAllCommands (Array <CommandID>& commands)
{
    CodeEditorComponent::getAllCommands (commands);

    const CommandID ids[] = { MainLayout::showFindPanel,
                              MainLayout::findSelection,
                              MainLayout::findNext,
                              MainLayout::findPrevious };

    commands.addArray (ids, numElementsInArray (ids));
}

void GenericCodeEditorComponent::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{
    const bool anythingSelected = isHighlightActive();
	DBG("getCommandInfo anythingSelected = "+String(anythingSelected));
    switch (commandID)
    {
        case MainLayout::showFindPanel:
            result.setInfo (translate ("Find"), translate ("Searches for text in the current document."), "Editing", 0);
            result.defaultKeypresses.add (KeyPress ('f', ModifierKeys::commandModifier, 0));
            break;

        case MainLayout::findSelection:
            result.setInfo (translate ("Find Selection"), translate ("Searches for the currently selected text."), "Editing", 0);
            result.setActive (anythingSelected);
            result.defaultKeypresses.add (KeyPress ('l', ModifierKeys::commandModifier, 0));
            break;

        case MainLayout::findNext:
            result.setInfo (translate ("Find Next"), translate ("Searches for the next occurrence of the current search-term."), "Editing", 0);
            result.defaultKeypresses.add (KeyPress ('g', ModifierKeys::commandModifier, 0));
            break;

        case MainLayout::findPrevious:
            result.setInfo (translate ("Find Previous"), translate ("Searches for the previous occurrence of the current search-term."), "Editing", 0);
            result.defaultKeypresses.add (KeyPress ('g', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
            result.defaultKeypresses.add (KeyPress ('d', ModifierKeys::commandModifier, 0));
            break;

        default:
            CodeEditorComponent::getCommandInfo (commandID, result);
            break;
    }
}

bool GenericCodeEditorComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case MainLayout::showFindPanel:     showFindPanel();         return true;
        case MainLayout::findSelection:     findSelection();         return true;
        case MainLayout::findNext:          findNext (true, true);   return true;
        case MainLayout::findPrevious:      findNext (false, false); return true;
        default:                            break;
    }

    return CodeEditorComponent::perform (info);
}

//==============================================================================
class GenericCodeEditorComponent::FindPanel  : public Component,
                                               private TextEditor::Listener,
                                               private ButtonListener
{
public:
    FindPanel()
        : caseButton ("Case-sensitive"),
          findPrev ("<"),
          findNext (">")/*,
		  replaceLabel ("Replace"),
		  regexpButton ("Regular expressions"),
		  currentDocButton ("Current document"),
		  openDocsButton ("Open documents"),
		  currentProjectButton ("Current project"),
		  replaceButton ("Replace"),
		  replaceAllButton ("Replace All")*/

    {
        editor.setColour (CaretComponent::caretColourId, Colours::black);
		//replaceEditor.setColour (CaretComponent::caretColourId, Colours::black);

        addAndMakeVisible (&editor);

        label.setText ("Find:", dontSendNotification);
        label.setColour (Label::textColourId, Colours::white);
        label.attachToComponent (&editor, false);

        addAndMakeVisible (&caseButton);
        caseButton.setColour (ToggleButton::textColourId, Colours::white);
        caseButton.setToggleState (isCaseSensitiveSearch(), dontSendNotification);
        caseButton.addListener (this);

        findPrev.setConnectedEdges (Button::ConnectedOnRight);
        findNext.setConnectedEdges (Button::ConnectedOnLeft);
        addAndMakeVisible (&findPrev);
        addAndMakeVisible (&findNext);

        setWantsKeyboardFocus (false);
        setFocusContainer (true);
        findPrev.setWantsKeyboardFocus (false);
        findNext.setWantsKeyboardFocus (false);

		//Extended FindPanel
		/*addAndMakeVisible (&replaceLabel);
		addAndMakeVisible (&regexpButton);
		addAndMakeVisible (&currentDocButton);
		addAndMakeVisible (&openDocsButton);
		addAndMakeVisible (&currentProjectButton);
		addAndMakeVisible (&replaceButton);
		addAndMakeVisible (&replaceAllButton)
		//~*/

        editor.setText (getSearchString());
        editor.addListener (this);
    }

    void setCommandManager (ApplicationCommandManager* cm)
    {
        findPrev.setCommandToTrigger (cm, MainLayout::findPrevious, true);
        findNext.setCommandToTrigger (cm, MainLayout::findNext, true);
    }

    void paint (Graphics& g) override
    {
        Path outline;
        outline.addRoundedRectangle (1.0f, 1.0f, getWidth() - 2.0f, getHeight() - 2.0f, 8.0f);

        g.setColour (Colours::black.withAlpha (0.6f));
        g.fillPath (outline);
        g.setColour (Colours::white.withAlpha (0.8f));
        g.strokePath (outline, PathStrokeType (1.0f));
    }

    void resized() override
    {
        int y = 30;
        editor.setBounds (10, y, getWidth() - 20, 24);
        y += 30;
        caseButton.setBounds (10, y, getWidth() / 2 - 10, 22);
        findNext.setBounds (getWidth() - 40, y, 30, 22);
        findPrev.setBounds (getWidth() - 70, y, 30, 22);
    }

    void buttonClicked (Button*) override
    {
        setCaseSensitiveSearch (caseButton.getToggleState());
    }

    void textEditorTextChanged (TextEditor&) override
    {
        setSearchString (editor.getText());

        if (GenericCodeEditorComponent* ed = getOwner())
            ed->findNext (true, false);
    }

    void textEditorFocusLost (TextEditor&) override {}

    void textEditorReturnKeyPressed (TextEditor&) override
    {
        JUCEDesignerApp::getCommandManager().invokeDirectly (MainLayout::findNext, true);
    }

    void textEditorEscapeKeyPressed (TextEditor&) override
    {
        if (GenericCodeEditorComponent* ed = getOwner())
            ed->hideFindPanel();
    }

    GenericCodeEditorComponent* getOwner() const
    {
        return findParentComponentOfClass <GenericCodeEditorComponent>();
    }

    TextEditor editor;//, replaceEditor;
    Label label;
    ToggleButton caseButton;//, regexpButton, replaceLabel;
    TextButton findPrev, findNext;//, replaceButton, replaceAllButton;
	//ToggleButton currentDocButton, openDocsButton, currentProjectButton;
};

void GenericCodeEditorComponent::resized()
{
    CodeEditorComponent::resized();

    if (findPanel != nullptr)
    {
        findPanel->setSize (jmin (260, getWidth() - 32), 100);
        findPanel->setTopRightPosition (getWidth() - 16, 8);
    }
}

void GenericCodeEditorComponent::showFindPanel()
{
    if (findPanel == nullptr)
    {
        findPanel = new FindPanel();
        findPanel->setCommandManager (&JUCEDesignerApp::getCommandManager());

        addAndMakeVisible (findPanel);
        resized();
    }

    if (findPanel != nullptr)
    {
        findPanel->editor.grabKeyboardFocus();
        findPanel->editor.selectAll();
    }
}

void GenericCodeEditorComponent::hideFindPanel()
{
    findPanel = nullptr;
}

void GenericCodeEditorComponent::findSelection()
{
    const String selected (getTextInRange (getHighlightedRegion()));

    if (selected.isNotEmpty())
    {
        setSearchString (selected);
        findNext (true, true);
    }
}

void GenericCodeEditorComponent::findNext (bool forwards, bool skipCurrentSelection)
{
    const Range<int> highlight (getHighlightedRegion());
    const CodeDocument::Position startPos (getDocument(), skipCurrentSelection ? highlight.getEnd()
                                                                               : highlight.getStart());
    int lineNum = startPos.getLineNumber();
    int linePos = startPos.getIndexInLine();

    const int totalLines = getDocument().getNumLines();
    const String searchText (getSearchString());
    const bool caseSensitive = isCaseSensitiveSearch();

    for (int linesToSearch = totalLines; --linesToSearch >= 0;)
    {
        String line (getDocument().getLine (lineNum));
        int index;

        if (forwards)
        {
            index = caseSensitive ? line.indexOf (linePos, searchText)
                                  : line.indexOfIgnoreCase (linePos, searchText);
        }
        else
        {
            if (linePos >= 0)
                line = line.substring (0, linePos);

            index = caseSensitive ? line.lastIndexOf (searchText)
                                  : line.lastIndexOfIgnoreCase (searchText);
        }

        if (index >= 0)
        {
            const CodeDocument::Position p (getDocument(), lineNum, index);
            selectRegion (p, p.movedBy (searchText.length()));
            break;
        }

        if (forwards)
        {
            linePos = 0;
            lineNum = (lineNum + 1) % totalLines;
        }
        else
        {
            if (--lineNum < 0)
                lineNum = totalLines - 1;

            linePos = -1;
        }
    }
}

void GenericCodeEditorComponent::handleEscapeKey()
{
    CodeEditorComponent::handleEscapeKey();
    hideFindPanel();
}

//==============================================================================
static CPlusPlusCodeTokeniser cppTokeniser;

CppCodeEditorComponent::CppCodeEditorComponent (const File& f, CodeDocument& doc)
    : GenericCodeEditorComponent (f, doc, &cppTokeniser)
{
}

CppCodeEditorComponent::~CppCodeEditorComponent() {}

void CppCodeEditorComponent::handleReturnKey()
{
    GenericCodeEditorComponent::handleReturnKey();

    CodeDocument::Position pos (getCaretPos());

    String blockIndent, lastLineIndent;
    CodeHelpers::getIndentForCurrentBlock (pos, getTabString (getTabSize()), blockIndent, lastLineIndent);

    const String remainderOfBrokenLine (pos.getLineText());
    const int numLeadingWSChars = CodeHelpers::getLeadingWhitespace (remainderOfBrokenLine).length();

    if (numLeadingWSChars > 0)
        getDocument().deleteSection (pos, pos.movedBy (numLeadingWSChars));

    if (remainderOfBrokenLine.trimStart().startsWithChar ('}'))
        insertTextAtCaret (blockIndent);
    else
        insertTextAtCaret (lastLineIndent);

    const String previousLine (pos.movedByLines (-1).getLineText());
    const String trimmedPreviousLine (previousLine.trim());

    if ((trimmedPreviousLine.startsWith ("if ")
          || trimmedPreviousLine.startsWith ("if(")
          || trimmedPreviousLine.startsWith ("for ")
          || trimmedPreviousLine.startsWith ("for(")
          || trimmedPreviousLine.startsWith ("while(")
          || trimmedPreviousLine.startsWith ("while "))
         && trimmedPreviousLine.endsWithChar (')'))
    {
        insertTabAtCaret();
    }
}

void CppCodeEditorComponent::insertTextAtCaret (const String& newText)
{
    if (getHighlightedRegion().isEmpty())
    {
        const CodeDocument::Position pos (getCaretPos());

        if ((newText == "{" || newText == "}")
             && pos.getLineNumber() > 0
             && pos.getLineText().trim().isEmpty())
        {
            moveCaretToStartOfLine (true);

            String blockIndent, lastLineIndent;
            if (CodeHelpers::getIndentForCurrentBlock (pos, getTabString (getTabSize()), blockIndent, lastLineIndent))
            {
                GenericCodeEditorComponent::insertTextAtCaret (blockIndent);

                if (newText == "{")
                    insertTabAtCaret();
            }
        }
    }

    GenericCodeEditorComponent::insertTextAtCaret (newText);
}

void CppCodeEditorComponent::addPopupMenuItems (PopupMenu& menu, const MouseEvent* e)
{
    GenericCodeEditorComponent::addPopupMenuItems (menu, e);

    //menu.addSeparator();
    //menu.addItem (insertComponentID, TRANS("Insert code for a new Component class..."));
}

void CppCodeEditorComponent::performPopupMenuAction (int menuItemID)
{
    if (menuItemID == insertComponentID)
        insertComponentClass();

    GenericCodeEditorComponent::performPopupMenuAction (menuItemID);
}

void CppCodeEditorComponent::insertComponentClass()
{
    /*AlertWindow aw (TRANS ("Insert a new Component class"),
                    TRANS ("Please enter a name for the new class"),
                    AlertWindow::NoIcon, nullptr);

    const char* classNameField = "Class Name";

    aw.addTextEditor (classNameField, String::empty, String::empty, false);
    aw.addButton (TRANS ("Insert Code"),  1, KeyPress (KeyPress::returnKey));
    aw.addButton (TRANS ("Cancel"),       0, KeyPress (KeyPress::escapeKey));

    while (aw.runModalLoop() != 0)
    {
        const String className (aw.getTextEditorContents (classNameField).trim());

        if (className == CodeHelpers::makeValidIdentifier (className, false, true, false))
        {
            String code (BinaryData::jucer_InlineComponentTemplate_h);
            code = code.replace ("COMPONENTCLASS", className);

            insertTextAtCaret (code);
            break;
        }
    }*/
}
