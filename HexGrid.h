/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef HEXGRID_H
#define HEXGRID_H

#include <List.h>
class HexTileView;

class HexTile
{
public:
	HexTile(void);
	HexTile(const uint8 &tl, const uint8 &t, const uint8 &tr, 
			const uint8 &bl, const uint8 &b, const uint8 &br);
	HexTile(const HexTile &t);
	HexTile &operator=(const HexTile &t);
	
	void SetValues( const uint8 &tl, const uint8 &t, const uint8 &tr, 
					const uint8 &bl, const uint8 &b, const uint8 &br);
	
	bool IsEmpty(void) const { return (topleft<0 || top<0 || topright<0 || bottomleft<0 || bottom<0 || bottomright<0); }
	void MakeEmpty(void) { topleft=top=topright=bottomleft=bottom=bottomright=-1; }
	
	int8 topleft;
	int8 top;
	int8 topright;
	int8 bottomleft;
	int8 bottom;
	int8 bottomright;
	uint16 id;
	
	HexTile *toplefttile,*toptile,*toprighttile,*bottomlefttile,*bottomtile,*bottomrighttile;
};

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
	BList fTiles;
	uint32 fSize;
};

#endif
