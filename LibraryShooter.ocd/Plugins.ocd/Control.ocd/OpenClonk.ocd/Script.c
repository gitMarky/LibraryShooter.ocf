/**
	Control scheme that is typical for OC:
	Pressing the button goes to aim mode, releasing the button fires the weapon.
 */


/* --- Settings --- */

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
		return this->StartFireCycle(user, x, y);
	}
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

	this->CancelFireCycle(user, x, y);
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

/* --- Override, make sure to inlude this AFTER Library_Firearm --- */

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
		this->CancelFireCycle(user, x, y);
		return false;
	}

	DoStartAiming(user);
	this->DoUpdateAiming(user, x, y);
	
	if (this->CanSendFireRequest())
	{
		// Do preparations only, because firing will be done when the button is released.
		this->DoFireCyclePreparations(user, x, y);
	}
	return true;
}


/* --- Aiming --- */

// TODO: Move this to a separate class/interface sort of thing
// For now, this overrides the other function, removing the settings,
// but a more generic solution via plugins is desired
func DoStartAiming(object user)
{
	if (user && !user->~IsAiming())
	{
		user->~StartAim(this);
	}
}

// TODO: Move this to a separate class/interface sort of thing
// For now, this overrides the other function, removing the settings,
// but a more generic solution via plugins is desired
func DoStopAiming(object user)
{
	if (user && user->~IsAiming())
	{
		user->~StopAim();
	}
}


/**
	Called by the clonk (Aim Manager) when aiming is stopped. Fires a shot if ready.@br
	Checks: {@link Library_Firearm#IsReadyToFire}.@br

	@par user The object that is using the weapon.
	@par angle The firing angle the user is aiming at.
*/
func FinishedAiming(object user, int angle)
{
	if (this->CanSendFireRequest() && this->IsReadyToFire())
	{
		var x = +Sin(angle, 1000);
		var y = -Cos(angle, 1000);

		this->Fire(user, x, y);
	}
	_inherited(user, angle, ...);
}
