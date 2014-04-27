/*
 * Copyright 2009-2012 Scott McCreary
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
#include "AboutWindow.h"
#include "Preferences.h"
#include "HexTile.h"
#include "HexTileView.h"
#include "HexGrid.h"
#include "ImageAlert.h"

enum
{
	M_NEW_GAME='nwgm',

	M_SMALL_TILES,
	M_MEDIUM_TILES,
	M_LARGE_TILES,
	M_HUGE_TILES,

	M_SET_BACKGROUND,

	M_SET_TILE_COUNT_3,
	M_SET_TILE_COUNT_4,
	M_SET_TILE_COUNT_5,
	M_SET_TILE_COUNT_6,
	M_SET_TILE_COUNT_7,

	M_HOW_TO_PLAY
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

	if(Preferences::Message().FindInt8("gridsize",(int8*)&fGridSize)!=B_OK)
	{
		fGridSize = 3;
		Preferences::Message().AddInt8("gridsize",3);
	}

	if(Preferences::Message().FindInt8("tilesize",(int8*)&fTileSize)!=B_OK)
	{
		fTileSize = TILESIZE_MEDIUM;
		Preferences::Message().AddInt8("tilesize",TILESIZE_MEDIUM);
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

	BMenuItem *item = submenu->ItemAt(fGridSize - 3);
	item->SetMarked(true);

	submenu = new BMenu("Tile Size");
	submenu->AddItem(new BMenuItem("Small",new BMessage(M_SMALL_TILES)));
	submenu->AddItem(new BMenuItem("Medium",new BMessage(M_MEDIUM_TILES)));
	submenu->AddItem(new BMenuItem("Large",new BMessage(M_LARGE_TILES)));
	submenu->AddItem(new BMenuItem("Extra Large",new BMessage(M_HUGE_TILES)));
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);

	fBackMenu = new BMenu("Background");
	menu->AddItem(fBackMenu);
	ScanBackgrounds();

	switch(fTileSize)
	{
		case TILESIZE_SMALL:
		{
			submenu->ItemAt(0)->SetMarked(true);
			break;
		}
		case TILESIZE_MEDIUM:
		{
			submenu->ItemAt(1)->SetMarked(true);
			break;
		}
		case TILESIZE_LARGE:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
		case TILESIZE_HUGE:
		{
			submenu->ItemAt(3)->SetMarked(true);
			break;
		}
		default:
		{
			submenu->ItemAt(2)->SetMarked(true);
			break;
		}
	}

	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("How to Play…",new BMessage(M_HOW_TO_PLAY)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("About HexVexed…",new BMessage(B_ABOUT_REQUESTED)));
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
		case M_SMALL_TILES:
		{
			fTileSize = TILESIZE_SMALL;
			Preferences::Message().ReplaceInt8("tilesize",TILESIZE_SMALL);
			HexTileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize, false);
			break;
		}
		case M_MEDIUM_TILES:
		{
			fTileSize = TILESIZE_MEDIUM;
			Preferences::Message().ReplaceInt8("tilesize",TILESIZE_MEDIUM);
			HexTileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize, false);
			break;
		}
		case M_LARGE_TILES:
		{
			fTileSize = TILESIZE_LARGE;
			Preferences::Message().ReplaceInt8("tilesize",TILESIZE_LARGE);
			HexTileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize, false);
			break;
		}
		case M_HUGE_TILES:
		{
			fTileSize = TILESIZE_HUGE;
			Preferences::Message().ReplaceInt8("tilesize",TILESIZE_LARGE);
			HexTileView::CalcLayout(fTileSize);
			GenerateGrid(fGridSize, false);
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
			HexTileView *from, *to;
			if(msg->FindPointer("from",(void**)&from)!=B_OK ||
					msg->FindPointer("to",(void**)&to)!=B_OK)
				break;

			// If we're dropping to the storage grid
			if(fGrid->HasTile(to->GetTile()))
			{
				*to->GetTile() = *from->GetTile();
				from->GetTile()->MakeEmpty();
				to->Invalidate();
				from->Invalidate();
			}
			else
			// If we're dropping to the tile grid
			if(fWorkGrid->HasTile(to->GetTile()))
			{
				if(fWorkGrid->TryTile(from->GetTile(),to->GetTile()))
				{
					*to->GetTile() = *from->GetTile();
					from->GetTile()->MakeEmpty();
					to->Invalidate();
					from->Invalidate();

					if(fWorkGrid->IsSolved())
					{
						ImageAlert *alert = new ImageAlert("HexVexedYouWin.jpg",'JPEG');
						alert->Show();
						GenerateGrid(fGridSize, true);
					}
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

void MainWindow::GenerateGrid(uint8 size, bool newGame)
{
	const double factor1 = 0.75; //was 0.75 horizontal spacing
	const double factor2 = 0.425;  //was 0.433 vertical spacing
	//const double factor3 = 1.7;  //was 1.6
	const double offset1 = 70; //was 70
	const double offset2 = 20;  //was 20 
	
	if(fGrid && newGame)
	{
		delete fGrid;
		delete fWorkGrid;
	}

	fBack->RemoveChild(fMenuBar);
	while(fBack->CountChildren()>0)
	{
		BView *child = fBack->ChildAt(0);
		child->RemoveSelf();
		delete child;
	}
	fBack->AddChild(fMenuBar);
	if(newGame)
	{
		fGrid = new HexGrid(size);
		fGrid->GeneratePuzzle();
		fWorkGrid = new HexGrid(size);
	}

/*	ResizeTo( ((fTileSize+5) * size * 2) + (fTileSize * 0.5),
			(  (fTileSize+5) * size ) + fMenuBar->Frame().Height() + (fTileSize * 0.5));
*/
	ResizeTo( ((fTileSize - 10) * (size - 1) + fTileSize) * 2 + offset1 + offset2 * 2,
			(  fTileSize * size ) + fMenuBar->Frame().Height() + (fTileSize * 0.866) + 10);

	BRect r(10,
			fMenuBar->Frame().bottom + 10,
			10 + fTileSize,
			10 + fMenuBar->Frame().bottom + fTileSize);

	for(uint8 row=0; row<size; row++)
	{
		for(uint8 col=0; col<size; col++)
		{
			HexTileView *tile = new HexTileView(r.LeftTop(),fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fBack->AddChild(tile);
			tile->Invalidate(tile->Bounds());

		    if ( (col % 2) == 1 ) { 
		    	r.OffsetBy(((fTileSize * factor1) + fTileSize * 0.1), -(fTileSize * factor2 + fTileSize * 0.125) +1); //was 0.1
		    	tile->SetTile(fWorkGrid->TileAt((size * row) + col));
	    		r.OffsetBy(0, 0);
			} else {
		    	r.OffsetBy(((fTileSize * factor1) + fTileSize * 0.1), fTileSize * factor2 + fTileSize * 0.125); //was 0.1
		    	tile->SetTile(fWorkGrid->TileAt((size * row) + col));
		    	r.OffsetBy(0, 0);
				}
		}

		if ( (size % 2) == 1 ) 
			r.OffsetBy(fTileSize, fTileSize * -0.5);
		else
			r.OffsetBy(fTileSize, 0);
			
		for(uint8 col=0; col<size; col++)
		{
			HexTileView *tile = new HexTileView(r.LeftTop(),fTileSize,"tile",
											B_FOLLOW_NONE,B_WILL_DRAW);
			fBack->AddChild(tile);
			tile->Invalidate(tile->Bounds());
			
		    if ( (col % 2) == 1 ) { 
				r.OffsetBy(((fTileSize * factor1) + fTileSize * 0.1), fTileSize * -factor2 - fTileSize * 0.1);
				tile->SetTile(fGrid->TileAt((size * row) + col));
	 		   	r.OffsetBy(0, 0);
		    } else {
		    	r.OffsetBy(((fTileSize * factor1) + fTileSize * 0.1), fTileSize * factor2 + fTileSize * 0.1);
		    	tile->SetTile(fGrid->TileAt((size * row) + col));
		    	r.OffsetBy(0, 0);
				}
		}

		if ( (size % 2) == 1 ) 
			r.OffsetBy(-(r.left - 10), factor2 * fTileSize);
		else
			r.OffsetBy(-(r.left - 10), fTileSize);
	}

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
