/**
	Shared functions between all firearms.

	@note Firemodes
	A firemode has three stages: Charge - (Fire/Recover) - Cooldown
	@author Marky
	@credits Hazard Team, Zapper
	@version 0.1.0
*/

/*-- Important Library Properties --*/

local is_using = false;    // bool: is the user holding the fire button

static const WEAPON_Firemode_Default = "default";

static const WEAPON_FM_Single	= 1;
static const WEAPON_FM_Burst 	= 2;
static const WEAPON_FM_Auto 	= 3;


static const WEAPON_PR_Bullet = 1;
static const WEAPON_PR_Ballistic = 2;
static const WEAPON_PR_Hitscan = 3;

local fire_modes = [fire_mode_default];

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

local weapon_properties = 
{
	gfx_distance = 6,
	gfx_offset_y = -6,
};

local shot_counter; // proplist
local ammo_rate_counter; // proplist

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

/*-- Engine Callbacks --*/

func Initialize()
{
	shot_counter = {};
	ammo_rate_counter = {};
	_inherited(...);
}

/*-- Controls --*/

/**
 This is executed each time the user presses the fire button.@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnPressUse}@br
 - tell the user to start aiming@br
 - call {@link Library_Firearm#ControlUseHolding}@br
 - call {@link Library_Firearm#Fire}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseStart(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	if (this->OnPressUse(user, x, y))
		return true;

	user->StartAim(this);

	ControlUseHolding(user, x, y);

	//if(!weapon_properties.delay_shot && !weapon_properties.full_auto)
	//	Fire(user, x, y); //user->GetAimPosition());
	return true;
}

/**
 This is executed each time the user presses the alternative use button (must be defined, not standard in OpenClonk).@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnPressUseAlt} (which should define some kind of behaviour)@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseAltStart(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	this->OnPressUseAlt(user, x, y);

	return true;
}

/**
 This is executed regularly while the user is holding the primary use button.@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnHoldingUse}@br
 - call {@link Library_Firearm#ControlFireHolding}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseHolding(object user, int x, int y)
{
	if (this->OnHoldingUse(user, x, y))
		return true;

	return ControlFireHolding(user, x, y);
}

/**
 This should be executed while the user is holding the fire button.@br@br

 The function does the following:@br
 - check if the {@link Library_Firearm#RejectUse} is true, if it is, {@link Library_Firearm#ControlUseStop} is called@br
 - call {@link Library_Firearm#DoFireCycle}
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aiming at. Relative to the user.
 @version 0.2.0
 */
public func ControlFireHolding(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	if (this->~RejectUse(user))
	{
		ControlUseStop(user, x, y);
		return false;
	}

	DoFireCycle(user, x, y, true);

	return true;
}

/**
 This is executed regularly while the user is holding the alternative use button (must be defined, not standard in OpenClonk).@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnHoldingUseAlt} (which should define some kind of behaviour)@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseAltHolding(object user, int x, int y)
{
	this->OnHoldingUseAlt(user, x, y);

	return true;
}

/**
 This is executed when the user stops holding the fire button.@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnUseStop}@br
 - call {@link Library_Firearm#CancelUsing}@br
 - call {@link Library_Firearm#CancelCharge}@br
 - call {@link Library_Firearm#CancelReload}@br
 - check if the weapon is not {@link Library_Firearm#IsRecovering} and if not, call {@link Library_Firearm#CheckCooldown}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseStop(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	if (this->OnUseStop(user, x, y))
		return true;

	CancelUsing();

	CancelCharge(user, x, y, GetFiremode(), true);
	CancelReload(user, x, y, GetFiremode(), true);

	if (!IsRecovering())
	{
		CheckCooldown(user, GetFiremode());
	}

	return true;
}

/**
 This is executed when the user stops holding the alternative use button (must be defined, not standard in OpenClonk).@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnStopUseAlt}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseAltStop(object user, int x, int y)
{
	this->OnStopUseAlt(user, x, y);

	return true;
}

/**
 This is executed when the user cancels the fire procedure (usually by pressing a dedicated cancel button).@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnUseCancel}@br
 - call {@link Library_Firearm#ControlUseStop}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseCancel(object user, int x, int y)
{
	if (this->OnUseCancel(user, x, y))
		return true;

	return ControlUseStop(user, x, y);
}

/**
 This is executed when the user cancels the alternative use (usually by pressing a dedicated cancel button; must be defined, not standard in OpenClonk).@br@br

 The function does the following:@br
 - call {@link Library_Firearm#OnCancelUseAlt}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
public func ControlUseAltCancel(object user, int x, int y)
{
	this->OnUseAltCancel(user, x, y);

	return true;
}

/**
 Sets is_using to false.
 @version 0.2.0
*/
public func CancelUsing()
{
	is_using = false;
}

