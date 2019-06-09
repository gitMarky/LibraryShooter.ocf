/**
	Contains some common stance behaviours.

	@author Marky
 */

/**
 * Applies animation replacements, as in the OpenClonk AimManager.
 */
static const StanceBehaviour_ReplaceAction = new Global
{
	/**
	 * Constructor function.
	 */
	Create = func (array replacements)
	{
		// Copy the list, instead of
		var behaviour = new StanceBehaviour_ReplaceAction { AnimationReplacements = [] };
		var internal = behaviour["AnimationReplacements"];
		for (var replace in replacements)
		{
			PushBack(internal, replace);
		}
		return behaviour;
	},

	OnStanceSet = func (object clonk, any channel, bool force)
	{
		var replacements = this["AnimationReplacements"];
		if (replacements != nil)
		{
			for (var replace in replacements)
			{
				clonk->~ReplaceAction(replace[0], replace[1]);
			}
		}
	},

	OnStanceReset = func (object clonk, any channel, bool force)
	{
		var replacements = this["AnimationReplacements"];
		if (replacements != nil)
		{
			for (var replace in replacements)
			{
				clonk->~ReplaceAction(replace[0], nil);
			}
		}
	},
};


/**
 * Applies walk speed, as in the OpenClonk AimManager.
 */
static const StanceBehaviour_WalkSpeed = new Global
{
	/**
	 * Constructor function.
	 */
	Create = func (int walk_speed, int walk_back)
	{
		var behaviour = new StanceBehaviour_WalkSpeed { };
		// Define properties only if they were provided
		if (walk_speed != nil)
		{
			behaviour.WalkSpeed = walk_speed;
		}
		if (walk_back != nil)
		{
			behaviour.WalkBack = walk_back;
		}
		return behaviour;
	},

	OnStanceSet = func (object clonk, any channel, bool force)
	{
		var walk_speed = this["WalkSpeed"];
		var walk_back = this["WalkBack"];

		if (walk_speed != nil)
		{
			clonk->~PushActionSpeed("Walk", walk_speed);
		}

		if (walk_back != nil)
		{
			clonk->~SetBackwardsSpeed(walk_back);
		}
	},

	OnStanceReset = func (object clonk, any channel, bool force)
	{
		clonk->~PopActionSpeed("Walk");
		clonk->~SetBackwardsSpeed(nil);
	},
};
