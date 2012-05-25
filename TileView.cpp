/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#include "TileView.h"
#include <Font.h>
#include <Bitmap.h>
#include <String.h>
#include <Message.h>
#include <Window.h>

static bool sInit=false;

static float sFontHeight = 0;
static float sFontWidth = 0;
static float sFontSize = 0;

static BPoint sLeftPoint;
static BPoint sTopPoint;
static BPoint sRightPoint;
static BPoint sBottomPoint;

void TileView::CalcLayout(uint8 tilesize)
{
	sInit = true;
	
	switch(tilesize)
	{
		case TILESIZE_SMALL:
		{
			sFontSize = 14;
			break;
		}
		case TILESIZE_MEDIUM:
		{
			sFontSize = 16;
			break;
		}
		case TILESIZE_LARGE:
		{
			sFontSize = 20;
			break;
		}
		case TILESIZE_HUGE:
		{
			sFontSize = 24;
			break;
		}
		default:
		{
			tilesize = TILESIZE_MEDIUM;
			sFontSize = 16;
		}
	}
	
	BFont font(be_bold_font);
	font.SetSize(sFontSize);
	
	font_height fh;
	font.GetHeight(&fh);
	
	// We only use the ascent value because we're not using any characters
	// with descenders and leading isn't necessary in this instance, either.
	sFontHeight = fh.ascent;
	sFontWidth = font.StringWidth("0");
	
	sLeftPoint.x = (tilesize / 4.0) - sFontWidth;
	sLeftPoint.y = (tilesize + sFontHeight) / 2.0;
	
	sTopPoint.x = (tilesize - sFontWidth) / 2.0;
	sTopPoint.y = (tilesize / 4.0) + (sFontHeight / 4.0);
	
	sRightPoint.x = (tilesize * .75);
	sRightPoint.y = sLeftPoint.y;
	
	sBottomPoint.x = (tilesize - sFontWidth) / 2.0;
	sBottomPoint.y = (tilesize * .75) + (sFontHeight / 2.0);
}

TileView::TileView(const BPoint &pt, uint8 tilesize, const char *name, const int32 &resize,
					const int32 &flags)
 :	BView(BRect(pt.x,pt.y,pt.x+tilesize,pt.y+tilesize),name,resize,flags),
 	fMouseDown(false),
 	fTile(NULL)
{
	if(tilesize!=TILESIZE_SMALL && tilesize!=TILESIZE_MEDIUM && tilesize!=TILESIZE_LARGE)
		tilesize = TILESIZE_MEDIUM;
	
	if(!sInit)
		CalcLayout(tilesize);
	
	BFont font(be_bold_font);
	font.SetSize(sFontSize);
	SetFont(&font);
}

void TileView::Draw(BRect r)
{
	DrawTile(this,Bounds());
}

rgb_color TileView::NumberColor(int num)
{
	rgb_color white = {255,255,255,255};
	rgb_color black = {0,0,0,255};
	
	switch (num)
    { 
        case 0 :
	    case 1 :
	    case 2 :
	    case 3 :
	    case 6 :
	    case 7 :  return white;
	              break;
	    case 4 :
	    case 5 :
	    case 8 :
	    case 9 :  return black;
	              break;
    }                
    return black;
}

