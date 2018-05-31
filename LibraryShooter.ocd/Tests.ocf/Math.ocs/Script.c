
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


// --------------------------------------------------------------------------------------------------------

global func Test1_OnStart()
{
	Log("Test for math function: GetExponent");
	return true;
}

global func Test1_Execute()
{
	var bases = [2, 3, 5, 7, 10];

	for (var base in bases)
	{
		for (var exponent = 0; exponent <= 5; ++exponent)
		{
			var value = base ** exponent;
			Log("Testing %d ^ %d = %d", base, exponent, value);
			doTest("Exponent is %d, should be %d", GetExponent(value, base), exponent);
		}
	}
	
	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------

global func Test2_OnStart()
{
	Log("Test for math function, projectile deviations: Projectile_Deviation");
	return true;
}

global func Test2_Execute()
{	
	doTest("Projectile_Deviation has the correct angle. Got %d, expected %d.", Projectile_Deviation(4, 100)->GetValue(), 4);
	doTest("Projectile_Deviation has the correct precision. Got %d, expected %d.", Projectile_Deviation(4, 100)->GetPrecision(), 100);
	doTest("Projectile_Deviation keeps arrays as angle. Got %v, expected %v.", Projectile_Deviation([1, 2, 3], 10)->GetValue(), [1, 2, 3]);

	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------


global func Test3_OnStart()
{
	Log("Test for math function, projectile deviations: NormalizeDeviations, with comfortable values");
	return true;
}

global func Test3_Execute()
{
	var deviation1 = Projectile_Deviation(1, 1);
	var deviation2 = Projectile_Deviation(2, 10);
	var deviation3 = Projectile_Deviation(3, 100);
	var deviation4 = Projectile_Deviation(4, 1000);
	
	var deviation_normalized = NormalizeDeviations([deviation1, deviation2, deviation3, deviation4]);
	
	doTest("Normalizing deviations uses the maximum precision. Got %d, expected %d.", deviation_normalized->GetPrecision(), 1000);
	doTest("Normalizing deviations scales the angles correctly. Got %v, expected %v.", deviation_normalized->GetValue(), [1000, 200, 30, 4]);

	return Evaluate();
}


// --------------------------------------------------------------------------------------------------------


global func Test4_OnStart()
{
	Log("Test for math function, projectile deviations: NormalizeDeviations, with uncomfortable values");
	return true;
}

global func Test4_Execute()
{
	var deviation1 = Projectile_Deviation(1, 3);
	var deviation2 = Projectile_Deviation(2, 17);
	var deviation3 = Projectile_Deviation(3, 31);
	var deviation4 = Projectile_Deviation(4, 5);
	
	var deviation_normalized = NormalizeDeviations([deviation1, deviation2, deviation3, deviation4]);
	
	doTest("Normalizing deviations uses the maximum precision. Got %d, expected %d.", deviation_normalized->GetPrecision(), 31);
	doTest("Normalizing deviations scales the angles correctly. Got %v, expected %v.", deviation_normalized->GetValue(), [10, 3, 3, 24]);

	return Evaluate();
}

// --------------------------------------------------------------------------------------------------------


/**
 Gets the exponent of a value.

	@par value The value.

	@par base The base.

	@return int The exponent, so that {@code value = base ** exponent}.
 */
global func GetExponent(int value, int base)
{
	var exponent = 0;
	
	base = base ?? 10;
	
	for (var test = value; test%base == 0; test = test/base)
	{
		exponent += 1;
	}
	
	return exponent;
}
