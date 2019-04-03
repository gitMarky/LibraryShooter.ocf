
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
	manager->testCall(amount_initial, manager.GetAmmo, ammo);
	manager->testCall(set_result, manager.SetAmmo, ammo, set_ammo);
	manager->testCall(amount_after_set, manager.GetAmmo, ammo);
	manager->testCall(do_result, manager.DoAmmo, ammo, do_ammo);
	manager->testCall(amount_after_do, manager.GetAmmo, ammo);
}

global func DoAmmoTestContainer(object container, id ammo, int container_start, int container_end, object manager, int amount_initial, int change_ammo, int change_result, int amount_after_change, bool set_else_do)
{
    manager->testCall(amount_initial, manager.GetAmmo, ammo);
    container->testCall(container_start, container.GetAmmo, ammo);
	    
	if (set_else_do)
	{
	    manager->testCall(change_result, manager.SetAmmo, ammo, change_ammo);
	    manager->testCall(amount_after_change, manager.GetAmmo, ammo);
	}
	else
	{
	    manager->testCall(change_result, manager.DoAmmo, ammo, change_ammo);
	    manager->testCall(amount_after_change, manager.GetAmmo, ammo);
	}
    container->testCall(container_end, container.GetAmmo, ammo);
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

	DoAmmoTest(manager, Rock,  1, 
	                          10, 10, 1,  // Set to 10
	                           5,  5, 1); // Do +5 = 15
	DoAmmoTest(manager, Wood,  1,
	                          20, 20, 1,  // Set to 20
                              -7, -7, 1); // Do -7 = 13

	// Test outside bounds
	// For ammo with limits the ammo manager starts at the limit
	// Takes as much ammo as is requested
	
	DoAmmoTest(manager, Test_AmmoLimited, TEST_AMMO_LIMIT,
	                                      OVER_AMMO_LIMIT,  OVER_AMMO_LIMIT, TEST_AMMO_LIMIT,
	                                     -OVER_AMMO_LIMIT, -OVER_AMMO_LIMIT, TEST_AMMO_LIMIT);

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
	// For ammo without limits the ammo manager starts with 0

	DoAmmoTest(manager, Rock,  0, 
	                          10, 10, 10,  // Set to 10
	                           5,  5, 15); // Do +5 = 15
	DoAmmoTest(manager, Wood,  0,
	                           20, 20, 20,  // Set to 20
	                           -7, -7, 13); // Do -7 = 13

	// Test outside bounds
	// For ammo with limits the ammo manager starts at the limit
	// Takes as much ammo as is requested
	
	DoAmmoTest(manager, Test_AmmoLimited, 0,
	                                      OVER_AMMO_LIMIT,  TEST_AMMO_LIMIT, TEST_AMMO_LIMIT,
	                                     -OVER_AMMO_LIMIT, -TEST_AMMO_LIMIT, 0);

	return Evaluate();
}

// --------------------------------------------------------------------------------------------------------

global func Test3_OnStart()
{
	Log("Ammo Manager with container ammo source");
	return true;
}

global func Test3_Execute()
{
	// This one is a bit tricky:
	// Every manipulation that you do with the manager
	// is applied directly to the container

	var container = CreateObject(Test_AmmoManager, 0, 0, NO_OWNER);
	container.Test_AmmoSource = AMMO_Source_Local;
	container->SetAmmo(Rock, 11);
	container->SetAmmo(Wood, 0);
	container->SetAmmo(Test_AmmoLimited, UNDER_AMMO_LIMIT);

	var manager = CreateObject(Test_AmmoManager, 0, 0, NO_OWNER);
	manager.Test_AmmoSource = AMMO_Source_Container;
	manager.Test_AmmoContainer = container;

	// Tests inside bounds

	DoAmmoTestContainer(container, Rock, 11, 10, manager, 11, 10, 10, 10, true);  // Set to 10
	DoAmmoTestContainer(container, Rock, 10, 15, manager, 10,  5,  5, 15, false); // Do +5

	DoAmmoTestContainer(container, Wood,  0, 20, manager,  0, 20, 20, 20, true);  // Set to 20
	DoAmmoTestContainer(container, Wood, 20, 13, manager, 20, -7, -7, 13, false); // Do -7

	// Test outside bounds
	
	DoAmmoTestContainer(container, Test_AmmoLimited, UNDER_AMMO_LIMIT, TEST_AMMO_LIMIT, manager,  UNDER_AMMO_LIMIT, OVER_AMMO_LIMIT, TEST_AMMO_LIMIT, TEST_AMMO_LIMIT, true); // Exceed limit by set
	DoAmmoTestContainer(container, Test_AmmoLimited, TEST_AMMO_LIMIT, 0, manager, TEST_AMMO_LIMIT, -OVER_AMMO_LIMIT, -TEST_AMMO_LIMIT, 0, false); // Do -7

	return Evaluate();
}
