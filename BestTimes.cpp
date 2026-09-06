/*
 * Copyright 2026 Scott McCreary
 *
 * Distributed under terms of the MIT License.
 *
 */
#include "BestTimes.h"

#include <algorithm>
#include <Application.h>
#include <AppFileInfo.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Roster.h>
#include <String.h>
#include <stdio.h>
#include <Screen.h>
#include <time.h>
#include <TranslationUtils.h>
#include <vector>

// In case I want to localize this later
#define TRANSLATE(x) x

BLocker BestTimes::fBestTimesLock;
BPath BestTimes::fBestTimesPath;
BMessage BestTimes::fBestTimes;

BString BestTimes::KeyFor(uint8 numberBase, uint8 gridSize)
{
	BString key;
	key.SetToFormat("times_nb%u_gs%u", numberBase, gridSize);
	return key;
}


static bool CompareEntriesBySeconds(const BestTimeEntry &a, const BestTimeEntry &b)
{
	return a.seconds < b.seconds;
}


BString BestTimes::DateKeyFor(uint8 numberBase, uint8 gridSize)
{
	BString key;
	key.SetToFormat("date_nb%u_gs%u", numberBase, gridSize);
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


static BMessage sEmptyBestTimesMessage;

BMessage & BestTimes::Message()
{
	if (!fBestTimesLock.IsLocked())
		return sEmptyBestTimesMessage;
	return fBestTimes;
}


void BestTimes::AddTime(uint8 numberBase, uint8 gridSize, int32 seconds)
{
	if (LockBestTimes() != B_OK)
		return;

	Load();

	BString timesKey = KeyFor(numberBase, gridSize);
	BString datesKey = DateKeyFor(numberBase, gridSize);

	time_t now = time(NULL);
	struct tm *tmNow = localtime(&now);
	char dateStr[11];
	strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", tmNow);

	std::vector<BestTimeEntry> entries;
	int32 existingSeconds;
	for (int32 i = 0;
			fBestTimes.FindInt32(timesKey.String(), i, &existingSeconds) == B_OK;
			i++) {
		BString existingDate;
		if(fBestTimes.FindString(datesKey.String(), i, &existingDate) != B_OK)
			existingDate = "unknown";

		BestTimeEntry entry;
		entry.seconds = existingSeconds;
		entry.date = existingDate;
		entries.push_back(entry);
	}

	BestTimeEntry newEntry;
	newEntry.seconds = seconds;
	newEntry.date = dateStr;
	entries.push_back(newEntry);

	std::sort(entries.begin(), entries.end(), CompareEntriesBySeconds);

	if (entries.size() > TOPXTIMES)
		entries.resize(TOPXTIMES);

	fBestTimes.RemoveName(timesKey.String());
	fBestTimes.RemoveName(datesKey.String());
	for (size_t i = 0; i < entries.size(); i++) {
		fBestTimes.AddInt32(timesKey.String(), entries[i].seconds);
		fBestTimes.AddString(datesKey.String(), entries[i].date);
	}

	status_t saveStatus = Save();
	printf("BestTimes::AddTime saved %zu entries for %s, status=%s\n",
		entries.size(), timesKey.String(), strerror(saveStatus));
	UnlockBestTimes();
}


void BestTimes::PrintBestTimes(uint8 numberBase, uint8 gridSize)
{
	if (LockBestTimes() != B_OK)
		return;

	Load();

	BString timesKey = KeyFor(numberBase, gridSize);
	BString datesKey = DateKeyFor(numberBase, gridSize);

	printf("Best times (number base %u, grid size %u):\n", numberBase, gridSize);

	int32 seconds;
	int32 rank = 0;
	for (int32 i = 0; fBestTimes.FindInt32(timesKey.String(), i, &seconds) == B_OK; i++) {
		rank++;
		BString date;
		if (fBestTimes.FindString(datesKey.String(), i, &date) != B_OK)
			date = "unknown";
		printf("  %2d. %02d:%02d  %s\n", rank, seconds / 60, seconds % 60, date.String());
	}
	if (rank == 0)
		printf("  (no times recorded yet)\n");

	UnlockBestTimes();
}


BestTimesWindow::BestTimesWindow(BRect parentFrame, uint8 numberBase, uint8 gridSize)
 : BWindow(BRect(100,100,500,400),"BestTimes", B_MODAL_WINDOW_LOOK,
 	B_MODAL_APP_WINDOW_FEEL,
 	B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	BestTimesView *besttimesview=new BestTimesView(Bounds(), numberBase, gridSize);
	AddChild(besttimesview);

	if(parentFrame.IsValid())
		CenterIn(parentFrame);
	else
		CenterOnScreen();
}


BestTimesView::BestTimesView(BRect frame, uint8 numberBase, uint8 gridSize)
 : BView (frame, "BestTimesView", B_FOLLOW_ALL, B_WILL_DRAW),
	fNumberBase(numberBase),
	fGridSize(gridSize)
{
	SetViewColor(126,126,190);

	fLogo=BTranslationUtils::GetBitmap('PNG ',"HexVexedAbout.png");  //reuse this one for now

	app_info ai;
	version_info vi;
	be_app->GetAppInfo(&ai);
	BFile file(&ai.ref,B_READ_ONLY);
	BAppFileInfo appinfo(&file);
	appinfo.GetVersionInfo(&vi,B_APP_VERSION_KIND);

	LoadTimes();
}


void BestTimesView::LoadTimes(void)
{
	fTimes.clear();

	if(BestTimes::LockBestTimes() != B_OK)
		return;

	BestTimes::Load();

	BString timesKey = BestTimes::KeyFor(fNumberBase, fGridSize);
	BString datesKey = BestTimes::DateKeyFor(fNumberBase, fGridSize);

	int32 seconds;
	for (int32 i = 0;
		BestTimes::Message().FindInt32(timesKey.String(), i, &seconds) == B_OK;
		i++) {
		BString date;
		if (BestTimes::Message().FindString(datesKey.String(), i, &date) != B_OK)
			date = "unknown";

		BestTimeEntry entry;
		entry.seconds = seconds;
		entry.date = date;
		fTimes.push_back(entry);
	}

	BestTimes::UnlockBestTimes();
}


BestTimesView::~BestTimesView(void)
{
	delete fLogo;
}


void BestTimesView::MouseDown(BPoint pt)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}


