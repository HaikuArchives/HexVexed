/*
 * Copyright 2009-2026 Scott McCreary
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
#include "AboutWindow.h"
#include "DrawHelpers.h"

// In case I want to localize this later
#define TRANSLATE(x) x

AboutWindow::AboutWindow(BRect parentFrame)
 : BWindow(BRect(100,100,500,400),"HexVexed", B_MODAL_WINDOW_LOOK,
 	B_MODAL_APP_WINDOW_FEEL,
 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	AboutView *aboutview=new AboutView(Bounds());
	AddChild(aboutview);
	
	if (parentFrame.IsValid())
		CenterIn(parentFrame);
	else
		CenterOnScreen();
}

AboutView::AboutView(BRect frame)
 : BView (frame, "AboutView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(126,126,190);
	
	fLogo=BTranslationUtils::GetBitmap('PNG ',"HexVexedAbout.png");
	
	app_info ai;
	version_info vi;
	be_app->GetAppInfo(&ai);
	BFile file(&ai.ref,B_READ_ONLY);
	BAppFileInfo appinfo(&file);
	appinfo.GetVersionInfo(&vi,B_APP_VERSION_KIND);

	BString variety;
	switch(vi.variety)
	{
		case 0:
			variety=TRANSLATE("d");
			break;
		case 1:
			variety=TRANSLATE("a");
			break;
		case 2:
			variety=TRANSLATE("b");
			break;
		case 3:
			variety=TRANSLATE("g");
			break;
		case 4:
			variety=TRANSLATE("rc");
			break;
		default:
			variety=TRANSLATE("Final");
			break;
	}
	
	if(variety!="Final")
		sprintf(version,"%s %lu.%lu %s%lu",TRANSLATE("v"),vi.major,
			vi.middle,variety.String(),vi.internal);
	else
		sprintf(version,"%s %lu.%lu",TRANSLATE("v"),vi.major,vi.middle);
	
	font_height height;
	be_plain_font->GetHeight(&height);
	versionpos.y = fLogo->Bounds().bottom - 5 - height.descent;

	SetDrawingMode(B_OP_OVER);
}


AboutView::~AboutView(void)
{
	delete fLogo;
}


void AboutView::MouseDown(BPoint pt)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}


void AboutView::AttachedToWindow(void)
{
	Window()->ResizeTo(fLogo->Bounds().Width(),fLogo->Bounds().Height());
}


void AboutView::Draw(BRect update)
{
	DrawTriangleBackground(this, Bounds());
	SetHighColor(0,0,0,180);
	BFont font;
	font.SetSize(96);
	SetFont(&font);
	DrawStringCentered(this, "HexVexed", Bounds().Width() * 0.5, Bounds().Height() * 0.23);
	font.SetSize(28);
	SetFont(&font);
	DrawStringCentered(this, "by Scott McCreary", Bounds().Width() * 0.5, Bounds().Height() * 0.35);
	font.SetSize(24);
	SetFont(&font);
	DrawStringCentered(this, "Based on BeVexed", Bounds().Width() * 0.25, Bounds().Height() * 0.45);
	DrawStringCentered(this, "by DarkWyrm", Bounds().Width() * 0.25, Bounds().Height() * 0.55);
	DrawStringCentered(this, "Graphic and Icon", Bounds().Width() * 0.75, Bounds().Height() * 0.45);
	DrawStringCentered(this, "by Stephanie Fu", Bounds().Width() * 0.75, Bounds().Height() * 0.55);
	font.SetSize(20);
	SetFont(&font);
	DrawStringCentered(this, "Other Contributers:", Bounds().Width() * 0.5, Bounds().Height() * 0.65);
	DrawStringCentered(this, "Humdinger, Owen Pan, Puck Meerburg, Luke (noryb009)", Bounds().Width() * 0.5, Bounds().Height() * 0.72);
	DrawStringCentered(this, "Ojasva Jain, Claire50", Bounds().Width() * 0.5, Bounds().Height() * 0.79);
	//DrawStringCentered(this, "Big list of names here, this is a test", Bounds().Width() * 0.5, Bounds().Height() * 0.84);
	font.SetSize(18);
	SetFont(&font);
	DrawStringCentered(this, version, Bounds().Width() /2, versionpos.y);
	DrawAppIcon(this, BPoint(Bounds().Width() * 0.2 , Bounds().Height() * 0.85), 96);
	DrawAppIcon(this, BPoint(Bounds().Width() * 0.85 , Bounds().Height() * 0.85), 96);
}
