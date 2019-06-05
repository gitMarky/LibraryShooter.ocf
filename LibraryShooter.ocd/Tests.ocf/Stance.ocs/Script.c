
static const POSE_STANDING = "Standing";
static const POSE_CROUCHING = "Crouching";
static const POSE_PRONE = "Prone";

static const WEAPON_IDLE = "WeaponIdle";
static const WEAPON_READY = "WeaponReady";
static const WEAPON_AIMING = "WeaponAiming";

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

global func CreateStanceManager()
{
	// Standing state
	var standing = new StanceDefinition { Name = POSE_STANDING, };
	var crouching = new StanceDefinition { Name = POSE_CROUCHING, };
	var prone = new StanceDefinition { Name = POSE_PRONE, };
	
	// Define transitions
	standing->AddTransition(crouching)
	        ->AddTransition(prone)
	        ->AddTransition(crouching)
	        ->AddTransition(standing);
	        
	// Aiming state
	var weapon_idle = new StanceDefinition { Name = WEAPON_IDLE, };
	var weapon_ready = new StanceDefinition { Name = WEAPON_READY, };
	var weapon_aiming = new StanceDefinition { Name = WEAPON_AIMING, };
	
	weapon_idle->AddTransition(weapon_ready)->AddTransition(weapon_idle);
	weapon_ready->AddTransition(weapon_aiming)->AddTransition(weapon_ready);

	// Add everything to the stance maanger

	var manager = CreateObject(Library_StanceManager);
	
	// So that it can be retrieved via string

	manager.StanceMap = { "pose0" = standing, "pose1" = crouching, "pose2" = prone,
	                      "weap0" = weapon_idle, "weap1" = weapon_ready, "weap2" = weapon_aiming};

	// Initial state
	manager->SetStance(standing);
	manager->SetStance(weapon_idle, 1);
	return manager;
}

global func doTestTransition(object manager, any channel, string from, string to, bool result)
{
	var desc = Format("[%s => %s](%v): ", from, to, channel);

	var final;
	if (result)
	{
		final = to;
	}
	else
	{
		final = from;
	}

	doTest(Format("%s%s", desc, "Initial stance should be \"%s\", got \"%s\""), from, manager->GetStance().Name);
	doTest(Format("%s%s", desc, "Transition should return %v, got %v"), result, manager->SetStance(to));
	doTest(Format("%s%s", desc, "Final stance should be \"%s\", got \"%s\""), final, manager->GetStance().Name);
}

/* --- The actual tests --- */


// --------------------------------------------------------------------------------------------------------

global func Test1_OnStart() { return true; }
global func Test1_Execute()
{
	var manager = CreateStanceManager();
	
	Log("Stance manager tracks stances correctly in GetStance()");

	doTest("Stance for channel(nil) should be \"%s\", got \"%s\"", POSE_STANDING, manager->GetStance().Name);
	doTest("Stance for channel(0) should be \"%s\", got \"%s\"", POSE_STANDING, manager->GetStance(0).Name);
	doTest("Stance for channel(1) should be \"%s\", got \"%s\"", WEAPON_IDLE, manager->GetStance(1).Name);

	return Evaluate();
}

// --------------------------------------------------------------------------------------------------------

global func Test2_OnStart() { return true; }
global func Test2_Execute()
{
	var manager = CreateStanceManager();
	
	Log("SetStance() works correctly for the known transitions");

	// Standing
	doTestTransition(manager, 0, POSE_STANDING, POSE_STANDING, false);
	doTestTransition(manager, 0, POSE_STANDING, POSE_PRONE, false);
	doTestTransition(manager, 0, POSE_STANDING, POSE_CROUCHING, true);
	
	// Crouching
	doTestTransition(manager, 0, POSE_CROUCHING, POSE_CROUCHING, false);
	doTestTransition(manager, 0, POSE_CROUCHING, POSE_PRONE, true);
	
	// Prone
	doTestTransition(manager, 0, POSE_PRONE, POSE_PRONE, false);
	doTestTransition(manager, 0, POSE_PRONE, POSE_STANDING, false);
	
	// Back
	doTestTransition(manager, 0, POSE_PRONE, POSE_CROUCHING, true);
	doTestTransition(manager, 0, POSE_CROUCHING, POSE_STANDING, true);

	return Evaluate();
}