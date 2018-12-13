#include Library_Firearm

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
	ClearFiremodes();
	firemode_default = new firemode_default {};
	AddFiremode(firemode_default);
	SetFiremode(firemode_default->GetIndex());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fire modes

local Collectible = 1;

local firemode_default = 
{
	Prototype = Library_Firearm_Firemode,

	name = 				"Standard", // string - menu caption
	icon = 				nil, // id - menu icon
	condition = 		nil, // string - callback for a condition

	ammo_id = 			Dummy,
	ammo_usage =          1, // int - this many units of ammo
	ammo_rate =           1, // int - per this many shots fired

	delay_charge  =       0, // int, frames - time that the button must be held before the shot is fired
	delay_recover =      10, // int, frames - time between consecutive shots
	delay_cooldown =      0, // int, frames - time of cooldown after the last shot is fired
	delay_reload =	    100, // int, frames - time to reload

	mode = WEAPON_FM_Single,

	damage =              0, 
	damage_type = 		nil,	

	projectile_id = 	Bullet,
	projectile_speed = 	1000,
	projectile_range =  1000,
	projectile_distance = 10,
	projectile_offset_y = -10,
	projectile_number =    1,
	projectile_spread = {angle = 0, precision = 100}, // default inaccuracy of a single projectile

	spread = {angle = 0, precision = 100},			  // inaccuracy from prolonged firing

	burst = 0, // number of projectiles fired in a burst
};


local weapon_properties = 
{
		gfx_distance = 6,
		gfx_offset_y = 0,
};



func Hit()
{
	Sound("Hits::GeneralHit?");
}

public func GetAmmoSource(id ammo)
{
	return AMMO_Source_Container;
}

public func GetAmmoContainer()
{
	return Contained();
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
