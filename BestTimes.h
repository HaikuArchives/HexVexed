/*
 * Copyright 2026 Scott McCreary
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef BESTTIMES_H
#define BESTTIMES_H

#include <Locker.h>
#include <Message.h>
#include <Path.h>
#include <String.h>
#include <SupportDefs.h>

// Number of best times kept per (numberBase, gridSize) combination.
// Change this single value to adjust how many entries are retained
// everywhere BestTimes is used.
#define TOPXTIMES 10

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

private:
	static BString KeyFor(uint8 numberBase, uint8 gridSize);

	static BLocker fBestTimesLock;
	static BPath fBestTimesPath;
	static BMessage fBestTimes;
};

#endif

