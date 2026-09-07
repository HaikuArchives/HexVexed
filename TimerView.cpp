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
	BTextView::AttachedToWindow();
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
	if(fRunning) {
		fStopTime = system_time();
		fRunning = false;
		UpdateDisplay(Elapsed());
	}
}


int TimerView::Elapsed()
{
	bigtime_t end = fRunning ? system_time() : fStopTime;
	return (end - fStartTime) / 1000 / 1000;
}


void TimerView::Pulse()
{
	if (!fRunning)
		return;

	UpdateDisplay(Elapsed());
}

		
void TimerView::UpdateDisplay(int seconds)
{
	int mins = seconds / 60;
	int secs = seconds % 60;

	BString str;
	str.SetToFormat("%02d:%02d",mins,secs);

	Delete(0,TextLength());
	Insert(str);
}
