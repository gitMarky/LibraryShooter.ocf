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

/**
 * Applies an aim animation, as in the OpenClonk AimManager.
 */
static const StanceBehaviour_AimAnimation = new Global
{
	/**
	 * Constructor function.
	 */
	Create = func (any aim_animation, int anim_slot)
	{
		if (GetType(aim_animation) == C4V_String)
		{
			aim_animation = [aim_animation];
		}
		else if (GetType(aim_animation) == C4V_Array)
		{
			// Everything OK, leave as is
		}
		else
		{
			FatalError("Only string or proplist allowed, got %v: %v", GetType(aim_animation), aim_animation);
		}
		anim_slot = anim_slot ?? CLONK_ANIM_SLOT_Arms;
		var behaviour = new StanceBehaviour_AimAnimation
		{
			AnimSlot = anim_slot,    // Play animations in this slot
			AnimAim = aim_animation, // Define the default aim animation
			AnimAimStatus = nil,     // Status for the loop animation: integer = start playing in that frame; nil = needs to play; bool = is playing
		};
		return behaviour;
	},

	Timer = func (object clonk, any channel)
	{
		if (GetType(this.AnimAimStatus) == C4V_Int)
		{
			// Do not keep the aim angle up to date
			if (FrameCounter() < this.AnimAimStatus)
			{
				return;
			}
			// Animation can be resumed
			this.AnimAimStatus = nil;
		}
		if (this.AnimAimStatus == nil)
		{
			// TODO: Check status and whether to stop aiming
			this.AnimAimStatus = !!PlayAnimLoop(clonk, "AnimAim");
		}
		else // Animation is playing
		{
			// TODO: Update aim angle
		}
	},

	OnStanceSet = func (object clonk, any channel, bool force)
	{
	},

	OnStanceReset = func (object clonk, any channel, bool force)
	{
		// Stop the  animation
		clonk->StopAnimation(clonk->GetRootAnimation(this.AnimSlot));
	},

	PlayAnimLoop = func (object clonk, string name)
	{
		var animation_index;
		var animations = this[name];
		var slot = this.AnimSlot;

		// Stop the previous animation, if there is any
		clonk->StopAnimation(clonk->GetRootAnimation(slot));

		if (animations == nil || GetLength(animations) == 0)
		{
			// TODO
		}
		else
		{
			var anim0 = animations[0];
			var anim1 = animations[1];

			// Do we just have one animation? Then just play it
			if (anim1 == nil)
			{
				animation_index = clonk->PlayAnimation(anim0, slot, Anim_Const(clonk->GetAnimationLength(anim0) / 2), Anim_Const(1000));
			}
			else
			{
				var time = 10; // Provide a basic time. Was aim_set["AimTime"] originally
				animation_index = clonk->PlayAnimation(anim0, slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim0), time, ANIM_Loop), Anim_Const(1000));
				animation_index = clonk->PlayAnimation(anim1, slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim1), time, ANIM_Loop), Anim_Const(1000), animation_index);
				animation_index += 1;
				clonk->SetAnimationWeight(animation_index, Anim_Const(500));
			}
		}
		return animation_index;
	},

	PlayAnimSingle = func (object clonk, string name, int aim_angle, int duration)
	{
		var animation_index;
		var animations = this[name];
		var slot = this.AnimSlot;

		// Stop the previous animation, if there is any
		clonk->StopAnimation(clonk->GetRootAnimation(slot));

		if (animations == nil || GetLength(animations) == 0)
		{
			// TODO
		}
		else
		{
			// Note when to resume the loop animation
			this.AnimAimStatus = FrameCounter() + duration;

			var anim0 = animations[0];
			var anim1 = animations[1];
			var anim2 = animations[2];
			// Do we just have one animation? Then just play it
			if (anim1 == nil)
			{
				animation_index = clonk->PlayAnimation(anim0, slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim0), duration, ANIM_Remove), Anim_Const(1000));
			}
			// Well two animations blend between them (animation 1 is 0° animation2 for 180°)
			else if (anim2 == nil)
			{
				animation_index = clonk->PlayAnimation(anim0,  slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim0), duration, ANIM_Remove), Anim_Const(1000));
				animation_index = clonk->PlayAnimation(anim1, slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim1), duration, ANIM_Remove), Anim_Const(1000), animation_index);
				animation_index += 1;
				SetAnimationWeight(animation_index, Anim_Const(1000 * Abs(aim_angle) / 180));
			}
			// Well then we'll have three to blend (animation 1 is 90°, animation 2 is 0°, animation 2 for 180°)
			else
			{
				if (Abs(aim_angle) < 90)
				{
					animation_index = clonk->PlayAnimation(anim1, slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim1), duration, ANIM_Remove), Anim_Const(1000));
					animation_index = clonk->PlayAnimation(anim0,  slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim0), duration, ANIM_Remove), Anim_Const(1000), animation_index);
					animation_index += 1;
					clonk->SetAnimationWeight(animation_index, Anim_Const(1000 * Abs(aim_angle) / 90));
				}
				else
				{
					animation_index = clonk->PlayAnimation(anim0,  slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim0), duration, ANIM_Remove), Anim_Const(1000));
					animation_index = clonk->PlayAnimation(anim2, slot, Anim_Linear(0, 0, clonk->GetAnimationLength(anim2), duration, ANIM_Remove), Anim_Const(1000), animation_index);
					animation_index += 1;
					clonk->SetAnimationWeight(animation_index, Anim_Const(1000 *(Abs(aim_angle) - 90) / 90));
				}
			}
		}
		return animation_index;
	},
};
