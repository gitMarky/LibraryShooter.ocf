
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
	Check if the weapon should fire on a call of {@link Library_Firearm#ControlUseHolding}.@br

	@return {@c true} if either {@link Library_Firearm#Setting_AimOnUseStart} is false or if the selected fire mode is an automatic one.
*/
func FireOnHolding()
{
	return !Setting_AimOnUseStart() || this->GetFiremode()->GetMode() == WEAPON_FM_Auto;
}


/**
	Check if the weapon should fire on a call of {@link Library_Firearm#ControlUseStop}.@br

	@return {@c true} if {@link Library_Firearm#Setting_AimOnUseStart} is true and if the selected fire mode is not an automatic one.
*/
func FireOnStopping()
{
	return Setting_AimOnUseStart() && this->GetFiremode()->GetMode() != WEAPON_FM_Auto;
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
		return this->StartFireCycle(user, x, y);
	}
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

	this->CancelFireCycle(user, x, y);
	return true;
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
