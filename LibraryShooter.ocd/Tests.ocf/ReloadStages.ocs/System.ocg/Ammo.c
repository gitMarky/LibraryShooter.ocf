#include Library_AmmoManager
#appendto Clonk


public func GetAmmoSource(id ammo)
{
	return this.test_ammo_source;
}

local test_ammo_source = AMMO_Source_Infinite;