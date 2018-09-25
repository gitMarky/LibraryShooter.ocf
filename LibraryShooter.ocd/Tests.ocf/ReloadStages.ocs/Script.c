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
		test.weapon = test.user->CreateContents(weapon_type);
		test.initialized = true;
		test.loading = false;
		test.states = [];
	}
}

global func Test_Reset()
{
	CurrentTest().initialized = false;
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
	                "Reload_Container_InsertAmmo",
	                "Reload_Container_Close",
	                "Reload_Container_ReadyWeapon",
	                nil];

	if (CurrentTest().loading)
	{
		// Check the transitions
		var num_e = GetLength(expected);
		var num_a = GetLength(CurrentTest().states);
		doTest("Amount of state transitions was %d, should be %d", num_a, num_e);
		for (var i = 0; i < Max(num_e, num_a); ++i)
		{
			doTest("State was '%s', should be '%s'", CurrentTest().states[i], expected[i]);
		}

		// Check the remaining ammo
		var user_final_ammo = 4;
		var weapon_final_ammo = 10;

		doTest("Final user ammo was %d, should be %d", CurrentTest().user->GetAmmo(Dummy), user_final_ammo);
		doTest("Final weapon ammo was %d, should be %d", CurrentTest().weapon->GetAmmo(Dummy), weapon_final_ammo);

		Test_Reset();
		return Evaluate();
	}
	else
	{
		// Setup 
		var user_initial_ammo = 14;
		var weapon_initial_ammo = 0;
		CurrentTest().user.test_ammo_source = AMMO_Source_Local;
		CurrentTest().user->SetAmmo(Dummy, user_initial_ammo);
		CurrentTest().weapon->SetAmmo(Dummy, weapon_initial_ammo);

		// Sanity check
		doTest("Initial user ammo was %d, should be %d", CurrentTest().user->GetAmmo(Dummy), user_initial_ammo);
		doTest("Initial weapon ammo was %d, should be %d", CurrentTest().weapon->GetAmmo(Dummy), weapon_initial_ammo);

		// Do it!
		CurrentTest().weapon->StartReload(CurrentTest().user, 100, 0, true);
		CurrentTest().loading = true;
		return Wait(20);
	}
}


global func Test2_OnStart()
{
	Log("Test for reloading: Reloading with container plugin, default settings");
	return true;
}

global func Test2_Execute()
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
		// Check the transitions
		var num_e = GetLength(expected);
		var num_a = GetLength(CurrentTest().states);
		doTest("Amount of state transitions was %d, should be %d", num_a, num_e);
		for (var i = 0; i < Max(num_e, num_a); ++i)
		{
			doTest("State was '%s', should be '%s'", CurrentTest().states[i], expected[i]);
		}

		// Check the remaining ammo
		var user_final_ammo = 9;
		var weapon_final_ammo = 10;

		doTest("Final user ammo was %d, should be %d", CurrentTest().user->GetAmmo(Dummy), user_final_ammo);
		doTest("Final weapon ammo was %d, should be %d", CurrentTest().weapon->GetAmmo(Dummy), weapon_final_ammo);

		Test_Reset();
		return Evaluate();
	}
	else
	{
		// Setup 
		var user_initial_ammo = 14;
		var weapon_initial_ammo = 5;
		CurrentTest().user.test_ammo_source = AMMO_Source_Local;
		CurrentTest().user->SetAmmo(Dummy, user_initial_ammo);
		CurrentTest().weapon->SetAmmo(Dummy, weapon_initial_ammo);

		// Sanity check
		doTest("Initial user ammo was %d, should be %d", CurrentTest().user->GetAmmo(Dummy), user_initial_ammo);
		doTest("Initial weapon ammo was %d, should be %d", CurrentTest().weapon->GetAmmo(Dummy), weapon_initial_ammo);

		// Do it!
		CurrentTest().weapon->StartReload(CurrentTest().user, 100, 0, true);
		CurrentTest().loading = true;
		return Wait(20);
	}
}
