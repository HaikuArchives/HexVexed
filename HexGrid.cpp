/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
 
/*
  example layout, 4 rows, 4 columns = 16 hexagon tiles

              col0    col1     col2     col3
             ______            ______
            /\    /\          /\    /\
           /tl\ t/tr\        /tl\ t/tr\
 row0     /____\/____\ ____ /____\/____\______
          \    /\    /\    /\    /\    /\    /\
           \bl/ b\br/tl\ t/tr\bl/ b\br/tl\ t/tr\
            \/____\/____\/____\/____\/____\/____\
            /\    /\    /\    /\    /\    /\    /
           /tl\ t/tr\bl/ b\br/tl\ t/tr\bl/ b\br/
 row1     /____\/____\/____\/____\/____\/____\/
          \    /\    /\    /\    /\    /\    /\
           \bl/ b\br/tl\ t/tr\bl/ b\br/tl\ t/tr\
            \/____\/____\/____\/____\/____\/____\
                   \    /\    /      \    /\    /
                    \bl/ b\br/        \bl/ b\br/
                     \/____\/          \/____\/


*/

#include "HexGrid.h"
#include <OS.h>
#include <stdlib.h>
#include <stdio.h>
#include "HexTileView.h"

//#define TESTMODE 1

#define MKRAND 	uint8(float(rand()) / RAND_MAX * 10)

HexTile::HexTile(void)
 :	topleft(-1),
 	top(-1),
 	topright(-1),
	bottomleft(-1),
 	bottom(-1),
 	bottomright(-1),
 	id(0),
 	toplefttile(NULL),
 	toptile(NULL),
 	toprighttile(NULL),
 	bottomlefttile(NULL),
 	bottomtile(NULL),
 	bottomrighttile(NULL)
{
	SetValues(-1,-1,-1,-1,-1,-1);
	id=0;
}

HexTile::HexTile(const uint8 &tl, const uint8 &t, const uint8 &tr, 
				 const uint8 &bl, const uint8 &b, const uint8 &br)
 :	topleft(tl),
 	top(t),
 	topright(tr),
	bottomleft(bl),
 	bottom(b),
 	bottomright(br),
 	id(0),
 	toplefttile(NULL),
 	toptile(NULL),
 	toprighttile(NULL),
 	bottomlefttile(NULL),
 	bottomtile(NULL),
 	bottomrighttile(NULL)
{
	id=0;
}

HexTile::HexTile(const HexTile &t)
 :	topleft(t.topleft),
 	top(t.top),
 	topright(t.topright),
	bottomleft(t.bottomleft),
 	bottom(t.bottom),
 	bottomright(t.bottomright),
 	id(t.id),
 	toplefttile(NULL),
 	toptile(NULL),
 	toprighttile(NULL),
 	bottomlefttile(NULL),
 	bottomtile(NULL),
 	bottomrighttile(NULL)
{
	id=t.id;
}

HexTile &HexTile::operator=(const HexTile &t)
{
	SetValues(t.topleft,t.top,t.topright,t.bottomleft,t.bottom,t.bottomright);
	id=t.id;
	return *this;
}


void HexTile::SetValues(const uint8 &tl, const uint8 &t, const uint8 &tr, 
                        const uint8 &bl, const uint8 &b, const uint8 &br)
{
	topleft = tl;
	top = t;
	topright = tr;
	bottomleft = bl;
	bottom = b;
	bottomright = br;
}

HexGrid::HexGrid(uint8 size)
{
	if(size == 0)
		debugger("Programmer Error: Grid has 0 tiles");

	fSize = size;

	// create all tiles
	for(uint8 row=0; row<fSize; row++)
	{
		for(uint8 col=0; col<fSize; col++)
		{
			HexTile *tile = new HexTile();
			fTiles.AddItem(tile);
		}
	}

	for(uint8 row=0; row<fSize; row++)
	{
		uint16 index = row * fSize;
		for(uint8 col=0; col<fSize; col++)
		{
			HexTile *tile = (HexTile*)(HexTile*)fTiles.ItemAt(index+col);
			// top
			if(row == 0) {
				tile->toptile = NULL;
			}
			else
				tile->toptile = (HexTile*)fTiles.ItemAt(index+col-fSize);
			
			// top left
			if(col == 0 || (row == 0 && col % 2 != 1)) {
				tile->toplefttile = NULL;
			}
			else {
				if(col % 2 == 1)
					tile->toplefttile = (HexTile*)fTiles.ItemAt(index+col-1);
				else
					tile->toplefttile = (HexTile*)fTiles.ItemAt(index+col-fSize-1);
			}
			
			// bottom left
			if(col == 0 || (row == fSize-1 && col % 2 != 0)) {
				tile->bottomlefttile = NULL;
				}
			else {
				if(col % 2 == 0)
					tile->bottomlefttile = (HexTile*)fTiles.ItemAt(index+col-1);
				else
					tile->bottomlefttile = (HexTile*)fTiles.ItemAt(index+col+fSize-1);
			}
			
			tile->toprighttile = NULL;
			tile->bottomrighttile = NULL;
			tile->bottomtile = NULL;
		}
	}
	uint16 arraysize = fSize * fSize;
	for(uint16 i=0; i<arraysize; i++)
	{
		HexTile *tile = (HexTile*)fTiles.ItemAt(i);
		if(tile->toplefttile)
			tile->toplefttile->bottomrighttile = tile;
		if(tile->bottomlefttile)
			tile->bottomlefttile->toprighttile = tile;
		if(tile->toptile)
			tile->toptile->bottomtile = tile;
	}
}

