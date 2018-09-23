func InitializePlayer(int plr)
{
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(plr, 300, nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, plr);
		
	// Start!
	LaunchTest(1);
	return;
}


/* --- The actual tests --- */

global func Test_Init(id weapon_type)
{
	var test = CurrentTest();
	if (!test.initialized)
	{
		test.user = GetHiRank(test.player);
		test.user->SetPosition(LandscapeWidth() / 2, test.user->GetY());		
		test.target = CreateObject(Clonk, LandscapeWidth() / 4, test.user->GetY(), NO_OWNER);
		test.target->SetColor(RGB(255, 0, 255));
		test.weapon = test.user->CreateContents(weapon_type);
		test.initialized = true;
	}
}


// --------------------------------------------------------------------------------------------------------


global func Test1_OnStart()
{
	Log("Test for reloading: Reloading with container plugin, default settings");
	return true;
}

global func Test1_Execute()
{
	Test_Init(Realod_Container_Default);
	
	if (CurrentTest().loading)
	{
		//Wait(100);
	}
	else
	{
		CurrentTest().weapon->StartReload(CurrentTest().user, 100, 0, true);
		CurrentTest().loading = true;
	}

	return Wait(100);

	//return Evaluate();
}
