/*
 * Copyright 2009-2012 Scott McCreary
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */

#include <Application.h>
#include "MainWindow.h"
#include <OS.h>
#include <stdlib.h>

class App : public BApplication
{
public:
	App(void);
};

App::App(void)
 : BApplication("application/x-vnd.wgp-HexVexed")
{
	MainWindow *mainwin = new MainWindow();
	mainwin->Show();
}

int main(void)
{
	srand(system_time());
	App app;
	app.Run();
	return 0;
}
