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

global func Test_Init(string message, id weapon_type)
{
	var test = CurrentTest();
	if (!test.initialized)
	{
		Log(message);
		test.user = GetHiRank(test.player);
		test.user->SetPosition(LandscapeWidth() / 2, test.user->GetY());		
		test.weapon = test.user->CreateContents(weapon_type);
		test.initialized = true;
		test.loading = false;
		test.states = [];
	}
	return true;
}

global func Test_Reset()
{
	CurrentTest().initialized = false;
}

global func Test_ReloadSequence(int user_initial_ammo, int weapon_initial_ammo, int user_final_ammo, int weapon_final_ammo, array expected)
{
	if (CurrentTest().loading)
	{
		doTestTransitions(expected); // Check the transitions
		doTestAmmo("Final", user_final_ammo, weapon_final_ammo); // Check the remaining ammo
		Test_Reset();
		return Evaluate();
	}
	else
	{
		SetupAmmo(user_initial_ammo, weapon_initial_ammo);

		// Do it!
		CurrentTest().weapon->StartReload(CurrentTest().user, 100, 0, true);
		CurrentTest().loading = true;
		return Wait(20);
	}
}


global func SetupAmmo(int user_initial_ammo, int weapon_initial_ammo)
{
		// Setup 
		CurrentTest().user.test_ammo_source = AMMO_Source_Local;
		CurrentTest().user->SetAmmo(Dummy, user_initial_ammo);
		CurrentTest().weapon->SetAmmo(Dummy, weapon_initial_ammo);

		// Sanity check
		doTest("Initial user ammo was %d, should be %d", CurrentTest().user->GetAmmo(Dummy), user_initial_ammo);
		doTest("Initial weapon ammo was %d, should be %d", CurrentTest().weapon->GetAmmo(Dummy), weapon_initial_ammo);
}

global func doTestAmmo(string description, int user_ammo, int weapon_ammo)
{
		doTest(Format("%s user ammo was %%d, should be %%d", description), CurrentTest().user->GetAmmo(Dummy), user_ammo);
		doTest(Format("%s weapon ammo was %%d, should be %%d", description), CurrentTest().weapon->GetAmmo(Dummy), weapon_ammo);
}

global func doTestTransitions(array expected)
{
	var num_e = GetLength(expected);
	var num_a = GetLength(CurrentTest().states);
	doTest("Amount of state transitions was %d, should be %d", num_a, num_e);
	for (var i = 0; i < Max(num_e, num_a); ++i)
	{
		doTest("State was '%s', should be '%s'", CurrentTest().states[i], expected[i]);
	}
}

// --------------------------------------------------------------------------------------------------------


global func Test1_OnStart()
{
	return Test_Init("Test for reloading: Reloading with container plugin, default settings", Realod_Container_Default);
}
global func Test1_Execute()
{
	return Test_ReloadSequence(14,  0, 
	                            4, 10,
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test2_OnStart()
{
	return Test_Init("Test for reloading: Reloading with container plugin, default settings", Realod_Container_Default);
}
global func Test2_Execute()
{
	return Test_ReloadSequence(14,  5, 
	                            9, 10,
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_StashStart}",
	                            "Reload_Container_StashFinish",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}