/**
 Must return true if the weapon wants to receive holding updates for controls.
 @version 0.1.0
*/
public func HoldingEnabled() { return true; }

/**
 Callback: Pressed the regular use button (fire). Called by {@link Library_Firearm#ControlUseStart}.@br@br

 If return value is true, the regular execution of {@link Library_Firearm#ControlUseStart} is skipped.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnPressUse(object user, int x, int y)
{
}

/**
 Callback: The regular use button (fire) is held. Called in regular intervals by {@link Library_Firearm#ControlUseHolding}.@br@br

 If return value is true, the regular execution of {@link Library_Firearm#ControlUseHolding} is skipped.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnHoldingUse(object user, int x, int y)
{
}

/**
 Callback: Released the regular use button (fire). Called by {@link Library_Firearm#ControlUseStop}.@br@br

 If return value is true, the regular execution of {@link Library_Firearm#ControlUseStop} is skipped.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnUseStop(object user, int x, int y)
{
}

/**
 Callback: Use (fire) was cancelled. Called by {@link Library_Firearm#ControlUseCancel}.@br@br

 If return value is true, the regular execution of {@link Library_Firearm#ControlUseCancel} is skipped.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnUseCancel(object user, int x, int y)
{
}

/**
 Callback: Pressed the alternative use button. Called by {@link Library_Firearm#ControlUseAltStart}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnPressUseAlt(object user, int x, int y)
{
}

/**
 Callback: The alternative use button is held. Called in regular intervals by {@link Library_Firearm#ControlUseAltHolding}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnHoldingUseAlt(object user, int x, int y)
{
}

/**
 Callback: Released the alternative use button. Called by {@link Library_Firearm#ControlUseAltStop}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnUseAltStop(object user, int x, int y)
{
}

/**
 Callback: Alternative use was cancelled. Called by {@link Library_Firearm#ControlUseAltCancel}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnUseAltCancel(object user, int x, int y)
{
}

/*-- Charging --*/

/**
 Will start the charging process if the weapon needs charging. Can be called multiple times even if already charging to check if the charge process is done.@br@br

 This function does the following:
 - check if charging is needed ({@link Library_Firearm#NeedsCharge}).@br
 - check if there is a charging effect running ({@link Library_Firearm#IsCharging}).@br
 - if yes, check if that effect is still in the process of charging.@br
 - if no, create a charging effect and call {@link Library_Firearm#OnStartCharge}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @return {@c true} if any kind of charging process is happening or charging is for some reason hampered. In this case, nothing should happen otherwise. {@c false} if no charging is necessary at the moment.
 @version 0.2.0
*/
func StartCharge(object user, int x, int y)
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
					return true; // keep charging, because someone overrode DoCharge()
				}
			}
			else
			{
				if (effect.progress > 0)
				{
					this->OnProgressCharge(user, x, y, firemode, effect.percentage, effect.progress);
					effect.percent_old = effect.percentage;
				}
				return true; // keep charging
			}
		}
		else
		{
			CancelCharge(user, x, y, firemode, false);
		}
	}

	CreateEffect(IntChargeEffect, 1, 1, user, firemode);
	this->OnStartCharge(user, x, y, firemode);
	return true; // keep charging
}

