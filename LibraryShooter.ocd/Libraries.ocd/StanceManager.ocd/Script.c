/**
	Stance manager.

	You can save known stances in a local property StanceMap.
	Originally, the library was designed without a StanceMap,
	but it makes handling the stances easier.

	@author Marky
 */

/* --- Properties --- */

local lib_stance_manager;

local StanceMap; // "ActMap" for Stances

/* --- Engine Callbacks --- */

func Construction(object by)
{
	lib_stance_manager = lib_stance_manager ?? {};
	lib_stance_manager.current_stance = lib_stance_manager.current_stance ?? {};
	return _inherited(by, ...);
}


/* --- Interface --- */

/**
	Adds one or more stances to the stance map.
	This is useful if you define stances dynamically in a script.
	
	@par stance This stance will be added. Optionally, you can provide
	            additional stances as second, third, ... parameter.
	            These will be added in order, until the parameter is nil
	            (you do not have to pass nil explicitely).
	            
	@return object Returns this object, so that further function calls
	               can be issued.
 */
public func AddStance(proplist stance)
{
	if (stance)
	{
		// Init proplist if necessary
		if (StanceMap == nil)
		{
			StanceMap = {};
		}

		// Add the current stance.
		// If it has a name, use that as the property name,
		// otherwise provide a name.
		var name = stance.Name ?? Format("stance%06d", GetLength(GetProperties(StanceMap)));
		StanceMap[name] = stance;
		
		// Keep adding
		AddStance(...);
	}
	return this;
}


/**
	Gets the stance that is currently active for the given channel.
	
	@par channel The stance channel.
	
	@return proplist Returns the stance that is active, or {@code nil}
	                 if there is no active stance.
 */
public func GetStance(any channel)
{
	return lib_stance_manager.current_stance[GetStanceChannel(channel)];
}


/**
	Changes the active stance for the given channel.
	
	@par stance Can be a proplist or the stance name.
	            This stance wil be active.
	            Must not be {@code nil} if the channel is not given
	            Can be nil if the channel is given, in order to
	            remove the stance altogether.

	@par channel The stance channel.
	
	@par force (optional) By default, the stance is
	           changed only if there is a valid transition
	           from the active stance to the new stance.
	           By providing {@code force = true} the
	           desired stance is set, even if the transition
	           is invalid.
	           
	@return bool Returns {@code true} if the stance
	             has been changed, implying that
	             an animation update is necessary.
	             
	@note When the stance changes, a callback is issued
	      to the object: {@code OnStanceChanged(proplist new_stance, any channel, bool force, proplist previous_stance)}
 */
public func SetStance(any stance, any channel, bool force)
{
	if (!channel)
	{
		AssertNotNil(stance);
	}
	stance = GetStanceDefinition(stance);
	var current = GetStance(channel);
	if (current == nil || force || current->HasTransitionTo(stance))
	{
		lib_stance_manager.current_stance[GetStanceChannel(channel)] = stance;
		
		var has_changed = stance != current;
		if (has_changed)
		{
			this->OnStanceChanged(stance, channel, force, current);
		}

		GetStanceMonitor()->AddChannel(channel);
		
		return has_changed;
	}
	return false;
}

/* --- Internals --- */

func GetStanceChannel(any channel)
{
	var default = "0";
	var type = GetType(channel);
	if (type == C4V_Nil || (type == C4V_String && GetLength(channel) == nil))
	{
		return default;
	}
	else
	{
		return Format("%v", channel);
	}
}


/**
	Gets the stance definition.
	
	@par stance In case this is a proplist it will be returned directly.
	            Passing a string will prefer stances in {@code this.StanceMap}
	            where {@code stance.Name} equals the string, otherwise of the
	            property name is equal to the string.
	@note For {@code StanceMap = { StanceAbc = {Name = "xyz"}, xyz = {Name = "StanceAbc"}}}
	      {@code GetStanceDefinition("xyz")} will return the first stance in the list, because the name is preferred.
	      {@code GetStanceDefinition("StanceAbc")} will also return the first stance in the list, because they are
	      searched in order, but while the name property "xyz" does not match the name of the property matches.
 */
