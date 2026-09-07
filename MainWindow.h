/*
 * Copyright 2009-2026 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef MAINWIN_H
#define MAINWIN_H

#include <Application.h>
#include <Bitmap.h>
#include <Entry.h>
#include <map>
#include <MenuBar.h>
#include <Path.h>
#include <Rect.h>
#include <Roster.h>
#include <Screen.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <TranslationUtils.h>
#include <Window.h>

#include "HexTile.h"
#include "TimerView.h"

class HexGrid;

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

	std::map<uint16, HexTile> fTileBackup;

	void	UpdateTileBackup(HexTile *tile);
	void	VerifyBoardIntegrity(void);

	HexGrid *fGrid, *fWorkGrid;
	BView *fBack;
	BMenuBar *fMenuBar;
	BMenu *fBackMenu;
	TimerView *fTimer;
	uint8 fGridSize;
	uint8 fTileSize;
	uint8 fNumberBase;

	BString fBackPath;
	BString fBackName;
	bool fGameOver;
};

#endif
