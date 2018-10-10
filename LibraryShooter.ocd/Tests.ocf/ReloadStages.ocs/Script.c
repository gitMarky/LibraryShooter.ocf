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

/* --- Modified test logic --- */

global func CurrentTest(bool create)
{
	if (create)
	{
		return Scenario->CreateEffect(IntReloadTestControl, 100, 2);
	}
	else
	{
		return GetEffect("IntReloadTestControl", Scenario);
	}
}


static const IntReloadTestControl = new IntTestControl
{
	HasNextTest = func ()
	{
		return !!Test_Data(this->GetIndex());
	},
	
	ExecuteTest = func ()
	{
		if (this.initialized)
		{
			return Test_ReloadSequence();
		}
		else
		{
			return Test_Init();
		}
	},

	CleanupTest = func ()
	{
		Test_Reset();
	},
};


/* --- The actual tests --- */

global func Test_Data(int index)
{
	return Scenario.test_scenarios[index];
}

global func Test_Init()
{
	var test = CurrentTest();
	var data = Test_Data(test->GetIndex());
	
	// Basic initialization

	Log("%s [%s]", data.Title, data.Item.Description);

	test.user = GetHiRank(test.player);
	test.user->SetPosition(LandscapeWidth() / 2, test.user->GetY());		
	test.weapon = test.user->CreateContents(data.Item);
	test.initialized = true;
	test.states = [];
	
	// Specifics
	
	SetupAmmo(data.User_Ammo.Initial, data.Weapon_Ammo.Initial);
	
	if (data.State_Initial)
	{
		test.weapon->SetReloadState(test.weapon->GetFiremode(), data.State_Initial);
	}
	
	if (data.AmmoChamberEmpty || data.Weapon_Ammo.Initial == 0)
	{
		test.weapon->~AmmoChamberEject(Dummy);
	}
	else
	{
		test.weapon->~AmmoChamberInsert(Dummy);
	}

	// Start the reloading
	test.weapon->StartReload(CurrentTest().user, 100, 0, true);
	return Wait(30);
}

global func Test_Reset()
{
	CurrentTest().initialized = false;
}

global func Test_ReloadSequence()
{
	var test = CurrentTest();
	var data = Test_Data(test->GetIndex());
	
	doTestTransitions(data.States); // Check the transitions
	doTestAmmo("Final", data.User_Ammo.Final, data.Weapon_Ammo.Final); // Check the remaining ammo
	return Evaluate();
}


global func SetupAmmo(int user_initial_ammo, int weapon_initial_ammo)
{
	// Setup 
	CurrentTest().user.test_ammo_source = AMMO_Source_Local;
	CurrentTest().user->SetAmmo(Dummy, user_initial_ammo);
	CurrentTest().weapon->SetAmmo(Dummy, weapon_initial_ammo);

	// Sanity check
	doTest("Initial user ammo was %d, should be %d", CurrentTest().user->GetAmmo(Dummy), user_initial_ammo);
	doTest("Initial weapon ammo was %d, should be %d", CurrentTest().weapon->GetAmmo(Dummy), weapon_initial_ammo);
}

global func doTestAmmo(string description, int user_ammo, int weapon_ammo)
{
		doTest(Format("%s user ammo was %%d, should be %%d", description), CurrentTest().user->GetAmmo(Dummy), user_ammo);
		doTest(Format("%s weapon ammo was %%d, should be %%d", description), CurrentTest().weapon->GetAmmo(Dummy), weapon_ammo);
}

global func doTestTransitions(array expected)
{
	var num_e = GetLength(expected);
	var num_a = GetLength(CurrentTest().states);
	doTest("Amount of state transitions was %d, should be %d", num_a, num_e);
	for (var i = 0; i < Max(num_e, num_a); ++i)
	{
		doTest("State was '%s', should be '%s'", CurrentTest().states[i], expected[i]);
	}
}

// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------


