#include "TimerView.h"

#include <String.h>

TimerView::TimerView()
	:
	BTextView("timer"),
	fRunning(false)
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
}

int TimerView::Elapsed()
{
	return (system_time() - fStartTime) / 1000 / 1000;
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
