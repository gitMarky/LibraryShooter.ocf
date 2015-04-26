#include Library_AmmoManager

/**
 @note Firemodes
  A firemode has three stages: Charge - (Fire/Recover) - Cooldown
 @author Marky
 @credits Hazard Team, Zapper
 @version 0.1.0
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;


local is_selected = true;  // bool: is the weapon currently selected?
local is_using = false;    // bool: is the user holding the fire button

static const WEAPON_Firemode_Default = "default";

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
		ammo_usage =		1, // int - this many units of ammo
		ammo_rate =			1, // int - per this many shots fired
	
		delay_charge  =     0, // int, frames - time that the button must be held before the shot is fired
		delay_recover = 	7, // int, frames - time between consecutive shots
		delay_cooldown =    0, // int, frames - time of cooldown after the last shot is fired
		delay_reload =		6, // int, frames - time to reload
	
		mode = 			 WEAPON_FM_Single,
	
		damage = 			10, 
		damage_type = 		nil,	
	
		projectile_id = 	NormalBullet,
		projectile_speed = 	100,
		projectile_range = 600,
		projectile_distance = 10,
		projectile_offset_y = -6,
		projectile_number = 1,
		projectile_spread = [0, 100], // default inaccuracy of a single projectile

		spread = [1, 100],			   // inaccuracy from prolonged firing
		
		burst = 0, // number of projectiles fired in a burst
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


//----------------------------------------------------------------------------------------------------------------
//
// charging the weapon

private func StartCharge(object user, proplist firemode)
{
	if (!is_using || firemode.delay_charge < 1 || !NeedsCharge(firemode)) return false;
	
	var effect = IsCharging();
	
	if (effect != nil)
	{
		if (effect.user == user && effect.firemode == firemode)
		{
			if (effect.has_charged)
			{
				return false; // fire away
			}
			else if (effect.is_charged)
			{
				effect.has_charged = true;
				DoCharge(user, firemode);
				return false; // fire away
			}
			
			return true; // keep charging
		}
		else
		{
			CancelCharge(false);
		}
	}

	AddEffect("IntCharge", this, 1, 1, this, nil, user, firemode);
	OnStartCharge(user, firemode);
	return true; // keep charging
}

private func CancelCharge(bool callback)
{
	var effect = IsCharging();
	
	if (effect != nil)
	{
		if (callback) OnCancelCharge(effect.user, effect.firemode);
		
		RemoveEffect(nil, nil, effect);
	}
}

private func DoCharge(object user, proplist firemode)
{
	OnFinishCharge(user, firemode);
}

private func IsCharging()
{
	return GetEffect("IntCharge", this);
}

/**
 Condition when the weapon needs to be charged.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default. Overload this function
         for a custom condition.
 @version 0.1.0
 */
public func NeedsCharge(object user, proplist firemode)
{
	return true;
}

/**
 Callback: the weapon starts charging. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnStartCharge(object user, proplist firemode)
{
}

/**
 Callback: the weapon has successfully charged. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnFinishCharge(object user, proplist firemode)
{
}

/**
 Gets the current status of the charging process.
 @return A value of 0 to 100, if the weapon is charging.@br
         If the weapon is not charging, this function returns -1.
 */
public func GetChargeProgress()
{
	var effect = IsCharging();
	
	if (effect = nil)
	{
		return -1;
	}
	else
	{
		return effect.percent;
	}
}

/**
 Callback: the weapon user cancelled charging. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnCancelCharge(object user, proplist firemode)
{
}

private func FxIntChargeStart(object target, proplist effect, int temp, object user, proplist firemode)
{
	if (temp) return;
	
	effect.user = user;
	effect.firemode = firemode;
}

private func FxIntChargeTimer(object target, proplist effect, int time)
{
	effect.percent = BoundBy(time * 100 / effect.firemode.delay_charge, 0, 100);

	if (time > effect.firemode.delay_charge && !effect.is_charged)
	{
		effect.is_charged = true;
	}
}

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
	
	OnPressUse(user, x, y);

//	if(!Ready(user, x, y)) return true; // checks loading etc

//	if(!ReadyToFire())
//	{
//		CheckReload();
//		Sound("DryFire?");
//		return true;
//	}

//	AimStartSound();

	//user->StartAim(this);

	//ControlUseHolding(user, x, y);
	
	//if(!weapon_properties.delay_shot && !weapon_properties.full_auto)
	//	Fire(user, x, y); //user->GetAimPosition());
	return true;
}

/**
 The function does the following:@br
 - tell the user to start aiming@br
 - call {@link Library_Weapon#ControlUseHolding}@br
 - call {@link Library_Weapon#Fire}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected func ControlUseAltStart(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}
	
	OnPressUseAlt(user, x, y);
	
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

	DoFireCycle(user, x, y, true);
	
	return true;
}

protected func ControlUseAltHolding(object user, int x, int y)
{
	return ControlUseHolding(user, x, y);
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
	
	CancelCharge(true);
	
	if (!IsRecovering())
	{
		CheckCooldown(user, GetFiremode());
	}
	
	OnUseStop(user, x, y);
	
	return true;
}

protected func ControlUseAltStop(object user, int x, int y)
{
	return ControlUseStop(user, x, y);
}

protected func ControlUseCancel(object user, int x, int y)
{
	return ControlUseStop(user, x, y);
}

protected func ControlUseAltCancel(object user, int x, int y)
{
	return ControlUseStop(user, x, y);
}

private func DoFireCycle(object user, int x, int y, bool is_pressing_trigger)
{
	var angle = GetAngle(x, y);
	user->SetAimPosition(angle);
	
	if (is_pressing_trigger)
	{
	 	is_using = true;	
	}

	if (IsReadyToFire())
	{
		Fire(user, x, y);
	}
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
private func Fire(object user, int x, int y)
{
	if (user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}
	
	var firemode = GetFiremode(); //GetProperty(firemode, fire_modes);
	
	if (firemode == nil)
	{
		FatalError(Format("Fire mode '%s' not supported", firemode));
	}
	
	if (StartCharge(user, firemode)) return;

	var angle = GetFireAngle(x, y, firemode);

	FireSound(user, firemode);
	FireEffect(user, angle, firemode);

	FireProjectiles(user, angle, firemode);
//	AddDeviation();

	FireRecovery(user, x, y, firemode);
}

private func RejectUse(object user)
{
	return !IsReadyToUse() || !user->HasHandAction();
}

/**
 Interface for signaling that the weapon is ready to use (attack). 
 @return true, if the object is ready to use.
 */
