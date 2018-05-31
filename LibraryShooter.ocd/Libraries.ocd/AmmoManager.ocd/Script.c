/**
	This is an interface for saving ammunition in an object. It should be included in 
	objects that carry ammunition.

	@author Marky
 */


/* --- Global functions and constants --- */

// These are numbers and should not be used as bits
static const AMMO_Source_Local = 1;	// ammo is saved as numbers
static const AMMO_Source_Items = 2; // ammo is saved from collected items, such as arrow packs

static const AMMO_Source_Container = 3;	// ammo is saved in the carrier of the weapon (as in arcade games)
static const AMMO_Source_Infinite = 4;	// ammo is unlimited - yay for cheats

/* --- Properties --- */

local library_ammo_manager;

/* --- Engine callbacks --- */

/**
	Objects that include this library must call {@link Global#inherited}
	for this function.
 */
public func Construction()
{
	library_ammo_manager = library_ammo_manager ?? {};
	return _inherited(...);
}


/* --- Library code --- */

/**
	Tells the object where to get its ammunition from.@br
	The following options exist:@br
	{@table
		{@tr {@th Constant} {@th Value} {@th Description}}
		{@tr {@td AMMO_Source_Local} {@td 1} {@td The ammunition is saved as a local variable in this object.}}
		{@tr {@td AMMO_Source_Items} {@td 2} {@td The ammunition is requested from a certain object.}}
		{@tr {@td AMMO_Source_Container} {@td 3} {@td The ammunition is requested from a certain object.}}
		{@tr {@td AMMO_Source_Infinite} {@td 4} {@td The object has unlimited ammunition.}}
	}

	@par ammo The type of the ammunition.

	@return int One of the options in the table above. The default return value is
                {@c AMMO_Source_Infinite}. Override this function if you want a different source.
 */
public func GetAmmoSource(id ammo)
{
	return AMMO_Source_Infinite;
}


/**
	Gets the current amount of ammunition of a certain type.

	@par ammo The type of the ammunition.

	@return int The amount that the object currently has.

	@note The function considers the ammunition source {@link Library_AmmoManager#GetAmmoSource}.

	@related {@link Library_AmmoManager#DoAmmo}, {@link Library_AmmoManager#SetAmmo}
 */
public func GetAmmo(id ammo)
{
	if (ammo == nil)
	{
		FatalError("You have to specify a type of ammunition.");
	}
	
	var ammo_source = GetAmmoSource(ammo);

	if (ammo_source == AMMO_Source_Local)
	{
		return Max(0, library_ammo_manager[Format("%i", ammo)]);
	}
	else if (ammo_source == AMMO_Source_Items)
	{
		FatalError("This is not implemented yet.");
	}
	else if (ammo_source == AMMO_Source_Container)
	{
		var owner = this->~GetAmmoContainer();
		
		if (owner == nil)
		{
			FatalError("Could not find the object that should contain the ammunition. Make sure that GetAmmoContainer() returns an existing object.");
		}

		return owner->~GetAmmo(ammo);
	}
	else if (ammo_source == AMMO_Source_Infinite)
	{
		return Max(1, ammo->~MaxAmmo());
	}
}


/**
	Changes the amount of ammunition that the object currently has.

	@par ammo The type of the ammunition.

	@par amount The change, can be positive or negative.
                The amount of ammunition cannot be changed beyond the capacity
                of the object, so the actual amount by which the ammunition was
                changed will be returned. 

	@return The actual change that happened.

	@note The function considers the ammunition source {@link Library_AmmoManager#GetAmmoSource}.

	@related {@link Library_AmmoManager#GetAmmo}, {@link Library_AmmoManager#SetAmmo}
 */
public func DoAmmo(id ammo, int amount)
{
	if (ammo == nil)
	{
		FatalError("You have to specify a type of ammunition.");
	}
	
	if (GetAmmoSource(ammo) == AMMO_Source_Infinite)
	{
		return amount;
	}
	else
	{
		var before = GetAmmo(ammo);
		var after = SetAmmo(ammo, before + amount);
	
		return after - before;
	}
}


/**
	Defines the amount of ammunition that the object currently has.

	@par ammo The type of the ammunition.

	@par amount The new value. The amount of ammunition cannot be changed
                beyond the capacity of the object.

	@return The current amount of ammunition.

	@note The function considers the ammunition source {@link Library_AmmoManager#GetAmmoSource}.

	@related {@link Library_AmmoManager#GetAmmo}, {@link Library_AmmoManager#DoAmmo}
 */
public func SetAmmo(id ammo, int new_value)
{
	var ammo_source = GetAmmoSource(ammo);

	if (ammo_source == AMMO_Source_Local)
	{
		var max = ammo->~MaxAmmo() ?? new_value;
		var value = BoundBy(new_value, 0, max);
		library_ammo_manager[Format("%i", ammo)] = value;
		return value;
	}
	else if (ammo_source == AMMO_Source_Items)
	{
		FatalError("This is not implemented yet.");
	}
	else if (ammo_source == AMMO_Source_Container)
	{
		var owner = this->~GetAmmoContainer();
		
		if (owner == nil)
		{
			FatalError("Could not find the object that should contain the ammunition. Make sure that GetAmmoContainer() returns an existing object.");
		}

		return owner->~SetAmmo(ammo, new_value);
	}
	else if (ammo_source == AMMO_Source_Infinite)
	{
		return Max(1, new_value);
	}
}


/**

	@return an ammunition manager object that handles the ammunition counting
         if {@link Library_AmmoManager#GetAmmoSource} is {@c AMMO_Source_Container}
 */
public func GetAmmoContainer()
{
	return nil;
}


/**
	Identifies the object as an ammo manager.

	@return {@c true}
 */
public func IsAmmoManager()
{
	return true;
}
