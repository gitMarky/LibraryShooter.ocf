#include Library_AmmoManager

/* --- Override, so that we can set the values for the test --- */

local Test_AmmoSource = AMMO_Source_Infinite;

public func GetAmmoSource(id ammo)
{
	return Test_AmmoSource;
}