private func IsReadyToUse()
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
		
		
	
		projectile->Shooter(user)
				  ->Weapon(this)
				  ->Damage(firemode.damage)
		          ->DamageType(firemode.damage_type)
		          ->Velocity(SampleVelocity(firemode.projectile_speed))
				  ->Range(firemode.projectile_range)
		          ->Launch(angle, GetSpread(firemode));
	}
	
	shot_counter[firemode.name]++;
}

private func GetSpread(proplist firemode)
{
	return [firemode.spread, firemode.projectile_spread];
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

private func FireRecovery(object user, int x, int y, proplist firemode)
{
	AddEffect("IntRecovery", this, 1, firemode.delay_recover, this, nil, user, x, y, firemode);
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
	target->DoRecovery(effect.user, effect.x, effect.y, effect.firemode);

	return FX_Execute_Kill;
}

private func CancelRecovery()
{
	var effect = IsRecovering();
	
	if (effect != nil)
	{
		RemoveEffect(nil, nil, effect);
	}
}

private func IsRecovering()
{
	return GetEffect("IntRecovery", this);
}

private func DoRecovery(object user, int x, int y, proplist firemode)
{
	if (firemode == nil) return;
	
	OnRecovery(user, firemode);

	if (firemode.burst)
	{
		if (firemode.mode != WEAPON_FM_Burst)
		{
			FatalError(Format("This fire mode has a burst value of %d, but the mode is not burst mode WEAPN_FM_Burst (value: %d)", firemode.burst, firemode.mode));
		}
	
		if (shot_counter[firemode.name] >= firemode.burst)
		{
			shot_counter[firemode.name] = 0;
		}
		else 
		{
			
			if (!is_using)
			{
				CancelRecovery();
				DoFireCycle(user, x, y, false);
			}
			
			return; // prevent cooldown
		}
	}
	
	CheckCooldown(user, firemode);
}

private func CheckCooldown(object user, proplist firemode)
{
	if ((firemode.mode != WEAPON_FM_Auto) || (firemode.mode == WEAPON_FM_Auto && !is_using))
	{
		StartCooldown(user, firemode);
	}
}

/**
 Callback: the weapon finished one firing cycle. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnRecovery(object user)
{
	
}

/**
 The weapon is ready to fire a shot.
 @version 0.1.0
 */
private func IsReadyToFire()
{
	return !IsRecovering()
	    && !IsCoolingDown();
}


//----------------------------------------------------------------------------------------------------------------
//
// charging the weapon

private func StartCooldown(object user, proplist firemode)
{
	if (firemode.delay_cooldown < 1 || !NeedsCooldown(user, firemode)) return;
	
	var effect = IsCoolingDown();

	if (effect == nil)
	{
		AddEffect("IntCooldown", this, 1, firemode.delay_cooldown, this, nil, user, firemode);
		OnStartCooldown(user, firemode);
	}
}

private func DoCooldown(object user, proplist firemode)
{
	OnFinishCooldown(user, firemode);
}

private func IsCoolingDown()
{
	return GetEffect("IntCooldown", this);
}

/**
 Condition when the weapon needs a cooldown.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default. Overload this function
         for a custom condition.
 @version 0.1.0
 */
public func NeedsCooldown(object user, proplist firemode)
{
	return true;
}

/**
 Callback: the weapon starts cooldown. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnStartCooldown(object user, proplist firemode)
{
}

/**
 Callback: the weapon has successfully cooled down. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnFinishCooldown(object user, proplist firemode)
{
}

private func FxIntCooldownStart(object target, proplist effect, int temp, object user, proplist firemode)
{
	if (temp) return;
	
	effect.user = user;
	effect.firemode = firemode;
}

private func FxIntCooldownTimer(object target, proplist effect, int time)
{
	target->DoCooldown(effect.user, effect.firemode);
	
	return FX_Execute_Kill;
}

private func SampleVelocity(value)
{
	if (GetType(value) == C4V_Array)
	{
		var min = value[0];
		var range = value[1] - min;
		var step = Max(value[2], 1);
		
		return min + step * Random(range / step);
	}
	else if (GetType(value) == C4V_Int)
	{
		return value;
	}
	else
	{
		FatalError(Format("Expected int or array, got %v", value));
	}
}

/**
 Callback: the current firemode. Overload this function for
 @return proplist The current firemode.
 @version 0.1.0
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
public func GetFiremode()
{
}

/**
 Callback: Pressed the regular use button (fire).
 */
public func OnPressUse(object user, int x, int y)
{
}

/**
 Callback: Pressed the alternate use button (fire secondary).
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
public func OnPressUseAlt(object user, int x, int y)
{
}

public func CanChangeFiremode()
{
	return !IsRecovering()
	    && !IsCharging();
}