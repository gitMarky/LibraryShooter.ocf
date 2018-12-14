
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
	if (!test.initialized)
	{
		test.user = GetHiRank(test.player);
		test.user->SetPosition(LandscapeWidth() / 2, test.user->GetY());		
		test.target = CreateObject(Clonk, LandscapeWidth() / 4, test.user->GetY(), NO_OWNER);
		test.target->SetColor(RGB(255, 0, 255));
		test.weapon = test.user->CreateContents(Weapon);
		test.initialized = true;
	}
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

		doTest("Aiming angle is %d, should be %d", aim_angle, expected_aim_angle);
		doTest("Firing angle is %d, should be %d", fire_angle, expected_fire_angle);
	}

	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------
global func Test2_OnStart()
{
	Log("Test for Weapon: Fire a bullet in single fire mode");
	Test_Init();
	return true;
}

global func Test2_Execute()
{
	if (CurrentTest().test2_started)
	{
		return Evaluate();
	}
	else
	{
		CurrentTest().test2_started = true;
		var delay = 10;
		var user = CurrentTest().user;
		var weapon = CurrentTest().weapon;
		var aim_x = 1000;
		var aim_y = -50;
		PressControlUse(delay, user, weapon, aim_x, aim_y);
		return Wait(delay + 2);
	}
}


global func PressControlUse(int hold_frames, object user, object weapon, int aim_x, int aim_y)
{
	ScheduleCall(weapon, weapon.ControlUseStart, 1, nil, user, aim_x, aim_y);
	for (var delay = 2; delay < hold_frames; ++delay)
	{
		ScheduleCall(weapon, weapon.ControlUseHolding, delay, nil, user, aim_x, aim_y);
	}
	ScheduleCall(weapon, weapon.ControlUseStop, hold_frames, nil, user, aim_x, aim_y);
}