/**
 Will cancel the charging process currently running. Is automatically called by {@link Library_Firearm#StartCharge} if the user or firemode changed during a charging process.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @par callback Set to true to call {@link Library_Firearm#OnCancelCharge}.
 @version 0.2.0
*/
func CancelCharge(object user, int x, int y, proplist firemode, bool callback)
{
	var effect = IsCharging();

	if (effect != nil)
	{
		if (callback) this->OnCancelCharge(effect.user, x, y, effect.firemode);

		RemoveEffect(nil, nil, effect);
	}
}

/**
 Called by {@link Library_Firearm#StartCharge} if charging should be finished. If it returns false, the firing process holds and assumes that something else needs to be done.@br@br

 Calls {@link Library_Firearm#OnFinishCharge}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default.
 @version 0.2.0
*/
func DoCharge(object user, int x, int y, proplist firemode)
{
	this->OnFinishCharge(user, x, y, firemode);
	return true;
}

/**
 Checks if the weapon is currently charging.@br
 @return The charging effect.
 @version 0.2.0
*/
func IsCharging()
{
	return GetEffect("IntChargeEffect", this);
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
		return -1;
	else
		return effect.percent;
}

/**
 Condition if the weapon needs to be charged.@br
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default. Overload this function for a custom condition.
 @version 0.1.0
 */
public func NeedsCharge(object user, proplist firemode)
{
	return true;
}

/**
 Callback: the weapon starts charging. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnStartCharge(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: the weapon has successfully charged. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @version 0.1.0
 */