void DrawTile(TileView *owner, BRect r)
{
	static rgb_color highlight = { 255,255,255,255 };
	static rgb_color shadow = { 90,90,90,255 };
	static rgb_color white = { 255,255,255,255 };
	static rgb_color base = { 224,224,224,255 };
	static rgb_color trianglecolor[] = { { 0,0,0,255 }, //Black
	                            { 145,97,0,255 },      //Brown
	                            { 224,0,0,255 },        //Red
	                            { 255,126,0,255 },      //Orange
	                            { 255,255,0,255 },      //Yellow
	                            { 0,208,0,255 },        //Green
	                            { 63,63,255 },          //Blue
	                            { 124,33,176 },         //Purple
	                            { 192,192,192,255 },    //Grey
	                            { 255,255,255,255 } };  //White

    rgb_color numcolor;	                     
	BPoint rectcenter;
	rectcenter.x = (r.RightTop().x-r.LeftTop().x)/2;
	rectcenter.y = (r.LeftBottom().y-r.LeftTop().y)/2;
    owner->SetHighColor(base);
	owner->FillRect(owner->Bounds());
	
	owner->SetDrawingMode(B_OP_COPY);
	
	if(owner->GetTile()->IsEmpty())
	{
		// Draw empty square
		owner->BeginLineArray(4);
		owner->SetHighColor(shadow);
		owner->StrokeLine(r.LeftTop(),r.LeftBottom());
		owner->StrokeLine(r.LeftTop(),BPoint(r.right-1,r.top));
		owner->SetHighColor(highlight);
		owner->StrokeLine(r.RightBottom(),r.RightTop());
		owner->StrokeLine(r.RightBottom(),BPoint(r.left+1,r.bottom));
		owner->EndLineArray();
		return;
	}
	
	// frame
	owner->SetHighColor(highlight);
	owner->StrokeLine(r.LeftTop(),r.LeftBottom());
	owner->StrokeLine(r.LeftTop(),BPoint(r.right-1,r.top));
	owner->SetHighColor(shadow);
	owner->StrokeLine(r.RightBottom(),r.RightTop());
	owner->StrokeLine(r.RightBottom(),BPoint(r.left+1,r.bottom));
	
	// Color Triangles
	
    owner->SetHighColor(trianglecolor[(int)owner->GetTile()->left]);
	owner->FillTriangle(r.LeftTop(),r.LeftBottom(),rectcenter);
	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->bottom]);
	owner->FillTriangle(r.LeftBottom(),r.RightBottom(),rectcenter);
	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->right]);
	owner->FillTriangle(r.RightBottom(),r.RightTop(),rectcenter);
	owner->SetHighColor(trianglecolor[(int)owner->GetTile()->top]);
	owner->FillTriangle(r.LeftTop(),r.RightTop(),rectcenter);

	// dividers
	
	// r.InsetBy(10,10);
	owner->SetHighColor(shadow);
	owner->SetPenSize(3.0);
	owner->StrokeLine(r.LeftTop(),r.RightBottom());
	owner->StrokeLine(r.LeftBottom(),r.RightTop());
    owner->SetHighColor(white);
	owner->SetPenSize(1.0);
	owner->StrokeLine(r.LeftTop(),r.RightBottom());
	owner->StrokeLine(r.LeftBottom(),r.RightTop());
	owner->SetHighColor(shadow);
	owner->SetPenSize(1.0);
	owner->StrokeLine(r.LeftTop(),r.LeftBottom());
	owner->StrokeLine(r.LeftBottom(),r.RightBottom());
	owner->StrokeLine(r.RightTop(),r.RightBottom());
	owner->StrokeLine(r.LeftTop(),r.RightTop());
		
	owner->SetDrawingMode(B_OP_BLEND);
	BRect bevel(r);
	bevel.InsetBy(1,1);
	owner->SetHighColor(255,255,255);
	owner->StrokeLine(bevel.LeftTop(),bevel.RightTop());
	owner->StrokeLine(bevel.LeftTop(),bevel.LeftBottom());
	owner->SetHighColor(0,0,0);
	owner->StrokeLine(bevel.LeftBottom(),bevel.RightBottom());
	owner->StrokeLine(bevel.RightTop(),bevel.RightBottom());
	
	// Numbers
	BString string;
	
	owner->SetDrawingMode(B_OP_ALPHA);
	
	string = "";
	string << (int)owner->GetTile()->left;
	numcolor = owner->NumberColor((int)owner->GetTile()->left);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sLeftPoint);
	
	string = "";
	string << (int)owner->GetTile()->top;
	numcolor = owner->NumberColor((int)owner->GetTile()->top);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sTopPoint);
	
	string = "";
	string << (int)owner->GetTile()->right;
	numcolor = owner->NumberColor((int)owner->GetTile()->right);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sRightPoint);
	
	string = "";
	string << (int)owner->GetTile()->bottom;
	numcolor = owner->NumberColor((int)owner->GetTile()->bottom);
	owner->SetHighColor(numcolor);
	owner->DrawString(string.String(),sBottomPoint);
}

void TileView::SetTile(Tile *tile)
{
	fTile = tile;
	Invalidate();
}

void TileView::MouseDown(BPoint pt)
{
	fMouseDown = true;
	
	if(!fTile->IsEmpty())
		DoDrag();
}

void TileView::MouseMoved(BPoint pt,uint32 code, const BMessage *msg)
{
}

void TileView::MouseUp(BPoint pt)
{
	fMouseDown = false;
}

void TileView::MessageReceived(BMessage *msg)
{
	if(msg->WasDropped())
	{
		TileView *view;
		if(msg->FindPointer("view",(void**)&view)!=B_OK)
			return;
		
		if(!fTile->IsEmpty())
			return;
		
		// Well, we've gotten this far, so this tile must be empty and the other one not.
		// Now we actually have to do some game logic. We can only do this from the
		// hosting window, so post a message to determine the logic
		if(Window())
		{
			BMessage post(M_CHECK_DROP);
			post.AddPointer("from",view);
			post.AddPointer("to",this);
			Window()->PostMessage(&post);
		}
		return;
	}
	
	BView::MessageReceived(msg);
}

void TileView::DoDrag(void)
{
	BBitmap *bitmap = new BBitmap(Bounds(),B_RGB32, true);
	TileView *view = new TileView(BPoint(0,0),(uint8)Bounds().IntegerWidth(),NULL,0,0);
	view->SetTile(GetTile());
	bitmap->AddChild(view);
	bitmap->Lock();
	
	BFont font;
	GetFont(&font);
	view->SetFont(&font);
	DrawTile(view,Bounds());
	view->Sync();
	bitmap->Unlock();
	
	BMessage msg(B_SIMPLE_DATA);
	msg.AddPointer("view",this);
	DragMessage(&msg,bitmap,B_OP_BLEND,BPoint(Bounds().Width()/2,Bounds().Height()/2));
}
