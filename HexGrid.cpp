/*
 * Copyright 2009-2012 Scott McCreary
 * Copyright 2013 Luke (noryb009)
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

#include "HexTile.h"
#include "HexGrid.h"
#include <OS.h>
#include <stdlib.h>
#include <stdio.h>
#include "HexTileView.h"

#define RAND(_min_, _max_) ({ uint8 low = _min_; uint8 high = _max_; rand() % (high-low) + low; })

#define MKRAND RAND(0, 10)


HexGrid::HexGrid(uint8 size)
{
	if(size == 0)
		debugger("Programmer Error: Grid has 0 tiles");

	fSize = size;

	uint16 arraysize = fSize * fSize;
	// create all tiles
	for (uint8 arrno = 0; arrno < arraysize; arrno++)
	{
		fTiles.AddItem(new HexTile());
	}

	for (uint8 row = 0; row < fSize; row++)
	{
		uint16 index = row * fSize;
		for (uint8 col = 0; col < fSize; col++)
		{
			HexTile *tile = fTiles.ItemAt(index+col);
			// top
			if (row == 0)
				tile->toptile = NULL;
			else
				tile->toptile = fTiles.ItemAt(index+col-fSize);
			
			// top left
			if (col == 0 || (row == 0 && col % 2 != 1))
				tile->toplefttile = NULL;
			else
				if (col % 2 == 1)
					tile->toplefttile = fTiles.ItemAt(index+col-1);
				else
					tile->toplefttile = fTiles.ItemAt(index+col-fSize-1);
			
			// bottom left
			if (col == 0 || (row == fSize-1 && col % 2 != 0))
				tile->bottomlefttile = NULL;
			else
				if(col % 2 == 0)
					tile->bottomlefttile = fTiles.ItemAt(index+col-1);
				else
					tile->bottomlefttile = fTiles.ItemAt(index+col+fSize-1);
			
			tile->toprighttile = NULL;
			tile->bottomrighttile = NULL;
			tile->bottomtile = NULL;
		}
	}
	for (uint16 i = 0; i < arraysize; i++)
	{
		HexTile *tile = fTiles.ItemAt(i);

		if (tile->toplefttile)
			tile->toplefttile->bottomrighttile = tile;
		if (tile->bottomlefttile)
			tile->bottomlefttile->toprighttile = tile;
		if (tile->toptile)
			tile->toptile->bottomtile = tile;
	}
}

HexGrid::~HexGrid(void)
{
	for(int32 i=0; i<fTiles.CountItems(); i++)
		delete fTiles.ItemAt(i);
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
		uint16 indexone = RAND(0, arraysize);
		uint16 indextwo = RAND(0, arraysize);

		HexTile *itemone = fTiles.ItemAt(indexone);
		HexTile *tempitem = fTiles.SwapWithItem(indextwo, itemone);
		fTiles.SwapWithItem(indexone, tempitem);
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
