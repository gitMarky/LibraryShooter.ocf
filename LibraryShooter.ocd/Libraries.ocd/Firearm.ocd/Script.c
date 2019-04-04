﻿﻿﻿﻿/**
	Shared functions between all firearms.

	@note Basic workings
	When used, a weapon will go through several stages in the firing process that can be configured to taste:@br
	Charging the weapon comes first and effectively delays the firing of a shot. Example: a minigun that needs to spin up before firing.@br
	After that, one shot will be fired. A shot can, however, fire multiple projectiles at once (e.g. firing a shotgun).@br
	The weapon will then start the recovery process if needed. Recovery is the delay between two consecutive shots and is therefore only necessary for automatic or burst fire modes. The weapon will go over to firing shots again after recovery finished.@br
	Last, the cooldown procedure will start. Weapons cannot fire again until the cooldown has been finished. Example: A powerful railgun that needs some time to cool off after a shot.@br

	@note Fire modes
	Each weapon must define at least one fire mode. fire_mode_default provides an example of how these could look and should also be used as a Prototype, to provide default values.@br

	@note Reloading
	Reloading can be implemented via {@link Plugin_Firearm_ReloadLogic}, or a custom plugin that
	implements the functions
	<ul>
	<li>func IsReloading() with an effect return value</li>
	<li>
	</ul>

	@author Marky, Clonkonaut

	@credits Hazard Team and CMC developer team, Zapper for Caedes, each for inspirations on the architecture and requirements.
*/

/*-- Important Library Properties --*/

local is_using = false;    // bool: is the user holding the fire button

// Constants for distinguishing the fire behaviour
static const WEAPON_FM_Single = 1;
static const WEAPON_FM_Burst  = 2;
static const WEAPON_FM_Auto   = 3;

// Constants for distinguishing positions and FX positions
static const WEAPON_POS_Muzzle = "Muzzle";
static const WEAPON_POS_Chamber = "Chamber";
static const WEAPON_POS_Magazine = "Magazine";

/* --- Properties --- */

local weapon_properties = nil;

/* --- Settings --- */

/**
	An important general setting that should be decided on for as a whole in an implementation and best not be mixed throughout a pack.@br
	If returns true, {@link Library_Firearm#ControlUseStart} will initiate the aiming procedure. As long as the use button is not pressed, it is then assumed that the weapon is simply held ready.@br
	Single shot and burst fire modes will only fire on {@link Library_Firearm#ControlUseStop}, giving the user time to take aim as long as the use button is held.@br
	Automatic weapons will fire right away.@br
	If set to false, the weapon itself will not make the user aim and it must be initiated elsewhere, e.g. a clonk can always aim when the weapon is selected.@br
	You can use the Mouse1Move key assignment to continuously forward cursor updates to the script.@br

	@return {@code true} by default.
*/
public func Setting_AimOnUseStart()
{
	return true;
}


/**
	An important general setting that should be decided on for as a whole in an implementation and best not be mixed throughout a pack.
	@br
	If returns a definition other than 'nil' the weapon will use this definition as an aim manager interface,
	instead of relying on the "Library_AimManager" from Objects.ocd.

	@return {@code nil} by default.
*/
public func Setting_CustomAimManager()
{
	return nil;
}


/**
	Check if the weapon should fire on a call of {@link Library_Firearm#ControlUseHolding}.@br

	@return {@c true} if either {@link Library_Firearm#Setting_AimOnUseStart} is false or if the selected fire mode is an automatic one.
*/
func FireOnHolding()
{
	return !Setting_AimOnUseStart() || GetFiremode().mode == WEAPON_FM_Auto;
}


/**
	Check if the weapon should fire on a call of {@link Library_Firearm#ControlUseStop}.@br

	@return {@c true} if {@link Library_Firearm#Setting_AimOnUseStart} is true and if the selected fire mode is not an automatic one.
*/
func FireOnStopping()
{
	return Setting_AimOnUseStart() && GetFiremode().mode != WEAPON_FM_Auto;
}


/**
	If returns true, it is assumed that all functions regarding ammunition handling are configured as desired.@br
	Ammunition handling can be done by for example by including {@link Library_AmmoManager}@br

	@return {@link Global#inherited} by default.
*/
public func Setting_WithAmmoLogic()
{
	return _inherited();
}


/* --- Engine Callbacks --- */

/**
	Make sure to call this via _inherited();
*/
func Initialize()
{
	// Initialize custom property namespace
	weapon_properties = weapon_properties ?? {};
	weapon_properties.weapon_offset = weapon_properties.weapon_offset ?? {};
	weapon_properties.shot_counter = [];
	weapon_properties.firemode_selected = 0;
	weapon_properties.firemodes = [];
	
	// Editor properties
	this.EditorProps = this.EditorProps ?? {};
	this.EditorProps.DebugWeaponPositions = 
	{
		Type = "bool",
		Name = "$EditorDebugWeaponPosName$",
		EditorHelp = "$EditorDebugWeaponPosHelp$",
		AsyncGet = "EditorProps_GetDebugEnabled",
		Set = "EditorProps_SetDebugEnabled",
	};
	
	if (EditorProps_GetDebugEnabled())
	{
		CreateEffect(FxEditorPropsDebug, 1, 1);
	}

	// Default values
	_inherited();
}


/* --- Controls --- */

