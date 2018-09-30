#include Library_Firearm
#include Weapon_Base
#include Library_AmmoManager
#include Plugin_Firearm_AmmoLogic
#include Plugin_Firearm_ReloadFromAmmoSource
#include Plugin_Firearm_ReloadStates
#include Plugin_Firearm_ReloadStates_Container
#include Plugin_Firearm_AmmoChamber

local Description = "Reloading with container plugin, ammo chamber";

public func GetAmmoSource(id ammo)
{
	return AMMO_Source_Local;
}

public func GetAmmoReloadContainer()
{
	return Contained();
}
