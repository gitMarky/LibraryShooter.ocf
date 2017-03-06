/**
 Allows fire mode selection via the use function:
 - normal use is firemode "primary"
 - alternative use is firemode "secondary"
 */

#include Plugin_Weapon_FiremodeBySelection

local next_firemode_index = 0;


public func ToggleFiremode()
{
	var modes = this->GetAvailableFiremodes();
	
	if (!modes || GetLength(modes) == 0)
	{
		FatalError("Cannot toggle fire modes: The GetAvailableFiremodes() returns nothing or an empty array.");
	}
	
	next_firemode_index += 1;
	if (next_firemode_index >= modes.length) next_firemode_index = 0;
	
	ScheduleChangeFiremode(modes[next_firemode_index]);
}
