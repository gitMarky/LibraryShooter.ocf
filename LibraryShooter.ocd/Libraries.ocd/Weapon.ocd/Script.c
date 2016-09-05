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
	default = new fire_mode_default {},
};


local fire_mode_default = 
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
	
	auto_reload = false, // the weapon should "reload itself", i.e not require the user to hold the button when it reloads
	
	anim_shoot_name = nil,	// for animation set: shoot animation
	anim_load_name = nil,	// for animation set: reload animation
	walk_speed_front = nil,	// for animation set: relative walk speed
	walk_speed_back = nil,	// for animation set: relative walk speed
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
local ammo_rate_counter; // proplist


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions


//----------------------------------------------------------------------------------------------------------------
//
// charging the weapon

private func StartCharge(object user, int x, int y)
{
	var firemode = GetFiremode();

	if (firemode == nil)
	{
		FatalError(Format("Fire mode '%s' not supported", firemode));
	}

	if (!is_using || firemode.delay_charge < 1 || !NeedsCharge(user, firemode)) return false;
	
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
				if (DoCharge(user, x, y, firemode))
				{
					return false; // fire away
				}
				else
				{
					return true; // keep charging, because someone overrided DoCharge()
				}
			}
			else
			{
				if (effect.progress > 0)
				{
					OnProgressCharge(user, x, y, firemode, effect.percent, effect.progress);
					effect.percent_old = effect.percent;
				}
				return true; // keep charging
			}
		}
		else
		{
			CancelCharge(user, x, y, firemode, false);
		}
	}

	AddEffect("IntCharge", this, 1, 1, this, nil, user, firemode);
	OnStartCharge(user, x, y, firemode);
	return true; // keep charging
}

private func CancelCharge(object user, int x, int y, proplist firemode, bool callback)
{
	var effect = IsCharging();
	
	if (effect != nil)
	{
		if (callback) OnCancelCharge(effect.user, x, y, effect.firemode);
		
		RemoveEffect(nil, nil, effect);
	}
}

