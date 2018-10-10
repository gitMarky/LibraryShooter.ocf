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

local ReloadStateMap; // ActMap for ReloadStates

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
	var name = firearm_reload.current_state[GetReloadStateID(firemode)];
	if (name)
	{
		return FindReloadState(name);
	}
	else
	{
		return nil;
	}
}


func FindReloadState(string name)
{
	for (var prop in GetProperties(ReloadStateMap))
	{
		if (ReloadStateMap[prop].Name == name)
		{
			return ReloadStateMap[prop];
		}
	}
	return nil;
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
func SetReloadState(proplist firemode, state)
{
	if (GetType(state) == C4V_Nil)
	{
		// Do nothing - for compatibility with ActMap behaviour
	}
	else if (GetType(state) == C4V_String)
	{
		if (state == "Hold")
		{
			// Do nothing - for compatibility with ActMap behaviour
		}
		else
		{
			if (state == "Idle")
			{
				state = nil; // "Idle" resets to default (=nil) - for compatibility with ActMap behaviour
			}
			
			firearm_reload.current_state[GetReloadStateID(firemode)] = state;
			_inherited(firemode, state, ...);
		}
	}
	else if (GetType(state) == C4V_PropList)
	{
		return SetReloadState(firemode, state.Name);
	}
	else
	{
		FatalError(Format("Unsupported format: %v, expected C4V_PropList or C4V_String", GetType(state)));
	}
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
		Log("Reset state");
		this.Time = 0;
		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.state_started = false;
		this.state_finished = false;
		this.state_event = false;
		
		if (this.user_animation && this.user)
		{
			this.user->StopAnimation(this.user_animation);
		}
		this.user_animation = nil;
	},

	Timer = func (int time)
	{
		// Request current state, or default state
		// The if-clause is not part of GetReloadState(), so that the reloading can stop in the exit condition where next_state is determined
		var state = this.Target->GetReloadState(this.firemode);
		if (nil == state)
		{
			var default_name = this.Target->GetReloadStartState(this.firemode);
			state = this.Target->FindReloadState(default_name);
			this.Target->SetReloadState(this.firemode, state);
		}

		// Cancel if user cannot reload
		if (!this.Target->IsUserReadyToReload(this.user) || !state)
		{
			if (state)
			{
				IssueCallbacks(state, "Abort");
				if (state.AbortAction)
				{
					this.Target->SetReloadState(this.firemode, Evaluate(state.AbortAction));
				}
			}
			this.Target->CancelReload(this.user, this.x, this.y, this.firemode, false);
			return FX_Execute_Kill;
		}
		
		// Initial callback
		if (!this.state_started)
		{
			Log("Starting state: %s", state.Name);
			this.state_started = true;
			IssueCallbacks(state, "Start");
			
			if (state.UserAnimation)
			{
				var begin = state.UserAnimation.Begin ?? 0;
				var end = state.UserAnimation.End ?? this.user->GetAnimationLength(state.UserAnimation.Name);
				var position = BoundBy(state.UserAnimation.Position, begin, end);
				this.UserAnimation = this.user->PlayAnimation(state.UserAnimation.Name, 
				                                          state.UserAnimation.Slot ?? (CLONK_ANIM_SLOT_Arms + 1),
				                                          Anim_Linear(position, begin, end, state.Delay, ANIM_Remove),
				                                          Anim_Linear(0, 0, 1000, Max(1, state.Delay / 4), ANIM_Remove));
			}
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
			IssueCallbacks(state, "Event");
		}

		// Done with this state?
		if (time > state.Delay && !this.state_finished)
		{
			Log("Finishing state: %s", state.Name);
			this.state_finished = true;
			IssueCallbacks(state, "End");

			// Do the reload if anything is necessary and end the effect if successful
			var next_state = Evaluate(state.NextAction); // WAS: this.Target->GetReloadState(this.firemode);
			Log("Calling SetReloadState %s", next_state);
			this.Target->SetReloadState(this.firemode, next_state);
			
			// Cleanup
			if (next_state == "Idle")
			{
				Log("Next state is idle");
				this.Target->DoReload(this.user, this.x, this.y, this.firemode);
				return FX_Execute_Kill;
			}
			else if (next_state == "Hold")
			{
				return FX_OK;
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
	
	IssueCallbacks = func (proplist state, string type)
	{
		var internal         = state[Format("%sFunc", type)];        // This is the one that should not be overloaded
		var internal_default = Format("~%s_On%s", state.Name, type); // This is a fallback option
		var user_defined     = state[Format("%sCall", type)];        // This is like the ActMap definition an can be defined by the user

		IssueCallback(internal ?? internal_default); 
		IssueCallback(user_defined);
	},
	
	IssueCallback = func (callback)
	{
		if (callback)
		{
			return this.Target->Call(callback, this.user, this.x, this.y, this.firemode);
		}
	},
	
	Evaluate = func (string state_name)
	{
		// State name starts with '#' => evaluate as a function!
		if (state_name && RegexSearch(state_name, "#")[0] == 0)
		{
			var call_name = RegexReplace(state_name, "#(.+)", "$1");
			state_name = IssueCallback(call_name);
		}
		return state_name;
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
	<li>Name - string, name of this state.</li> 
	<li>NextAction - string, name of the state that is set once this state finishes. Special values:
	                 <ul>
	                 <li>nil: (same behavior as in ActMap) Repeat the same state.</li>
	                 <li>{@code "Idle"}: (same behavior as in ActMap) Revert the weapon to default state.</li>
	                 <li>{@code "Hold"}: (same behavior as in ActMap) The current state stays, but is not repeated.</li>
	                 <li>{@code "#MyFunctionName"}: Will issue a callback "MyFunctionName()" in the firearm, with callback parameters (see below).</li>
	                 </ul>
	</li>
	<li>Delay - int, the delay of the state, in frames; Default value = 1</li>
	<li>Event - int, if set to a value other than 0, in frames, there will be a callback; Default value = 0</li>
	<li>*Func - (optional) string, callbacks where * may be [Start, End, Abort, Event]: Use this for functions that do internal changes in the weapon.
	                                                                                    Overload with care.
	                                                                                    Defaults to callback "~%s_On%s", the first placeholder being the state name, and the second placeholder being *</li>
	<li>*Call - (optional) string, callbacks where * may be [Start, End, Abort, Event]: Use this for user defined effects and overload at will.</li>
	<li>Parameters for all callbacks: {@code object user, int x, int y, proplist firemode}</li>
	<li>UserAnimation - (optional) proplist, animation that should be played in the user
	<ul>
		<li>Name - string, name of the animation in the user.</li>
		<li>Begin - (optional) int, beginning position of the animation; Default value = 0</li>
		<li>End - (optional) int, end position of the animation; Default value = animation length</li>
		<li>Position - (optional) int, position that the animation plays from, between Begin and End</li>
	</ul>
	</li>
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
	NextAction    = nil,  // The state will be assumed after regular finish of the action; As in animations: nil = Repeat, "Hold" = Stay in the same state, but do not repeat, "Idle" = default state 
	AbortCall     = nil,  // 
	EndCall       = nil,  // 
	StartCall     = nil,  // 
	EventCall     = nil,  // 
	Animation     = nil,  // 
	UserAnimation = nil,  // Animation in the object that 'uses' the weapon
};
