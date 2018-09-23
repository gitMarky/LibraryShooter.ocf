#include Library_Firearm
#include Weapon_Base
#include Plugin_Firearm_AmmoLogic
#include Plugin_Firearm_ReloadFromAmmoSource
#include Plugin_Firearm_ReloadStates
#include Plugin_Firearm_ReloadStates_Container


public func GetAmmoContainer()
{
	return GetAmmoReloadContainer();
}

public func GetAmmoReloadContainer()
{
	return Contained();
}
