/**
	Plugin for weapons: Ammo can be put in a chamber before it is fired.

	This concept is abstracted from the following principle: Certain firearms
	load a bullet into the chamber, then shoot, and (semi)-automatically
	chamber a new bullet.
	
	This is modelled in with the {@link Library_Firearm#NeedsRecovery} mechanism
	that simulates recoil and chambering the bullet.
	
	Most of the time you can actually ignore the bullet in the chamber, this is
	relevant only if you want to make a close to realistic weapon where you can
	have a bullet in the chamber and insert a full magazine, so that you essentially
	have one extra bullet.
	
	@note
	The abstraction works as follows:
	- There is a function that loads ammo to the chamber.
	- If the chamber is loaded, the ammo that is necessary
	  for a shot is left in the chamber (that is defined by
	  the fire mode!) when reloading.
	- If the weapon is reloaded, it keeps the saved ammo
	  additionally.
	  
	
	@note
	There is a lot of things that would need to be addressed
	if we handled everything in this plugin:
	- what if you chamber ammo in one firemode, change to 
	  a different firemode that has the same ammo type,
	  but a different ammo usage amount?

    @note
	The abstraction works as follows (old / planned):
	- There is a function that loads ammo to the chamber: TODO
	- You can define how much ammo is put into the chamber: TODO
	- You can define that chambering is possible only if there
	  is enough ammo: TODO
	- You can define that chambering is skipped if the chamber
	  is not completely empty: TODO 
	- Ammo is subtracted and kept in the chamber virtually: TODO

	@author Marky
 */

/* --- Properties --- */

local firearm_ammo;

/* --- Engine callbacks --- */

func Construction(object by)
{
	firearm_ammo = firearm_ammo ?? {};
	firearm_ammo.ammo_chamber = {};
	return _inherited(by, ...);
}


/* --- Interface --- */

/**
	Inserts ammo into the ammo chamber.

	@par ammo The ammo type.

	@return bool {@code true}, if the chamber is empty
	             and there is {@link Plugin_Firearm_AmmoChamber#AmmoChamberCapacity}
	             ammo available.
	             Otherwise returns {@code false}.
 */
public func AmmoChamberInsert(id ammo)
{
	if (!AmmoChamberIsLoaded()
	&& (this->GetAmmo(ammo) >= this->AmmoChamberCapacity(ammo)))
	{
		firearm_ammo.ammo_chamber[Format("%i", ammo)] = true;
		return true;
	}
	else
	{
		return false;
	}
}


/**
	Removes ammo from the ammo chamber. This just removes the 'chamber loaded'
	status, but does not change the ammo amount in any way.
	
	@par ammo The ammo type.
	@return bool {@code true}, if the chamber was loaded and is now empty.
	        Otherwise returns {@code false}.
 */
public func AmmoChamberEject(id ammo)
{
	var can_eject = AmmoChamberIsLoaded();
	firearm_ammo.ammo_chamber[Format("%i", ammo)] = false;
	return can_eject;
}


/**
	Gets the amount of ammo that is protected by the ammo chamber
	when reloading the weapon.
	
	@par ammo The ammo type.
	@return int The amount. Defaults to '1'.
 */
public func AmmoChamberCapacity(id ammo)
{
	return 1;
}


/**
	Find out whether ammo is currently in the chamber.
	
	@par ammo The ammo type.
	@return bool {@code} true, if ammo is currently in the chamber.
 */
public func AmmoChamberIsLoaded(id ammo)
{
	return firearm_ammo.ammo_chamber[Format("%i", ammo)];
}
