protected func InitializePlayer(int plr)
{
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(plr, 300, nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, plr);
		
	// Start!
	LaunchTest(1);
	return;
}

/*-- Test Control --*/

static const FX_TEST_CONTROL = "IntTestControl";

// Aborts the current test and launches the specified test instead.
global func LaunchTest(int nr)
{
	// Get the control effect.
	var effect = GetEffect(FX_TEST_CONTROL, nil);
	if (!effect)
	{
		// Create a new control effect and launch the test.
		effect = AddEffect(FX_TEST_CONTROL, nil, 100, 2);
		effect.test_number = nr;
		effect.launched = false;
		effect.player = GetPlayerByIndex(0, C4PT_User);
		effect.user = GetHiRank(effect.player);
		effect.user->SetPosition(LandscapeWidth() / 2, effect.user->GetY());		
		effect.target = CreateObject(Clonk, LandscapeWidth() / 4, effect.user->GetY(), NO_OWNER);
		effect.target->SetColor(RGB(255, 0, 255));
		CreateProjectile();
		return;
	}
	// Finish the currently running test.
	Call(Format("~Test%d_OnFinished", effect.test_number));
	// Start the requested test by just setting the test number and setting 
	// effect.launched to false, effect will handle the rest.
	effect.test_number = nr;
	effect.launched = false;
	return;
}

// Calling this function skips the current test, does not work if last test has been ran already.
global func SkipTest()
{
	Test().tests_skipped++;
	NextTest();
}

global func FailTest()
{
	Test().tests_failed++;
	NextTest();
}

global func NextTest()
{
	// Get the control effect.
	var effect = Test();
	if (!effect)
		return;
	// Finish the previous test.
	Call(Format("~Test%d_OnFinished", effect.test_number));
	// Start the next test by just increasing the test number and setting 
	// effect.launched to false, effect will handle the rest.
	effect.test_number++;
	effect.launched = false;
	return;
}

global func Test()
{
	// Get the control effect.
	var effect = GetEffect(FX_TEST_CONTROL, nil);
	return effect;
}


/*-- Test Effect --*/

global func FxIntTestControlStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Set default interval.
	effect.Interval = 2;
	return FX_OK;
}

global func FxIntTestControlTimer(object target, proplist effect)
{
	// Launch new test if needed.
	if (!effect.launched)
	{
		// Log test start.
		Log("=====================================");
		Log("Test %d started:", effect.test_number);
		// Start the test if available, otherwise finish test sequence.
		if (!Call(Format("~Test%d_OnStart", effect.test_number)))
		{
			Log("Test %d not available, the previous test was the last test.", effect.test_number);
			Log("=====================================");
			
			if (effect.tests_skipped <= 0 && effect.tests_failed <= 0)
			{
				Log("All tests have been successfully completed!");
			}
			else
			{
				Log("%d tests were skipped.", effect.tests_skipped);
				Log("%d tests failed.", effect.tests_failed);
			}
			return FX_Execute_Kill;
		}
		effect.launched = true;
	}		
	// Check whether the current test has been finished.
	if (Call(Format("Test%d_Completed", effect.test_number)))
	{
		effect.launched = false;
		// Call the test on finished function.
		Call(Format("~Test%d_OnFinished", effect.test_number));
		// Log result and increase test number.
		Log("Test %d successfully completed.", effect.test_number);
		effect.test_number++;
	}
	return FX_OK;
}

global func pass(string message)
{
	Log(Format("[Passed] %s", message));
}

global func fail(string message)
{
	Log(Format("[Failed] %s", message));
}

global func doTest(string message, actual, expected)
{
	var passed = actual == expected;
	
	var log_message = Format(message, actual, expected);
	
	if (passed)
		pass(log_message);
	else
		fail(log_message);
		
	return passed;
}

global func askUser(string message)
{
	return false;
}

/*-- The actual tests --*/

