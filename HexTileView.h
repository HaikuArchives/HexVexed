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

class HexTile;

#define M_CHECK_DROP 'chdr'

#define TILESIZE_SMALL 50
#define TILESIZE_MEDIUM 60
#define TILESIZE_LARGE 75
#define TILESIZE_HUGE 100

class HexTileView : public BView
{
public:
	HexTileView(const BPoint &pt, uint8 tilesize, const char *name,
			const int32 &resize,const int32 &flags);

	HexTile *GetTile(void) const { return fTile; }
	void SetTile(HexTile *hextile);

	void Draw(BRect r);
	rgb_color NumberColor(int num);

	void MouseDown(BPoint where);
	void MouseMoved(BPoint pt,uint32 code, const BMessage *msg);
	void MouseUp(BPoint where);
	void MessageReceived(BMessage *msg);

	static void CalcLayout(uint8 hextilesize);

private:

	void DoDrag(void);

	bool fMouseDown;
	HexTile *fTile;
};

void DrawHexTile(HexTileView *owner, BRect r);

#endif
