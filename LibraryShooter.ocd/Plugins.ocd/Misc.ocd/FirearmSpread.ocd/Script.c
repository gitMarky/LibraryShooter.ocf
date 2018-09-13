/**
	Plugin for the user of a firemode.
	
	Enables accumulation of spread for the user.
	
	Must call inherited function in:
	- Construction

	@author Marky
 */

/* --- Properties --- */

local plugin_firearm_spread = 0;
local plugin_firearm_spread_max = 0;

/* --- Engine callbacks --- */

func Construction(object creator)
{
	AddTimer(this.UpdateFirearmSpread, this->~UpdateFirearmSpreadInterval() ?? 1);
	_inherited(creator,...);
}

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
	if (plugin_firearm_spread_max > 0)
	{
		plugin_firearm_spread = Min(plugin_firearm_spread, plugin_firearm_spread_max);
	}
}

func RaiseFirearmSpread(int spread)
{
	var change = spread - GetFirearmSpread();
	if (change > 0)
	{
		DoFirearmSpread(change);
	}
}

func SetFirearmSpreadLimit(int limit)
{
	plugin_firearm_spread_max = limit;
}

func UpdateFirearmSpread()
{
	DoFirearmSpread(-10);
}
