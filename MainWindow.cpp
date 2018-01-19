/*
 * Copyright 2009-2017 Scott McCreary
 * Copyright 2014-2016 Puck Meerburg
 * Copyright 2013 Luke (noryb009)
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#include "MainWindow.h"
#include <View.h>
#include <Menu.h>
#include <MenuItem.h>
#include <stdio.h>
#include <stdlib.h>
#include <Alert.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <Roster.h>
#include <Path.h>
#include <Entry.h>
#include <Directory.h>
#include <Box.h>
#include "AboutWindow.h"
#include "Preferences.h"
#include "HexTile.h"
#include "HexTileView.h"
#include "HexGrid.h"
#include "ImageAlert.h"

enum
{
	M_NEW_GAME='nwgm',

	M_SIZE1_TILES,
	M_SIZE2_TILES,
	M_SIZE3_TILES,
	M_SIZE4_TILES,
	M_SIZE5_TILES,

	M_SET_BACKGROUND,

	M_SET_TILE_COUNT_3,
	M_SET_TILE_COUNT_4,
	M_SET_TILE_COUNT_5,
	M_SET_TILE_COUNT_6,
	M_SET_TILE_COUNT_7,
	M_SET_TILE_COUNT_8,
	
	M_SET_BINARY,
	M_SET_QUARTERNARY,
	M_SET_HEXIMAL,
	M_SET_OCTAL,
	M_SET_DECIMAL,
	M_SET_HEXADECIMAL,

	M_HOW_TO_PLAY
};

static const float cos30 = 0.866;

static const uint8 tileSizes[] = {
	TILESIZE_1, TILESIZE_2, TILESIZE_3, TILESIZE_4, TILESIZE_5
};

MainWindow::MainWindow(void)
 :	BWindow(BRect(100,100,500,400),"HexVexed",B_TITLED_WINDOW_LOOK,
 	B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
 	fGrid(NULL),
 	fWorkGrid(NULL)
{
	app_info ai;
	be_app->GetAppInfo(&ai);
	BPath path(&ai.ref);
	path.GetParent(&path);
	path.Append("backgrounds");
	fBackPath = path.Path();
	fBackPath << "/";

	static const rgb_color beos_blue = {51,102,152,255};
	Preferences::Init();
	Preferences::LockPreferences();
	Preferences::Load();
	
	Preferences::Message().PrintToStream();

	if(Preferences::Message().FindInt8("gridsize",(int8*)&fGridSize)!=B_OK
		|| Preferences::Message().FindInt8("tilesize", (int8 *)&fTileSize)!=B_OK
		|| Preferences::Message().FindInt8("numberbase", (int8 *)&fNumberBase)!=B_OK)
	{
		printf("Loading defaults\n");
		// Preferences not working, so load these as the defaults
		fGridSize = 6;
		Preferences::Message().AddInt8("gridsize",fGridSize);
		fTileSize = TILESIZE_3;
		Preferences::Message().AddInt8("tilesize",fTileSize);
		fNumberBase = NUMBERBASE_HEXADECIMAL;
		Preferences::Message().AddInt8("numberbase", fNumberBase);
	}

	fBack = new BackView(Bounds(),"background",B_FOLLOW_ALL,B_WILL_DRAW | B_DRAW_ON_CHILDREN);
	AddChild(fBack);
	rgb_color randa = {rand() % 255, rand() % 255, rand() % 255, 255};
	fBack->SetViewColor(B_TRANSPARENT_COLOR);

	fMenuBar = new BMenuBar(BRect(0,0,Bounds().Width(),20),"menubar");
	fBack->AddChild(fMenuBar);

	BMenu *menu = new BMenu("Game");
	fMenuBar->AddItem(menu);

	menu->AddItem(new BMenuItem("New",new BMessage(M_NEW_GAME),'N'));

	BMenu *submenu = new BMenu("Difficulty");
	submenu->AddItem(new BMenuItem("Beginner",new BMessage(M_SET_TILE_COUNT_3)));
	submenu->AddItem(new BMenuItem("Easy",new BMessage(M_SET_TILE_COUNT_4)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_SET_TILE_COUNT_5)));
	submenu->AddItem(new BMenuItem("Hard",new BMessage(M_SET_TILE_COUNT_6)));
	submenu->AddItem(new BMenuItem("Master",new BMessage(M_SET_TILE_COUNT_7)));
	submenu->SetRadioMode(true);
	menu->AddSeparatorItem();
	menu->AddItem(submenu);
	
	switch (fGridSize)
	{
		case 3:
			submenu->ItemAt(0)->SetMarked(true);
			break;
		case 4:
			submenu->ItemAt(1)->SetMarked(true);
			break;
		case 5:
			submenu->ItemAt(2)->SetMarked(true);
			break;
		case 6:
			submenu->ItemAt(3)->SetMarked(true);
			break;
		case 7:
			submenu->ItemAt(4)->SetMarked(true);
			break;
	}

	submenu = new BMenu("Tile Size");
	submenu->AddItem(new BMenuItem("Small",new BMessage(M_SIZE1_TILES)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_SIZE2_TILES)));
	submenu->AddItem(new BMenuItem("Large",new BMessage(M_SIZE3_TILES)));
	submenu->AddItem(new BMenuItem("Extra Large",new BMessage(M_SIZE4_TILES)));
	submenu->AddItem(new BMenuItem("Jumbo",new BMessage(M_SIZE5_TILES)));	
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);
	
	for (int32 index = 0; index < 5; index++)
		if (fTileSize == tileSizes[index]) {
			submenu->ItemAt(index)->SetMarked(true);
			break;
		}
	
	submenu = new BMenu("Number Base");
	submenu->AddItem(new BMenuItem("Binary",new BMessage(M_SET_BINARY)));
	submenu->AddItem(new BMenuItem("Quarternary",new BMessage(M_SET_QUARTERNARY)));
	submenu->AddItem(new BMenuItem("Heximal",new BMessage(M_SET_HEXIMAL)));
	submenu->AddItem(new BMenuItem("Octal",new BMessage(M_SET_OCTAL)));
	submenu->AddItem(new BMenuItem("Decimal",new BMessage(M_SET_DECIMAL)));
	submenu->AddItem(new BMenuItem("Hexadecimal",new BMessage(M_SET_HEXADECIMAL)));
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);

	switch(fNumberBase)
	{
		case NUMBERBASE_BINARY:
			submenu->ItemAt(0)->SetMarked(true);
			break;
		case NUMBERBASE_QUARTERNARY:
			submenu->ItemAt(1)->SetMarked(true);
			break;
		case NUMBERBASE_HEXIMAL:
			submenu->ItemAt(2)->SetMarked(true);
			break;
		case NUMBERBASE_OCTAL:
			submenu->ItemAt(3)->SetMarked(true);
			break;
		case NUMBERBASE_DECIMAL:
			submenu->ItemAt(4)->SetMarked(true);
			break;
		case NUMBERBASE_HEXADECIMAL:
			submenu->ItemAt(5)->SetMarked(true);
			break;
	}

	fBackMenu = new BMenu("Background");
	menu->AddItem(fBackMenu);
	ScanBackgrounds();

	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("How to Play…",new BMessage(M_HOW_TO_PLAY)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("About HexVexed…",new BMessage(B_ABOUT_REQUESTED)));

	BBox *timeBox = new BBox("box");
	timeBox->ResizeTo(70, 34);
	timeBox->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fBack->AddChild(timeBox);

	fTimer = new TimerView();
	fTimer->ResizeTo(70, 30);

	BFont font = be_bold_font;
	font.SetSize(20);
	fTimer->SetFontAndColor(&font);

	fTimer->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fTimer->SetAlignment(B_ALIGN_CENTER);
	timeBox->AddChild(fTimer);

	GenerateGrid(fGridSize, true);

	BPoint corner;
	if(Preferences::Message().FindPoint("corner",&corner)!=B_OK)
	{
		corner = Frame().LeftTop();
		Preferences::Message().AddPoint("corner",Frame().LeftTop());
	}

	BRect r(Frame());
	r.OffsetTo(corner.x,corner.y);
	ConstrainWindowFrameToScreen(&r);
	MoveTo(r.left,r.top);

	if(Preferences::Message().FindString("background",&fBackName) == B_OK)
		SetBackground(fBackName.String());

	Preferences::UnlockPreferences();
}

bool MainWindow::QuitRequested(void)
{
	Preferences::LockPreferences();
	Preferences::Message().ReplacePoint("corner",Frame().LeftTop());
	Preferences::Save();
	Preferences::UnlockPreferences();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void MainWindow::MessageReceived(BMessage *msg)
{
	Preferences::LockPreferences();

	switch(msg->what)
	{
		case B_ABOUT_REQUESTED:
		{
			AboutWindow *ab = new AboutWindow();
			ab->Show();
			break;
		}
		case M_NEW_GAME:
		{
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SIZE1_TILES:
		case M_SIZE2_TILES:
		case M_SIZE3_TILES:
		case M_SIZE4_TILES:
		case M_SIZE5_TILES:
		{
			const uint8 tileSize = tileSizes[msg->what - M_SIZE1_TILES];
			if (fTileSize != tileSize) {
				fTileSize = tileSize;
				Preferences::Message().ReplaceInt8("tilesize", fTileSize);
				HexTileView::CalcLayout(fTileSize);
				GenerateGrid(fGridSize, false);
			}
			break;
		}
		case M_SET_TILE_COUNT_3:
		{
			fGridSize = 3;
			Preferences::Message().ReplaceInt8("gridsize",3);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_TILE_COUNT_4:
		{
			fGridSize = 4;
			Preferences::Message().ReplaceInt8("gridsize",4);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_TILE_COUNT_5:
		{
			fGridSize = 5;
			Preferences::Message().ReplaceInt8("gridsize",5);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_TILE_COUNT_6:
		{
			fGridSize = 6;
			Preferences::Message().ReplaceInt8("gridsize",6);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_TILE_COUNT_7:
		{
			fGridSize = 7;
			Preferences::Message().ReplaceInt8("gridsize",7);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_BINARY:
		{
			fNumberBase = NUMBERBASE_BINARY;
			Preferences::Message().ReplaceInt8("numberbase",NUMBERBASE_BINARY);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_QUARTERNARY:
		{
			fNumberBase = NUMBERBASE_QUARTERNARY;
			Preferences::Message().ReplaceInt8("numberbase",NUMBERBASE_QUARTERNARY);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_HEXIMAL:
		{
			fNumberBase = NUMBERBASE_HEXIMAL;
			Preferences::Message().ReplaceInt8("numberbase",NUMBERBASE_HEXIMAL);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_OCTAL:
		{
			fNumberBase = NUMBERBASE_OCTAL;
			Preferences::Message().ReplaceInt8("numberbase",NUMBERBASE_OCTAL);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_DECIMAL:
		{
			fNumberBase = NUMBERBASE_DECIMAL;
			Preferences::Message().ReplaceInt8("numberbase",NUMBERBASE_DECIMAL);
			GenerateGrid(fGridSize, true);
			break;
		}
		case M_SET_HEXADECIMAL:
		{
 			fNumberBase = NUMBERBASE_HEXADECIMAL;
 			Preferences::Message().ReplaceInt8("numberbase",NUMBERBASE_HEXADECIMAL);
			GenerateGrid(fGridSize, true);
  			break;
		}
		case M_SET_BACKGROUND:
		{
			BString name;
			if (msg->FindString("name",&name) == B_OK)
				SetBackground(name.String());
			else
				SetBackground(NULL);
			break;
		}
		case M_HOW_TO_PLAY:
		{
			BString string =
				"How to Play HexVexed:\n\n"
				"Place the tiles in the grid on the right into the "
				"grid on the left so that the numbers match wherever "
				"one tile touches another. Easy to learn, tough to master.\n";
			BAlert *help = new BAlert("HexVexed",string.String(),"OK");
			help->Go();
			break;
		}
		case M_CHECK_DROP:
		{
			HexTile *tile;
			HexTileView *to;
			BMessage originalMsg;
			if(msg->FindPointer("to",(void**)&to)!=B_OK ||
					msg->FindPointer("tile",(void**)&tile)!=B_OK ||
					msg->FindMessage("original", &originalMsg)!=B_OK) {
				msg->SendReply(B_MESSAGE_NOT_UNDERSTOOD);
				break;
			}

			// If we're dropping to the storage grid
			if(to->GridId() == fGrid->Id())
			{
				if (!to->GetTile()->IsEmpty()) {
					originalMsg.SendReply(B_MESSAGE_NOT_UNDERSTOOD);
					break;
				}

				*to->GetTile() = *tile;
				tile->gridid = to->GridId();

				to->Invalidate();
			}
			else if(to->GridId() == fWorkGrid->Id())
			{
				if(HexGrid::TryTile(tile, to->GetTile()))
				{
					*to->GetTile() = *tile;
					tile->gridid = to->GridId();
					to->Invalidate();

					if(fWorkGrid->IsSolved())
					{
						fTimer->Stop();
						ImageAlert *alert = new ImageAlert("HexVexedYouWin.png",'PNG ');
						alert->Show();
						GenerateGrid(fGridSize, true);
					}
				} else {
					originalMsg.SendReply(B_MESSAGE_NOT_UNDERSTOOD);
				}
			}
			else
				debugger("Programmer Error: Orphaned Tile");
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
	Preferences::UnlockPreferences();
}


float
MainWindow::GenerateTiles(const BPoint& point, HexGrid* grid)
{
	float x = point.x;
	for (uint8 col = 0; col < fGridSize; col++, x += fTileSize * cos30) {
		float y = point.y;
		if (col % 2)
			y += fTileSize * 0.5;

		for (uint8 row = 0; row < fGridSize; row++, y += fTileSize) {
			HexTileView* tile = new HexTileView(BPoint(x, y), fTileSize, "tile",
				B_FOLLOW_NONE, B_WILL_DRAW);
			tile->SetGridId(grid->Id());
			fBack->AddChild(tile);
			tile->Invalidate(tile->Bounds());
			tile->SetTile(grid->TileAt(row * fGridSize + col));
		}
	}

	return x;
}


void MainWindow::GenerateGrid(uint8 size, bool newGame)
{
	if (newGame) {
		if (fTimer->Running()) {
			BAlert* alert = new BAlert("",
				"Do you want to abort the current game?", "No", "Yes");
			alert->SetShortcut(0, B_ESCAPE);

			if (alert->Go() == 0)
				return;

			fTimer->Stop();
		}

		delete fGrid;
		delete fWorkGrid;

		fGrid = new HexGrid(size, 0);
		fGrid->SetNumberBase(fNumberBase);
		fGrid->GeneratePuzzle();

		fWorkGrid = new HexGrid(size, 1);

		fTimer->Start();
	}

	while (fBack->CountChildren() > 2) {
		BView *child = fBack->ChildAt(2);
		child->RemoveSelf();
		delete child;
	}

	const float inset = 10;
	const float w = fTileSize * cos30 * (size * 2 + 1);
	const float h = fTileSize * (size + 0.5);

	const BRect timerBox(FindView("box")->Bounds());
	const BPoint leftTop(inset, fMenuBar->Frame().bottom + inset);

	const float timerY = leftTop.y + h + inset;
	const float windowWidth = leftTop.x + w + inset;
	const float windowHeight = timerY + timerBox.Height() + inset;

	ResizeTo(windowWidth, windowHeight);

	const float x = GenerateTiles(leftTop, fWorkGrid);
	GenerateTiles(BPoint(x + fTileSize * 0.75, leftTop.y), fGrid);

	fBack->ChildAt(1)->MoveTo((windowWidth - timerBox.Width()) / 2, timerY);
	fBack->Invalidate();
}

void MainWindow::ScanBackgrounds(void)
{
	while (fBackMenu->CountItems() > 0)
	{
		BMenuItem *item = fBackMenu->ItemAt(0L);
		delete item;
	}

	BMessage *msg = new BMessage(M_SET_BACKGROUND);
	msg->AddString("name","");
	fBackMenu->AddItem(new BMenuItem("None",msg));
	fBackMenu->AddSeparatorItem();

	BDirectory dir(fBackPath.String());
	dir.Rewind();
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		BPath path(&ref);
		BString name(path.Leaf());
		int32 ext = name.FindLast(".");
		if (ext > 0)
			name.Truncate(ext);

		msg = new BMessage(M_SET_BACKGROUND);
		msg->AddString("name",path.Leaf());
		fBackMenu->AddItem(new BMenuItem(name.String(),msg));
	}
}

void MainWindow::SetBackground(const char *name)
{
	Preferences::LockPreferences();

	if (!name || strlen(name) == 0)
	{
		fBack->ClearViewBitmap();
		fBack->Invalidate();
		fBackName = "";
		Preferences::Message().RemoveData("background");
		Preferences::UnlockPreferences();
		return;
	}

	BString path(fBackPath);
	path << name;
	BBitmap *bmp = BTranslationUtils::GetBitmapFile(path.String());
	if (bmp)
	{
		fBack->SetViewBitmap(bmp);
		delete bmp;
		fBackName = name;
		Preferences::Message().RemoveData("background");
		Preferences::Message().AddString("background", fBackName);
		fBack->Invalidate();
	}
	Preferences::UnlockPreferences();
}
