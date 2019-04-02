
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


global func DoAmmoTest(object manager, id ammo, int amount_initial, int set_ammo, int set_result, int amount_after_set, int do_ammo, int do_result, int amount_after_do)
{
	var expect_int = "should be %03d, is %03d";
	doTest(Format("GetAmmo(%i)     - %s", ammo,           expect_int), amount_initial,   manager->GetAmmo(ammo));
	doTest(Format("SetAmmo(%i, %d) - %s", ammo, set_ammo, expect_int), set_result,       manager->SetAmmo(ammo, set_ammo));
	doTest(Format("GetAmmo(%i)     - %s", ammo,           expect_int), amount_after_set, manager->GetAmmo(ammo));
	doTest(Format("DoAmmo(%i, %d)  - %s", ammo, do_ammo,  expect_int), do_result,        manager->DoAmmo(ammo, do_ammo));
	doTest(Format("GetAmmo(%i)     - %s", ammo,           expect_int), amount_after_do,  manager->GetAmmo(ammo));
}


/* --- The actual tests --- */


// --------------------------------------------------------------------------------------------------------

global func Test1_OnStart()
{
	Log("Ammo Manager with infinite ammo source");
	return true;
}

global func Test1_Execute()
{
	var manager = CreateObject(Test_AmmoManager, 0, 0, NO_OWNER);
	manager.Test_AmmoSource = AMMO_Source_Infinite;

	// Tests inside bounds
	// For ammo without limits the ammo manager starts with 1

	DoAmmoTest(manager, Rock, 1, 10, 10, 1, +5, +5, 1); // Set to 10, do +5 = 15
	DoAmmoTest(manager, Wood, 1, 20, 20, 1, -7, -7, 1); // Set to 20, do -7 = 13

	// Test outside bounds
	// For ammo with limits the ammo manager starts at the limit
	// Takes as much ammo as is requested
	var exceeding_limit = TEST_AMMO_LIMIT + 1;
	
	DoAmmoTest(manager, Test_AmmoLimited, TEST_AMMO_LIMIT, exceeding_limit, exceeding_limit, TEST_AMMO_LIMIT, -exceeding_limit, -exceeding_limit, TEST_AMMO_LIMIT);

	return Evaluate();
}

// --------------------------------------------------------------------------------------------------------

global func Test2_OnStart()
{
	Log("Ammo Manager with local ammo source");
	return true;
}

global func Test2_Execute()
{
	var manager = CreateObject(Test_AmmoManager, 0, 0, NO_OWNER);
	manager.Test_AmmoSource = AMMO_Source_Local;

	// Tests inside bounds
	// For ammo without limits the ammo manager starts with 1

	DoAmmoTest(manager, Rock, 0, 10, 10, 10, +5, +5, 15); // Set to 10, do +5 = 15
	DoAmmoTest(manager, Wood, 0, 20, 20, 20, -7, -7, 13); // Set to 20, do -7 = 13

	// Test outside bounds
	// For ammo with limits the ammo manager starts at the limit
	// Takes as much ammo as is requested
	var exceeding_limit = TEST_AMMO_LIMIT + 1;
	
	DoAmmoTest(manager, Test_AmmoLimited, 0, exceeding_limit, TEST_AMMO_LIMIT, TEST_AMMO_LIMIT, -exceeding_limit, -TEST_AMMO_LIMIT, 0);

	return Evaluate();
}
