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
	firearm_reload.current_state = {};
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
	return firearm_reload.current_state[GetReloadStateID(firemode)];
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
	
	@par firemode The reload state is changed for this firemode.
	@par state the reload state. A value of {@code nil} means, that
	     the weapon is reloaded.
 */
func SetReloadState(proplist firemode, proplist state)
{
	firearm_reload.current_state[GetReloadStateID(this->GetFiremode())] = state; // FIXME: Hack, needs a parameter
}


/**
	Gets an identifier for accessing the reload state
	from the saved data.
	
	@par firemode The identifier is queried for this firemode.
	@return string Should return the type of reload state
	               depending on the firemode.
	               By default, that is the {@link Library_Firearm_Firemode#GetAmmoID}
	               but you are free to overload according to whatever logic
	               seems best.
 */
func GetReloadStateID(proplist firemode)
{
	return Format("%i", firemode->GetAmmoID());
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
		this.state_event = false;
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
		this.percentage = BoundBy(time * 100 / state.Delay, 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;
		
		// Do a progress update if necessary
		if (this.progress > 0)
		{
			this.Target->OnProgressReload(this.user, this.x, this.y, this.firemode, this.percentage, this.progress);
			this.percent_old = this.progress;
		}
		
		// Check if there should be an event callback
		if (state.Event > 0 && time > state.Event && !this.state_event)
		{
			this.state_event = true;
			state->~OnEvent(this.Target, this.user, this.x, this.y, this.firemode);
		}

		// Done with this state?
		if (time > state.Delay && !this.state_finished)
		{
			this.state_finished = true;
			state->~OnFinish(this.Target, this.user, this.x, this.y, this.firemode);
						
			// Do the reload if anything is necessary and end the effect if successful
			var next_state = this.Target->GetReloadState(this.firemode);
			if (next_state == nil)
			{
				this.Target->DoReload(this.user, this.x, this.y, this.firemode);
				return FX_Execute_Kill;
			}
			else
			{
				ResetState();
				return FX_OK;
			}
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
	
	@note
	State 'nil' is the 'ready to reload' or default state.
	
	@note
	For every state you will have to implement some functions or properties:
	<ul>
	<li>Delay - the delay of the state, in frames; Default value = 1</li>
	<li>Event - if set to a value other than 0, in frames, there will be a callback; Default value = 0</li>
	<li>OnStart - (optional) callback when the state starts</li>
	<li>OnFinish - (optional) callback when the state is completed</li>
	<li>OnCancel - (optional) callback when the state is interrupted</li>
	<li>OnEvent - (optional) callback when the event is fired, if .Event is other than 0</li>
	<li>Parameters for all callbacks: {@code object firearm, object user, int x, int y, proplist firemode}</li>
	</ul>
	
	@note
	As a design rule, each state should play create permanent effects or status changes only when it is finished.
	If you create a state that does something when it begins, then it does not matter whether you finish it
	correctly or cancel it.
 */
static const Firearm_ReloadState = new Global
{
	Event = 0,          // An event takes place at this time
	Delay = 1,          // The state takes this long.
};