private func DoCharge(object user, int x, int y, proplist firemode)
{
	OnFinishCharge(user, x, y, firemode);
	return true;
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
public func OnStartCharge(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: the weapon has successfully charged. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnFinishCharge(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: the weapon has successfully charged. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @par current_percent The progress of charging, in percent.
 @par change_percent The change since the last update, in percent.
 @version 0.2.0
 @note The function existed in version 0.1.0 too, passing {@code change_percent} in place
       of {@code current_percent}.
 */
public func OnProgressCharge(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
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
	
	if (effect == nil)
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
public func OnCancelCharge(object user, int x, int y, proplist firemode)
{
}

private func FxIntChargeStart(object target, proplist effect, int temp, object user, proplist firemode)
{
	if (temp) return;
	
	effect.user = user;
	effect.firemode = firemode;
	effect.percent_old = 0;
}

private func FxIntChargeTimer(object target, proplist effect, int time)
{
	effect.percent = BoundBy(time * 100 / effect.firemode.delay_charge, 0, 100);
	effect.progress = effect.percent - effect.percent_old;

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
	ammo_rate_counter = {};
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


public func GetAnimationSet()
{
	var firemode = GetFiremode();

	var anim_shoot_name = nil;
	var anim_shoot_time = nil;
	var anim_load_name = nil;
	var anim_load_time = nil;
	var anim_walk_speed_front = nil;
	var anim_walk_speed_back = nil;

	if (firemode)
	{
		anim_shoot_name = firemode.anim_shoot_name;
		anim_shoot_time = firemode.delay_recover;
		anim_load_name = firemode.anim_load_name;
		anim_load_time = firemode.delay_reload;
		anim_walk_speed_front = firemode.walk_speed_front;	
		anim_walk_speed_back = firemode.walk_speed_back;
	}

	return {
		AimMode        = AIM_Position, // The aiming animation is done by adjusting the animation position to fit the angle
		AnimationAim   = "MusketAimArms",
		AnimationLoad  = anim_load_name,
		LoadTime       = anim_load_time,
		AnimationShoot = anim_shoot_name,
		ShootTime      = anim_shoot_time,
		WalkSpeed      = anim_walk_speed_front,
		WalkBack       = anim_walk_speed_back,
	};
}

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
	
	CancelCharge(user, x, y, GetFiremode(), true);
	CancelReload(user, x, y, GetFiremode(), true);
	
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
		if (!StartReload(user, x, y))
		if (!StartCharge(user, x, y))
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
	
	var firemode = GetFiremode();
	
	if (firemode == nil)
	{
		FatalError(Format("Fire mode '%s' not supported", firemode));
	}
	
	if (HasAmmo(firemode))
	{
		var angle = GetFireAngle(x, y, firemode);
	
		FireSound(user, firemode);
		FireEffect(user, angle, firemode);
	
		FireProjectiles(user, angle, firemode);
	//	AddDeviation();
	
		FireRecovery(user, x, y, firemode);
	}
	else
	{
		this->OnNoAmmo(user, firemode);
	}
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
	for (var i = 0; i < Max(1, GetProjectiles(firemode)); i++)
	{
		var projectile = CreateObject(firemode.projectile_id, x, y, user->GetController());
	
		projectile->Shooter(user)
				  ->Weapon(this)
				  ->DamageAmount(firemode.damage)
		          ->DamageType(firemode.damage_type)
		          ->Velocity(SampleValue(firemode.projectile_speed))
				  ->Range(SampleValue(firemode.projectile_range));

		OnFireProjectile(user, projectile, firemode);
		projectile->Launch(angle, GetSpread(firemode));
	}
	
	shot_counter[firemode.name]++;
	ammo_rate_counter[firemode.name]--;
	
	HandleAmmoUsage(firemode);
}

private func GetProjectiles(proplist firemode)
{
	return firemode.projectile_number;
}

private func GetSpread(proplist firemode)
{
	if (firemode.spread || firemode.projectile_spread)
	{
		return NormalizeDeviations([firemode.spread, firemode.projectile_spread]);
	}
	else
	{
		return nil;
	}
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

private func EffectMuzzleFlash(object user, int x, int y, int angle, int size, bool sparks, bool light, int color, string particle)
{
	if (user == nil)
	{
		FatalError("This function expects a user that is not nil");
	}
	
	particle = particle ?? "MuzzleFlash";
	
	var r, g, b;
	if (color == nil)
	{
		r = g = b = 255;
	}
	else
	{
		r = GetRGBaValue(color, RGBA_RED);
		g = GetRGBaValue(color, RGBA_GREEN);
		b = GetRGBaValue(color, RGBA_BLUE);
	}

	user->CreateParticle(particle, x, y, 0, 0, 10, {Prototype = Particles_MuzzleFlash(), Size = size, Rotation = angle, R = r, G = g, B = b}, 1);

	if (sparks)
	{
		var xdir = +Sin(angle, size * 2);
		var ydir = -Cos(angle, size * 2);
	
		CreateParticle("StarFlash", x, y, PV_Random(xdir - size, xdir + size), PV_Random(ydir - size, ydir + size), PV_Random(20, 60), Particles_Glimmer(), size);
	}

	if (light)
	{
		user->CreateTemporaryLight(x, y)->LightRangeStart(3 * size)->SetLifetime(2)->Color(color)->Activate();
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
public func OnRecovery(object user, proplist firemode)
{
	
}

/**
 The weapon is ready to fire a shot.
 @version 0.1.0
 */
private func IsReadyToFire()
{
	return !IsRecovering()
	    && !IsCoolingDown()
	    && !IsWeaponLocked();
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


/**
 Gets a sample of a random value.
 
 @par value The value. This can be a {@code C4V_Int}, or {@code C4V_Array}. 
 @return int The sampled value. This is either the {@code value}, if {@code C4V_Int}
         was passed, or if an array was passed: a random value between
         {@code value[0]} and {@code value[1]}, where the possible increments are
         {@code value[2]}.  
 */
private func SampleValue(value)
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
 */
public func GetFiremode()
{
}


public func GetFiremodes()
{
	return fire_modes;
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

//----------------------------------------------------------------------------------------------------------------
//
// ammo management

/**
 Checks whether the weapon has ammo.
 
 @par firemode The ammo type for this firemode is checked.
 @return bool Returns {@code true} if the weapon has enough ammo for the firemode
 @version 0.2.0
 */
public func HasAmmo(proplist firemode)
{
	return GetAmmo(firemode) >= firemode.ammo_usage // enough ammo for the firemode?
	    || ammo_rate_counter[firemode.name] > 0;    // or ammo left from previously using the weapon?
}


/**
 Overrides func Fx{@link Library_AmmoManager#GetAmmo}, so that you can ask theDamage(obj, effect)
 amount of ammunition for a specific firemode.
 
 @par type_or_firemode You can pass an ID as in the original implementation,
                       or you can pass a firemode. If you pass {@code nil}
                       the value for {@link Library_Weapon#func GetFiremode} is
                       requested. The method will fail if the proplist is not a
                       firemode.
 @return int The current amount of ammunition for an ID or firemode.
 @version 0.2.0
 */
public func GetAmmo(type_or_firemode)
{
	if (GetType(type_or_firemode) == C4V_Def)
	{
		return _inherited(type_or_firemode, ...);
	}
	else if (GetType(type_or_firemode) == C4V_PropList)
	{
		return _inherited(type_or_firemode.ammo_id, ...);
	}
	else if (GetType(type_or_firemode) == C4V_Nil)
	{
		var fm = GetFiremode();
		if (fm == nil)
		{
			FatalError("Cannot get firemode!");
		}
		else
		{
			return GetAmmo(fm);
		}
	}
	else
	{
		FatalError("You have to specify an id or proplist (firemode), but you specified %v", GetType(type_or_firemode));
	}
}


/**
 Callback: The weapon has no ammo during {@link Library_Weapon#DoFireCycle},
           see {@link Library_Weapon#HasAmmo}.

 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnNoAmmo(object user, proplist firemode)
{
}


/**
 Callback: The weapon ammo in the weapon changes.

 @par ammo_type The ammo that was affected.
 @version 0.2.0
 */
public func OnAmmoChange(id ammo_type)
{
}


protected func HandleAmmoUsage(proplist firemode)
{
	// default values
	var rate = firemode.ammo_rate ?? 1;
	var ammo_type = firemode.ammo_id;
	var ammo_requested = firemode.ammo_usage ?? 1;

	// status
	var ammo_changed = false;
	var enough_ammo = true;

	// only use actual ammo if there is no spare ammo per ammo rate
	if (ammo_rate_counter[firemode.name] <= 0)
	{
		var ammo_available = GetAmmo(ammo_type);
		
		// cancel if not enough ammo
		if (ammo_available < ammo_requested)
		{
			enough_ammo = false;
		}
		else
		{
			// undo if something went wrong
			var ammo_received = Abs(DoAmmo(ammo_type, -ammo_requested));
			if (ammo_received < ammo_requested)
			{
				DoAmmo(ammo_type, ammo_received);
				enough_ammo = false;
			}
			else // everything ok: fill up the spare ammo per rate, signal that something has changed
			{
				ammo_changed = true;
				ammo_rate_counter[firemode.name] += rate;
			}
		}
	}

	if (ammo_changed)
	{
		this->OnAmmoChange(ammo_type);
	}
}


//----------------------------------------------------------------------------------------------------------------
//
// Locking the weapon

/**
 Locks the weapon against interaction.

 @par lock_time The weapon will be locked for this many frames. On a lock time of {@code 0}
                the weapon stays locked until you call {@link Library_Weapon#UnlockWeapon}
 @version 0.2.0
 */
public func LockWeapon(int lock_time)
{
	var effect = IsWeaponLocked();
	if (effect == nil)
	{
		AddEffect("IntWeaponLocked", this, 1, lock_time, this, nil);
	}
	else
	{
		effect.Interval = lock_time;
	}
}

/**
 Unlocks the weapon, so that it can be interacted with

 @version 0.2.0
 */
public func UnlockWeapon()
{
	var effect = IsWeaponLocked();
	if (effect) RemoveEffect(nil, this, effect);
}

private func IsWeaponLocked()
{
	return GetEffect("IntWeaponLocked", this);
}

//----------------------------------------------------------------------------------------------------------------
//
// Reloading the weapon

private func StartReload(object user, int x, int y)
{
	var firemode = GetFiremode();

	if (firemode == nil)
	{
		FatalError(Format("Fire mode '%s' not supported", firemode));
	}

	if (!is_using || !NeedsReload(user, firemode)) return false;
	
	var effect = IsReloading();
	
	if (effect != nil)
	{
		if (effect.user == user && effect.firemode == firemode)
		{
			effect.x = x;
			effect.y = y;

			if (effect.has_reloaded)
			{
				return false; // fire away
			}
			else
			{
				return true; // keep reloading
			}
		}
		else
		{
			CancelReload(user, x, y, firemode, false);
		}
	}

	if (CanReload(user, firemode))
	{
		AddEffect("IntReload", this, 1, 1, this, nil, user, firemode);
		OnStartReload(user, x, y, firemode);
	}
	return true; // keep reloading
}

private func CancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
	var effect = IsReloading();
	
	var auto_reload = firemode.auto_reload && requested_by_user; 

	if (effect != nil)
	{
		OnCancelReload(effect.user, x, y, effect.firemode, requested_by_user);

		if (!auto_reload) RemoveEffect(nil, nil, effect);
	}
}

private func DoReload(object user, int x, int y, proplist firemode)
{
	RemoveEffect(nil, this, IsReloading());
	OnFinishReload(user, x, y, firemode);
	return true;
}

private func IsReloading()
{
	return GetEffect("IntReload", this);
}

/**
 Condition when the weapon can be reloaded.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default. Overload this function
         for a custom condition.
 @version 0.2.0
 */
public func CanReload(object user, proplist firemode)
{
	return true;
}

/**
 Condition when the weapon needs to be reloaded.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c false} by default. Overload this function
         for a custom condition.
 @version 0.2.0
 */
public func NeedsReload(object user, proplist firemode)
{
	return false;
}

/**
 Callback: the weapon starts reloading. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnStartReload(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: the weapon has successfully reloaded. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnFinishReload(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: the weapon has successfully reloaded. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @par current_percent The progress of reloading, in percent.
 @par change_percent The change of progress, since the last update.
 @version 0.2.0
 */
public func OnProgressReload(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
{
}

/**
 Gets the current status of the reloading process.
 @return A value of 0 to 100, if the weapon is reloading.@br
         If the weapon is not reloading, this function returns -1.
 */
public func GetReloadProgress()
{
	var effect = IsReloading();
	
	if (effect == nil)
	{
		return -1;
	}
	else
	{
		return effect.percent;
	}
}

/**
 Callback: the weapon user cancelled reloading. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @par requested_by_user Is {@code true} if the user releases the use button while
                        the weapon is reloading. Otherwise, for example if the
                        user changes the parameter is {@code false}.
 @version 0.2.0
 */
public func OnCancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
}

private func FxIntReloadStart(object target, proplist effect, int temp, object user, proplist firemode)
{
	if (temp) return;
	
	effect.user = user;
	effect.firemode = firemode;
	effect.percent_old = 0;
}

private func FxIntReloadTimer(object target, proplist effect, int time)
{
	effect.percent = BoundBy(time * 100 / effect.firemode.delay_reload, 0, 100);
	effect.progress = effect.percent - effect.percent_old;

	if (time > effect.firemode.delay_reload && !effect.is_reloaded)
	{
		effect.is_reloaded = true;
		
		if (target->DoReload(effect.user, effect.x, effect.y, effect.firemode))
		{
			effect.has_reloaded = true;
			return FX_Execute_Kill;
		}
	}

	if (effect.progress > 0)
	{
		target->OnProgressReload(effect.user, effect.x, effect.y, effect.firemode, effect.percent, effect.progress);
		effect.percent_old = effect.percent;
	}
}
