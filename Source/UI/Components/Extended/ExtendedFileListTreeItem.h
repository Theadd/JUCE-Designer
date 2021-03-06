/*
  ==============================================================================

    ExtendedFileListTreeItem.h
    Created: 23 Nov 2013 4:08:43am
    Author:  admin

  ==============================================================================
*/
#include "../../../Headers.h"

#ifndef EXTENDEDFILELISTTREEITEM_H_INCLUDED
#define EXTENDEDFILELISTTREEITEM_H_INCLUDED

#include "ExtendedFileTreeComponent.h"

class ExtendedFileListTreeItem   : public TreeViewItem,
                           private TimeSliceClient,
                           private AsyncUpdater,
                           private ChangeListener,
						   public Component
{
public:
    ExtendedFileListTreeItem (ExtendedFileTreeComponent& treeComp,
                      DirectoryContentsList* const parentContents,
                      const int indexInContents,
                      const File& f,
                      TimeSliceThread& t)
        : file (f),
          owner (treeComp),
          parentContentsList (parentContents),
          indexInContentsList (indexInContents),
          subContentsList (nullptr, false),
          thread (t),
		  Component("ExtendedFileListTreeItem")
    {
		setName("ExtendedFileListTreeItem for "+f.getFileName());
		normalIcon = nullptr;
		hoverIcon = nullptr;
		selectedIcon = nullptr;
        DirectoryContentsList::FileInfo fileInfo;

        if (parentContents != nullptr
             && parentContents->getFileInfo (indexInContents, fileInfo))
        {
            fileSize = File::descriptionOfSizeInBytes (fileInfo.fileSize);
            modTime = fileInfo.modificationTime.formatted ("%d %b '%y %H:%M");
            isDirectory = fileInfo.isDirectory;
        }
        else
        {
            isDirectory = true;
        }
		//addMouseListener(this, false);
    }

    ~ExtendedFileListTreeItem()
    {
        thread.removeTimeSliceClient (this);
        clearSubItems();
        removeSubContentsList();
		normalIcon = nullptr;
		hoverIcon = nullptr;
		selectedIcon = nullptr;
    }

    //==============================================================================
    bool mightContainSubItems() override                 { return isDirectory; }
    String getUniqueName() const override                { return file.getFullPathName(); }
    int getItemHeight() const override                   { return owner.getItemHeight(); }

    var getDragSourceDescription() override              { return owner.getDragAndDropDescription(); }

    void itemOpennessChanged (bool isNowOpen) override
    {
        if (isNowOpen)
        {
            clearSubItems();

            isDirectory = file.isDirectory();

            if (isDirectory)
            {
                if (subContentsList == nullptr)
                {
                    jassert (parentContentsList != nullptr);

                    DirectoryContentsList* const l = new DirectoryContentsList (parentContentsList->getFilter(), thread);

                    l->setDirectory (file,
                                     parentContentsList->isFindingDirectories(),
                                     parentContentsList->isFindingFiles());

                    setSubContentsList (l, true);
                }

                changeListenerCallback (nullptr);
            }
        }
		updateIcon(true, true);
    }

    void removeSubContentsList()
    {
        if (subContentsList != nullptr)
        {
            subContentsList->removeChangeListener (this);
            subContentsList.clear();
        }
    }

    void setSubContentsList (DirectoryContentsList* newList, const bool canDeleteList)
    {
        removeSubContentsList();

        OptionalScopedPointer<DirectoryContentsList> newPointer (newList, canDeleteList);
        subContentsList = newPointer;
        newList->addChangeListener (this);
    }

    bool selectFile (const File& target)
    {
        if (file == target)
        {
            setSelected (true, true);
            return true;
        }

        if (target.isAChildOf (file))
        {
            setOpen (true);

            for (int maxRetries = 500; --maxRetries > 0;)
            {
                for (int i = 0; i < getNumSubItems(); ++i)
                    if (ExtendedFileListTreeItem* f = dynamic_cast <ExtendedFileListTreeItem*> (getSubItem (i)))
                        if (f->selectFile (target))
                            return true;

                // if we've just opened and the contents are still loading, wait for it..
                if (subContentsList != nullptr && subContentsList->isStillLoading())
                {
                    Thread::sleep (10);
                    rebuildItemsFromContentList();
                }
                else
                {
                    break;
                }
            }
        }

        return false;
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        rebuildItemsFromContentList();
    }

    void rebuildItemsFromContentList()
    {
        clearSubItems();

        if (isOpen() && subContentsList != nullptr)
        {
            for (int i = 0; i < subContentsList->getNumFiles(); ++i)
                addSubItem (new ExtendedFileListTreeItem (owner, subContentsList, i,
                                                  subContentsList->getFile(i), thread));
        }
    }

    void paintItem (Graphics& g, int width, int height) override
    {
		if (file != File::nonexistent)
		{
			updateIcon(true);

			if (normalIcon == nullptr)
				thread.addTimeSliceClient (this);
		}

		bool isItemUnderMouse = (owner.itemUnderMouse == this) ? true : false;
		bool selected = isSelected();
		//DBG("[ITEM] "+file.getFileName()+" getItemPosition(): "+getItemPosition(true).toString()+" owner.getScreenBounds(): "+owner.getScreenBounds().toString()+" Item size: "+String(width)+", "+String(height));
		//DBG("[ITEM] "+file.getFileName()+" getItemPosition(): "+getItemPosition(true).toString()+" isItemUnderMouse: "+String(isItemUnderMouse));

		

		Drawable *icon;
		if (selected)
			icon = selectedIcon;
		else if (isItemUnderMouse)
			icon = hoverIcon;
		else
			icon = normalIcon;

		((ExtendedLookAndFeel*) &getLookAndFeel())->drawFileBrowserRow (g, width, height,
                                         file.getFileName(), icon,
										 fileSize, modTime, false, selected,
										 isOpen(), isItemUnderMouse,
                                         indexInContentsList, owner);
        /*if (file != File::nonexistent)
        {
            updateIcon (true);

            if (icon.isNull())
                thread.addTimeSliceClient (this);
        }

        owner.getLookAndFeel().drawFileBrowserRow (g, width, height,
                                                   file.getFileName(),
                                                   &icon, fileSize, modTime,
                                                   isDirectory, false, //isOpen(),
                                                   indexInContentsList, owner);*/
    }

    void itemClicked (const MouseEvent& e) override
    {
        owner.sendMouseClickMessage (file, e);
    }

    void itemDoubleClicked (const MouseEvent& e) override
    {
        TreeViewItem::itemDoubleClicked (e);

        owner.sendDoubleClickMessage (file);
    }

    void itemSelectionChanged (bool) override
    {
        owner.sendSelectionChangeMessage();
    }

    int useTimeSlice() override
    {
        updateIcon (false);
        return -1;
    }

    void handleAsyncUpdate() override
    {
        owner.repaint();
    }

	/*bool isMouseHover()
	{
		MouseInputSource mouse = Desktop::getInstance().getMainMouseSource();

		Rectangle<int> ownerScreenBounds = owner.getScreenBounds();
		Rectangle<int> itemPosition = getItemPosition(true);
		Rectangle<int> itemScreenBounds(ownerScreenBounds.getX() + itemPosition.getX(), ownerScreenBounds.getY() + itemPosition.getY(), itemPosition.getWidth(), itemPosition.getHeight());
		
		if (itemScreenBounds.contains(mouse.getScreenPosition()))
			return true;

		return false;
	}*/

    const File file;

private:
    ExtendedFileTreeComponent& owner;
    DirectoryContentsList* parentContentsList;
    int indexInContentsList;
    OptionalScopedPointer<DirectoryContentsList> subContentsList;
    bool isDirectory;
    TimeSliceThread& thread;
    Image icon;
    String fileSize, modTime;
	ScopedPointer<Drawable> normalIcon;
	ScopedPointer<Drawable> hoverIcon;
	ScopedPointer<Drawable> selectedIcon;

	String getIconNameForFile(String filename)
	{
		String id;
		if (!isDirectory)
		{
			id = filename.fromLastOccurrenceOf(".", true, true);
			//DBG("[getIconNameForFile] "+filename+" id=["+id+"]");
			if (id.isEmpty())
				id = "note.svg";
			else
			{
				if (id.equalsIgnoreCase(".cpp"))
					id = "file-cpp.svg";
				else if (id.equalsIgnoreCase(".h"))
					id = "file-header.svg";
				else if (id.equalsIgnoreCase(".design"))
					id = "pen-brush.svg";
				else if (id.equalsIgnoreCase(".zip"))
					id = "archive.svg";
				else if (id.equalsIgnoreCase(".gz"))
					id = "archive.svg";
				else if (id.equalsIgnoreCase(".bz2"))
					id = "archive.svg";
				else if (id.equalsIgnoreCase(".png"))
					id = "picture.svg";
				else if (id.equalsIgnoreCase(".svg"))
					id = "picture.svg";
				else
					id = "file.svg";
			}
		}
		else
		{
			if (isOpen())
				id = "folder-open.svg";
			else
				id = "folder-close.svg";
		}

		return id;
	}

	

	/** Update icons for TreeView Item if they are not already set.
	 * @param onlyUpdateIfCached bool unused since we don't have a cache for drawables like ImageCache
	 * @param openStateChanged bool force update, used when a directory changes its open state.
	 */
    void updateIcon (const bool /*onlyUpdateIfCached*/, bool openStateChanged = false)
    {
		if (normalIcon == nullptr || openStateChanged == true)
		{
			String iconName = getIconNameForFile(file.getFileName());
			normalIcon = Icons::get (iconName, Icons::lightgray);
			hoverIcon = Icons::get(iconName, Icons::yellow);
			selectedIcon = Icons::get(iconName, Icons::blue);

			triggerAsyncUpdate();
		}
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtendedFileListTreeItem)
};



#endif  // EXTENDEDFILELISTTREEITEM_H_INCLUDED
