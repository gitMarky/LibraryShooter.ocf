/**
	Clonk append script from Caedes OC.

	@author Zapper (original), Marky (cut and modified where necessary)
 */
/*
#appendto Clonk


func Recruitment()
{
	AddEffect("IntAimRestarter", this, 1, 50 + Random(10), this);
	return _inherited(...);
}


// this has to be refactored, it still uses some of caedes weapon properties
func FxIntAimRestarterTimer(object target, proplist effect, int time)
{
	// for a very quick check after cancelling the last aiming..
	if (effect.Interval == 1) effect.Interval = 10;

	// calling this assumes that the Clonk is not aiming currently
	var weapon = GetHandItem(0);
	if (!weapon) return;
	if (weapon->~RejectUse(this)) return;
	
	aim_set = weapon.animation_set;
	if (!aim_set) return;
	
	aim_weapon = weapon;
	
	if (aim_set["AnimationAim"] != nil)
	{
		if (aim_set["AimMode"] == AIM_Position)
			aim_animation_index = PlayAnimation
			(
				aim_set["AnimationAim"],
				CLONK_ANIM_SLOT_Arms,
				Anim_Const(GetAnimationLength(aim_set["AnimationAim"]) / 2),
				Anim_Const(1000)
			);
		if (aim_set["AimMode"] == AIM_Weight)
		{
			aim_animation_index = PlayAnimation
			(
				aim_set["AnimationAim"],
				CLONK_ANIM_SLOT_Arms,
				Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationAim"]), aim_set["AimTime"], ANIM_Loop),
				Anim_Const(1000)
			);
			aim_animation_index = PlayAnimation
			(
				aim_set["AnimationAim2"],
				10,
				Anim_Linear(0, 0, GetAnimationLength(aim_set["AnimationAim2"]), aim_set["AimTime"], ANIM_Loop),
				Anim_Const(1000),
				aim_animation_index
			);
			aim_animation_index++;
			SetAnimationWeight(aim_animation_index, Anim_Const(500));
		}
	}
	SetTurnType(1, 1);
	aim_stop = 0;
	var e = AddEffect("IntAim", this, 1, 2, this);
	e.artificial = true;
}

func StartAim(object weapon, int angle)
{
	// special case: artificial aiming
	var e = GetEffect("IntAim", this);
	if (e && e.artificial)
	{
		RemoveEffect(nil, this, e);
	}
	return inherited(weapon, angle);
}

func FxIntAimStart(object target, proplist effect, int temp)
{
	if (temp) return;
	var e = GetEffect("IntAimRestarter", this);
	if (e)
	{
		e.Interval = 0;
		SetPlayerControlEnabled(target->GetOwner(), CON_CaedesAimingCursor, true);
	}
}

func FxIntAimStop(object target, proplist effect, int reason, int temp)
{
	if (temp) return;
	var e = GetEffect("IntAimRestarter", target);
	if (e)
	{
		e.Interval = 1;
		SetPlayerControlEnabled(target->GetOwner(), CON_CaedesAimingCursor, false);
	}
	
	if (effect.artificial)
	{
		StopAnimation(GetRootAnimation(10));
		aim_weapon = nil;
		aim_set = nil;
		SetTurnForced(-1);
		SetTurnType(0, -1);
		SetHandAction(0);
	}
}

func FxIntAimTimer(object target, proplist effect, int time)
{
	if (effect.artificial)
	{
		var weapon = GetHandItem(0);
		if (!aim_weapon || weapon != aim_weapon) return FX_Execute_Kill;
		if (aim_weapon->Contained() != target) return FX_Execute_Kill;
		if (aim_weapon->RejectUse(target)) return FX_Execute_Kill;
	}
	
	if (!aim_set) return;
	
	var angle, delta_angle, length;
	var speed = aim_set["AimSpeed"];
	if (speed == nil)
		speed = 50;
	else
		speed *= 10;
	if (aim_angle < 0)
		SetTurnForced(DIR_Left);
	if (aim_angle > 0)
		SetTurnForced(DIR_Right);
	if (aim_set["AimMode"] == AIM_Position)
	{
		length = GetAnimationLength(aim_set["AnimationAim"]);
		angle = Abs(aim_angle) * 10;
		delta_angle = 0;
		SetAnimationPosition(aim_animation_index, Anim_Const((angle + delta_angle) * length / 1800));
	}
	if (aim_set["AimMode"] == AIM_Weight)
	{
		angle = Abs(aim_angle) * 10;
		delta_angle = 0;
		SetAnimationWeight(aim_animation_index, Anim_Const((angle + delta_angle) * 1000 / 1800));
	}
	// We have reached the angle and we want to stop
	if (Abs(delta_angle) <= 5 && aim_stop == 1)
	{
		DoStopAim();
		return -1;
	}
}
*/