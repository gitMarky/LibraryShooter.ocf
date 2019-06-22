/**
 * Applies walk speed, as in the OpenClonk AimManager.
 */
static const StanceBehaviour_WalkSpeed = new Global
{
	/**
	 * Constructor function.
	 */
	Create = func (int walk_speed, int walk_back, any identifier)
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
		behaviour.Identifier = identifier ?? Clonk;
		return behaviour;
	},

	OnStanceSet = func (object clonk, any channel, bool force)
	{
		var walk_speed = this["WalkSpeed"];
		var walk_back = this["WalkBack"];

		if (walk_speed != nil)
		{
			clonk->~PushActionSpeed("Walk", walk_speed, this.Identifier);
		}

		if (walk_back != nil)
		{
			clonk->~SetBackwardsSpeed(walk_back);
		}
	},

	OnStanceReset = func (object clonk, any channel, bool force)
	{
		clonk->~PopActionSpeed("Walk", this.Identifier);
		clonk->~SetBackwardsSpeed(nil);
	},
};
