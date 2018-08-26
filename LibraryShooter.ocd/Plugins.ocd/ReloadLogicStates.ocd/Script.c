/**
	Plugin for weapons: Reloading stages.
	
	@note
	You have to call the inherited function in:
	- Construction()
	if you use this.
	
	@note
	Issues: Handle multiple fire modes how?
	Theoretically there can be fire modes that use the same ammo, so it makes sense to share the reload state between ammo types.

	@author Marky
 */

/* --- Properties --- */

local firearm_reload;

/* --- Engine callbacks --- */

func Construction(object by)
{
	firearm_reload = firearm_reload ?? {};
	firearm_reload.current_state = nil; // Ready to go. Can be changed to array or proplist in case that multiple states are necessary.
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
	Gets the current reload state.
 */
func GetReloadState()
{
	return firearm_reload.current_state ?? GetReloadStateDefault();
}


/**
	Gets the default reload state that the weapon starts reloading from.
 */
func GetReloadStateDefault()
{
	return nil;
}


/**
	Sets the current reload state.
	
	@par state the reload state. A value of {@code nil} means, that
	     the weapon is reloaded.
 */
func SetReloadState(proplist state)
{
	firearm_reload.current_state = state;
}


/* --- Callbacks from the firearm --- */

/**
	Callback: The weapon reload effect should be created.

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
 */
func StartReloadProcess(object user, int x, int y, proplist firemode)
{
	var state = GetReloadState();
	if (state)
	{
		var process = CreateEffect(IntReloadStagesEffect, 1, 1, user, x, y, firemode);
		process->SetState(state);
	}
	else
	{
		FatalError("Cannot start reloading: No default state is set");
	}
}


/**
	Callback: the weapon user cancelled reloading. Does nothing by default.@br

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
	@par requested_by_user Is {@c true} if the user releases the use button while the weapon is reloading. Otherwise, for example if the user changes the firemode is {@c false}.
 */
public func OnCancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
	var state = GetReloadState();
	if (state)
	{
		SetReloadState(state->GetCancelState());
	}
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
	
	SetState = func (proplist state)
	{
		this.state = state;
		state->~OnStart(this.Target, this.user, this.x, this.y, this.firemode);
	},

	Timer = func (int time)
	{
		// Cancel if user cannot reload
		if (!this.Target->IsUserReadyToReload() || !this.state)
		{
			if (this.state)
			{
				this.state->~OnCancel(this.Target, this.user, this.x, this.y, this.firemode);
			}
			this.Target->CancelReload(this.user, this.x, this.y, this.firemode, false);
		}
	
		// Increase progress percentage depending on the reloading delay of the firemode
		this.percentage = BoundBy(time * 100 / this.firemode->GetReloadDelay(), 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the reloading process is finished based on the reloading delay of the firemode
		if (time > this.state->GetDelay() && !this.is_reloaded)
		{
			this.is_reloaded = true;
		}
		
		// Done reloading?
		if (this.is_reloaded)
		{
			// TODO
			this.state->~OnFinish(this.Target, this.user, this.x, this.y, this.firemode);
			
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
	state_next = nil,   // This state is assumed if the current state was finished successfully.
	state_cancel = nil, // This state is assumed if the current state was cancelled.
	delay = 1,          // The state takes this long.

	// Use this function as a constructor for the state; Call
	Create = func ()
	{
		// Repeat the state upon cancellation, as a default
		SetCancelState(this);
		// Allow user-defined setup
		this->~Setup();
		// Finish
		is_defined = true;
		return this;
	},

	// --- Getters
	
	// The state lasts this long
	GetDelay = func ()
	{
		return this.delay;
	},

	// Gets the state that should be set after this one,
	// if the action was successful.
	//
	// If the function returns 'nil', this means that reloading
	// is done.
	GetNextState = func ()
	{
		return this.state_next;
	},
	
	// Gets the state that should be set after this one,
	// if the action was cancelled.
	//
	// If the function returns 'nil', this means that reloading
	// is done.
	GetCancelState = func ()
	{
		return this.state_cancel;
	},
	
	// --- Setters
	
	SetDelay = func (int delay)
	{
		AssertOnlyBeforeCreation();
		this.delay = delay;
		return this;
	},
	
	SetNextState = func (proplist state)
	{
		AssertOnlyBeforeCreation();
		this.state_next = state;
		return this;
	},
	
	SetCancelState = func (proplist state)
	{
		AssertOnlyBeforeCreation();
		this.state_cancel = state;
		return this;
	},
	
	// --- Internals
	
	AssertOnlyBeforeCreation = func ()
	{
		if (is_defined)
		{
			FatalError("You should called this function after the state was created with ->Create(). This is prohibited.");
		}
	},
};
