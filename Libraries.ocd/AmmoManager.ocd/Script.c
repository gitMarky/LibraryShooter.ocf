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
static const AMMO_Mode_Abstract = 1;		// ammo is saved as numbers
static const AMMO_Mode_Items = 2;			// ammo is saved from collected items, such as arrow packs

static const AMMO_Mode_Weapon_Owner = 3;	// ammo is saved in the carrier of the weapon (as in arcade games)
static const AMMO_Mode_Weapon_Local = 4;	// ammo is saved in a local storage (magazine, clip, chamber, etc.) in the weapon
static const AMMO_Mode_Weapon_Unlimited = 5;// ammo is unlimited - yay for cheats

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local ammo_mode = AMMO_Mode_Abstract; // default

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

public func GetAmmo(id ammo)
{
	if (ammo_mode == AMMO_Mode_Abstract)
	{
		return GetItems(ammo);
	}
}