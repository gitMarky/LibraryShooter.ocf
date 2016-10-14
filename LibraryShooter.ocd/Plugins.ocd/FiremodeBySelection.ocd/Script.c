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
				return;
			}			
		}

		FatalError(Format("No firemode '%s'", firemode));
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


/**
 Changes the firemode at the next possible time.
 
 @version 0.2.0
 */
func ScheduleChangeFiremode(string firemode)
{
	if (this->~CanChangeFiremode())
	{
		ChangeFiremode(firemode);
	}
	else
	{
		var schedule = GetEffect("scheduled_firemode", this) ?? CreateEffect(scheduled_firemode, 1, 1);
		schedule.mode = firemode;
	}
}



/**
 Gets the scheduled firemode
 
 @return the firemode, which may be a string or a proplist.
 @version 0.2.0
 */
func GetScheduledFiremode()
{
	var schedule = GetEffect("scheduled_firemode", this);
	
	if (schedule)
	{
		return schedule.mode;
	}
	return nil;
}

func ResetChangeFiremode()
{
	var schedule = GetEffect("scheduled_firemode", this);
	if (schedule)
	{
		schedule.mode = nil;
	}
}

local scheduled_firemode = new Effect
{
	Timer = func(int time)
	{
		// Stop if there is no mode
		if (this.mode == nil) return FX_Execute_Kill;

		if (Target->~CanChangeFiremode())
		{
			Target->ChangeFiremode(this.mode);
			return FX_Execute_Kill;
		}
		
		return FX_OK;
	}
};
