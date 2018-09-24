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
		test.states = [];
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
	
	var expected = ["Reload_Container_Prepare",
	                "Reload_Container_EjectAmmo",
	                "Reload_Container_StashStart}",
	                "Reload_Container_StashFinish",
	                "Reload_Container_InsertAmmo",
	                "Reload_Container_Close",
	                "Reload_Container_ReadyWeapon",
	                nil];
	                
	if (CurrentTest().loading)
	{
		var num_e = GetLength(expected);
		var num_a = GetLength(CurrentTest().states);
		doTest("Amount of state transitions was %d, should be %d", num_a, num_e);
		for (var i = 0; i < Max(num_e, num_a); ++i)
		{
			doTest("State was '%s', should be '%s'", CurrentTest().states[i], expected[i]);
		}
		return Evaluate();
	}
	else
	{
		CurrentTest().weapon->StartReload(CurrentTest().user, 100, 0, true);
		CurrentTest().loading = true;
		return Wait(20);
	}
}
