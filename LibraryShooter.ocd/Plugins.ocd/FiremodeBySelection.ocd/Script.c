

local selected_firemode;

/**
 Callback: the current firemode. Overload this function for
 @return proplist The current firemode.
 @version 0.2.0
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.2.0
 */
public func GetFiremode()
{
	return GetProperty(selected_firemode, this.fire_modes);
}


/**
 Use this to change the firemode of the weapon.
 @par firemode The name of the new firemode.
 @version 0.2.0
 */
public func ChangeFiremode(string firemode)
{
	if (firemode == nil)
	{
		FatalError("The function expects a fire mode that is not nil");
	}

	selected_firemode = firemode;
}
