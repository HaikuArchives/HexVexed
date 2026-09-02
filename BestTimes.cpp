/*
 * Copyright 2026 Scott McCreary
 *
 * Distributed under terms of the MIT License.
 *
 */
#include "BestTimes.h"

#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <stdio.h>

#include <algorithm>
#include <vector>

BLocker BestTimes::fBestTimesLock;
BPath BestTimes::fBestTimesPath;
BMessage BestTimes::fBestTimes;

BString BestTimes::KeyFor(uint8 numberBase, uint8 gridSize)
{
	BString key;
	key.SetToFormat("times_nb%u_gs%u", numberBase, gridSize);
	return key;
}

void BestTimes::Init()
{
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &fBestTimesPath) == B_OK) {
		status_t status = fBestTimesPath.Append("HexVexed");
		if (status == B_OK)
			status = create_directory(fBestTimesPath.Path(), 0777);
		if (status == B_OK)
			fBestTimesPath.Append("HexVexed_BestTimes");
	}
	printf("BestTimes path: %s\n", fBestTimesPath.Path());
}

status_t BestTimes::Save()
{
	if (!fBestTimesLock.IsLocked())
		return B_ERROR;

	BFile file(fBestTimesPath.Path(), B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	status_t status = file.InitCheck();
	if (status != B_OK)
		return status;

	return fBestTimes.Flatten(&file);
}

status_t BestTimes::Load()
{
	if (!fBestTimesLock.IsLocked())
		return B_ERROR;

	BFile file(fBestTimesPath.Path(), B_READ_ONLY);
	status_t status = file.InitCheck();
	if (status != B_OK)
		return status;

	return fBestTimes.Unflatten(&file);
}

status_t BestTimes::LockBestTimes()
{
	return fBestTimesLock.Lock() ? B_OK : B_ERROR;
}

void BestTimes::UnlockBestTimes()
{
	fBestTimesLock.Unlock();
}

BMessage & BestTimes::Message()
{
	if (!fBestTimesLock.IsLocked())
		return *(BMessage *)NULL;
	return fBestTimes;
}

void BestTimes::AddTime(uint8 numberBase, uint8 gridSize, int32 seconds)
{
	if (LockBestTimes() != B_OK)
		return;

	Load();

	BString key = KeyFor(numberBase, gridSize);

	std::vector<int32> times;
	int32 existing;
	for (int32 i = 0; fBestTimes.FindInt32(key.String(), i, &existing) == B_OK; i++)
		times.push_back(existing);

	times.push_back(seconds);
	std::sort(times.begin(), times.end());

	if (times.size() > TOPXTIMES)
		times.resize(TOPXTIMES);

	fBestTimes.RemoveName(key.String());
	for (size_t i = 0; i < times.size(); i++)
		fBestTimes.AddInt32(key.String(), times[i]);
	status_t saveStatus = Save();
	printf("BestTimes::AddTime saved %zu entries for %s, status=%s\n",
		times.size(), key.String(), strerror(saveStatus));
	UnlockBestTimes();
}

void BestTimes::PrintBestTimes(uint8 numberBase, uint8 gridSize)
{
	if (LockBestTimes() != B_OK)
		return;

	Load();

	BString key = KeyFor(numberBase, gridSize);
	printf("Best times (number base %u, grid size %u):\n", numberBase, gridSize);

	int32 seconds;
	int32 rank = 0;
	for (int32 i = 0; fBestTimes.FindInt32(key.String(), i, &seconds) == B_OK; i++) {
		rank++;
		printf("  %2d. %02d:%02d\n", rank, seconds / 60, seconds % 60);
	}
	if (rank == 0)
		printf("  (no times recorded yet)\n");

	UnlockBestTimes();
}