/**
	This is executed each time the user presses the fire button.@br@br

	The function does the following:@br
	- call {@link Library_Firearm#OnPressUse}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func ControlUseStart(object user, int x, int y)
{
	AssertNotNil(user);
	this->OnPressUse(user, x, y);
	return true;
}


/**
	This is executed each time the user presses the alternative use button (must be defined, not standard in OpenClonk).@br@br

	The function does the following:@br
	- call {@link Library_Firearm#OnPressUseAlt} (which should define some kind of behaviour)@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func ControlUseAltStart(object user, int x, int y)
{
	AssertNotNil(user);
	this->OnPressUseAlt(user, x, y);
	return true;
}


/**
	This is executed regularly while the user is holding the primary use button.@br@br

	The function does the following:@br
 	- call {@link Library_Firearm#OnHoldingUse}@br
	- call {@link Library_Firearm#StartFireCycle}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func ControlUseHolding(object user, int x, int y)
{
	if (this->OnHoldingUse(user, x, y))
	{
		return true;
	}
	else
	{
		return StartFireCycle(user, x, y);
	}
}


/**
	This should be executed while the user is holding the fire button.@br@br

	The function does the following:@br
	- check if the {@link Library_Firearm#RejectUse} is true, if it is, {@link Library_Firearm#ControlUseStop} is called@br
	- start aiming if {@link Library_Firearm#Setting_AimOnUseStart} is set.@br
	- call {@link Library_Firearm#DoFireCycle}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aiming at. Relative to the user.
 */
