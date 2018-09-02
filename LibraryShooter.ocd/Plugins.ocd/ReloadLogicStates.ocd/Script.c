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
	
	@par firemode The reload state is requested for this firemode.
 */
func GetReloadState(proplist firemode)
{
	return firearm_reload.current_state;
}


/**
	Gets the reload state that the weapon starts reloading from.
	
	@par firemode The reload state is requested for this firemode.
 */
func GetReloadStartState(proplist firemode)
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
	CreateEffect(IntReloadStagesEffect, 1, 1, user, x, y, firemode);
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
	_inherited(user, x, y, firemode, requested_by_user, ...);
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
		ResetState();
	},
	
	ResetState = func ()
	{
		this.Time = 0;
		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.state_started = false;
		this.state_finished = false;
	},

	Timer = func (int time)
	{
		// Request current state, or default state
		// The if-clause is not part of GetReloadState(), so that the reloading can stop in the exit condition where next_state is determined
		var state = this.Target->GetReloadState(this.firemode);
		if (nil == state)
		{
			state = this.Target->GetReloadStartState(this.firemode);
			this.Target->SetReloadState(state);
		}

		// Cancel if user cannot reload
		if (!this.Target->IsUserReadyToReload(this.user) || !state)
		{
			//TODO: Move this to CancelReload-Callback
			if (state)
			{
				state->~OnCancel(this.Target, this.user, this.x, this.y, this.firemode);
			}
			this.Target->CancelReload(this.user, this.x, this.y, this.firemode, false);
			return FX_Execute_Kill;
		}
		
		// Initial callback
		if (!this.state_started)
		{
			this.state_started = true;
			state->~OnStart(this.Target, this.user, this.x, this.y, this.firemode);
		}

		// Increase progress percentage depending on the reloading delay of the firemode
		this.percentage = BoundBy(time * 100 / state->GetDelay(), 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the reloading process is finished based on the reloading delay of the firemode
		if (time > state->GetDelay() && !this.state_finished)
		{
			this.state_finished = true;
		}
		
		// Done with this state?
		if (this.state_finished)
		{
			// TODO
			state->~OnFinish(this.Target, this.user, this.x, this.y, this.firemode);
			
			var next_state = this.Target->GetReloadState(this.firemode);
			
			// Do the reload if anything is necessary and end the effect if successful
			if (next_state == nil)
			{
				this.Target->DoReload(this.user, this.x, this.y, this.firemode);
				return FX_Execute_Kill;
			}
			else
			{
				ResetState();
			}
		}
		else if (this.progress > 0) // Do a progress update if necessary
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