public func OnFinishCharge(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: called each time during the charging process if the percentage of the charge progress changed. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @par current_percent The progress of charging, in percent.
 @par change_percent The change of progress, since the last update.
 @version 0.2.0
 @note The function existed in version 0.1.0 too, passing {@code change_percent} in place of {@code current_percent}.
 */
public func OnProgressCharge(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
{
}

/**
 Callback: the weapon user cancelled charging. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnCancelCharge(object user, int x, int y, proplist firemode)
{
}

local IntChargeEffect = new Effect {
	Construction = func(object user, int x, int y, proplist firemode)
	{
		this.user = user;
		this.firemode = firemode;

		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.is_charged = false;
		this.has_charged = false;
	},
	Timer = func(int time)
	{
		// Increase progress percentage depending on the charging delay of the firemode
		this.percentage = BoundBy(time * 100 / this.firemode.delay_charge, 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the charging process is finished based on the charging delay of the firemode
		if (time > this.firemode.delay_charge && !this.is_charged)
		{
			this.is_charged = true;
			// Do not make subsequent calls of the timer function because every-frame-effects do hurt performance
			this.Interval = 0;
			// However, keep the effect, to track that this weapon is now charged
		}
	}
};

/*-- Firing --*/

/**
 The weapon is ready to fire a shot.
 @version 0.1.0
 */
func IsReadyToFire()
{
	return !IsRecovering()
	    && !IsCoolingDown()
	    && !IsWeaponLocked();
}

/**
 This function will go through the fire cycle: reloading, charging, firing and recovering, checking for cooldown.@br@br

 This function does the following:@br
 - set the aiming angle for the user@br
 - check {@link Library_Firearm#IsReadyToFire}@br
 - check {@link Library_Firearm#StartReload}@br
 - check {@link Library_Firearm#StartCharge}@br
 - if all of the above check out, call {@link Library_Firearm#Fire}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par is_pressing_trigger Should be true if the fire button is held to indicate the is_using state.
 @version 0.2.0
*/
func DoFireCycle(object user, int x, int y, bool is_pressing_trigger)
{
	var angle = GetAngle(x, y);
	user->~SetAimPosition(angle);

	if (is_pressing_trigger)
		is_using = true;

	if (IsReadyToFire())
		if (!StartReload(user, x, y))
			if (!StartCharge(user, x, y))
				Fire(user, x, y);
}

/**
 Converts coordinates to an aiming angle for the weapon.
 @par x The x coordinate, local.
 @par y The y coordinate, local.
 @return int The angle in degrees, normalized to the range of -180° to 180°.
 @version 0.1.0
 */
func GetAngle(int x, int y)
{
	var angle = Angle(0, weapon_properties.gfx_offset_y, x, y);
		angle = Normalize(angle, -180);

	return angle;
}

func GetFireAngle(int x, int y, proplist firemode)
{
	var angle = Angle(0, firemode.projectile_offset_y, x, y);
		angle = Normalize(angle, -180);

	return angle;
}

/**
 Fires the weapon.@br@br

 The function does the following:@br
 - check ammo ({@link Library_Firearm#HasAmmo}) for the selected firemode (should be fine if this was called through {@link Library_Firearm#DoFireCycle)).@br
 - call {@link Library_Firearm#OnNoAmmo} if no ammunition was found.@br
 - call {@link Library_Firearm#FireSound}.@br
 - call {@link Library_Firearm#FireEffect}.@br
 - call {@link Library_Firearm#FireProjectiles}.@br
 - call {@link Library_Firearm#FireRecovery}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @version 0.1.0
 */
func Fire(object user, int x, int y)
{
	if (user == nil)
		FatalError("The function expects a user that is not nil");

	var firemode = GetFiremode();

	if (firemode == nil)
		FatalError(Format("Fire mode '%s' not supported", firemode));

	if (HasAmmo(firemode))
	{
		var angle = GetFireAngle(x, y, firemode);

		FireSound(user, firemode);
		FireEffect(user, angle, firemode);
		FireProjectiles(user, angle, firemode);
		FireRecovery(user, x, y, firemode);
	}
	else
	{
		this->OnNoAmmo(user, firemode);
	}
}

func RejectUse(object user)
{
	return !IsWeaponReadyToUse(user) || !IsUserReadyToUse(user);
}

/**
 Interface for signaling that the weapon is ready to use (attack).
 @par user The object that is trying to use this weapon. 
 @return true, if the object is ready to use. By default this is true,
         if the weapon is contained in the user.
 */
func IsWeaponReadyToUse(object user)
{
	return Contained() == user;
}

/**
 Interface for signaling that the user is ready to use (attack).
 @par user The object that is trying to use this weapon. 
 @return true, if the object is ready to use.
 */
func IsUserReadyToUse(object user)
{
	return user->HasHandAction();
}

func FireProjectiles(object user, int angle, proplist firemode)
{
	if (user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}
	if (firemode == nil)
	{
		FatalError("The function expects a fire mode that is not nil");
	}
	
	var user_x = user->~GetWeaponX(this); if (user_x) user_x -= GetX();
	var user_y = user->~GetWeaponY(this); if (user_y) user_y -= GetY();

	var x = +Sin(angle, firemode.projectile_distance) + user_x;
	var y = -Cos(angle, firemode.projectile_distance) + user_y + firemode.projectile_offset_y;

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

		this->OnFireProjectile(user, projectile, firemode);
		projectile->Launch(angle, GetSpread(firemode));
	}
	
	shot_counter[firemode.name]++;
	ammo_rate_counter[firemode.name]--;
	
	HandleAmmoUsage(firemode);
}

func GetProjectiles(proplist firemode)
{
	return firemode.projectile_number;
}

func GetSpread(proplist firemode)
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

func EffectMuzzleFlash(object user, int x, int y, int angle, int size, bool sparks, bool light, int color, string particle)
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

/*-- Recovering --*/

func FireRecovery(object user, int x, int y, proplist firemode)
{
	var delay;
	if (!NeedsRecovery(user, firemode))
	{
		delay = firemode.delay_recover;
	}
	else
	{
		delay = 1;
	}

	var recovery = AddEffect("IntRecovery", this, 1, delay, this, nil, user, x, y, firemode);
	recovery.delay = delay;
}

/**
 Condition when the weapon needs to be recover after firing.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default. Overload this function
         for a custom condition.
 @version 0.2.0
 */
public func NeedsRecovery(object user, proplist firemode)
{
	return true;
}

func FxIntRecoveryStart (object target, proplist effect, int temporary, object user, int x, int y, proplist firemode)
{
	if (temporary) return;
	
	effect.user = user;
	effect.x = x;
	effect.y = y;
	effect.firemode = firemode;
	effect.start = FrameCounter();
}

func FxIntRecoveryTimer(object target, proplist effect, int time)
{
	target->DoRecovery(effect.user, effect.x, effect.y, effect.firemode);

	return FX_Execute_Kill;
}

func CancelRecovery()
{
	var effect = IsRecovering();
	
	if (effect != nil)
	{
		RemoveEffect(nil, nil, effect);
	}
}

func GetRecoveryProgress()
{
	var recovery = IsRecovering();
	if (recovery)
	{
		var progress = BoundBy(FrameCounter() - recovery.start, 0, recovery.delay);
		return progress * 100 / recovery.delay;
	}
	else
	{
		return 0;
	}
}

func IsRecovering()
{
	return GetEffect("IntRecovery", this);
}

func DoRecovery(object user, int x, int y, proplist firemode)
{
	if (firemode == nil) return;
	
	this->OnRecovery(user, firemode);

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

func CheckCooldown(object user, proplist firemode)
{
	if (!HasAmmo(firemode) || RejectUse(user))
	{
		CancelUsing();
	}

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

/*-- Cooldown --*/

func StartCooldown(object user, proplist firemode)
{
	if (firemode.delay_cooldown < 1 || !NeedsCooldown(user, firemode))
	{
		this->OnSkipCooldown(user, firemode);
		return;
	}
	
	var effect = IsCoolingDown();

	if (effect == nil)
	{
		AddEffect("IntCooldown", this, 1, firemode.delay_cooldown, this, nil, user, firemode);
		this->OnStartCooldown(user, firemode);
	}
}

func DoCooldown(object user, proplist firemode)
{
	this->OnFinishCooldown(user, firemode);
}

func IsCoolingDown()
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

/**
 Callback: the weapon has skipped cooling down. Does nothing by default.
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnSkipCooldown(object user, proplist firemode)
{
}

func FxIntCooldownStart(object target, proplist effect, int temp, object user, proplist firemode)
{
	if (temp) return;

	effect.user = user;
	effect.firemode = firemode;
}

func FxIntCooldownTimer(object target, proplist effect, int time)
{
	target->DoCooldown(effect.user, effect.firemode);

	return FX_Execute_Kill;
}

/*-- Firemodes --*/

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
	if (!fire_modes)
	{
		FatalError("Fire modes is somehow empty??");
	}

	return fire_modes;
}

public func GetAvailableFiremodes()
{
	var available = [];
	
	for (var i = 0; i < GetLength(GetFiremodes()); ++i) // firemode in GetFiremodes())
	{
		var firemode = GetFiremodes()[i];
	
		var is_available = firemode.condition == nil || this->Call(firemode.condition);

		if (is_available)
		{
			PushBack(available, firemode);
		}
	}

	return available;
}

public func ClearFiremodes()
{
	fire_modes = [];
}

public func AddFiremode(proplist fire_mode)
{
	PushBack(fire_modes, fire_mode);
}

public func CanChangeFiremode()
{
	return !IsRecovering()
	    && !IsCharging()
	    && !IsReloading()
	    && !IsWeaponLocked();
}

/*-- Ammo --*/

/**
 Changes the amount of ammunition that the object currently has.
 @par ammo The type of the ammunition.
 @par amount The change, can be positive or negative.
             The amount of ammunition cannot be changed beyond the capacity
             of the object, so the actual amount by which the ammunition was
             changed will be returned. 
 @return The actual change that happened.
 @author Marky
 @version 0.3.0
 @note This function should be implemented by the weapon. A quick implementation
       is available by including {@link Library_AmmoManager}.
 @related {@link Library_AmmoManager#DoAmmo}, {@link Library_AmmoManager#SetAmmo}
 */
public func DoAmmo(id ammo, int amount)
{
	return _inherited(ammo, amount, ...);
}

/**
 Checks whether the weapon has ammo.
 
 @par firemode The ammo type for this firemode is checked.
 @return bool Returns {@code true} if the weapon has enough ammo for the firemode
 @version 0.2.0
 */
public func HasAmmo(proplist firemode)
{
	return this->GetAmmo(firemode) >= firemode.ammo_usage // enough ammo for the firemode?
	    || ammo_rate_counter[firemode.name] > 0     // or ammo left from previously using the weapon?
	    || this->GetAmmoSource(firemode) == AMMO_Source_Infinite; // or infinite ammo
}

/**
 Overrides func {@link Library_AmmoManager#GetAmmo}, so that you can ask the
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
		FatalError(Format("You have to specify an id or proplist (firemode), but you specified %v", GetType(type_or_firemode)));
	}
}

/**
 Overrides func {@link Library_AmmoManager#GetAmmoSource}, so that you can ask the
 ammo source for a specific firemode.
 
 @par type_or_firemode You can pass an ID as in the original implementation,
                       or you can pass a firemode. If you pass {@code nil}
                       the value for {@link Library_Weapon#func GetFiremode} is
                       requested. The method will fail if the proplist is not a
                       firemode.
 @return int The current source of ammunition for an ID or firemode.
 @version 0.2.0
 */
public func GetAmmoSource(type_or_firemode)
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
			return _inherited(fm, ...);
		}
	}
	else
	{
		FatalError(Format("You have to specify an id or proplist (firemode), but you specified %v", GetType(type_or_firemode)));
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

func HandleAmmoUsage(proplist firemode)
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

/*-- Locking --*/

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

func IsWeaponLocked()
{
	return GetEffect("IntWeaponLocked", this);
}

/*-- Reloading --*/

/**
 Will start the reloading process if the weapon needs reloading. Can be called multiple times even if already reloading to check if the reload process is done.@br@br

 This function does the following:
 - check if a reload is needed ({@link Library_Firearm#NeedsReload}).@br
 - check if there is a reloading effect running ({@link Library_Firearm#IsReloading}).@br
 - if yes, check if that effect is still in the process of reloading.@br
 - if no, check if reloading is possible ({@link Library_Firearm#CanReload}).@br
 - if yes, create a reloading effect and call {@link Library_Firearm#OnStartReload}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par forced If true, the weapon will reload even if it is currently not in use ({@link Library_Firearm#IsInUse}).@br
 @return {@c true} if any kind of reloading process is happening or reloading is for some reason hampered. In this case, nothing should happen otherwise. {@c false} if no reloading is necessary at the moment.
 @version 0.2.0
*/
func StartReload(object user, int x, int y, bool forced)
{
	var firemode = GetFiremode();

	if (firemode == nil)
	{
		FatalError(Format("Fire mode '%s' not supported", firemode));
	}

	if ((!is_using && !forced) || !NeedsReload(user, firemode)) return false;

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
		CreateEffect(IntReloadEffect, 1, 1, user, x, y, firemode);
		this->OnStartReload(user, x, y, firemode);
	}

	return true; // keep reloading
}

