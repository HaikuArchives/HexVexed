/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef HEXGRID_H
#define HEXGRID_H

#include <ObjectList.h>

class HexTileView;
class HexTile;

class HexGrid
{
public:
	HexGrid(uint8 size);
	~HexGrid(void);
	
	uint8 Size(void) const { return fSize; }
	void GeneratePuzzle(void);
	HexTile *TileAt(const uint16 &index);
	bool HasTile(HexTile *t) { return fTiles.HasItem(t); }
	
	bool TryTile(HexTile *src, HexTile *dest);
	bool IsSolved(void);
private:
	BObjectList<HexTile> fTiles;
	uint32 fSize;
};

#endif
