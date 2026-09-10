#ifndef TIMERVIEW_H
#define TIMERVIEW_H

#include <TextView.h>

class TimerView : public BTextView {
public:
	TimerView();

	void Start();
	void Stop();
	bool Running();

	int Elapsed();

	virtual void Pulse();
	virtual void AttachedToWindow();

private:
	void UpdateDisplay(int seconds);

	bool fRunning;
	bigtime_t fStartTime;
	bigtime_t fStopTime;
};

#endif
