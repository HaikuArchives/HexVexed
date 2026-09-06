/*
 * Copyright 2026 Scott McCreary
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef BESTTIMES_H
#define BESTTIMES_H

#include <Bitmap.h>
#include <Button.h>
#include <Font.h>
#include <Locker.h>
#include <Message.h>
#include <Messenger.h>
#include <Path.h>
#include <StatusBar.h>
#include <String.h>
#include <SupportDefs.h>
#include <vector>
#include <View.h>
#include <Window.h>


/*
enum
{
	ABOUT_STARTUP=0,
	ABOUT_OK,
	ABOUT_OK2=2
};

enum
{
	M_RESET_STATUS='mrst',
	M_SET_STATUS='msst',
	M_UPDATE_STATUS='mups'
};
*/


// Number of best times kept per (numberBase, gridSize) combination.
// Change this single value to adjust how many entries are retained
// everywhere BestTimes is used.
#define TOPXTIMES 10


struct BestTimeEntry {
	int32 seconds;
	BString date; // YYYY-MM-DD
};


class BestTimes {
public:
	static void Init();
	static status_t Load();
	static status_t Save();

	static status_t LockBestTimes();
	static void UnlockBestTimes();

	static BMessage & Message();

	static void AddTime(uint8 numberBase, uint8 gridSize, int32 seconds);

	static void PrintBestTimes(uint8 numberBase, uint8 gridSize);
	static BString KeyFor(uint8 numberBase, uint8 gridSize);
	static BString DateKeyFor(uint8 numberBase, uint8 gridSize);

private:

	static BLocker fBestTimesLock;
	static BPath fBestTimesPath;
	static BMessage fBestTimes;

};


class BestTimesView : public BView
{
public:
	BestTimesView(BRect frame, uint8 numberBase, uint8 gridSize);
	~BestTimesView(void);
	void AttachedToWindow(void);
	void Draw(BRect update);
	void MouseDown(BPoint pt);

	BBitmap *fLogo;

	char besttimestext[64];
	BPoint textpos;

	uint8 fNumberBase;
	uint8 fGridSize;
	int32 fEntryCount;

private:
	void LoadTimes(void);

	std::vector<BestTimeEntry> fTimes;
};

class BestTimesWindow : public BWindow
{
public:
	BestTimesWindow(BRect parentFrame = BRect(), uint8 numberBase = 1, uint8 gridSize = 3);
};

#endif

