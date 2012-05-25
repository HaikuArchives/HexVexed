/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef IMAGEALERT_H
#define IMAGEALERT_H

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Button.h>
#include <Messenger.h>
#include <Font.h>
#include <StatusBar.h>

class AlertView : public BView
{
public:
	AlertView(BRect frame,const char *image,int32 format);
	~AlertView(void);
	void AttachedToWindow(void);
	void Draw(BRect update);
	void MouseDown(BPoint pt);
	
	BBitmap *fLogo;
	
	char version[64];
	BPoint versionpos;
	
	uint8 fAboutFlags;
	int32 fEntryCount;
};

class ImageAlert : public BWindow
{
public:
	ImageAlert(const char *image, int32 format);
};

#endif
