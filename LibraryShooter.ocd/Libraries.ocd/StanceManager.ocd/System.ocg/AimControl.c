/**
 * Applies aim controls.
 */
static const StanceBehaviour_AimControl = new Global
{
	/**
	 * Constructor function.
	 * Not that it is necessary, it just became a convention already.
	 *
	 * @par aim_animation see DefineAnimation
	 * @par anim_slot The animations are played in this animation slot. Defaults to CLONK_ANIM_SLOT_Arms.
	 * @return proplist the created behaviour.
	 */
	Create = func (any aim_animation, int anim_slot)
	{
		return new StanceBehaviour_AimAnimation {};
	},

	OnStanceSet = func (object clonk, any channel, bool force)
	{
		clonk->~SetAimControlEnabled(true);
	},

	OnStanceReset = func (object clonk, any channel, bool force)
	{
		clonk->~SetAimControlEnabled(false);
	},
};
