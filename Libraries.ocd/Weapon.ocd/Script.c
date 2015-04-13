#include Library_AmmoManager

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;


local is_selected = true; // bool: is the weapon currently selected?
local is_using = false; // bool: is the user holding the fire button

static const WEAPON_FM_Single	= 1;
static const WEAPON_FM_Burst 	= 2;
static const WEAPON_FM_Auto 	= 3;


static const WEAPON_PR_Bullet = 1;
static const WEAPON_PR_Ballistic = 2;
static const WEAPON_PR_Hitscan = 3;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions


local fire_modes =
{
	default = 
	{
		name = 				"default", // string - menu caption
		icon = 				nil, // id - menu icon
		condition = 		nil, // string - callback for a condition
		
		ammo_id = 			nil,
		ammo_usage =		1,	// this many units of ammo
		ammo_rate =			1, // per this many shots fired
	
		delay_prior = 		0, // time before the first shot is fired
		delay_reload =		6, // time to reload, in frames
		delay_recover = 	7, // time between consecutive shots
		delay_burst = 		0, // time between consecutive bursts
	
		mode = 			 WEAPON_FM_Single,
	
		damage = 			10, 
		damage_type = 		nil,	
	
		projectile_id = 	NormalBullet,
		projectile_speed = 	100,
		projectile_range = 600,
		projectile_distance = 10,
		projectile_offset_y = -6,
		projectile_number = 1,
		projectile_spread = 0, // default inaccuracy of a single projectile
		projectile_spread_factor = 100, // factor

		spread = 1,			   // inaccuracy from prolonged firing
		spread_factor = 100,   // factor
		
		burst = 0, // number of projectiles fired in a burst

//	static const FM_Accuracy = 		14;		//
//	static const FM_AimAngle = 		15;		//
//	static const FM_ProjSize = 		20;		// wie breit ist das Projektil / die Spur
//	static const FM_ProjTrail = 	21;		// wie lang ist die Spur?
//	static const FM_ProjSound =		22;		// welchen Sound macht der Modus
//	static const FM_ProjEffects = 	23;		// hat der Feuermodus einen eigenen Effekte-Call?
//	static const FM_ProjCustom = 	24;		// hat der Feuermodus einen eigenen Launch-Call?
//	static const FM_Old = 			25;		// macht der Feuermods einen Fire%d-Call?
//	static const FM_SightBonus = 	28;		// so viel kriegt der Spieler zu seiner ViewRange
	},
};

local ammo_containers = 
{
	clip = {
	},
	
	box = {
	},
};

local weapon_properties = 
{
		gfx_distance = 6,
		gfx_offset_y = -6,
};


local shot_counter; // proplist


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

private func Initialize()
{
	shot_counter = {};
	_inherited();
}

public func GetCarryMode(object user) {    if (is_selected) return CARRY_Hand; }
public func GetCarrySpecial(object user) { if (is_selected) return "pos_hand2"; }
public func GetCarryBone() { return "main"; }
public func GetCarryTransform()
{
	return Trans_Rotate(-90, 0, 1, 0);
}

local animation_set = {
		AimMode        = AIM_Position, // The aiming animation is done by adjusting the animation position to fit the angle
		AnimationAim   = "MusketAimArms",
		AnimationLoad  = "MusketLoadArms",
		LoadTime       = 80,
		AnimationShoot = nil,
		ShootTime      = 20,
		WalkSpeed      = nil,
		WalkBack       = nil,
	};


public func GetAnimationSet() { return animation_set; }

// holding callbacks are made
public func HoldingEnabled() { return true; }


/**
 This is executed each time the user presses the fire button.@br@br

 The function does the following:@br
 - tell the user to start aiming@br
 - call {@link Library_Weapon#ControlUseHolding}@br
 - call {@link Library_Weapon#Fire}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected func ControlUseStart(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

//	if(!Ready(user, x, y)) return true; // checks loading etc

//	if(!ReadyToFire())
//	{
//		CheckReload();
//		Sound("DryFire?");
//		return true;
//	}

//	AimStartSound();

	//user->StartAim(this);

	ControlUseHolding(user, x, y);
	//if(!weapon_properties.delay_shot && !weapon_properties.full_auto)
	//	Fire(user, x, y); //user->GetAimPosition());
	return true;
}

/**
 This is executed while the user is holding the fire button.@br@br

 The function does the following:@br
 - update the aiming angle according to the parameters
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected func ControlUseHolding(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	var angle = GetAngle(x, y);
	user->SetAimPosition(angle);
	
	is_using = true;
	
	
//	if(weapon_properties.delay_shot)
//		ResetAim(angle);
//	if (weapon_properties.full_auto)
//	{
//		if(!TryFire(user, angle))
//		{
//			ControlUseStop(user, x, y);
//			return false;
//		}
//	}

	if (!IsRecovering())
	{
		Fire(user, x, y);
	}
	return true;
}

/**
 This is executed when the user stops holding the fire button.@br@br

 The function does the following:@br
 - tell the user to stop aiming.
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected func ControlUseStop(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	is_using = false;
	
	OnUseStop(user, x, y);
	
	//user->CancelAiming();
	
	return true;
	//return -1;
}

/**
 Converts coordinates to an aiming angle for the weapon.
 @par x The x coordinate, local.
 @par y The y coordinate, local.
 @return int The angle in degrees, normalized to the range of [-180°, 180°].
 @version 0.1.0
 */
