/*
 * Copyright 2026 Scott McCreary
 * Copyright 2018 Claire
 * Copyright 2018 Owen
 *
 * Distributed under terms of the MIT License.
 *
 */

#include <String.h>

#include "TimerView.h"


TimerView::TimerView()
	:
	BTextView("timer"),
	fRunning(false)
{
	SetStylable(true);
	MakeEditable(false);
	MakeSelectable(false);
	SetFlags(Flags() | B_PULSE_NEEDED);
}

void TimerView::AttachedToWindow()
{
}

void TimerView::Start()
{
	fRunning = true;
	fStartTime = system_time();
}

bool TimerView::Running()
{
	return fRunning;
}

void TimerView::Stop()
{
	fRunning = false;
}

int TimerView::Elapsed()
{
	return fRunning ? (system_time() - fStartTime) / 1000 / 1000 : 0;
}

void TimerView::Pulse()
{
	if (!fRunning)
		return;
		
	bigtime_t elapsed = Elapsed();
	int mins = elapsed / 60;
	int secs = elapsed % 60;

	BString str;
	str.SetToFormat("%02d:%02d",mins,secs);

	Delete(0,TextLength());
	Insert(str);
}
