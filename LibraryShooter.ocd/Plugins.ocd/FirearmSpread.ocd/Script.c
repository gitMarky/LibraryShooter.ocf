/**
	Plugin for the user of a firemode.
	
	Enables accumulation of spread for the user.

	@author Marky
 */

/* --- Properties --- */

local plugin_firearm_spread;

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

func DoFirearmSpread(proplist spread)
{
	if (!plugin_firearm_spread)
	{
		plugin_firearm_spread = Projectile_Deviation(0, 100);
		plugin_firearm_spread->SetDynamic(true);
	}
	// Too lazy for making the detailed change, this is enough at the moment
	var scaled = spread->ScalePrecision(plugin_firearm_spread->GetPrecision());
	plugin_firearm_spread->DoValue(scaled->GetValue());
	return [plugin_firearm_spread, scaled];
}
