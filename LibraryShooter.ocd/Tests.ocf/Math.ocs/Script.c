
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
