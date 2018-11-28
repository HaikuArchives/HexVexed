#include "TimerView.h"

#include <String.h>

TimerView::TimerView()
	:
	BTextView("timer"),
	fRunning(false),
	elapsedTime(0)
{
	SetStylable(true);
	MakeEditable(false);
	MakeSelectable(false);
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
	elapsedTime = Elapsed();
}

int TimerView::Elapsed()
{
	return (system_time() - fStartTime) / 1000 / 1000;
}

void TimerView::Pulse()
{
	bigtime_t elapsed = fRunning ? Elapsed() : elapsedTime;
	int mins = elapsed / 60;
	int secs = elapsed % 60;

	BString str;
	str.SetToFormat("%02d:%02d",mins,secs);

	Delete(0,TextLength());
	Insert(str);
}
