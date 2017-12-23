/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef MAINWIN_H
#define MAINWIN_H

#include <Application.h>
#include <Bitmap.h>
#include <Screen.h>
#include <Window.h>
#include <MenuBar.h>
#include <Rect.h>
#include <String.h>
#include <stdio.h>
#include <stdlib.h>
#include <TranslationUtils.h>

class HexGrid;

class BackView : public BView
{
public:
	BackView(BRect r, const char* t, uint32 fl, uint32 fl2)
	: BView(r, t, fl, fl2) {
		fBackgroundColor = BScreen().DesktopColor();
		bitmap = BTranslationUtils::GetBitmap("/boot/home/hexvexed/background.png");
		}
	
	
	
	void Draw(BRect rect) {
		SetHighColor(fBackgroundColor);
		FillRect(Bounds());
		if (bitmap)
			DrawBitmap(bitmap, bitmap->Bounds(), Bounds());
		SetLowColor(fBackgroundColor);
	}
	
	void	SetBackgroundColor(rgb_color color) {
		fBackgroundColor = color;
		Invalidate();
	}

	rgb_color BackgroundColor() {
		return fBackgroundColor;
	}

private:
	rgb_color fBackgroundColor;
	BBitmap *bitmap;
};

class MainWindow : public BWindow
{
public:
			MainWindow(void);
	bool	QuitRequested(void);
	void	MessageReceived(BMessage *msg);
private:
	float	GenerateTiles(const BPoint& point, HexGrid* grid);
	void	GenerateGrid(uint8 size, bool newGame);
	void	ScanBackgrounds(void);
	void	SetBackground(const char *name);
	
	HexGrid *fGrid, *fWorkGrid;
	BackView *fBack;
	BMenuBar *fMenuBar;
	BMenu *fBackMenu;
	uint8 fGridSize;
	uint8 fTileSize;
	uint8 fNumberBase;
	
	BString fBackPath;
	BString fBackName;
};

#endif
