#include Clonk
#include Library_StanceManager

func Construction()
{
	_inherited(...);
	Setup();
	SetGraphics(nil, Clonk);
}

public func Setup()
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

	// So that it can be retrieved via string
	this->AddStance(standing, crouching, prone, weapon_idle, weapon_ready, weapon_aiming);
	return this;
}

public func SetupBowStance()
{
	var crouching = GetStanceDefinition(POSE_CROUCHING);
	
	crouching->AddBehaviour(StanceBehaviour_ReplaceAction->Create([
			["Walk", "BowWalk"],
			["Walk_Position", 20],
			["Stand", "BowStand"],
			["Jump", "BowJump"],
			["KneelDown", "BowKneel"]
		]))->AddBehaviour(StanceBehaviour_WalkSpeed->Create(84, 56))
		   ->AddBehaviour(StanceBehaviour_AimAnimation->Create("BowAimArms")->SetCondition(this.BowAimCondition));
	return this;
}

public func BowAimCondition (object clonk)
{
	return clonk->HasActionProcedure(false);
}

public func Test()
{
	SetOwner(0);
	MakeCrewMember(0);
	SetGraphics(nil, Clonk);
}


public func BowTest()
{
	Setup();
	SetupBowStance();
	Test();
	SetCursor(GetOwner(), this);
	SetStance(POSE_CROUCHING);
}
