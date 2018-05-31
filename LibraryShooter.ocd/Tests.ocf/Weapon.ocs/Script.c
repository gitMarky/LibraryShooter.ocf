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
