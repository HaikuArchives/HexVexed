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
#include <Window.h>
#include <MenuBar.h>
#include <String.h>

class HexGrid;

class MainWindow : public BWindow
{
public:
			MainWindow(void);
	bool	QuitRequested(void);
	void	MessageReceived(BMessage *msg);
	
private:
	void	GenerateGrid(uint8 size, bool newGame);
	void	ScanBackgrounds(void);
	void	SetBackground(const char *name);
	
	HexGrid *fGrid, *fWorkGrid;
	BView *fBack;
	BMenuBar *fMenuBar;
	BMenu *fBackMenu;
	uint8 fGridSize;
	uint8 fTileSize;
	
	BString fBackPath;
	BString fBackName;
};

#endif
