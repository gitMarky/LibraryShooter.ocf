/**
 Allows fire mode selection via the use function:
 - normal use is firemode "primary"
 - alternative use is firemode "secondary"
 */

#include Plugin_Firearm_FiremodeBySelection

static const WEAPON_Firemode_Primary = "primary";
static const WEAPON_Firemode_Secondary = "secondary";

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