func GetStanceDefinition(any stance)
{
	if (GetType(stance) == C4V_PropList || stance == nil)
	{
		return stance;
	}
	else if (GetType(stance) == C4V_String)
	{
		return GetPropertyByAttribute(stance, this.StanceMap, "Name", "StanceMap");
	}
	else
	{
		FatalError("Stance type %v not supported; Value was %v", GetType(stance), stance);
	}
}


func GetStanceMonitor()
{
	if (lib_stance_manager.monitor == nil)
	{
		lib_stance_manager.monitor = CreateEffect(FxStanceMonitor, 1, 1);
	}
	return lib_stance_manager.monitor;
}


func OnStanceChanged(proplist stance, any channel, bool force, proplist previous_stance)
{
	if (previous_stance != nil)
	{
		for (var behaviour in previous_stance->GetBehaviours())
		{
			behaviour->~OnStanceReset(this, channel, force);
		}
	}

	if (stance != nil)
	{
		for (var behaviour in stance->GetBehaviours())
		{
			behaviour->~OnStanceSet(this, channel, force);
		}
	}
}

local FxStanceMonitor = new Effect
{
	Name = "StanceMonitor",
	
	Construction = func ()
	{
		this.Channels = [];
	},
	
	Timer = func ()
	{
		for (var channel in this.Channels)
		{
			var stance = Target->GetStance(channel);
			if (stance)
			{
				for (var behaviour in stance->GetBehaviours())
				{
					behaviour->~Timer(Target, channel);
				}
			}
		}
	},
	
	AddChannel = func (any channel)
	{
		if (!IsValueInArray(this.Channels, channel))
		{
			PushBack(this.Channels, channel);
		}
	},
};

/* --- Data Structure --- */

static const StanceDefinition = new Global
{
	Transitions = nil, // Array, defines stances that you can change to from this stance
	Behaviours = nil,  // Array, defines the behaviour of the stance

	// --- Interface

	/**
		Adds a behaviour to this stance.

		@par behaviour This behaviour is added. A behaviour is a proplist
		               with the following functions:
		               * OnStanceSet(object user, any channel, bool force)
		               * OnStanceReset(object user, any channel, bool force)
		               
		               Where {@code user} is the object in that the stance
		               is changed, {@code channel} is the stance channel,
		               and {@code force} say wether the change was forced.
		               
		               For easier reference, it is suggested to name
		               make a global proplist StanceBehaviour_* that
		               defines the format of this behaviour and
		               contains the implementations of both functions.
		               
		@return proplist Returns the currenct stance, so that you can 
		                 add further behaviours to it.
	 */
	AddBehaviour = func (proplist behaviour)
	{
		if (this.Behaviours == nil)
		{
			this.Behaviours = [];
		}
		if (!IsValueInArray(this.Behaviours, behaviour))
		{
			PushBack(this.Behaviours, behaviour);
		}
		return this;
	},
	
	/**
		Clears the behaviours. Use with care!
	 */
	ClearBehaviours = func ()
	{
		this.Behaviours = [];
		return this;
	},

	/**
		Adds a valid transition from this stance
		to another, unidirectional.
		
		@par to_stance The valid target stance.
		@return proplist Returns {@code to_stance}, so that you can 
		                 define further transitions from there on.
		                 Looks more natural when scripting.
	 */
	AddTransition = func (proplist to_stance)
	{
		if (this.Transitions == nil)
		{
			this.Transitions = [];
		}
		if (!HasTransitionTo(to_stance))
		{
			PushBack(this.Transitions, to_stance);
		}
		return to_stance;
	},

	/**
		Gets all behaviours that are defined for this stance.
		
		@return array Returns the behaviours, as an array.
		              Never returns {@code nil}.
	 */
	GetBehaviours = func ()
	{
		return this.Behaviours ?? [];
	},

	/**
		Finds out whether you can transition to the desired stance.
		
		@par to_stance The target stance.
		@return {@code true} if the transition is valid.
	 */
	HasTransitionTo = func (proplist to_stance)
	{
		return IsValueInArray(this.Transitions, to_stance);
	},
};
