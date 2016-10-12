/**
 Basic plugin class for weapons with a changeable fire mode.
 */

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
	return selected_firemode;
}


/**
 Use this to change the firemode of the weapon.
 @par firemode The name of the new firemode.
 @version 0.2.0
 */
public func ChangeFiremode(firemode)
{
	if (firemode == nil)
	{
		FatalError("The function expects a fire mode that is not nil");
	}
	
	if (GetType(firemode) == C4V_String)
	{
		for (var mode in this->GetFiremodes())
		{
			if (mode.name == firemode)
			{
				selected_firemode = mode;
				break;
			}
			
			FatalError(Format("No firemode '%s'", firemode));
		}
	} 
	else if (GetType(firemode) == C4V_PropList)
	{
		selected_firemode = firemode;
	}
	else
	{
		FatalError(Format("The function expects a string or proplist argument, you passed %v: %v", GetType(firemode), firemode));
	}
}
