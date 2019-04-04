
func Initialize()
{
	ClearFreeRect(0, 0, LandscapeWidth(), 180);
	DrawMaterialQuad("Brick", 0, 160, LandscapeWidth(), 160, LandscapeWidth(), LandscapeHeight(), 0, LandscapeHeight());
	DrawMaterialQuad("Brick", 600, 160, LandscapeWidth(), 160, LandscapeWidth(), 0, 600, 0);
	DrawMaterialQuad("Brick", 0, 160, 100, 160, 100, 0, 0, 0);
}

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

global func Test_Init()
{
	var test = CurrentTest();

	// Remove all objects except the crew members
	for (var obj in FindObjects(Find_Not(Find_OCF(OCF_CrewMember))))
	{
		obj->RemoveObject();
	}
	if (test.target)
	{
		test.target->RemoveObject();
	}

	test.user = GetHiRank(test.player);
	test.user->SetPosition(LandscapeWidth() / 2, test.user->GetY());		
	test.target = CreateObject(Clonk, LandscapeWidth() / 4, test.user->GetY(), NO_OWNER);
	test.target->SetColor(RGB(255, 0, 255));
	test.weapon = test.user->CreateContents(Weapon);
	test.data = new DataContainer {};
	test.test_pressed = 0;
	test.test_started = false;
	return true;
}


global func PressControlUse(int hold_frames, object user, object weapon, int aim_x, int aim_y)
{
	user = user ?? CurrentTest().user;
	weapon = weapon ?? CurrentTest().weapon;
	aim_x = aim_x ?? 1000;
	aim_y = aim_y ?? -50;
	ScheduleCall(weapon, weapon.ControlUseStart, 1, nil, user, aim_x, aim_y);
	ScheduleCall(nil, Scenario.LogFrame, 1, nil, "Start firing");
	for (var delay = 2; delay < hold_frames; ++delay)
	{
		ScheduleCall(weapon, weapon.ControlUseHolding, delay, nil, user, aim_x, aim_y);
	}
	ScheduleCall(weapon, weapon.ControlUseStop, hold_frames, nil, user, aim_x, aim_y);
	ScheduleCall(nil, Scenario.LogFrame, hold_frames, nil, "Stop firing");
}

global func testFiredProjectiles(int expected_amount)
{
	return doTest("Weapon should fire %d projectiles, was %d.", expected_amount, CurrentTest().data.projectiles_fired);
}

