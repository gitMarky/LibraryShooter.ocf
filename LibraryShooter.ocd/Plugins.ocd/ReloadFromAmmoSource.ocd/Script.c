/**
	Plugin for weapons: The weapon has to reload from a specified ammo source container.
 
	It can reload only if the ammo source container has ammo.
	It updates the weapon ammo of the weapon when reloading is finished.
	Does not support reload animations yet.
 
	This plugin is best used to model weapons that have a magazine or clip of some kind in
	the weapon. The {@link Plugin_Weapon_ReloadFromAmmoSource#GetAmmoReloadContainer} in this
	case is the person, vehicle, or whatever that holds the ammo reserves.
	The weapon should, but may not have, the {@link Library_AmmoManager#GetAmmoSource}:
	{@link Library_AmmoManager#AMMO_Source_Local}.

	@author Marky
 */


/**
	Callback: the weapon has successfully reloaded.
	Updates the weapon ammo from the source container. Takes as much ammo
	as is needed or as is available, whichever is lower, from the container
	and feeds it to the weapon.

	@see {@link Plugin_Weapon_ReloadFromAmmoSource#GetAmmoReloadContainer}

	@par firemode A proplist containing the fire mode information.
 */
public func OnFinishReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode, ...);

	var source = this->GetAmmoReloadContainer();
	if (source)
	{	
		var ammo_type = firemode.ammo_id;
		var ammo_max = firemode.ammo_load ?? 1;
		var ammo_available = this->GetAmmo(ammo_type); // how many do I have
		
		var ammo_requested = ammo_max - ammo_available; // receive only as much as you need

// TODO: remove the log output once unit testing is complete
//		Log("Reloaded: ");
//		Log(" * ammo in source before = %d", source->GetAmmo(ammo_type));
//		Log(" * ammo_type = %d", ammo_type);
//		Log(" * ammo_max = %d", ammo_max);
//		Log(" * ammo_available = %d", ammo_available);
//		Log(" * ammo_requested = %d", ammo_requested);

		var ammo_received = Abs(source->DoAmmo(ammo_type, -ammo_requested)); // see how much you can get
//		Log(" * ammo_received = %d", ammo_received);
		var ammo_spare = (ammo_available + ammo_received) % (firemode.ammo_usage ?? 1); // get ammo only in increments of ammo_usage

		source->DoAmmo(ammo_type, ammo_spare); // give back the unecessary ammo
		ammo_received -= ammo_spare;           // adjust the counter accordingly
//		Log(" * ammo_spare = %d", ammo_spare);
//		Log(" * ammo_inserted = %d", ammo_received);
//		Log(" * ammo in source after = %d", source->GetAmmo(ammo_type));
	
		this->DoAmmo(ammo_type, ammo_received);
	}
	
	if (firemode.progress_bar) firemode.progress_bar->Close();
}


/**
	Condition when the weapon can be reloaded: 
	The {@link Plugin_Weapon_ReloadFromAmmoSource#GetAmmoReloadContainer}
	has ammo of the type that is specified in the firemode.

	@par user The object that is using the weapon.

	@par firemode A proplist containing the fire mode information.

	@return {@c true} by default. Overload this function
         for a custom condition.
 */
public func CanReload(object user, proplist firemode)
{
	var source = this->GetAmmoReloadContainer();
	if (source)
	{
		return source->GetAmmo(firemode.ammo_id) > 0;
	}
	else
	{
		return false;
	}
}


/**
	Gets the ammunition manager that provides the weapon with new ammo.

	@return object An object that is a {@link Library_AmmoManager}.
 */
public func GetAmmoReloadContainer()
{
	FatalError("You have to implement this function yourself.");
}