private func GetAngle(int x, int y)
{
	var angle = Angle(0, weapon_properties.gfx_offset_y, x, y);
		angle = Normalize(angle, -180);
		
	return angle;
}

private func GetFireAngle(int x, int y, proplist firemode)
{
	var angle = Angle(0, firemode.projectile_offset_y, x, y);
		angle = Normalize(angle, -180);
		
	return angle;
}

/**
 Fires the weapon.@br
 
 The function does the following:@br
 - write a message saying 'pew pew'
 @par user The object that is using the weapon.
 @par angle The angle the weapon is aimed at.
 @version 0.1.0
 */
private func Fire(object user, int x, int y, string firemode)
{
	if (user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}
	
	if (firemode == nil)
	{
		firemode = "default";
	}
	
	var info = GetProperty(firemode, fire_modes);
	
	if (info == nil)
	{
		FatalError(Format("Fire mode '%s' not supported", firemode));
	}

	var angle = GetFireAngle(x, y, info);

	FireSound(user, info);
	FireEffect(user, angle, info);

	FireProjectiles(user, angle, info);
//	AddDeviation();

	FireRecovery(user, x, y, info);
	
	user->Message("Pew pew %d", angle);
}

private func RejectUse(object user)
{
	return !IsReadyToUse() || !user->HasHandAction();
}

/**
 Interface for signaling that the weapon is ready to use (attack). 
 @return true, if the object is ready to use.
 */
protected func IsReadyToUse()
{
	return true;
}

private func FireProjectiles(object user, int angle, proplist firemode)
{
	if (user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}
	if (firemode == nil)
	{
		FatalError("The function expects a fire mode that is not nil");
	}
	
	var x = +Sin(angle, firemode.projectile_distance);
	var y = -Cos(angle, firemode.projectile_distance) + firemode.projectile_offset_y;

	// launch the single projectiles
	for (var i = 0; i < firemode.projectile_number; i++)
	{
		var projectile = CreateObject(firemode.projectile_id, x, y, user->GetController());
	
		OnFireProjectile(user, projectile, firemode);
	
		projectile->~Launch(user, GetID(), angle, GetSpread(angle), firemode.projectile_speed, firemode.projectile_range, firemode.damage, firemode.damage_type, false);
	}
	
	shot_counter[firemode.name]++;
}

private func GetSpread(int angle, proplist ) // TODO
{
	return angle;
}

/**
 Callback that happens each time an individual projectile is fired.
 @note By default this function is empty. You should create some kind of sound here.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 */
public func FireSound(object user, proplist firemode)
{
	
}

/**
 Callback that happens each time an individual projectile is fired.
 @note By default this function is empty. You should create graphical effects here.
 @par user The object that is using the weapon.
 @par angle The angle the weapon is aimed at.
 @par firemode A proplist containing the fire mode information.
 */
public func FireEffect(object user, int angle, proplist firemode)
{

}

/**
 Callback that happens after a projectile is created and before it is launched.
 @par user The object that is using the weapon.
 @par projectile The object that will be launched.
 @par firemode A proplist containing the fire mode information.
 */
public func OnFireProjectile(object user, object projectile, proplist firemode)
{
}

/**
 Callback from {@link Library_Weapon#ControlUserStop}, 
 so that you do not have to overload the entire function.
 */
public func OnUseStop(object user, int x, int y)
{
}

private func EffectMuzzleFlash(object user, int x, int y, int angle, int size, bool sparks, bool light)
{
	if (user == nil)
	{
		FatalError("This function expects a user that is not nil");
	}
	
	user->CreateParticle("MuzzleFlash", x, y, 0, 0, 10, {Prototype = Particles_MuzzleFlash(), Size = 3 * size, Rotation = angle}, 1);

	if (sparks)
	{
		var xdir = +Sin(angle, size * 2);
		var ydir = -Cos(angle, size * 2);
	
		CreateParticle("StarFlash", x, y, PV_Random(xdir - size, xdir + size), PV_Random(ydir - size, ydir + size), PV_Random(20, 60), Particles_Glimmer(), size);
	}
	
	if (light)
	{
		user->CreateTemporaryLight(x, y)->LightRangeStart(3 * size)->SetLifetime(2)->Activate();
	}
}

private func FireRecovery(object user, int x, int y, proplist firemode, bool burst)
{
	var delay;
	if (burst)
	{
		delay = firemode.delay_burst;
	}
	else
	{
		delay = firemode.delay_recover;
	}

	AddEffect("IntRecovery", this, 1, delay, this, nil, user, x, y, firemode);
}

private func FxIntRecoveryStart (object target, proplist effect, int temporary, object user, int x, int y, proplist firemode)
{
	if (temporary) return;
	
	effect.user = user;
	effect.x = x;
	effect.y = y;
	effect.firemode = firemode;
}

private func FxIntRecoveryTimer(object target, proplist effect, int time)
{
	target->Recovery(effect.user, effect.x, effect.y, effect.firemode);

	return FX_Execute_Kill;
}

private func IsRecovering()
{
	return GetEffect("IntRecovery", this);
}

private func Recovery(object user, int x, int y, proplist firemode)
{
	if (firemode == nil) return;

	if (firemode.burst)
	{
		if (shot_counter[firemode.name] >= firemode.burst)
		{
			shot_counter[firemode.name] = 0;
			
			FireRecovery(user, x, y, nil, true);
		}
		else if (!is_using)
		{
			Log("Burst!!");
			ControlUseStart(user, x, y); // TODO
		}
	}
}