local test_scenarios = 
[
// --- Container, default
{
	Title = "Reload an empty weapon",
	Item = Reload_Container_Default,
	Weapon_Ammo = { Initial =  0, Final = 10}, 
	User_Ammo   = { Initial = 14, Final =  4},
	States = ["Reload_Container_Prepare",
	          "Reload_Container_EjectAmmo",
	          "Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a partially filled weapon",
	Item = Reload_Container_Default,
	Weapon_Ammo = { Initial =  5, Final =  10},
	User_Ammo = { Initial = 14, Final =   9},
	States = ["Reload_Container_Prepare",
	          "Reload_Container_EjectAmmo",
	          "Reload_Container_StashStart",
	          "Reload_Container_StashFinish",
	          "Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon",
	Item = Reload_Container_Default,
	Weapon_Ammo = { Initial = 10, Final =  10},
	User_Ammo = { Initial = 14, Final =  14},
	States = [nil], // Does not do a useless reload
}, {
	Title = "Reload an empty weapon from non-default state",
	Item = Reload_Container_Default,
	Weapon_Ammo = { Initial =  0, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   4},
	States = ["Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_ReadyWeapon",
	          nil],
	State_Initial = "Reload_Container_InsertAmmo",
},
// --- Container, with ammo chamber
{
	Title = "Reload an empty weapon",
	Item = Reload_Container_AmmoChamber,
	Weapon_Ammo = { Initial =  0, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   4},
	States = ["Reload_Container_Prepare",
	          "Reload_Container_EjectAmmo",
	          "Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_LoadAmmoChamber",
	          "Reload_Container_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a partially filled weapon",
	Item = Reload_Container_AmmoChamber,
	Weapon_Ammo = { Initial =  5, Final =  11},
	User_Ammo = { Initial = 14, Final =   8},
	States = ["Reload_Container_Prepare",
	          "Reload_Container_EjectAmmo",
	          "Reload_Container_StashStart",
	          "Reload_Container_StashFinish",
	          "Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon, with ammo chamber loaded, magazine partially filled",
	Item = Reload_Container_AmmoChamber,
	Weapon_Ammo = { Initial = 10, Final =  11},
	User_Ammo = { Initial = 14, Final =  13},
	States = ["Reload_Container_Prepare",
	          "Reload_Container_EjectAmmo",
	          "Reload_Container_StashStart",
	          "Reload_Container_StashFinish",
	          "Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon, with ammo chamber loaded, magazine is full",
	Item = Reload_Container_AmmoChamber,
	Weapon_Ammo = { Initial = 11, Final =  11},
	User_Ammo = { Initial = 14, Final =  14},
	States = [nil], // Do not do a useless reload
}, {
	Title = "Reload a full weapon, with ammo chamber not loaded",
	Item = Reload_Container_AmmoChamber,
	Weapon_Ammo = { Initial = 10, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   14},
	States = ["Reload_Container_LoadAmmoChamber",
	          "Reload_Container_ReadyWeapon",
	          nil],
	AmmoChamberEmpty = true,
}, {
	Title = "Reload an empty weapon from non-default state",
	Item = Reload_Container_AmmoChamber,
	Weapon_Ammo = { Initial =  0, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   4},
	States = ["Reload_Container_InsertAmmo",
	          "Reload_Container_Close",
	          "Reload_Container_LoadAmmoChamber",
	          "Reload_Container_ReadyWeapon",
	          nil],
	State_Initial = "Reload_Container_InsertAmmo",
}
];/*,
// --- Magazine, default
{
	Title = "Reload an empty weapon",
	Item = Reload_Magazine_Default,
	Weapon_Ammo = { Initial =  0, Final = 10}, 
	User_Ammo   = { Initial = 14, Final =  4},
	States = ["Reload_Magazine_Prepare",
	          "Reload_Magazine_Drop",
	          "Reload_Magazine_Insert",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a partially filled weapon",
	Item = Reload_Magazine_Default,
	Weapon_Ammo = { Initial =  5, Final =  10},
	User_Ammo = { Initial = 14, Final =   9},
	States = ["Reload_Magazine_Prepare",
	          "Reload_Magazine_StashStart",
	          "Reload_Magazine_StashFinish",
	          "Reload_Magazine_Insert",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon",
	Item = Reload_Magazine_Default,
	Weapon_Ammo = { Initial = 10, Final =  10},
	User_Ammo = { Initial = 14, Final =  14},
	States = [nil], // Does not do a useless reload
}, {
	Title = "Reload an empty weapon from non-default state",
	Item = Reload_Magazine_Default,
	Weapon_Ammo = { Initial =  0, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   4},
	States = ["Reload_Magazine_Insert",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
	State_Initial = "Reload_Magazine_Insert",
},
// --- Magazine, with ammo chamber
{
	Title = "Reload an empty weapon",
	Item = Reload_Magazine_AmmoChamber,
	Weapon_Ammo = { Initial =  0, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   4},
	States = ["Reload_Magazine_Prepare",
	          "Reload_Magazine_Drop",
	          "Reload_Magazine_Insert",
	          "Reload_Magazine_LoadAmmoChamber",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a partially filled weapon",
	Item = Reload_Magazine_AmmoChamber,
	Weapon_Ammo = { Initial =  5, Final =  11},
	User_Ammo = { Initial = 14, Final =   8},
	States = ["Reload_Magazine_Prepare",
	          "Reload_Magazine_StashStart",
	          "Reload_Magazine_StashFinish",
	          "Reload_Magazine_Insert",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon, with ammo chamber loaded, magazine partially filled",
	Item = Reload_Magazine_AmmoChamber,
	Weapon_Ammo = { Initial = 10, Final =  11},
	User_Ammo = { Initial = 14, Final =  13},
	States = ["Reload_Magazine_Prepare",
	          "Reload_Magazine_StashStart",
	          "Reload_Magazine_StashFinish",
	          "Reload_Magazine_Insert",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon, with ammo chamber loaded, magazine is full",
	Item = Reload_Magazine_AmmoChamber,
	Weapon_Ammo = { Initial = 11, Final =  11},
	User_Ammo = { Initial = 14, Final =  14},
	States = [nil], // Do not do a useless reload
}, {
	Title = "Reload a full weapon, with ammo chamber not loaded",
	Item = Reload_Magazine_AmmoChamber,
	Weapon_Ammo = { Initial = 10, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   14},
	States = ["Reload_Magazine_LoadAmmoChamber",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
	AmmoChamberEmpty = true,
}, {
	Title = "Reload an empty weapon from non-default state",
	Item = Reload_Magazine_AmmoChamber,
	Weapon_Ammo = { Initial =  0, Final =  10}, 
	User_Ammo = { Initial = 14, Final =   4},
	States = ["Reload_Magazine_Insert",
	          "Reload_Magazine_LoadAmmoChamber",
	          "Reload_Magazine_ReadyWeapon",
	          nil],
	State_Initial = "Reload_Magazine_Insert",
},
// --- Single, default
{
	Title = "Reload an empty weapon",
	Item = Reload_Single_Default,
	Weapon_Ammo = { Initial =  0, Final = 10}, 
	User_Ammo   = { Initial = 14, Final =  4},
	States = ["Reload_Single_Prepare",
	          "Reload_Single_InsertAmmo",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
	          "Reload_Single_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a partially filled weapon",
	Item = Reload_Single_Default,
	Weapon_Ammo = { Initial =  7, Final =  10},
	User_Ammo = { Initial = 14, Final =   11},
	States = ["Reload_Single_Prepare",
	          "Reload_Single_InsertAmmo",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
	          "Reload_Single_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon",
	Item = Reload_Single_Default,
	Weapon_Ammo = { Initial = 10, Final =  10},
	User_Ammo = { Initial = 14, Final =  14},
	States = [nil], // Does not do a useless reload
}, {
	Title = "Reload an empty weapon from non-default state",
	Item = Reload_Single_Default,
	Weapon_Ammo = { Initial =  0, Final =  0}, 
	User_Ammo = { Initial = 14, Final =   14},
	States = ["Reload_Single_ReadyWeapon",
	          nil],
	State_Initial = "Reload_Single_ReadyWeapon",
},
// --- Single, with ammo chamber
{
	Title = "Reload an empty weapon",
	Item = Reload_Single_AmmoChamber,
	Weapon_Ammo = { Initial =  0, Final = 11}, 
	User_Ammo   = { Initial = 14, Final =  3},
	States = ["Reload_Single_Prepare",
	          "Reload_Single_OpenAmmoChamber",
	          "Reload_Single_InsertAmmo",
		          "Single_Ammo_Insert_Counter",
	          "Reload_Single_CloseAmmoChamber",
	          "Reload_Single_InsertAmmo",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
	          "Reload_Single_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a partially filled weapon",
	Item = Reload_Single_AmmoChamber,
	Weapon_Ammo = { Initial =  7, Final =  11},
	User_Ammo = { Initial = 14, Final =   10},
	States = ["Reload_Single_Prepare",
	          "Reload_Single_InsertAmmo",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
		          "Single_Ammo_Insert_Counter",
	          "Reload_Single_ReadyWeapon",
	          nil],
}, {
	Title = "Reload a full weapon, with ammo chamber loaded, magazine is full",
	Item = Reload_Single_AmmoChamber,
	Weapon_Ammo = { Initial = 10, Final =  10},
	User_Ammo = { Initial = 14, Final =  14},
	States = [nil], // Does not do a useless reload
}, {
	Title = "Reload a full weapon, with ammo chamber not loaded",
	Item = Reload_Single_AmmoChamber,
	Weapon_Ammo = { Initial = 10, Final =  10},
	User_Ammo = { Initial = 14, Final =  14},
	States = ["Reload_Single_LoadAmmoChamber",
	          "Reload_Single_ReadyWeapon",
	          nil],
	AmmoChamberEmpty = true,
}, {
	Title = "Reload an empty weapon from non-default state",
	Item = Reload_Single_AmmoChamber,
	Weapon_Ammo = { Initial =  0, Final =  0}, 
	User_Ammo = { Initial = 14, Final =   14},
	States = ["Reload_Single_ReadyWeapon",
	          nil],
	State_Initial = "Reload_Single_ReadyWeapon",
}
];*/
