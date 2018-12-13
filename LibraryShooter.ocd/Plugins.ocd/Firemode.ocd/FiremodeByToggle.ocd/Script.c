/**
	Allows fire mode selection by cycling through the modes.
 */


local next_firemode_index = 0;
local current_firemode_index = 0;

local IntToggleFiremode = new Effect
{
	current_index = 0,
	next_index = 0,
	delay = 0,
	modes = [],

	Timer = func (int time)
	{
		if (delay <= 0)
		{
			if (current_index != next_index)
			{
				Target->ScheduleSetFiremode(next_index);
			}

			return FX_Execute_Kill;
		}

		delay -= 1;
	},

	SetIndex = func (int index)
	{
		current_index = index;
		next_index = index;
		NextIndex();
	},

	SetFiremodes = func (array firemodes)
	{
		modes = firemodes;
	},

	SetDelay = func (int delay)
	{
		this.delay = delay;
	},

	NextIndex = func ()
	{
		next_index += 1;
		if (next_index >= GetLength(modes) || next_index < 0 ) 
		{
			next_index = 0;
		}
		Target->~OnSelectFiremode(modes[next_index]);
	},
};

public func ToggleFiremode()
{
	var modes = this->GetAvailableFiremodes();

	if (!modes || GetLength(modes) == 0)
	{
		FatalError("Cannot toggle fire modes: The GetAvailableFiremodes() returns nothing or an empty array.");
	}

	var toggle_effect = GetEffect("IntToggleFiremode", this);

	if (!toggle_effect)
	{
		toggle_effect = CreateEffect(IntToggleFiremode, 1, 1);
		toggle_effect->SetFiremodes(modes);
		toggle_effect->SetIndex(this->~GetFiremode());
		toggle_effect->SetDelay(ToggleFiremodeDelay());
	}
	else
	{
		toggle_effect->NextIndex();
		toggle_effect->SetDelay(ToggleFiremodeDelay());
	}
}


/**
	Delay until the toggled firemode is activated.

	@return int The delay, in frames.
 */
public func ToggleFiremodeDelay(){ return 30;}

public func OnSelectFiremode(proplist firemode)
{
	// do nothing at the moment
}