/**
 Will cancel the reloading process currently running. Is automatically called by {@link Library_Firearm#StartReload} if the user or firemode changed during a reloading process.@br@br

 Calls {@link Library_Firearm#OnCancelReload}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @par requested_by_user Set to true, if the stopping was a user's choice (usually when the fire button was released). If true, auto reloading weapon will also stop reloading.
 @version 0.2.0
*/
func CancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
	var effect = IsReloading();

	var auto_reload = firemode.auto_reload && requested_by_user;

	if (effect != nil)
	{
		this->OnCancelReload(effect.user, x, y, effect.firemode, requested_by_user);

		if (!auto_reload) RemoveEffect(nil, nil, effect);
	}
}

/**
 Called by the reloading effect if reloading should be finished. If it returns false, the reloading effect will linger and assumes that something else needs to be done. If it returns true, the reloading effect will end.@br@br

 Calls {@link Library_Firearm#OnFinishReload}.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default.
*/
func DoReload(object user, int x, int y, proplist firemode)
{
	this->OnFinishReload(user, x, y, firemode);
	return true;
}

/**
 Checks if the weapon is currently reloading.@br
 @return The reloading effect.
 @version 0.2.0
*/
func IsReloading()
{
	return GetEffect("IntReloadEffect", this);
}

