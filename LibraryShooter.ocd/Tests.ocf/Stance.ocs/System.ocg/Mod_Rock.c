#appendto Rock

local stance_idle, stance_aiming;

/* --- Selection --- */

public func Selection(object user)
{
	user->SetAimControlEnabled(true);
	SetUserStance(user, stance_aiming);
	_inherited(user, ...);
}

public func Deselection(object user)
{
	SetUserStance(user, stance_idle);
	user->SetAimControlEnabled(false);
	_inherited(user, ...);
}


public func SetUserStance(object user, proplist stance)
{
	user->~SetStance(stance, 1, true);
}


public func Construction()
{
	// Aiming state
	stance_idle = new StanceDefinition { Name = WEAPON_IDLE, };
	stance_aiming = new StanceDefinition { Name = WEAPON_AIMING, };

	stance_aiming->AddBehaviour(StanceBehaviour_ReplaceAction->Create([
			["Walk", "BowWalk"],
			["Walk_Position", 20],
			["Stand", "BowStand"],
			["Jump", "BowJump"],
			["KneelDown", "BowKneel"]
		]))->AddBehaviour(StanceBehaviour_WalkSpeed->Create(84, 56))
		   ->AddBehaviour(StanceBehaviour_AimAnimation->Create("BowAimArms")->SetCondition(this.BowAimCondition));
}


public func BowAimCondition (object clonk)
{
	return clonk->HasActionProcedure(false);
}
