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

global func Test_ReloadSequence(int weapon_initial_ammo, int weapon_final_ammo, int user_initial_ammo, int user_final_ammo, array expected, string initial_state)
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
		
		if (initial_state)
		{
			CurrentTest().weapon->SetReloadState(CurrentTest().weapon->GetFiremode(), CurrentTest().weapon[initial_state]);
		}

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
// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

// --- Container, default


global func Test1_OnStart()
{
	return Test_Init("Reload an empty weapon [Reloading with container plugin, default settings]", Reload_Container_Default);
}
global func Test1_Execute()
{
	return Test_ReloadSequence( 0, 10,  // Weapon ammo changes 
	                           14,  4,  // User ammo changes
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test2_OnStart()
{
	return Test_Init("Reload a partially filled weapon [Reloading with container plugin, default settings]", Reload_Container_Default);
}
global func Test2_Execute()
{
	return Test_ReloadSequence( 5, 10,  // Weapon ammo changes
	                           14,  9,  // User ammo changes
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_StashStart}",
	                            "Reload_Container_StashFinish",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test3_OnStart()
{
	return Test_Init("Reload a full weapon [Reloading with container plugin, default settings]", Reload_Container_Default);
}
global func Test3_Execute()
{
	return Test_ReloadSequence(10, 10,  // Weapon ammo changes
	                           14, 14,  // User ammo changes
	                           [nil]);  // Does not do a useless reload
}

global func Test4_OnStart()
{
	return Test_Init("Reload an empty weapon from non-default state [Reloading with container plugin, default settings]", Reload_Container_Default);
}
global func Test4_Execute()
{
	return Test_ReloadSequence( 0, 10,  // Weapon ammo changes 
	                           14,  4,  // User ammo changes
	                           ["Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil],
	                            "Reload_Container_InsertAmmo");
}

// --- Container, with ammo chamber


global func Test5_OnStart()
{
	return Test_Init("Reload an empty weapon [Reloading with container plugin, ammo chamber]", Reload_Container_AmmoChamber);
}
global func Test5_Execute()
{
	return Test_ReloadSequence( 0, 10,  // Weapon ammo changes 
	                           14,  4,  // User ammo changes
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_LoadAmmoChamber",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test6_OnStart()
{
	return Test_Init("Reload a partially filled weapon [Reloading with container plugin, ammo chamber]", Reload_Container_AmmoChamber);
}
global func Test6_Execute()
{
	return Test_ReloadSequence( 5, 11,  // Weapon ammo changes
	                           14,  8,  // User ammo changes
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_StashStart}",
	                            "Reload_Container_StashFinish",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test7_OnStart()
{
	return Test_Init("Reload a partially filled weapon [Reloading with container plugin, ammo chamber]", Reload_Container_AmmoChamber);
}
global func Test7_Execute()
{
	return Test_ReloadSequence( 5, 11,  // Weapon ammo changes
	                           14,  8,  // User ammo changes
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_StashStart}",
	                            "Reload_Container_StashFinish",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test8_OnStart()
{
	return Test_Init("Reload a full weapon [Reloading with container plugin, ammo chamber]", Reload_Container_AmmoChamber);
}
global func Test8_Execute()
{
	return Test_ReloadSequence(10, 11,  // Weapon ammo changes
	                           14, 13,  // User ammo changes
	                           ["Reload_Container_Prepare",
	                            "Reload_Container_EjectAmmo",
	                            "Reload_Container_StashStart}",
	                            "Reload_Container_StashFinish",
	                            "Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_ReadyWeapon",
	                            nil]);
}

global func Test9_OnStart()
{
	return Test_Init("Reload an empty weapon from non-default state [Reloading with container plugin, ammo chamber]", Reload_Container_AmmoChamber);
}
global func Test9_Execute()
{
	return Test_ReloadSequence( 0, 10,  // Weapon ammo changes 
	                           14,  4,  // User ammo changes
	                           ["Reload_Container_InsertAmmo",
	                            "Reload_Container_Close",
	                            "Reload_Container_LoadAmmoChamber",
	                            "Reload_Container_ReadyWeapon",
	                            nil],
	                            "Reload_Container_InsertAmmo");
}

global func Test10_OnStart()
{
	return Test_Init("Reload a full weapon with ammo chamber not loaded [Reloading with container plugin, ammo chamber]", Reload_Container_AmmoChamber);
}
global func Test10_Execute()
{
	return Test_ReloadSequence(10, 10,  // Weapon ammo changes 
	                           14,  14,  // User ammo changes
	                           ["Reload_Container_LoadAmmoChamber",
	                            nil],
	                            nil);
}
