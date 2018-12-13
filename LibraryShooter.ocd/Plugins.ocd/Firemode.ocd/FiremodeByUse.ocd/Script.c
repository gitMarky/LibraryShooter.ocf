/**
	Allows fire mode selection via the use function:
	- normal use is firemode "primary"
	- alternative use is firemode "secondary"
 */

static const WEAPON_Firemode_Primary = "primary";
static const WEAPON_Firemode_Secondary = "secondary";

/**
	Callback: Pressed the regular use button (fire).
 */
public func OnPressUse(object user, int x, int y)
{
	// May need scheduled fire mode change - currently you can press left and right and get both firemodes at the same time
	this->SetFiremode(this->~GetFiremodeByName(WEAPON_Firemode_Primary) ?? 0, true); // TODO - better than not working at all, but still a temporary solution
	_inherited(user, x, y, ...);
}


/**
	Callback: Pressed the alternate use button (fire secondary).

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at.
	@par y The y coordinate the user is aimint at.
 */
public func OnPressUseAlt(object user, int x, int y)
{
	this->SetFiremode(this->~GetFiremodeByName(WEAPON_Firemode_Secondary) ?? 1, true); // TODO - better than not working at all, but still a temporary solution

	user->StartAim(this);
	this->ControlUseHolding(user, x, y);

	_inherited(...);
}


/**
	Callback: The alternative use button is held. Called in regular intervals by {@link Library_Firearm#ControlUseAltHolding}.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnHoldingUseAlt(object user, int x, int y)
{
	this->ControlUseHolding(user, x, y);
}


/**
	Callback: Released the alternative use button. Called by {@link Library_Firearm#ControlUseAltStop}.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
 */
public func OnUseAltStop(object user, int x, int y)
{
	this->ControlUseStop(user, x, y);
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
	this->ControlUseCancel(user, x, y);
}