void BestTimesView::AttachedToWindow(void)
{
	Window()->ResizeTo(fLogo->Bounds().Width(),fLogo->Bounds().Height());
}


void BestTimesView::Draw(BRect update)
{
	DrawBitmap(fLogo, BPoint(0,0));
	SetHighColor(0,0,0,180);
	BFont font;
	font.SetSize(18);
	SetFont(&font);
	float textwidth;
	textpos.x = 50;
	BPoint center(update.left + (update.Width() /2), update.top + (update.Height() /2));

	if(fTimes.empty()) {
		textpos.y = 50;
		sprintf(besttimestext, "(no times recorded yet)");
		textwidth = StringWidth(besttimestext);
		textpos.x = center.x - (textwidth / 2);
		DrawString(besttimestext, textpos);
		return;
	}

	int32 rank = 0;
	for (std::vector<BestTimeEntry>::const_iterator it = fTimes.begin(); it != fTimes.end(); ++it) {
		rank++;
		// printf("  %2d. %02d:%02d\n", rank, seconds / 60, seconds % 60);
		textpos.y = 130 + (rank - 1) * 30;
		sprintf(besttimestext, "  %2d. %02d:%02d   %s", rank, it->seconds / 60, it->seconds % 60,
			it->date.String());
		textwidth = StringWidth(besttimestext);
		textpos.x = center.x - (textwidth / 2);
		DrawString(besttimestext, textpos);
	}
}
