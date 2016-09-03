/**
 This is an interface for saving ammunition in an object. It should be included in 
 objects that carry ammunition.
 @author Marky
 @version 0.1.0
 */
 
 #include Library_DefinitionBag
 
/*
 TODO: Planned stuff
 - two modes for distributing/saving ammo: abstract (variables) or with objects
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

// these are numbers and should not be used as bits
static const AMMO_Source_Local = 1;	// ammo is saved as numbers
static const AMMO_Source_Items = 2;		// ammo is saved from collected items, such as arrow packs

static const AMMO_Source_Container = 3;	// ammo is saved in the carrier of the weapon (as in arcade games)
static const AMMO_Source_Infinite = 4;	// ammo is unlimited - yay for cheats

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

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
 @author Marky
 @version 0.1.0
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
		return GetItems(ammo);
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
		return GetItems(ammo);
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
 @author Marky
 @version 0.1.0
 @note The function considers the ammunition source {@link Library_AmmoManager#GetAmmoSource}.
 @related {@link Library_AmmoManager#GetAmmo}, {@link Library_AmmoManager#SetAmmo}
 */
public func DoAmmo(id ammo, int amount)
{
	if (ammo == nil)
	{
		FatalError("You have to specify a type of ammunition.");
	}
	
	var before = GetAmmo(ammo);
	var after = SetAmmo(ammo, before + amount);
	
	return after - before;
}

/**
 Defines the amount of ammunition that the object currently has.
 @par ammo The type of the ammunition.
 @par amount The new value. The amount of ammunition cannot be changed
             beyond the capacity of the object.
 @return The current amount of ammunition.
 @author Marky
 @version 0.1.0
 @note The function considers the ammunition source {@link Library_AmmoManager#GetAmmoSource}.
 @related {@link Library_AmmoManager#GetAmmo}, {@link Library_AmmoManager#DoAmmo}
 */
public func SetAmmo(id ammo, int new_value)
{
	var ammo_source = GetAmmoSource(ammo);

	if (ammo_source == AMMO_Source_Local)
	{
		return SetItems(ammo, new_value);
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
		if (GetItems(ammo))
		{
			return new_value;
		}
		else
		{
			return 0;
		}
	}
}

/**
 @return an ammunition manager object that handles the ammunition counting
         if {@link Library_AmmoManager#GetAmmoSource} is {@c AMMO_Source_Container}
 @author Marky
 @version 0.1.0
 */
public func GetAmmoContainer()
{
	return nil;
}