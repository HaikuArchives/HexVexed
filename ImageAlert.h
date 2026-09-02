/*
 * Copyright 2009-2026 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#ifndef IMAGEALERT_H
#define IMAGEALERT_H

#include <Bitmap.h>
#include <Button.h>
#include <Font.h>
#include <Messenger.h>
#include <StatusBar.h>
#include <View.h>
#include <Window.h>


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
	ImageAlert(const char *image, int32 format, BRect parentFrame = BRect());
};

#endif
