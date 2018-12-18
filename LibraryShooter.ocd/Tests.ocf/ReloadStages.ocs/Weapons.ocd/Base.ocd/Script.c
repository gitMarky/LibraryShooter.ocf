
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// properties

local Name = "$Name$";
local Description = "$Description$";


public func GetCarryMode(object user) {    return CARRY_Hand; }
public func GetCarrySpecial(object user) { return "pos_hand2"; }
public func GetCarryBone() { return "main"; }
public func GetCarryTransform()
{
	return Trans_Rotate(90, 1, 0, 0);
}


public func Initialize()
{
	_inherited(...);
	this->ClearFiremodes();
	var firemode_default = new /*firemode_default*/ Library_Firearm_Firemode {};

	firemode_default->SetAmmoID(Dummy)
	                ->SetName("Standard")
	                ->SetRecoveryDelay(10)
	                ->SetAmmoAmount(10);

	this->AddFiremode(firemode_default);
	this->SetFiremode(firemode_default->GetIndex());
}

public func NeedsReload(object user)
{
	return true;
}

func SetReloadState(proplist firemode, state)
{
	if (GetType(state) == C4V_String || state == nil)
	{
		Log("Adding state transition: %s", state);
		PushBack(CurrentTest().states, state);
	}
	else
	{
		Log("Ignoring state %v", state);
	}
	_inherited(firemode, state, ...);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fire modes

local Collectible = 1;

func Hit()
{
	Sound("Hits::GeneralHit?");
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Effects


public func FireSound(object user, proplist firemode)
{
}

public func OnFireProjectile(object user, object projectile, proplist firemode)
{
}

public func FireEffect(object user, int angle, proplist firemode)
{
}