global func DebugOnProgressCharge(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
{
	if (change_percent != 0)
	{
		Log("Charging: %d%%", current_percent);
	}
}

global func DebugOnRecovery(object user, proplist firemode)
{
	LogFrame("Finished recovery");
}

global func DebugOnStartCooldown(object user, proplist firemode)
{
	LogFrame("Start cooldown");
}
global func DebugOnFinishCooldown(object user, proplist firemode)
{
	LogFrame("Finished cooldown");
}

global func AlwaysTrue() { return true; }
global func AlwaysFalse() { return false; }

global func LogFrame(string message, ...)
{
	Log("[%06d] %s", FrameCounter(), Format(message, ...));
}

// --------------------------------------------------------------------------------------------------------
global func Test1_OnStart()
{
	Log("Test for Weapon: Aiming works correctly");
	return true;
}

global func Test1_Execute()
{
	Test_Init();

	var test_data = [[0, -1000],    // 0°, aim up
	                 [1000, -1000], // 45°
	                 [1000, 0],     // 90°
	                 [1000, 1000]]; // 135°


	for (var coordinates in test_data)
	{
		var aim_angle = CurrentTest().weapon->GetAngle(coordinates[0], coordinates[1]);
		var fire_angle = CurrentTest().weapon->GetFireAngle(coordinates[0], coordinates[1], CurrentTest().weapon->GetFiremode());

		var expected_aim_angle = Angle(0, 0, coordinates[0], coordinates[1]);
		var expected_fire_angle = Angle(0, -5, coordinates[0], coordinates[1]);

		doTest("Aiming angle should be %d, was %d", expected_aim_angle, aim_angle);
		doTest("Firing angle should be %d, was %d", expected_fire_angle, fire_angle);
	}

	return Evaluate();
}

// ########################################################################################################

// --------------------------------------------------------------------------------------------------------
global func Test2_OnStart()
{
	Log("Single fire mode:");
	Log("Only one bullet should be fired if the button is pressed longer than recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetRecoveryDelay(5);
	return true;
}

global func Test2_Execute()
{
	if (CurrentTest().test_started)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_started = true;
		var hold_button = 20;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test3_OnStart()
{
	Log("Single fire mode:");
	Log("Only one bullet should be fired if the button is pressed twice within recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetRecoveryDelay(50);
	return true;
}

global func Test3_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 10;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test4_OnStart()
{
	Log("Single fire mode:");
	Log("Two bullets should be fired if the button is pressed again after the recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetRecoveryDelay(30);
	return true;
}

global func Test4_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15;
		PressControlUse(hold_button);
		return Wait(hold_button + 20);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test5_OnStart()
{
	Log("Single fire mode:");
	Log("No bullet fired if button released before charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	return true;
}

global func Test5_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(0);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 20;
		PressControlUse(hold_button);
		return Wait(hold_button + 22);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test6_OnStart()
{
	Log("Single fire mode:");
	Log("One bullet fired if button released after charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	CurrentTest().test_pressed = false;
	return true;
}

global func Test6_Execute()
{
	if (CurrentTest().test_pressed)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed = true;
		var hold_button = 35;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test7_OnStart()
{
	Log("Single fire mode:");
	Log("One bullet fired if button released after charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	return true;
}

global func Test7_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 35;
		PressControlUse(hold_button);
		return Wait(hold_button + 10);
	}
}


// --------------------------------------------------------------------------------------------------------
global func Test8_OnStart()
{
	Log("Single fire mode:");
	Log("No additional bullet fired if button released before cooldown finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetCooldownDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test8_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 20; // 10 from recovery, then abort cooldown at 10/30
		PressControlUse(hold_button);
		return Wait(hold_button + 2); // Press again before cooldown should end
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test9_OnStart()
{
	Log("Single fire mode:");
	Log("Additional bullet fired if button released/pressed after cooldown finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetCooldownDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test9_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button); // Second bullet should fire, or third if the implementation is incorrect
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 45; // Recovery + cooldown, and enough time to fire an unwanted bullet (correct is: bullet fired only after releasing the trigger)
		PressControlUse(hold_button);
		return Wait(hold_button + 50); // Wait long enough for second cooldown to finish, if an unwanted bullet was fired
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test10_OnStart()
{
	Log("Single fire mode:");
	Log("Recovery is done, even if there is a cooldown");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Single)->SetCooldownDelay(30)->SetRecoveryDelay(20);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test10_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 35; // Enough to finish cooldown, but not enough to finish recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 2); // Press again before recovery/cooldown finishes
	}
}


// ########################################################################################################

// --------------------------------------------------------------------------------------------------------
global func Test11_OnStart()
{
	Log("Burst fire mode:");
	Log("Only the burst amount (1) should be fired if the button is pressed longer than recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetRecoveryDelay(5);
	return true;
}

global func Test11_Execute()
{
	if (CurrentTest().test_started)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_started = true;
		var hold_button = 20;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test12_OnStart()
{
	Log("Burst fire mode:");
	Log("Only burst amount (3) bullets should be fired if the button is pressed twice within recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetBurstAmount(3)->SetRecoveryDelay(20);
	CurrentTest().test_started = FrameCounter();
	return true;
}

global func Test12_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		// It takes some time to finish the burst, wait more than is necessary to catch faulty behaviour
		if (FrameCounter() > CurrentTest().test_started + 70)
		{
			testFiredProjectiles(3);
			return Evaluate();
		}
		else
		{
			return Wait();
		}
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test13_OnStart()
{
	Log("Burst fire mode:");
	Log("Two bullets should be fired if the button is pressed again after the recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetRecoveryDelay(30);
	return true;
}

global func Test13_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15;
		PressControlUse(hold_button);
		return Wait(hold_button + 20);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test14_OnStart()
{
	Log("Burst fire mode:");
	Log("No bullet fired if button released before charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	return true;
}

global func Test14_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(0);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 20;
		PressControlUse(hold_button);
		return Wait(hold_button + 22);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test15_OnStart()
{
	Log("Burst fire mode:");
	Log("One bullet fired if button released after charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	CurrentTest().test_pressed = false;
	return true;
}

global func Test15_Execute()
{
	if (CurrentTest().test_pressed)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed = true;
		var hold_button = 35;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test16_OnStart()
{
	Log("Burst fire mode:");
	Log("One bullet fired if button released after charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	return true;
}

global func Test16_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 35;
		PressControlUse(hold_button);
		return Wait(hold_button + 10);
	}
}


// --------------------------------------------------------------------------------------------------------
global func Test17_OnStart()
{
	Log("Burst fire mode:");
	Log("No additional bullet fired if button released before cooldown finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetCooldownDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test17_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 20; // 10 from recovery, then abort cooldown at 10/30
		PressControlUse(hold_button);
		return Wait(hold_button + 2); // Press again before cooldown should end
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test18_OnStart()
{
	Log("Burst fire mode:");
	Log("Additional bullet fired if button released/pressed after cooldown finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetCooldownDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test18_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button); // Second bullet should fire, or third if the implementation is incorrect
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 45; // Recovery + cooldown, and enough time to fire an unwanted bullet (correct is: bullet fired only after releasing the trigger)
		PressControlUse(hold_button);
		return Wait(hold_button + 50); // Wait long enough for second cooldown to finish, if an unwanted bullet was fired
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test19_OnStart()
{
	Log("Burst fire mode:");
	Log("Recovery is done, even if there is a cooldown");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Burst)->SetCooldownDelay(30)->SetRecoveryDelay(20);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test19_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 35; // Enough to finish cooldown, but not enough to finish recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 2); // Press again before recovery/cooldown finishes
	}
}


// ########################################################################################################

// --------------------------------------------------------------------------------------------------------
global func Test20_OnStart()
{
	Log("Auto fire mode:");
	Log("Bullets should be fired as long as the button is pressed, with recovery delay in between");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetRecoveryDelay(15);
	return true;
}

global func Test20_Execute()
{
	if (CurrentTest().test_started)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_started = true;
		var hold_button = 20;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test21_OnStart()
{
	Log("Auto fire mode:");
	Log("Only one bullet should be fired if the button is pressed twice within recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetRecoveryDelay(50);
	return true;
}

global func Test21_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 10;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test22_OnStart()
{
	Log("Auto fire mode:");
	Log("Two bullets should be fired if the button is pressed again after the recovery delay");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetRecoveryDelay(30);
	return true;
}

global func Test22_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15;
		PressControlUse(hold_button);
		return Wait(hold_button + 20);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test23_OnStart()
{
	Log("Auto fire mode:");
	Log("No bullet fired if button released before charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	return true;
}

global func Test23_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(0);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 20;
		PressControlUse(hold_button);
		return Wait(hold_button + 22);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test24_OnStart()
{
	Log("Auto fire mode:");
	Log("One bullet fired if button released after charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	CurrentTest().test_pressed = false;
	return true;
}

global func Test24_Execute()
{
	if (CurrentTest().test_pressed)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed = true;
		var hold_button = 35;
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test25_OnStart()
{
	Log("Auto fire mode:");
	Log("One bullet fired if button released after charge delay finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetChargeDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnProgressCharge = Global.DebugOnProgressCharge;
	return true;
}

global func Test25_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 35;
		PressControlUse(hold_button);
		return Wait(hold_button + 10);
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test26_OnStart()
{
	Log("Auto fire mode:");
	Log("Recovery is done and has higher priority than cooldown");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetCooldownDelay(30)->SetRecoveryDelay(20);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	CurrentTest().weapon.OnRecovery = Global.DebugOnRecovery;
	CurrentTest().weapon.OnStartCooldown = Global.DebugOnStartCooldown;
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	CurrentTest().test_started = FrameCounter();
	return true;
}

global func Test26_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		if (FrameCounter() > CurrentTest().test_started + 90) // Wait more than is necessary: 2x recovery delay + 1x cooldown delay
		{
			testFiredProjectiles(2);
			return Evaluate();
		}
		return Wait();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15; // Release within recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15; // Release within recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 2); // Press again before recovery finishes
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test27_OnStart()
{
	Log("Auto fire mode:");
	Log("Additional bullet fired if button pressed after cooldown finishes");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetCooldownDelay(30)->SetRecoveryDelay(10);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	return true;
}

global func Test27_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5;
		PressControlUse(hold_button); // Finish within recovery, so that cooldown can start
		return Wait(hold_button + 42);// Wait long enough for cooldown to finish
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 5; // Finish within recovery, so that cooldown can start
		PressControlUse(hold_button);
		return Wait(hold_button + 37); // Wait long enough for cooldown to finish
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test28_OnStart()
{
	Log("Auto fire mode:");
	Log("Recovery is done, cannot fire during cooldown (this is probably a duplicate test)");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetCooldownDelay(30)->SetRecoveryDelay(20);
	CurrentTest().weapon.OnRecovery = Global.DebugOnRecovery;
	CurrentTest().weapon.OnStartCooldown = Global.DebugOnStartCooldown;
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	CurrentTest().test_started = FrameCounter();
	return true;
}

global func Test28_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		if (FrameCounter() > CurrentTest().test_started + 90) // Wait more than is necessary: 2x recovery delay + 1x cooldown delay
		{
			testFiredProjectiles(1);
			return Evaluate();
		}
		return Wait();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15; // Release within recovery/cooldown
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15; // Release within recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 7); // Press again after recovery finishes
	}
}

// --------------------------------------------------------------------------------------------------------
global func Test29_OnStart()
{
	Log("Auto fire mode:");
	Log("Recovery is done, continue holding fire during cooldown, weapon starts firing after cooldown");
	Test_Init();
	CurrentTest().weapon->GetFiremode()->SetMode(WEAPON_FM_Auto)->SetCooldownDelay(30)->SetRecoveryDelay(20);
	CurrentTest().weapon.OnFinishCooldown = Global.DebugOnFinishCooldown;
	CurrentTest().test_started = FrameCounter();
	return true;
}

global func Test29_Execute()
{
	if (CurrentTest().test_pressed >= 2)
	{
		if (FrameCounter() > CurrentTest().test_started + 90) // Wait more than is necessary: 2x recovery delay + 1x cooldown delay
		{
			testFiredProjectiles(2);
			return Evaluate();
		}
		return Wait();
	}
	else if (CurrentTest().test_pressed == 1)
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 40; // Release after cooldown, but within recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 2);
	}
	else
	{
		CurrentTest().test_pressed += 1;
		var hold_button = 15; // Release within recovery
		PressControlUse(hold_button);
		return Wait(hold_button + 7); // Press again after recovery finishes
	}
}
