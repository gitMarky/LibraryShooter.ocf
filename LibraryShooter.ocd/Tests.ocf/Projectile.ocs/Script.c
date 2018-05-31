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
		test.initialized = true;
		CreateProjectile();
	}
	return true;
}

global func CreateProjectile()
{
	if (CurrentTest().projectile)
	{
		CurrentTest().projectile->RemoveObject();
	}

	CurrentTest().projectile = CreateObject(Bullet, 10, 10, NO_OWNER);
	CurrentTest().projectile->Velocity(1000)->Range(1000)->Shooter(CurrentTest().user);

	return CurrentTest().projectile;
}


// --------------------------------------------------------------------------------------------------------

global func Test1_OnStart()
{
	Log("Test for projectile: The launch angle without precision or deviation is correct");
	return Test_Init();
}

global func Test1_Execute()
{
	var no_deviation = nil;

	var precision = 1;

	for (var range in [100, 500, 1000])
	{
		for (var angle = 0; angle < 360; angle +=5)
		{
			var projectile = CreateProjectile();

			projectile->Launch(angle, no_deviation);

			var actual_angle = Angle(0, 0, projectile->GetXDir(), projectile->GetYDir(), precision);
			var range = 1000;
			
			var x_actual = +Sin(actual_angle, range, precision);
			var y_actual = -Cos(actual_angle, range, precision);
			
			var x_expected = +Sin(angle * precision, range, precision);
			var y_expected = -Cos(angle * precision, range, precision);
			
			var dist = Distance(x_actual, y_actual, x_expected, y_expected);
			
			Log("Launched projectile at angle %d, final angle is %d; deviation from target would be %d", angle * precision, actual_angle, dist);

			doTest("Deviation from target is <= 3 pixels.", dist <= 3, true);
		}
	}
	
	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------

global func Test2_OnStart()
{
	Log("Test for projectile: Launch angles without deviation");
	return Test_Init();
}

global func Test2_Execute()
{
	var no_deviation = nil;

	var projectile = CreateProjectile();

	for (var precision in [100, 500, 1000])
	{
		for (var angle = 0; angle < 360; angle +=5)
		{

			var launch_angle = projectile->GetLaunchAngle(angle, precision, no_deviation);

			var expected_angle = angle * precision;

			doTest("Launch angle is %d, expected %d.", launch_angle, expected_angle);
		}
	}
	
	if (projectile) projectile->RemoveObject();

	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------


global func Test3_OnStart()
{
	Log("Test for projectile: Launch angles without deviation");
	return Test_Init();
}

global func Test3_Execute()
{
	var projectile = CreateProjectile();

	for (var precision in [100, 1000, 10000])
	{
		for (var angle in [0, 50])
		{
			Log("Testing precision %d, angle %d", precision, angle);
			var deviation = Projectile_Deviation(5, precision);

			
			var max_launch_angle = angle * precision;
			var min_launch_angle = angle * precision;

			var expected_max = angle * precision + 5;
			var expected_min = angle * precision - 5;

			for (var i = 0; i < 1000; ++i)
			{
				var launch_angle = projectile->GetLaunchAngle(angle, precision, deviation);
				
				if (launch_angle < min_launch_angle) min_launch_angle = launch_angle;
				if (launch_angle > max_launch_angle) max_launch_angle = launch_angle;
			}

			doTest("Min launch angle is %d, expected %d.", min_launch_angle, expected_min);
			doTest("Max launch angle is %d, expected %d.", max_launch_angle, expected_max);
		}
	}

	if (projectile) projectile->RemoveObject();

	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------


global func Test4_OnStart()
{
	Log("Test for projectile: Hit scan weapons are ranged");
	return Test_Init();
}

global func Test4_Execute()
{
	var no_deviation = nil;

	var precision = 1;

	for (var range in [50, 100, 180])
	{
		for (var angle in [0, 30, 55, 90])
		{
			var projectile = CreateProjectile();
			projectile->Range(range);
			projectile->HitScan();
			projectile.OnHitScan = Global.Test4_OnHitScan;

			projectile->SetPosition(CurrentTest().user->GetX(), CurrentTest().user->GetY());

			projectile->Launch(angle, no_deviation);

			var x_expected = CurrentTest().user->GetX() + Sin(angle * precision, range, precision);
			var y_expected = CurrentTest().user->GetY() - Cos(angle * precision, range, precision);

			var dist = Distance(CurrentTest().test4_x, CurrentTest().test4_y, x_expected, y_expected);

			Log("Launched projectile at angle %d, expected range %d, final range is %d; deviation from target would be %d", angle * precision, range, CurrentTest().test4_range, dist);

			doTest("Deviation from target is <= 3 pixels.", dist <= 3, true);
		}
	}

	return Evaluate();
}


global func Test4_OnHitScan(int x_start, int y_start, int x_end, int y_end)
{
	Log("Hit scan %d %d %d %d", x_start, y_start, x_end, y_end);
	CurrentTest().test4_x = x_end;
	CurrentTest().test4_y = y_end;
	CurrentTest().test4_range = Distance(x_start, y_start, x_end, y_end);
}
