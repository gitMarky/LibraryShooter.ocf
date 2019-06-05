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

public func GetStance(any channel)
{
	return lib_stance_manager.current_stance[GetStanceChannel(channel)];
}

public func SetStance(any stance, any channel, bool force)
{
	AssertNotNil(stance);
	stance = GetStanceDefinition(stance);
	var current = GetStance(channel);
	if (current == nil || force || current->HasTransitionTo(stance))
	{
		lib_stance_manager.current_stance[GetStanceChannel(channel)] = stance;
		return true;
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
	if (GetType(stance) == C4V_PropList)
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

/* --- Data Structure --- */

static const StanceDefinition = new Global
{
	Transitions = nil, // Array, defines stances that you can change to from this stance

	// --- Interface
	
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
		Finds out whether you can transition to the desired stance.
		
		@par to_stance The target stance.
		@return {@code true} if the transition is valid.
	 */
	HasTransitionTo = func (proplist to_stance)
	{
		return IsValueInArray(this.Transitions, to_stance);
	},
};