/**
 Gets the current status of the reloading process.@br
 @return A value of 0 to 100, if the weapon is reloading.@br
         If the weapon is not reloading, this function returns -1.
 */
public func GetReloadProgress()
{
	var effect = IsReloading();

	if (effect == nil)
		return -1;
	else
		return effect.percentage;
}

/**
 Condition if the weapon can be reloaded.@br
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c true} by default. Overload this function for a custom condition.
 @version 0.2.0
 */
public func CanReload(object user, proplist firemode)
{
	return true;
}

/**
 Condition if the weapon needs to be reloaded.@br
 @par user The object that is using the weapon.
 @par firemode A proplist containing the fire mode information.
 @return {@c false} by default. Overload this function for a custom condition. Otherwise no reloading needs ever to be done.
 @version 0.2.0
 */
public func NeedsReload(object user, proplist firemode)
{
	return false;
}

/**
 Callback: the weapon starts reloading. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnStartReload(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: the weapon has successfully reloaded. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @version 0.2.0
 */
public func OnFinishReload(object user, int x, int y, proplist firemode)
{
}

/**
 Callback: called each time during the reloading process if the percentage of the reload progress changed. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @par current_percent The progress of reloading, in percent.
 @par change_percent The change of progress, since the last update.
 @version 0.2.0
 */