global func CreateProjectile()
{
	if (Test().projectile) Test().projectile->RemoveObject();

	Test().projectile = CreateObject(Bullet, 10, 10, NO_OWNER);

	Test().projectile->Velocity(1000)->Range(1000)->Shooter(Test().user);

	return Test().projectile;
}

// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

global func Test1_OnStart()
{
	Log("Test for projectile: The launch angle without precision or deviation is correct");
	return true;
}

global func Test1_Completed()
{
	var no_deviation = nil;

	var passed = true;
	
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

			passed &= doTest("Deviation from target is <= 3 pixels.", dist <= 3, true);
		}
	}
	
	if (!passed)
	{
		fail("The projectile angles were not correct");
	}
	else
	{
		pass("The projectile angles were correct");
	}

	return passed || FailTest();
}

global func Test1_OnFinished(){}

// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

global func Test2_OnStart()
{
	Log("Test for projectile: Launch angles without deviation");
	return true;
}

global func Test2_Completed()
{
	var no_deviation = nil;

	var passed = true;
	
	var projectile = CreateProjectile();

	for (var precision in [100, 500, 1000])
	{
		for (var angle = 0; angle < 360; angle +=5)
		{

			var launch_angle = projectile->GetLaunchAngle(angle, precision, no_deviation);

			var expected_angle = angle * precision;

			passed &= doTest("Launch angle is %d, expected %d.", launch_angle, expected_angle);
		}
	}
	
	if (projectile) projectile->RemoveObject();
	
	if (!passed)
	{
		fail("The projectile angles were not correct");
	}
	else
	{
		pass("The projectile angles were correct");
	}

	return passed || FailTest();
}

global func Test2_OnFinished(){}

// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

global func Test3_OnStart()
{
	Log("Test for projectile: Launch angles without deviation");
	return true;
}

global func Test3_Completed()
{
	var passed = true;
	
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

			passed &= doTest("Min launch angle is %d, expected %d.", min_launch_angle, expected_min);
			passed &= doTest("Max launch angle is %d, expected %d.", max_launch_angle, expected_max);
		}
	}

	if (projectile) projectile->RemoveObject();

	if (!passed)
	{
		fail("The projectile angles were not correct");
	}
	else
	{
		pass("The projectile angles were correct");
	}

	return passed || FailTest();
}

global func Test3_OnFinished(){}

// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

global func Test4_OnStart()
{
	Log("Test for projectile: Hit scan weapons are ranged");
	return true;
}

global func Test4_Completed()
{
	var no_deviation = nil;

	var passed = true;
	
	var precision = 1;

	for (var range in [50, 100, 180])
	{
		for (var angle in [0, 30, 55, 90])
		{
			var projectile = CreateProjectile();
			projectile->Range(range);
			projectile->HitScan();
			projectile.OnHitScan = Global.Test4_OnHitScan;

			projectile->SetPosition(Test().user->GetX(), Test().user->GetY());

			projectile->Launch(angle, no_deviation);

			var x_expected = Test().user->GetX() + Sin(angle * precision, range, precision);
			var y_expected = Test().user->GetY() - Cos(angle * precision, range, precision);

			var dist = Distance(Test().test4_x, Test().test4_y, x_expected, y_expected);

			Log("Launched projectile at angle %d, expected range %d, final range is %d; deviation from target would be %d", angle * precision, range, Test().test4_range, dist);

			passed &= doTest("Deviation from target is <= 3 pixels.", dist <= 3, true);
		}
	}
	
	if (!passed)
	{
		fail("The hiscan projectile ranges were not correct");
	}
	else
	{
		pass("The hitscan projectile ranges were correct");
	}

	return passed || FailTest();
}

global func Test4_OnFinished(){}

global func Test4_OnHitScan(int x_start, int y_start, int x_end, int y_end)
{
	Log("Hit scan %d %d %d %d", x_start, y_start, x_end, y_end);
	Test().test4_x = x_end;
	Test().test4_y = y_end;
	Test().test4_range = Distance(x_start, y_start, x_end, y_end);
}
