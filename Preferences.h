/*
 * Copyright 2009-2012 Scott McCreary
 * Copyright 2014 Puck Meerburg
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <File.h>
#include <InterfaceDefs.h>
#include <Locker.h>
#include <Message.h>
#include <Path.h>
#include <String.h>

class Preferences {
public:
	static void Init();

	static status_t Save();
	static status_t Load();

	static status_t LockPreferences();
	static void UnlockPreferences();

	static BMessage &Message();

private:
	static BMessage fPreferences;
	static BPath fPrefsPath;
	static BLocker fPrefsLock;
};

void ConstrainWindowFrameToScreen(BRect *rect);

#endif
