/**
 Plugin for weapons: Basic ammunition logic for weapons.@br
 If the weapon is an Ammo Manager ({@link Library_AmmoManager}), it will make use of those functions.@br
 Be sure to include the ammo manager before this plugin.
 Otherwise, it will forward calls to whatever object is defined via {@link Library_Firearm_AmmoLogic#GetAmmoContainer}.@br

 @author Clonkonaut
 @version 0.3.0
 */

local ammo_rate_counter; // proplist

/**
 Ammo logic is set up.@br
 @related {@link Library_Firearm#Setting_WithAmmoLogic}
 @version 0.3.0
*/
public func Setting_WithAmmoLogic()
{
	return true;
}

/**
 Make sure to call this via _inherited();
*/
func Initialize()
{
	ammo_rate_counter = {};

	_inherited();
}

/**
 @return An object that receives all ammunition calls.
 @version 0.3.0
 */
public func GetAmmoContainer()
{
	return nil;
}

/**
 Checks whether the weapon has ammo.@br
 The weapon is considered to have ammo if:@br
 - {@link Library_Firearm_AmmoLogic#GetAmmo} is at least as high as {@link Library_Firearm_Firemode#GetAmmoUsage}@br
 - {@link Library_Firearm_Firemode#GetAmmoRate} is bigger than one and there are still shots left@br
 - The ammo source is AMMO_Source_Infinite@br
 @par firemode The ammo type for this fire mode is checked.
 @return bool Returns {@code true} if the weapon has enough ammo for the fire mode
 @version 0.3.0
 */
public func HasAmmo(proplist firemode)
{
	// Has no ammo if outside of a container
	if (this->GetFiremodeAmmoSource(firemode) == AMMO_Source_Container && !this->GetAmmoContainer())
	{
		FatalError("No ammo container is defined!");
		return false;
	}

	// Check three separate conditions
	return this->GetAmmo(firemode) >= firemode->GetAmmoUsage()    // 1. There is enough ammo left when checking with GetAmmo
	    || ammo_rate_counter[firemode.name] > 0                   // 2. There can still be shots fired before another ammunition piece is needed (when ammo rate is > 1)
	    || this->GetAmmoSource(firemode) == AMMO_Source_Infinite; // 3. This mode has infinite ammo
}

/**
 Get the specific ammo source for a fire mode.@br
 If {@link Library_AmmoManager#IsAmmoManager} will return {@link Library_AmmoManager#GetAmmoSource}.@br
 Otherwise it is assumed that {@link Library_Firearm_AmmoLogic#GetAmmoContainer} is the source for all ammo (AMMO_Source_Container).@br
 @par type_or_firemode You can pass an ID or you can pass a firemode.@br
                       If you pass {@code nil} the value for {@link Library_Weapon#GetFiremode} is requested.@br
                       The method will fail if the proplist is not a firemode.@br
                       The ammo ID is passed on to any calls of {@link Library_AmmoManager#GetAmmoSource}.
 @return int The current source of ammunition for an ID or firemode (see {@link Library_AmmoManager#GetAmmoSource}).
 @version 0.3.0
 */
public func GetFiremodeAmmoSource(type_or_firemode)
{
	var id = type_or_firemode;
	if (GetType(id) == C4V_PropList)
		id = id->GetAmmoID();
	if (id == nil)
	{
		var firemode = this->GetFiremode();
		if (firemode == nil)
			return FatalError("Cannot get firemode!");
		else
			id = firemode->GetAmmoID();
	}
	if (id == nil)
		FatalError("Cannot get ammunition ID!");

	if (this->~IsAmmoManager())
		return this->GetAmmoSource(id);

	if (GetAmmoContainer())
		return AMMO_Source_Container;

	FatalError("Could not get a valid ammo source!");
}

/**
 Get the specific ammo amount for a fire mode.@br
 If {@link Library_AmmoManager#IsAmmoManager} will return {@link Library_AmmoManager#GetAmmo}.@br
 Otherwise it is assumed that {@link Library_Firearm_AmmoLogic#GetAmmoContainer} is the source for all ammo and the call is passed on.@br
 @par type_or_firemode You can pass an ID or you can pass a firemode.@br
                       If you pass {@code nil} the value for {@link Library_Weapon#GetFiremode} is requested.@br
                       The method will fail if the proplist is not a firemode.@br
                       The ammo ID is passed on to any calls of {@link Library_AmmoManager#GetAmmo}.
 @return int The current amount of ammunition for an ID or firemode.
 @version 0.3.0
 */
public func GetAmmo(type_or_firemode)
{
	var id = type_or_firemode;
	if (GetType(id) == C4V_PropList)
		id = id->GetAmmoID();
	if (id == nil)
	{
		var firemode = this->GetFiremode();
		if (firemode == nil)
			return FatalError("Cannot get firemode!");
		else
			id = firemode->GetAmmoID();
	}
	if (id == nil)
		FatalError("Cannot get ammunition ID!");

	if (this->~IsAmmoManager())
		return _inherited(id);

	if (GetAmmoContainer())
		return GetAmmoContainer()->GetAmmo(id);

	FatalError("Could not get a valid ammo source!");
}

/**
 Called after a shot has been fired. Handles the depletion of ammo.@br
 Will call {@link Library_AmmoManager#DoAmmo} if shots equivalent to the the fire modes ammo rate have been fired. Either in this object if {@link Library_AmmoManager#IsAmmoManager} or {@link Library_Firearm_AmmoLogic#GetAmmoContainer}.@br
 Will call {@link Library_Firearm_AmmoLogic#OnAmmoChange}
 @par firemode The ammo type for this fire mode is checked.
 @version 0.3.0
*/
func HandleAmmoUsage(proplist firemode)
{
	ammo_rate_counter[firemode->GetName()]--;

	// default values
	var rate = firemode->GetAmmoRate() ?? 1;
	var ammo_type = firemode->GetAmmoID();
	var ammo_requested = firemode->GetAmmoUsage() ?? 1;

	// status
	var ammo_changed = false;
	var enough_ammo = true;

	// ammo container
	var container;
	if (this->~IsAmmoManager())
		container = this;
	else if (GetAmmoContainer())
		container = GetAmmoContainer();
	else
		return FatalError("Could not get a valid ammo source!");

	// only use actual ammo if there is no spare ammo per ammo rate
	if (ammo_rate_counter[firemode->GetName()] <= 0)
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
			var ammo_received = Abs(container->DoAmmo(ammo_type, -ammo_requested));
			if (ammo_received < ammo_requested)
			{
				container->DoAmmo(ammo_type, ammo_received);
				enough_ammo = false;
			}
			else // everything ok: fill up the spare ammo per rate, signal that something has changed
			{
				ammo_changed = true;
				ammo_rate_counter[firemode->GetName()] += rate;
			}
		}
	}

	if (ammo_changed)
	{
		this->OnAmmoChange(ammo_type);
	}
}

/**
 Callback: The weapon ammo in the weapon changes.

 @par ammo_type ID. The ammo that was affected.
 @version 0.3.0
 */
public func OnAmmoChange(id ammo_type)
{
}