/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef HEXTILEVIEW_H
#define HEXTILEVIEW_H

#include <View.h>
#include "HexGrid.h"
#include "HexTile.h"

#define M_CHECK_DROP 'chdr'
#define M_DROP_OK 'drok'

#define TILESIZE_SMALL 50
#define TILESIZE_MEDIUM 60
#define TILESIZE_LARGE 75
#define TILESIZE_HUGE 100

#define NUMBERBASE_BINARY		2
#define NUMBERBASE_QUARTERNARY	4
#define NUMBERBASE_HEXIMAL		6
#define NUMBERBASE_OCTAL		8
#define NUMBERBASE_DECIMAL		10
#define NUMBERBASE_HEXADECIMAL	16

class HexTileView : public BView
{
public:
	HexTileView(const BPoint &pt, uint8 tilesize, const char *name,
			const int32 &resize,const int32 &flags);

	HexTile *GetTile(void) const { return fTile; }
	void SetTile(HexTile *hextile);

	void Draw(BRect r);
	rgb_color NumberColor(int num);

	uint16	GridId() { return fGridId; }
	void SetGridId(uint16 gridid) { fGridId = gridid; if(fTile) fTile->gridid = gridid; }

	void MouseDown(BPoint where);
	void MouseMoved(BPoint pt,uint32 code, const BMessage *msg);
	void MouseUp(BPoint where);
	void MessageReceived(BMessage *msg);

	void AttachedToWindow();

	static void CalcLayout(uint8 hextilesize);

	bool FakeEmpty() { return fFakeEmpty; }

private:

	void DoDrag(void);

	bool fMouseDown;
	bool fFakeEmpty;

	uint16 fGridId;
	HexTile *fTile;
	HexTile *fOtherTile;
};

void DrawHexTile(HexTileView *owner, BRect r, bool lockedIn);

#endif
