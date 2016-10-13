/**
 Allows fire mode selection via the use function:
 - normal use is firemode "primary"
 - alternative use is firemode "secondary"
 */

#include Plugin_Weapon_FiremodeBySelection

static const WEAPON_Firemode_Primary = "primary";
static const WEAPON_Firemode_Secondary = "secondary";

local firemode_cached = nil;

/**
 Callback: Pressed the regular use button (fire).
 */
public func OnPressUse(object user, int x, int y)
{
	ScheduleChangeFiremode(WEAPON_Firemode_Primary);
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
	ScheduleChangeFiremode(WEAPON_Firemode_Secondary);
}


/**
 Callback from {@link Library_Weapon#ControlUserStop}, 
 so that you do not have to overload the entire function.
 */
public func OnUseStop(object user, int x, int y)
{
	ResetChangeFiremode();
}



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
