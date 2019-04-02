#include Library_AmmoManager

/* --- Override, so that we can set the values for the test --- */

local Test_AmmoSource = AMMO_Source_Infinite;
local Test_AmmoContainer = nil;

public func GetAmmoSource(id ammo)
{
	return Test_AmmoSource;
}

public func GetAmmoContainer()
{
	return Test_AmmoContainer;
}