public func StartFireCycle(object user, int x, int y)
{
	AssertNotNil(user);

	if (this->~RejectUse(user))
	{
		CancelFireCycle(user, x, y);
		return false;
	}
	
	DoStartAiming(user);
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
 */
public func ControlUseAltHolding(object user, int x, int y)
{
	this->OnHoldingUseAlt(user, x, y);
	return true;
}


/**
	This is executed when the user stops holding the fire button.@br@br

	The function does the following:@br
	- call {@link Library_Firearm#OnUseStop}, if that returns {@code true} abort here@br
	- call {@link Library_Firearm#CancelFireCycle}

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func ControlUseStop(object user, int x, int y)
{
	AssertNotNil(user);

	if (this->OnUseStop(user, x, y))
	{
		return true;
	}

	CancelFireCycle(user, x, y);
	return true;
}


/**
	This is executed when the user stops the fire cycle, holding the fire button.@br@br

	The function does the following:@br
	- call {@link Library_Firearm#DoStopAiming}
	- check {@link Library_Firearm#FireOnHolding} and if {@code true} do the following:@br
	- call {@link Library_Firearm#CancelUsing}@br
	- call {@link Library_Firearm#CancelCharge}@br
	- call {@link Library_Firearm#CancelReload}@br
	- check if the weapon is not {@link Library_Firearm#IsRecovering} and if not, call {@link Library_Firearm#CheckCooldown}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func CancelFireCycle(object user, int x, int y)
{
	AssertNotNil(user);
	DoStopAiming(user);

	if (FireOnHolding())
	{
		CancelUsing();

		CancelCharge(user, x, y, GetFiremode(), true);
		CancelReload(user, x, y, GetFiremode(), true);

		if (!IsRecovering())
		{
			CheckCooldown(user, GetFiremode());
		}
	}
}


/**
	This is executed when the user stops holding the alternative use button (must be defined, not standard in OpenClonk).@br@br

	The function does the following:@br
	- call {@link Library_Firearm#OnStopUseAlt}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func ControlUseAltStop(object user, int x, int y)
{
	this->OnUseAltStop(user, x, y);
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
 */
public func ControlUseCancel(object user, int x, int y)
{
	if (this->OnUseCancel(user, x, y))
	{
		return true;
	}
	else
	{
		return ControlUseStop(user, x, y);
	}
}


/**
	This is executed when the user cancels the alternative use (usually by pressing a dedicated cancel button; must be defined, not standard in OpenClonk).@br@br

	The function does the following:@br
	- call {@link Library_Firearm#OnCancelUseAlt}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func ControlUseAltCancel(object user, int x, int y)
{
	this->OnUseAltCancel(user, x, y);
	return true;
}


/**
	Sets is_using to false.
*/
public func CancelUsing()
{
	is_using = false;
}


/**
	Check if the weapon should not be used right now.@br@br

	Checks {@link Library_Firearm#IsWeaponReadyToUse} and {@link Library_Firearm#IsUserReadyToUse}. For custom behaviour, modify those functions.

	@par user The object that is trying to use this weapon.
*/
func RejectUse(object user)
{
	return !IsWeaponReadyToUse(user) || !IsUserReadyToUse(user);
}


/**
	Interface for signaling that the weapon is ready to use (attack).

	@par user The object that is trying to use this weapon.

	@return true, if the object is ready to use. By default this is true, if the weapon is contained in the user.
 */
func IsWeaponReadyToUse(object user)
{
	return Contained() == user;
}


/**
	Interface for signaling that the user is ready to use (attack).

	@par user The object that is trying to use this weapon. 

	@return true, if the object is ready to use. By default, it is true.
 */
func IsUserReadyToUse(object user)
{
	return true;
}


/**
	Must return true if the weapon wants to receive holding updates for controls.
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


/* --- Charging --- */

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
*/
func StartCharge(object user, int x, int y)
{
	var firemode = GetFiremode();
	AssertNotNil(firemode);

	if (!is_using                           // No charge necessary if we are in burst mode
	||  firemode->GetChargeDelay() < 1      // Charging does not make sense?
	||  !this->NeedsCharge(user, firemode)) // Charging necessary? By default this is the same condition as above, but someone may define a custom condition; Callback via "this->" for runtime overload
	{
		return false; // Do not charge then
	}

	var charge_process = IsCharging();

	if (charge_process != nil)
	{
		if (charge_process.user == user && charge_process.firemode == firemode)
		{
			if (charge_process.has_charged)
			{
				return false; // Fire away
			}
			else if (charge_process.is_charged)
			{
				charge_process.has_charged = true;
				if (DoCharge(user, x, y, firemode))
				{
					return false; // Fire away
				}
				else
				{
					return true; // Keep charging, because someone overrides DoCharge()
				}
			}
			else
			{
				if (charge_process.progress > 0)
				{
					this->OnProgressCharge(user, x, y, firemode, charge_process.percentage, charge_process.progress);
					charge_process.percent_old = charge_process.percentage;
				}
				return true; // Keep charging
			}
		}
		else
		{
			CancelCharge(user, x, y, firemode, false);
		}
	}

	CreateEffect(IntChargeEffect, 1, 1, user, firemode);
	this->OnStartCharge(user, x, y, firemode);
	return true; // Keep charging
}


/**
	Will cancel the charging process currently running. Is automatically called by {@link Library_Firearm#StartCharge} if the user or firemode changed during a charging process.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
	@par callback Set to true to call {@link Library_Firearm#OnCancelCharge}.
*/
func CancelCharge(object user, int x, int y, proplist firemode, bool callback)
{
	var charge_process = IsCharging();

	if (charge_process != nil)
	{
		if (callback) this->OnCancelCharge(charge_process.user, x, y, charge_process.firemode);

		RemoveEffect(nil, nil, charge_process);
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
*/
func DoCharge(object user, int x, int y, proplist firemode)
{
	this->OnFinishCharge(user, x, y, firemode);
	return true;
}


/**
	Checks if the weapon is currently charging.@br

	@return The charging effect.
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
	return GetEffectProgress(IsCharging());
}


/**
	Condition if the weapon needs to be charged.@br

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.

	@return default: {@c true}, if the {@link Library_Firearm_Firemode#GetChargeDelay} is > 0.
	                 Overload this function for a custom condition.
 */
public func NeedsCharge(object user, proplist firemode)
{
	if (firemode)
	{
		return firemode->GetChargeDelay() > 0;
	}
	return false;
}


/**
	Callback: the weapon starts charging. Does nothing by default.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
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
 */
public func OnCancelCharge(object user, int x, int y, proplist firemode)
{
}


local IntChargeEffect = new Effect
{
	Construction = func (object user, proplist firemode)
	{
		this.user = user;
		this.firemode = firemode;

		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.is_charged = false;
		this.has_charged = false;
	},

	Timer = func (int time)
	{
		// Increase progress percentage depending on the charging delay of the firemode
		this.percentage = BoundBy(time * 100 / this.firemode->GetChargeDelay(), 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the charging process is finished based on the charging delay of the firemode
		if (time > this.firemode->GetChargeDelay() && !this.is_charged)
		{
			this.is_charged = true;
			// Do not make subsequent calls of the timer function because every-frame-effects do hurt performance
			this.Interval = 0;
			// However, keep the effect, to track that this weapon is now charged
		}
	},

	GetProgress = func ()
	{
		return this.percent;
	},
};


/* --- Firing --- */

/**
	The weapon is ready to fire a shot.
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
	- check {@link Library_Firearm#IsReadyToFire}@br
	- check {@link Library_Firearm#StartCharge}@br
	- check {@link Library_Firearm#FireOnHolding}@br
	- if all of the above check out, call {@link Library_Firearm#Fire}@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par is_pressing_trigger Should be true if the fire button is held to indicate the is_using state.
*/
func DoFireCycle(object user, int x, int y, bool is_pressing_trigger)
{
	var angle = GetAngle(x, y);
	user->~SetAimPosition(angle);

	if (is_pressing_trigger)
	{
		is_using = true;
	}

	if (IsReadyToFire())
		if (!StartReload(user, x, y))
			if (!StartCharge(user, x, y))
				if (FireOnHolding() || !is_pressing_trigger)
					Fire(user, x, y);
}


/**
	Called by the clonk (Aim Manager) when aiming is stopped. Fires a shot if ready.@br
	Checks: {@link Library_Firearm#IsReadyToFire}.@br

	@par user The object that is using the weapon.
	@par angle The firing angle the user is aiming at.
*/
func FinishedAiming(object user, int angle)
{
	if (FireOnStopping() && is_using && IsReadyToFire())
	{
		var x = +Sin(angle, 1000);
		var y = -Cos(angle, 1000);

		Fire(user, x, y);
	}
	_inherited(user, angle, ...);
}


/**
	Converts coordinates to an aiming angle for the weapon.

	@par x The x coordinate, local.
	@par y The y coordinate, local.

	@return int The angle in degrees, normalized to the range of -180° to 180°.
 */
func GetAngle(int x, int y)
{
	var angle = Angle(0, 0, x, y);
		angle = Normalize(angle, -180);

	return angle;
}


/**
	Converts coordinates to a firing angle for the weapon, respecting the {@link Library_Firearm_Firemode#GetProjectileOffset} from the fire mode.@br

	@par x The x coordinate, local.
	@par y The y coordinate, local.
	@par firemode A proplist containing the fire mode information.

	@return int The angle in degrees, normalized to the range of -180° to 180°.
*/
func GetFireAngle(int x, int y, proplist firemode)
{
	//var angle = Angle(0, firemode->GetYOffset(), x, y);
	//	angle = Normalize(angle, -180);
	// Note:
	// It turned out, that it is actually more accurate
	// to NOT incorporate the offset into the angle
	return GetAngle(x, y);
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

	@note Issues a callback "OnFirearmFire(object weapon, proplist firemode)"
	      to the user after the projectile was fired.

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
func Fire(object user, int x, int y)
{
	AssertNotNil(user);

	var firemode = GetFiremode();
	AssertNotNil(firemode);

	if (this->~IsReloading())
	{
		CancelReload(user, x, y, firemode, false);
	}

	if (HasAmmo(firemode))
	{
		var angle = GetFireAngle(x, y, firemode);

		FireSound(user, firemode);
		FireEffect(user, angle, firemode);
		FireProjectiles(user, angle, firemode);
		FireRecovery(user, x, y, firemode);
		user->~OnFirearmFire(this, firemode);
	}
	else
	{
		this->OnNoAmmo(user, firemode);
	}
}


/**
	The actual firing function.@br@br

	The function will create new bullet objects, as many as the firemode defines. Since no actual ammo objects are taken or consumed, this should be handled in {@link Library_Firearm#HandleAmmoUsage}.@br
	Each time a single projectile is fired, {@link Library_Firearm#OnFireProjectile} is called.@br
	{@link Library_Firearm#GetProjectileAmount} and {@link Library_Firearm#GetSpread} can be used for custom behaviour.@br

	@par user The object that is using the weapon.
	@par angle The firing angle.
	@par firemode A proplist containing the fire mode information.
*/
func FireProjectiles(object user, int angle, proplist firemode)
{
	AssertNotNil(user);
	AssertNotNil(firemode);

	var weapon_offset = GetWeaponPosition(user, WEAPON_POS_Muzzle, angle);
	var firemode_offset = firemode->GetProjectileOffset(angle);

	var x = weapon_offset.X + firemode_offset.X;
	var y = weapon_offset.Y + firemode_offset.Y;

	// Launch the single projectiles
	for (var i = 0; i < Max(1, firemode->GetProjectileAmount()); i++)
	{
		var projectile = CreateObject(firemode->GetProjectileID(), x, y, user->GetController());

		projectile->Shooter(user)
		          ->Weapon(this)
		          ->DamageAmount(firemode->GetDamage())
		          ->DamageType(firemode->GetDamageType())
		          ->Velocity(Library_Random->SampleValue(firemode->GetProjectileSpeed()))
		          ->Range(Library_Random->SampleValue(firemode->GetProjectileRange()));

		this->OnFireProjectile(user, projectile, firemode);
		projectile->Launch(angle, ComposeSpread(user, firemode));
	}

	weapon_properties.shot_counter[firemode->GetIndex()]++;

	HandleAmmoUsage(firemode);
}


/**
	Gets the individual deviations for a shot.

	@par firemode A proplist containing the fire mode information.

	@return array By default, will compose the spread values:
			<ul>
			<li>{@link Library_Firemode#GetSpread}</li>
			<li>{@link Library_Firemode#GetProjectileSpread}</li>
			<li>{@code user->GetFirearmSpread(weapon, firemode}</li>
	        </ul>
            fire mode and weapon user into an array,
            or returns nil.
*/
func ComposeSpread(object user, proplist firemode)
{
	// Get static values from the weapon
	var weapon_spread = firemode->GetSpread();
	var projectile_spread = firemode->GetProjectileSpread();

	// Get dynamic values from the user
	var user_spread;
	if (user)
	{
		user_spread = user->~GetFirearmSpread(this, firemode);
	}

	// Compose everything
	var deviations = [weapon_spread, projectile_spread, user_spread];
	RemoveHoles(deviations);
	return deviations;
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
	AssertNotNil(user);

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


/* --- Recovering --- */

/**
	Will start the recovery process if the weapon needs recovering.@br@br

	This function does the following:
	- check if recovering is needed ({@link Library_Firearm#NeedsRecovery}).@br
	- if yes, create a recovery effect.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
*/
func FireRecovery(object user, int x, int y, proplist firemode)
{
	var delay;
	if (NeedsRecovery(user, firemode))
	{
		delay = firemode->GetRecoveryDelay();
	}
	else
	{
		delay = 1;
	}

	CreateEffect(IntRecoveryEffect, 1, delay, user, x, y, firemode);
}


/**
	Condition when the weapon needs to recover (pause between two consecutive shots) after firing.

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.

	@return {@c true} by default. Overload this function for a custom condition.
 */
public func NeedsRecovery(object user, proplist firemode)
{
	return true;
}


/**
	Will cancel the recovering process currently running.@br
*/
func CancelRecovery()
{
	var recovery_process = IsRecovering();

	if (recovery_process != nil)
		RemoveEffect(nil, nil, recovery_process);
}


/**
	Gets the current status of the recovering process.

	@return A value of 0 to 100, if the weapon is recovering.@br
            If the weapon is not recovering, this function returns -1.
 */
func GetRecoveryProgress()
{
	return GetEffectProgress(IsRecovering());
}


/**
	Checks if the weapon is currently recovering.@br

	@return The recovering effect.
*/
func IsRecovering()
{
	return GetEffect("IntRecoveryEffect", this);
}


/**
	This function is called after the recovery process is done.@br
	It calls {@link Library_Firearm#OnRecovery}.@br
	On a fire mode with burst set, it will go through {@link Library_Firearm#CancelRecovery} and {@link Library_Firearm#DoFireCycle} until the appropriate amounts of shot is fired.@br
	Otherwise {@link Library_Firearm#CheckCooldown} is called.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.

	@par firemode A proplist containing the fire mode information.
*/
func DoRecovery(object user, int x, int y, proplist firemode)
{
	if (firemode == nil) return;

	this->OnRecovery(user, firemode);

	if (firemode->GetBurstAmount())
	{
		if (firemode->GetMode() != WEAPON_FM_Burst)
		{
			FatalError(Format("This fire mode has a burst value of %d, but the mode is not burst mode WEAPON_FM_Burst (value: %d)", firemode->GetBurstAmount(), firemode->GetMode()));
		}

		if (weapon_properties.shot_counter[firemode->GetIndex()] >= firemode->GetBurstAmount())
		{
			weapon_properties.shot_counter[firemode->GetIndex()] = 0;
		}
		else
		{
			if (!is_using)
			{
				CancelRecovery();
				DoFireCycle(user, x, y, false);
			}

			return; // Prevent cooldown
		}
	}

	CheckCooldown(user, firemode);
}


/**
	Callback: the weapon finished one firing cycle. Does nothing by default.@br

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
 */
public func OnRecovery(object user, proplist firemode)
{
}


local IntRecoveryEffect = new Effect
{
	Construction = func (object user, int x, int y, proplist firemode)
	{
		this.user = user;
		this.x = x;
		this.y = y;
		this.firemode = firemode;
	},

	Timer = func ()
	{
		this.Target->DoRecovery(this.user, this.x, this.y, this.firemode);
		return FX_Execute_Kill;
	},

	GetProgress = func ()
	{
		var progress = BoundBy(this.Time, 0, this.Interval);
		return progress * 100 / this.Interval;
	},
};

/* --- Cooldown --- */

/**
	Called by {@link Library_Firearm#DoRecovery}.@br
	Will call {@link Library_Firearm#StartCooldown} if the weapon has still ammunition left and is not an automatic weapon.@br

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
*/
func CheckCooldown(object user, proplist firemode)
{
	if (!HasAmmo(firemode) || RejectUse(user))
	{
		CancelUsing();
	}

	if ((firemode->GetMode() != WEAPON_FM_Auto) || (firemode->GetMode() == WEAPON_FM_Auto && !is_using))
	{
		StartCooldown(user, firemode);
	}
}


/**
	Will start the cooldown process if the weapon or fire mode need cooling down. Can be called multiple times even if the cooldown is already in progress.@br@br

	This function does the following:
	- check if cooldown is needed ({@link Library_Firearm#NeedsCooldown}) or if the fire mode requires a cooldown.@br
	- if no, call {@link Library_Firearm#OnSkipCooldown}.@br
	- if yes, create a cooldown effect if not already present, call {@link Library_Firearm#OnStartCooldown}.@br

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
*/
func StartCooldown(object user, proplist firemode)
{
	if (firemode->GetCooldownDelay() < 1 || !this->NeedsCooldown(user, firemode))
	{
		this->OnSkipCooldown(user, firemode);
		return;
	}

	var cooldown_process = IsCoolingDown();

	if (cooldown_process == nil)
	{
		CreateEffect(IntCooldownEffect, 1, firemode->GetCooldownDelay(), user, firemode);
		this->OnStartCooldown(user, firemode);
	}
}


/**
	Simply forwards a call to {@link Library_Firearm#OnFinishCooldown}.@br
*/
func DoCooldown(object user, proplist firemode)
{
	this->OnFinishCooldown(user, firemode);
}


/**
	Gets the current status of the cooldown process.

	@return A value of 0 to 100, if the weapon is cooling down.@br
            If the weapon is not cooling down, this function returns -1.
 */
func GetCooldownProgress()
{
	return GetEffectProgress(IsCoolingDown());
}


/**
	Checks if the weapon is currently cooling down.@br

	@return The cooldown effect.
*/
func IsCoolingDown()
{
	return GetEffect("IntCooldownEffect", this);
}


/**
	Condition when the weapon needs a cooldown.

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.

	@return default: {@c true}, if the {@link Library_Firearm_Firemode#GetCooldownDelay} is > 0.
	                 Overload this function for a custom condition.
 */
public func NeedsCooldown(object user, proplist firemode)
{
	if (firemode)
	{
		return firemode->GetCooldownDelay() > 0;
	}
	return false;
}


/**
	Callback: the weapon starts cooling down. Does nothing by default.

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
 */
public func OnStartCooldown(object user, proplist firemode)
{
}


/**
	Callback: the weapon has successfully cooled down. Does nothing by default.

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
 */
public func OnFinishCooldown(object user, proplist firemode)
{
}


/**
	Callback: the weapon has skipped cooling down. Does nothing by default.

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
 */
public func OnSkipCooldown(object user, proplist firemode)
{
}


local IntCooldownEffect = new Effect
{
	Construction = func (object user, proplist firemode)
	{
		this.user = user;
		this.firemode = firemode;
	},

	Timer = func ()
	{
		this.Target->DoCooldown(this.user, this.firemode);
		return FX_Execute_Kill;
	},

	GetProgress = func ()
	{
		var progress = BoundBy(this.Time, 0, this.Interval);
		return progress * 100 / this.Interval;
	},
};

/* --- Reloading --- */

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
*/
func StartReload(object user, int x, int y, bool forced)
{
	var firemode = GetFiremode();
	AssertNotNil(firemode);

	if ((!is_using && !forced) || !NeedsReload(user, firemode, forced)) 
	{
		return false;
	}

	var process = this->~IsReloading();

	if (process != nil)
	{
		if (process.user == user && process.firemode == firemode && this->IsUserReadyToReload(user))
		{
			process.x = x;
			process.y = y;

			if (process.has_reloaded)
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

	if (CanReload(user, firemode) && this->IsUserReadyToReload(user))
	{
		this->~StartReloadProcess(user, x, y, firemode);
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
*/
func CancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
	var process = this->~IsReloading();

	var auto_reload = firemode->GetAutoReload() && requested_by_user;

	if (process != nil)
	{
		this->OnCancelReload(process.user, x, y, process.firemode, requested_by_user);

		if (!auto_reload) RemoveEffect(nil, nil, process);
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
	Gets the current status of the reloading process.@br

	@return A value of 0 to 100, if the weapon is reloading.@br
            If the weapon is not reloading, this function returns -1.
 */
public func GetReloadProgress()
{
	return GetEffectProgress(this->~IsReloading());
}


/**
	Condition if the weapon can be reloaded. Checked only when reloading is started, i.e.
	is there enough ammo.

	@par user The object that is using the weapon.

	@return {@c true} by default. Overload this function for a custom condition.
 */
public func CanReload(object user, proplist firemode)
{
	return true;
}


/**
	Interface for signaling that the user is ready to reload.

	@par user The object that is trying to reload this weapon. 

	@return true, if the object is ready to reload. By default, it is true.
	        False means, that the user cannot reload and an ongoing reload
	        will be cancelled.
 */
func IsUserReadyToReload(object user)
{
	return true;
}


/**
	Condition if the weapon needs to be reloaded.@br

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
	@par forced {@code true} If the reload is requested by a user,
	            instead of request from the fire cycle.

	@return {@c false} by default. Overload this function for a custom condition.
	        Otherwise no reloading needs ever to be done.
 */
public func NeedsReload(object user, proplist firemode, bool forced)
{
	return false;
}


/**
	Callback: the weapon starts reloading. Does nothing by default.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
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
 */
public func OnCancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
}

/* --- Firemodes --- */

/**
	Create a new, writable fire mode.@br
	With this function, fire modes can be created during runtime. However, this should not be used to create all fire modes of a weapon, 
	if it is not intended to ever write new values into these.
	In that case, fire modes should be defined as static (read-only) proplists in the definition.@br
	The newly created fire mode will inherit all information from {@link Library_Firearm_Firemode}.@br

	@par add A boolean, if true the fire mode will be added to the weapon ({@link Library_Firearm#AddFiremode}).

	@return The proplist of the newly created firemode
*/
public func CreateFiremode(bool add)
{
	var new_mode = new Library_Firearm_Firemode {};
	if (add)
	{
		AddFiremode(new_mode);
	}
	return new_mode;
}


/**
	Replace a fire mode with a writable copy of itself. With this, you can make fire modes writable during runtime if needed.@br

	@par number The index of the fire mode in the fire modes array.
*/
public func MakeFiremodeWritable(int number)
{
	AssertValidFiremode(number);
	weapon_properties.firemodes[number] = { Prototype = weapon_properties.firemodes[number] };
}


/**
	Sets a new fire mode.@br

	@par number The number key in the fire modes array to select.
	@par force If true, the fire mode is changed without checking whether the fire mode can currently be changed or if the condition is met.

	@return {@c true} if the fire mode was changed, {@c false} if it failed.
*/
public func SetFiremode(int number, bool force)
{
	AssertValidFiremode(number);

	if (force || CanChangeFiremode() || GetFiremode(number)->IsAvailable())
	{
		weapon_properties.firemode_selected = number;
		return true;
	}
	else
	{
		return false;
	}
}


/**
	Gets the currently selected fire mode, or an indexed fire mode.@br

	@par number The fire mode index in the array {@link Library_Firearm#GetFiremodes}, 
                or if you pass {@c nil} the currently selected fire mode.

	@return A {@c proplist} containing the fire mode information.
 */
public func GetFiremode(int number)
{
	number = number ?? weapon_properties.firemode_selected;
	AssertValidFiremode(number);
	return weapon_properties.firemodes[number];
}


/**
	Gets the index of a fire mode.

	@par firemode The fire mode proplist
	@par available If set to {@c false} this will check {@link Library_Firearm#GetFiremodes()}, and if 
                   set to {@c true} it will check {@link Library_Firearm#GetAvailableFiremodes()}.

	@return the fire mode index. Can be used in {@link Library_Firearm#SetFiremode}.
 */
public func GetFiremodeIndex(proplist firemode, bool available)
{
	if (available)
	{
		return GetIndexOf(GetAvailableFiremodes(), firemode);
	}
	else
	{
		return GetIndexOf(GetFiremodes(), firemode);
	}
}


/**
	Gets all configured fire modes for this weapon.@br

	@return An array of all fire modes.
*/
public func GetFiremodes()
{
	if (!weapon_properties.firemodes)
	{
		FatalError("Fire modes is somehow empty??");
	}

	return weapon_properties.firemodes;
}


/**
	Gets all available fire modes. Available fire modes are only those where the configured condition is met.@br

	@return An array of all available fire modes.
*/
public func GetAvailableFiremodes()
{
	var available = [];

	for (var i = 0; i < GetLength(GetFiremodes()); ++i)
	{
		var firemode = GetFiremode(i);
		if (firemode->IsAvailable())
		{
			PushBack(available, firemode);
		}
	}

	return available;
}


/**
	Delete all configured fire modes.@br
*/
public func ClearFiremodes()
{
	weapon_properties.firemodes = [];
}


/**
	Add a fire mode to the list of configured fire modes.@br

	@par firemode A proplist containing the fire mode information.
*/
public func AddFiremode(proplist firemode)
{
	var index = GetLength(weapon_properties.firemodes);
	PushBack(weapon_properties.firemodes, firemode);
	firemode->SetIndex(index);
}


/**
	Checks whether the weapon is currently recovering, charging, reloading or locked.@br

	@return {@c true} if change if fire modes is possible.
*/
public func CanChangeFiremode()
{
	return !IsRecovering()
	    && !IsCharging()
	    && !this->~IsReloading()
	    && !IsWeaponLocked();
}


/**
	Changes the firemode at the next possible time.

	@par number the desired fire mode index.
 */
public func ScheduleSetFiremode(int number)
{
	if (this->~CanChangeFiremode())
	{
		SetFiremode(number);
	}
	else
	{
		var schedule = GetEffect(IntChangeFiremodeEffect.Name, this) ?? CreateEffect(IntChangeFiremodeEffect, 1, 1);
		schedule.mode = number;
	}
}


/**
	Gets the scheduled fire mode

	@return the fire mode index.
 */
public func GetScheduledFiremode()
{
	var schedule = GetEffect(IntChangeFiremodeEffect.Name, this);

	if (schedule)
	{
		return schedule.mode;
	}
	return nil;
}


/**
	Cancels a scheduled fire mode change.
 */
public func ResetScheduledFiremode()
{
	var schedule = GetEffect(IntChangeFiremodeEffect.Name, this);
	if (schedule)
	{
		schedule.mode = nil;
	}
}


local IntChangeFiremodeEffect = new Effect
{
	Name = "IntChangeFiremodeEffect",
	Timer = func ()
	{
		// Stop if there is no mode
		if (this.mode == nil) return FX_Execute_Kill;

		if (Target->~CanChangeFiremode())
		{
			Target->SetFiremode(this.mode);
			return FX_Execute_Kill;
		}

		return FX_OK;
	},
};


func AssertValidFiremode(int number)
{
	if (number < 0 || number >= GetLength(weapon_properties.firemodes))
	{
		FatalError(Format("The fire mode (%v) is out of range of all configured fire modes (%v)", number, GetLength(weapon_properties.firemodes)));
	}
}

/* --- Ammo --- */

/**
	Changes the amount of ammunition that the object currently has.

	@par ammo The type of the ammunition.
	@par amount The change, can be positive or negative.
             The amount of ammunition cannot be changed beyond the capacity
             of the object, so the actual amount by which the ammunition was
             changed will be returned.

	@return The actual change that happened.

	@author Marky

	@note This function should be implemented by the weapon. A quick implementation
       is available by including {@link Library_AmmoManager}.

	@related {@link Library_AmmoManager#DoAmmo}, {@link Library_AmmoManager#SetAmmo}
 */
public func DoAmmo(id ammo, int amount)
{
	return _inherited(ammo, amount, ...);
}


/**
	Checks whether the weapon has ammo.@br
	Not implemented by default and will always return true (infinite ammo) as long as {@link Library_Firearm#Setting_WithAmmoLogic} is not implemented. Otherwise calls _inherited.@br

	@par firemode The ammo type for this firemode is checked.

	@return bool Returns {@code true} if the weapon has enough ammo for the firemode
 */
public func HasAmmo(proplist firemode)
{
	// No ammo handling set up, infinite ammo
	if (!Setting_WithAmmoLogic())
		return true;

	return _inherited(firemode);
}


/**
	Callback: The weapon has no ammo during {@link Library_Weapon#DoFireCycle},
              see {@link Library_Weapon#HasAmmo}.

	@par user The object that is using the weapon.
	@par firemode A proplist containing the fire mode information.
 */
public func OnNoAmmo(object user, proplist firemode)
{
}


/**
	Called after {@link Library_Firearm#FireProjectiles}. Should somehow reduce ammo.@br
	Not implemented by default and will always return true (infinite ammo) as long as {@link Library_Firearm#Setting_WithAmmoLogic} is not implemented. Otherwise calls _inherited.@br

	@par firemode The ammo type for this firemode is checked.

	@return bool Returns {@code true} if the weapon has enough ammo for the firemode
 */
func HandleAmmoUsage(proplist firemode)
{
	// No ammo handling set up, infinite ammo
	if (!Setting_WithAmmoLogic())
		return true;

	return _inherited(firemode);
}


/* --- Locking --- */

/**
	Locks the weapon so it cannot be used.@br

	@par lock_time The weapon will be locked for this many frames. On a lock time of {@code 0} the weapon stays locked until you call {@link Library_Weapon#UnlockWeapon}.
 */
public func LockWeapon(int lock_time)
{
	var locked = IsWeaponLocked();
	if (locked == nil)
	{
		AddEffect("IntWeaponLocked", this, 1, lock_time, this, nil);
	}
	else
	{
		locked.Interval = lock_time;
	}
}


/**
	Unlocks the weapon, so that it can be used again.@br
 */
public func UnlockWeapon()
{
	var locked = IsWeaponLocked();
	if (locked) RemoveEffect(nil, this, locked);
}


/**
	Checks if the weapon is currently locked against usage.@br

	@return The weapon locking effect.
*/
func IsWeaponLocked()
{
	return GetEffect("IntWeaponLocked", this);
}


/* --- Misc --- */

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
		anim_shoot_name = firemode->GetShootingAnimation();
		anim_shoot_time = firemode->GetRecoveryDelay();
		anim_load_name = firemode->GetReloadAnimation();
		anim_load_time = firemode->GetReloadDelay();
		anim_walk_speed_front = firemode->GetForwardWalkingSpeed();	
		anim_walk_speed_back = firemode->GetBackwardWalkingSpeed();
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
	Calls {@code GetProgress()} in a proplist

	@return the return value of the call, or {@code -1} if
	        there is no proplist.
 */
func GetEffectProgress(proplist process)
{
	if (process == nil)
	{
		return -1;
	}
	else
	{
		return process->GetProgress();
	}
}

/* --- Position calculations --- */

/**
	Defines an offset for various effects.

	@par name the name of the offset/position, so that it can be referenced later.
	@par x    the offset from the user hand to the position on the weapon,
	          assuming that the barrel is facing "right".
	@par y    the offset from the user hand to the position on the weapon,
	          assuming that the barrel is facing "right".
 */
public func DefineWeaponOffset(string name, int x, int y, int precision)
{
	weapon_properties.weapon_offset[name] = new PositionOffsetRotation {};
	weapon_properties.weapon_offset[name]->DefineOffset(x, y);
}

public func GetWeaponOffset(string name, int angle, int precision)
{
	return weapon_properties.weapon_offset[name]->GetPosition(angle, precision);
}

public func GetWeaponPosition(object user, string name, int angle, int precision)
{
	if (nil == user)
	{
		return GetWeaponOffset(name, angle, precision);
	}

	var user_offset = user->~GetAimAnimationOffset(this, angle) ?? {X = 0, Y = 0,};
	var weapon_offset = GetWeaponOffset(name, angle, precision);
	return
	{
		X = user_offset.X + weapon_offset.X,
		Y = user_offset.Y + weapon_offset.Y,
		DebugColor = weapon_offset.DebugColor
	};
}

/* --- Editor actions & properties --- */


func EditorProps_GetDebugEnabled()
{
	return Scenario.Library_Firearm_DebugWeaponPositions;
}

func EditorProps_SetDebugEnabled(bool enabled)
{
	Scenario.Library_Firearm_DebugWeaponPositions = enabled;
	
	for (var weapon in FindObjects(Find_Property("weapon_properties")))
	{
		RemoveEffect(FxEditorPropsDebug.Name, weapon);
		if (enabled)
		{
			weapon->CreateEffect(weapon.FxEditorPropsDebug, 1, 1);
		}
	}
}


local FxEditorPropsDebug = new Effect
{
	Name = "FxEditorPropsDebug",

	Timer = func ()
	{
		var user = this.Target->Contained();
		DebugAimPositions(user);
	},
	
	DebugAimPositions = func (object user)
	{
		if (!user
		||  !user->~IsAiming()
		||   user->~GetHandItem() != this.Target)
		{
			return;
		}

		var angle = user->GetAimPosition();

		var debug_positions = [];
		if (user.GetAimAnimationOffset)
		{
			PushBack(debug_positions, user->GetAimAnimationOffset(this.Target, angle));
		}
		for (var name in GetProperties(this.Target.weapon_properties.weapon_offset))
		{
			PushBack(debug_positions, this.Target->GetWeaponPosition(user, name, angle));
		}

		for (var position in debug_positions)
		{
			if (!position) continue;

			var color = SplitRGBaValue(position.DebugColor);
			user->CreateParticle("SphereSpark", position.X, position.Y, 0, 0, this.Interval,
			{
				Size = 1, R = color.R, G = color.G, B = color.B,
				Attach = ATTACH_Front | ATTACH_MoveRelative,
			});
		}
	},
};

/* --- Aiming --- */

func DoStartAiming(object user)
{
	if (Setting_CustomAimManager() == nil)
	{
		if (Setting_AimOnUseStart() && !user->~IsAiming())
		{
			user->~StartAim(this);
		}
	}
	else
	{
		FatalError("Custom aim manager not supported yet");
	}
}

func DoStopAiming(object user)
{
	if (Setting_CustomAimManager() == nil)
	{
		if (FireOnStopping() || Setting_AimOnUseStart())
		{
			user->~StopAim();
		}
	}
	else
	{
		FatalError("Custom aim manager not supported yet");
	}
}