public func OnProgressReload(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
{
}

/**
 Callback: the weapon user cancelled reloading. Does nothing by default.@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at. Relative to the user.
 @par y The y coordinate the user is aimint at. Relative to the user.
 @par firemode A proplist containing the fire mode information.
 @par requested_by_user Is {@c true} if the user releases the use button while the weapon is reloading. Otherwise, for example if the user changes the firemode is {@c false}.
 @version 0.2.0
 */
public func OnCancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
}

local IntReloadEffect = new Effect {
	Construction = func(object user, int x, int y, proplist firemode)
	{
		this.user = user;
		this.x = x; // x and y will be updated by StartReload
		this.y = y;
		this.firemode = firemode;

		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.is_reloaded = false;
	},
	Timer = func(int time)
	{
		// Increase progress percentage depending on the reloading delay of the firemode
		this.percentage = BoundBy(time * 100 / this.firemode.delay_reload, 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the reloading process is finished based on the reloading delay of the firemode
		if (time > this.firemode.delay_reload && !this.is_reloaded)
		{
			this.is_reloaded = true;

			// Do the reload if anything is necessary and end the effect if successful
			if (this.Target->DoReload(this.user, this.x, this.y, this.firemode))
				return FX_Execute_Kill;
		}

		// Do a progress update if necessary
		if (this.progress > 0)
		{
			this.Target->OnProgressReload(this.user, this.x, this.y, this.firemode, this.percentage, this.progress);
			this.percent_old = this.progress;
		}
	}
};

/*-- Misc --*/

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

/**
 Gets a sample of a random value.
 
 @par value The value. This can be a {@code C4V_Int}, or {@code C4V_Array}. 
 @return int The sampled value. This is either the {@code value}, if {@code C4V_Int}
         was passed, or if an array was passed: a random value between
         {@code value[0]} and {@code value[1]}, where the possible increments are
         {@code value[2]}.  
 */
func SampleValue(value)
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

local Name = "$Name$";
local Description = "$Description$";