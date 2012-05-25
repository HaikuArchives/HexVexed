/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <Locker.h>
#include <Message.h>
#include <File.h>
#include <String.h>
#include <InterfaceDefs.h>

extern BLocker prefsLock;
extern BMessage gPreferences;
extern BString gAppPath;

#define PREFERENCES_PATH "/boot/home/config/settings/BeVexed"

status_t SavePreferences(const char *path);
status_t LoadPreferences(const char *path);

void ConstrainWindowFrameToScreen(BRect *rect);

#endif
