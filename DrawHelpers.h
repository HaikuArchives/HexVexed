/*
 * Copyright 2026 Scott McCreary
 *
 * Distributed under terms of the MIT License.
 *
 */

#ifndef DRAWHELPERS_H
#define DRAWHELPERS_H

#include <Rect.h>
#include <View.h>

void DrawTriangleBackground(BView *view, BRect bounds);
void DrawStringCentered(BView *view, const char *text, float centerX, float y);
void DrawAppIcon(BView *view, BPoint center, float size);
void DrawResourcePNG(BView *view, const char *resourceName, BPoint center, float width);

#endif