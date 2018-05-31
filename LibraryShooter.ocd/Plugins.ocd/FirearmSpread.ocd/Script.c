/**
	Plugin for the user of a firemode.
	
	Enables accumulation of spread for the user.

	@author Marky
 */

/* --- Properties --- */

local plugin_firearm_spread = 0;

/* --- Callbacks from the firearm library --- */

func OnFirearmFire(object weapon, proplist firemode)
{
	DoFirearmSpread(firemode->GetSpreadPerShot());
}

func GetFirearmSpread(object weapon, proplist firemode)
{
	return plugin_firearm_spread;
}

/* --- Internals --- */

func DoFirearmSpread(int spread)
{
	plugin_firearm_spread = Max(0, plugin_firearm_spread + spread);
}
