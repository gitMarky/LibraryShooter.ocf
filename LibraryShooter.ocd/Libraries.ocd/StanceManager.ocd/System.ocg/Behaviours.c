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

