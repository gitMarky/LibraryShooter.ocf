#include Library_Weapon

local Name = "$Name$";
local Description = "$Description$";
local UsageHelp = "$UsageHelp$";
local Collectible = 1;
local Rebuy = true;

func Hit()
{
	Sound("GeneralHit?");
}

public func GetAmmoSource()
{
	return AMMO_Source_Container;
}

public func GetAmmoContainer()
{
	return Contained();
}