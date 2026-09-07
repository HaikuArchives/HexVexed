/*
 * Copyright 2026 Scott McCreary
 *
 * Distributed under terms of the MIT License.
 *
 */
#include "DrawHelpers.h"

#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <NodeInfo.h>
#include <Roster.h>

void DrawTriangleBackground(BView *view, BRect bounds)
{
	BPoint topLeft(bounds.left, bounds.top);
	BPoint topRight(bounds.right, bounds.top);
	BPoint bottomLeft(bounds.left, bounds.bottom);
	BPoint bottomRight(bounds.right, bounds.bottom);
	BPoint center((bounds.left + bounds.right) / 2, (bounds.top + bounds.bottom) / 2);
	BPoint midLeft(bounds.left, (bounds.top + bounds.bottom) / 2);
	BPoint midRight(bounds.right, (bounds.top + bounds.bottom) / 2);

	rgb_color veryLightOrange = {255, 228, 196, 255};
	rgb_color veryLightYellow = {255, 255, 224, 255};
	rgb_color veryLightGreen  = {224, 255, 224, 255};
	rgb_color veryLightRed    = {255, 224, 224, 255};
	rgb_color veryLightPurple = {230, 224, 255, 255};
	rgb_color veryLightBlue   = {224, 240, 255, 255};

	// Triangle 1: top-left -> center -> mid-left
	view->SetHighColor(veryLightOrange);
	view->FillTriangle(topLeft, center, midLeft);

	// Triangle 2: top-left -> top-right -> center
	view->SetHighColor(veryLightYellow);
	view->FillTriangle(topLeft, topRight, center);

	// Triangle 3: top-right -> center -> mid-right
	view->SetHighColor(veryLightGreen);
	view->FillTriangle(topRight, center, midRight);

	// Triangle 4: bottom-left -> center -> mid-left (mirrors 1)
	view->SetHighColor(veryLightRed);
	view->FillTriangle(bottomLeft, center, midLeft);

	// Triangle 5: bottom-left -> bottom-right -> center (mirrors 2)
	view->SetHighColor(veryLightPurple);
	view->FillTriangle(bottomLeft, bottomRight, center);

	// Triangle 6: bottom-right -> center -> mid-right (mirrors 3)
	view->SetHighColor(veryLightBlue);
	view->FillTriangle(bottomRight, center, midRight);

	// Overlay: both diagonals and a horizontal line through the center
	view->SetHighColor(255, 255, 255);
	view->StrokeLine(topLeft, bottomRight);
	view->StrokeLine(bottomLeft, topRight);
	view->StrokeLine(midLeft, midRight);
}


void DrawStringCentered(BView *view, const char *text, float centerX, float y)
{
	float width = view->StringWidth(text);
	view->DrawString(text, BPoint(centerX - width /2, y));
}


void DrawAppIcon(BView *view, BPoint center, float size)
{
	if(size < 1)
		return;

	BBitmap icon(BRect(0, 0, size - 1, size - 1), B_RGBA32);
	if(icon.InitCheck() != B_OK)
		return;

	app_info info;
	if(be_app->GetAppInfo(&info) != B_OK)
		return;

	BFile file(&info.ref, B_READ_ONLY);
	BNodeInfo nodeInfo(&file);
	if(nodeInfo.InitCheck() != B_OK)
		return;

	if(nodeInfo.GetIcon(&icon, B_LARGE_ICON) != B_OK)
		return;

	drawing_mode oldMode = view->DrawingMode();
	view->SetDrawingMode(B_OP_ALPHA);
	view->SetBlendingMode(B_PIXEL_ALPHA,  B_ALPHA_OVERLAY);

	view->DrawBitmap(&icon, BPoint(center.x - size / 2, center.y - size / 2));

	view->SetDrawingMode(oldMode);
}


