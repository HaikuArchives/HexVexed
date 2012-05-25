/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#include <Application.h>
#include <AppFileInfo.h>
#include <Roster.h>
#include <String.h>
#include <stdio.h>
#include <Screen.h>
#include <TranslationUtils.h>
#include "ImageAlert.h"

// In case I want to localize this later
#define TRANSLATE(x) x

ImageAlert::ImageAlert(const char *image, int32 format)
 : BWindow(BRect(100,100,500,400),"", B_MODAL_WINDOW_LOOK,
 	B_MODAL_APP_WINDOW_FEEL,
 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	BScreen screen;
	BRect screenrect(screen.Frame());
	
	AlertView *alertview=new AlertView(Bounds(),image, format);
	AddChild(alertview);
	
	MoveTo( (screenrect.Width()-Frame().Width())/2, (screenrect.Height()-Frame().Height())/2 );
}

AlertView::AlertView(BRect frame,const char *image, int32 format)
 : BView (frame, "AlertView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(126,126,190);
	
	fLogo=BTranslationUtils::GetBitmap(format,image);
}

AlertView::~AlertView(void)
{
	delete fLogo;
}

void AlertView::MouseDown(BPoint pt)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}

void AlertView::AttachedToWindow(void)
{
	Window()->ResizeTo(fLogo->Bounds().Width(),fLogo->Bounds().Height());
}

void AlertView::Draw(BRect update)
{
	DrawBitmap(fLogo, BPoint(0,0));
}
