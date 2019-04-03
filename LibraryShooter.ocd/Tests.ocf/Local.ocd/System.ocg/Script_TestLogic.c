
/* --- Test Control --- */

// Aborts the current test and launches the specified test instead.
global func LaunchTest(int nr)
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the currently running test.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Start the requested test by just setting the test number and setting 
		// test.launched to false, effect will handle the rest.
	}
	else
	{
		// Create a new control effect and launch the test.
		test = CurrentTest(true);
		test.player = GetPlayerByIndex(0, C4PT_User);
		test.global_result = true;
		test.count_total = 0;
		test.count_failed = 0;
		test.count_skipped = 0;
	}

	test.testnr = nr;
	test.launched = false;
}


// Calling this function skips the current test, does not work if last test has been ran already.
global func SkipTest()
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the previous test.
		Call(Format("~Test%d_OnFinished", test.testnr));
		// Start the next test by just increasing the test number and setting 
		// test.launched to false, effect will handle the rest.
		test.testnr++;
		test.launched = false;
		test.count_skipped++;
	}
}


/* --- Test Effect --- */

static const IntTestControl = new Effect
{
	Start = func (int temporary)
	{
		if (!temporary)
		{
			// Set default interval.
			this.Interval = 1;
		}
		return FX_OK;
	},

	Timer = func ()
	{
		// Launch new test if needed.
		if (!this.launched)
		{
			// Start the test if available, otherwise finish test sequence.
			if (!this->HasNextTest())
			{
				Log("Test %d not available, the previous test was the last test.", this.testnr);
				Log("=====================================");
				Log("All tests have been completed!");
				Log("* %d tests total", this.count_total);
				Log("%d tests failed", this.count_failed);
				Log("%d tests skipped", this.count_skipped);
				Log("=====================================");
				if (this.count_skipped == 0 && this.count_failed == 0 && this.count_total > 0)
				{
					Log("All tests passed!");
				}
				else
				{
					Log("At least one test failed or was skipped!");
				}
				return FX_Execute_Kill;
			}
			// Log test start.
			Log("=====================================");
			Log("Test %d started:", this.testnr);
			this.launched = true;
			this.count_total++;
			this.current_result = false;
			this.current_check = true;
		}

		// waiting
		if (this.wait > 0)
		{
			this.wait -= 1;
			return FX_OK;
		}

		// Check whether the current test has been finished.
		if (this->ExecuteTest())
		{
			this.launched = false;

			if (this.current_result)
			{
				Log(">> Test %d passed.", this.testnr);
			}
			else
			{
				Log(">> Test %d failed.", this.testnr);
				this.count_failed++;
			}

			// Update global result
			this.global_result &= this.current_result;

			// Call the test on finished function.
			this->CleanupTest();
			// Log result and increase test number.
			Log("Test %d successfully completed.", this.testnr);
			this.testnr++;
		}
		return FX_OK;
	},


	GetIndex = func () // Get the index of the test
	{
		return this.testnr - 1;
	},

	GetNumber = func () // Get the test number
	{
		return this.testnr;
	},


	HasNextTest = func ()
	{
		return Call(Format("~Test%d_OnStart", this.testnr), this.player);
	},


	ExecuteTest = func ()
	{
		return Call(Format("Test%d_Execute", this.testnr));
	},


	CleanupTest = func ()
	{
		Call(Format("~Test%d_OnFinished", this.testnr));
	},
};


global func doTest(string description, expected, returned)
{
	var test;

	if (GetType(returned) == C4V_PropList || GetType(returned) == C4V_Array)
	{
		test = DeepEqual(returned, expected);
	}
	else
	{
		test = (returned == expected);
	}

	var predicate = "[Fail]";
	if (test) predicate = "[Pass]";

	Log(Format("%s %s", predicate, description), expected, returned);

	CurrentTest().current_check &= test;
	return test;
}


global func testCall(expected, function_name)
{
	return doTest(Format("%v->%v(%s) %s", this, function_name, FormatAllPars(...), "should be %v, is %v"), expected, Call(function_name, ...));
}


global func FormatAllPars(par0, par1, par2, par3, par4, par5, par6, par7, par8, par9)
{
	if (par1 || par2 || par3 || par4 || par5 || par6 || par7 || par8 || par9)
	{
		return Format("%v, %s", par0, FormatAllPars(par1, par2, par3, par4, par5, par6, par7, par8, par9));
	}
	else if (par0)
	{
		return Format("%v", par0);
	}
	else
	{
		return "";
	}
}


global func CurrentTest(bool create)
{
	if (create)
	{
		return Scenario->CreateEffect(IntTestControl, 100, 2);
	}
	else
	{
		return GetEffect("IntTestControl", Scenario);
	}
}

global func Evaluate()
{
	var test = CurrentTest();
	test.current_result = test.current_check;
	return true;
}


global func PassTest()
{
	CurrentTest().current_result = true;
	return true;
}


global func FailTest()
{
	CurrentTest().current_result = false;
	return true;
}


global func Wait(int amount)
{
	CurrentTest().wait = Max(0, amount);
	return false;
}
