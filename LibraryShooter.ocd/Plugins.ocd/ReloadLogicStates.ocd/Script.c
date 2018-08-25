/**
	Plugin for weapons: Reloading stages.
	
	@note
	You have to call the inherited function in:
	- Construction()
	if you use this.

	@author Marky
 */

/* --- Properties --- */

local firearm_reload;

/* --- Engine callbacks --- */

func Construction(object by)
{
	firearm_reload = firearm_reload ?? {};
	firearm_reload.current_state = nil; // Ready to go!
	return _inherited(by, ...);
}

/* --- Interface --- */

/**
	Checks if the weapon is currently reloading.

	@return The reloading effect.
*/
func IsReloading()
{
	return GetEffect("IntReloadStagesEffect", this);
}


/**
	Callback: The weapon reload effect should be created.

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
 */
func StartReloadProcess(object user, int x, int y, proplist firemode)
{
	CreateEffect(IntReloadStagesEffect, 1, 1, user, x, y, firemode);
}

/* --- Internal --- */

local IntReloadStagesEffect = new Effect
{
	Construction = func (object user, int x, int y, proplist firemode)
	{
		this.user = user;
		this.x = x; // x and y will be updated by StartReload
		this.y = y;
		this.firemode = firemode;

		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.is_reloaded = false;
	},

	Timer = func (int time)
	{
		// Cancel if user cannot reload
		if (!this.Target->IsUserReadyToReload())
		{
			this.Target->CancelReload(this.user, this.x, this.y, this.firemode, false);
		}
	
		// Increase progress percentage depending on the reloading delay of the firemode
		this.percentage = BoundBy(time * 100 / this.firemode->GetReloadDelay(), 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the reloading process is finished based on the reloading delay of the firemode
		if (time > this.firemode->GetReloadDelay() && !this.is_reloaded)
		{
			this.is_reloaded = true;

			// Do the reload if anything is necessary and end the effect if successful
			if (this.Target->DoReload(this.user, this.x, this.y, this.firemode))
			{
				return FX_Execute_Kill;
			}
		}

		// Do a progress update if necessary
		if (this.progress > 0)
		{
			this.Target->OnProgressReload(this.user, this.x, this.y, this.firemode, this.percentage, this.progress);
			this.percent_old = this.progress;
		}
	},
	
	GetProgress = func ()
	{
		return this.percentage;
	},
};

/* --- State definition --- */

/**
	Prototype for reloading state.
	
	State 'nil' is the 'ready to reload' or default state.
	
	@note
	For every own state you will have to implement some functions:
	<ul>
	<li>{@code Setup = func ()}. This is an optional callback that lets you define the next state, duration, etc.</li>
	</ul>
 */
static const Firearm_ReloadState = new Global
{
	is_defined = false,
	state_next = nil,

	// Use this function as a constructor for the state; Call
	Create = func ()
	{
		this->~Setup();
		is_defined = true;
		return this;
	},

	// --- Getters

	// Gets the state that should be set after this one.
	// If the function returns 'nil', this means that reloading
	// is done and 
	GetNextState = func ()
	{
		return this.state_next;
	}
	
	// --- Setters
	
	SetNextState = func (proplist state)
	{
		AssertOnlyBeforeCreation();
		this.state_next = state;
		return this;
	}
	
	// --- Internals
	
	AssertOnlyBeforeCreation = func ()
	{
		if (is_defined)
		{
			FatalError("You should called this function after the state was created with ->Create(). This is prohibited.");
		}
	}
};