HexGrid::~HexGrid(void)
{
	for(int32 i=0; i<fTiles.CountItems(); i++)
		delete (HexTile*)fTiles.ItemAt(i);
	fTiles.MakeEmpty();
}

void HexGrid::GeneratePuzzle(void)
{
	HexTile *tile;

	// Generate tiles and index when solved
	uint16 id=0;
	for(uint8 row=0; row<fSize; row++)
	{
		uint16 index = row * fSize;
		for(uint8 col=0; col<fSize; col++)
		{
			tile = (HexTile*)fTiles.ItemAt(index + col);
			// if there is a tile above and its bottom has a value,
			tile->top = (tile->toptile && tile->toptile->bottom != -1) ?
				// get the bottom value. Otherwise, make a random value
				tile->toptile->bottom : MKRAND;
			tile->topleft = (tile->toplefttile && tile->toplefttile->bottomright != -1) ?
				tile->toplefttile->bottomright : MKRAND;
			tile->topright = (tile->toprighttile && tile->toprighttile->bottomleft != -1) ?
				tile->toprighttile->bottomleft : MKRAND;
			
			tile->bottom = (tile->bottomtile && tile->bottomtile->top != -1) ?
				tile->bottomtile->top : MKRAND;
			tile->bottomleft = (tile->bottomlefttile && tile->bottomlefttile->topright != -1) ?
				tile->bottomlefttile->topright : MKRAND;
			tile->bottomright = (tile->bottomrighttile && tile->bottomrighttile->topleft != -1) ?
				tile->bottomrighttile->topleft : MKRAND;
			tile->id = id++;
		}
	}

#ifndef TESTMODE
	// Now mix them all up
	uint16 arraysize = (fSize * fSize);
	uint16 count = arraysize * 4;
	for(uint8 i=0; i<count; i++)
	{
		// swap two random elements
		uint16 indexone = uint16(float(rand()) / RAND_MAX * arraysize);
		uint16 indextwo = uint16(float(rand()) / RAND_MAX * arraysize);

		fTiles.SwapItems(indexone,indextwo);
	}
#endif
}

HexTile *HexGrid::TileAt(const uint16 &index)
{
	if(index > (fSize * fSize) - 1)
		debugger("Programmer Error: Tile index too big");

	return (HexTile*)fTiles.ItemAt(index);
}

bool HexGrid::TryTile(HexTile *src, HexTile *dest)
{
	if(!src || !dest || src->IsEmpty())
		return false;

	// Return true if the thing is a valid play
	if(dest->toplefttile && !dest->toplefttile->IsEmpty())
	{
		if(dest->toplefttile != src && dest->toplefttile->bottomright != src->topleft)
			return false;
	}

	if(dest->toptile && !dest->toptile->IsEmpty())
	{
		if(dest->toptile != src && dest->toptile->bottom != src->top)
			return false;
	}

	if(dest->toprighttile && !dest->toprighttile->IsEmpty())
	{
		if(dest->toprighttile != src && dest->toprighttile->bottomleft != src->topright)
			return false;
	}

	if(dest->bottomlefttile && !dest->bottomlefttile->IsEmpty())
	{
		if(dest->bottomlefttile != src && dest->bottomlefttile->topright != src->bottomleft)
			return false;
	}

	if(dest->bottomtile && !dest->bottomtile->IsEmpty())
	{
		if(dest->bottomtile != src && dest->bottomtile->top != src->bottom)
			return false;
	}

	if(dest->bottomrighttile && !dest->bottomrighttile->IsEmpty())
	{
		if(dest->bottomrighttile != src && dest->bottomrighttile->topleft != src->bottomright)
			return false;
	}

	return true;
}

bool HexGrid::IsSolved(void)
{
	// return true if the puzzle is solved
	for(int32 i = 0; i < fTiles.CountItems(); i++)
	{
		HexTile *t = (HexTile*)fTiles.ItemAt(i);
		if(t->id != i || t->IsEmpty())
			return false;
	}
	return true;
}
