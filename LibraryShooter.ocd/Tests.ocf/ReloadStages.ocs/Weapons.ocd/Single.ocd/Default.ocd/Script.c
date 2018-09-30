#include Library_Firearm
#include Weapon_Base
#include Library_AmmoManager
#include Plugin_Firearm_AmmoLogic
#include Plugin_Firearm_ReloadFromAmmoSource
#include Plugin_Firearm_ReloadStates
#include Plugin_Firearm_ReloadStates_Single

local Description = "Reloading with single ammo plugin, default settings";

public func GetAmmoSource(id ammo)
{
	return AMMO_Source_Local;
}

public func GetAmmoReloadContainer()
{
	return Contained();
}

func PlaySoundInsertShell()
{
	PushBack(CurrentTest().states, "Single_Ammo_Insert_Counter"); // Special non-existing state
}
