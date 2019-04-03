
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
	return true;
}


global func PressControlUse(int hold_frames, object user, object weapon, int aim_x, int aim_y)
{
	user = user ?? CurrentTest().user;
	weapon = weapon ?? CurrentTest().weapon;
	aim_x = aim_x ?? 1000;
	aim_y = aim_y ?? -50;
	ScheduleCall(weapon, weapon.ControlUseStart, 1, nil, user, aim_x, aim_y);
	for (var delay = 2; delay < hold_frames; ++delay)
	{
		ScheduleCall(weapon, weapon.ControlUseHolding, delay, nil, user, aim_x, aim_y);
	}
	ScheduleCall(weapon, weapon.ControlUseStop, hold_frames, nil, user, aim_x, aim_y);
}

global func testFiredProjectiles(int expected_amount)
{
	return doTest("Weapon should fire %d projectiles, was %d.", expected_amount, CurrentTest().data.projectiles_fired);
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
	if (CurrentTest().test2_started)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test2_started = true;
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
	CurrentTest().test3_pressed = 0;
	return true;
}

global func Test3_Execute()
{
	if (CurrentTest().test3_pressed >= 2)
	{
		testFiredProjectiles(1);
		return Evaluate();
	}
	else
	{
		CurrentTest().test3_pressed += 1;
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
	CurrentTest().test3_pressed = 0;
	return true;
}

global func Test4_Execute()
{
	if (CurrentTest().test3_pressed >= 2)
	{
		testFiredProjectiles(2);
		return Evaluate();
	}
	else
	{
		CurrentTest().test3_pressed += 1;
		var hold_button = 15;
		PressControlUse(hold_button);
		return Wait(hold_button + 20);
	}
}
