/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#include "Preferences.h"
#include <Screen.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <PathFinder.h>
#include <string>
#include <stdio.h>
#include <Roster.h>
#include <Application.h>
#include <Path.h>

BLocker Preferences::fPrefsLock;
BPath Preferences::fPrefsPath;
BMessage Preferences::fPreferences;

void ConstrainWindowFrameToScreen(BRect *rect)
{
	if(!rect)
		return;
	
	// Even though we have the proper settings, we're going to make sure that
	// the whole window can be seen on screen.
	BRect screenframe = BScreen().Frame();
	
	// make sure that the top left corner is actually on the screen
	int32 xoff = 0, yoff = 0;
	if(rect->left < 0)
		xoff = (int32) (0-rect->left) + 5;
	if(rect->top < 0)
		yoff = (int32) (0-rect->top) + 15;
	rect->OffsetBy(xoff,yoff);
	
	// If either dimension is too large for the screen, we move and resize the rect so
	// that it occupies the screen in that dimension
	if(rect->Width() > screenframe.Width())
	{
		rect->left = 5;
		rect->right = screenframe.right - 5;
	}
	
	if(rect->Height() > screenframe.Height())
	{
		// We set top to 15 to account for the window tab
		rect->top = 15;
		rect->bottom = screenframe.bottom - 5;
	}
	
	if(rect->right > screenframe.right)
		rect->right = screenframe.right - 5;
	
	if(rect->bottom > screenframe.bottom)
		rect->bottom = screenframe.bottom - 5;
}


void
Preferences::Init()
{
	BPathFinder finder;
	if (finder.FindPath(B_FIND_PATH_SETTINGS_DIRECTORY, "HexVexed",
		B_FIND_PATH_CREATE_PARENT_DIRECTORY, fPrefsPath) != B_OK) {
		find_directory(B_USER_SETTINGS_DIRECTORY, &fPrefsPath);
		fPrefsPath.Append("global/HexVexed");
	}
	printf("Path: %s\n", fPrefsPath.Path());
}


status_t
Preferences::Save()
{
	if (!fPrefsLock.IsLocked())
		return B_ERROR;
	
	BFile prefsFile(fPrefsPath.Path(), B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	
	status_t status = prefsFile.InitCheck();
	
	if (status != B_OK)
		return status;
	
	return fPreferences.Flatten(&prefsFile);
}


status_t
Preferences::Load()
{
	if(!fPrefsLock.IsLocked())
		return B_ERROR;

	BFile prefsFile(fPrefsPath.Path(), B_READ_ONLY);
	
	status_t status = prefsFile.InitCheck();

	if (status != B_OK)
		return status;
	
	return fPreferences.Unflatten(&prefsFile);	
}


status_t
Preferences::LockPreferences()
{
	return fPrefsLock.Lock();
}


void
Preferences::UnlockPreferences()
{
	fPrefsLock.Unlock();
}


BMessage &
Preferences::Message()
{
	if(!fPrefsLock.IsLocked())
		return *(BMessage *)NULL;
	return fPreferences;
}
