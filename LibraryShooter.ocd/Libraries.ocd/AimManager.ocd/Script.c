/**
	Custom aim manager for shooters.

	@author Marky
 */

/* --- Properties --- */

local lib_aim_manager_shooter;

/* --- Engine Callbacks --- */

func Construction(object by)
{
	lib_aim_manager_shooter = lib_aim_manager_shooter ?? {};
	
	// Currently defines the offsets for the musket aiming animation only
	// Has to be changed to 
	if (!lib_aim_manager_shooter.position_hand)
	{
		lib_aim_manager_shooter.position_hand = new PositionOffsetAnimation {};
		lib_aim_manager_shooter.position_hand->DefineOffsetForward(0, -3, nil, -3)
		                                     ->DefineOffsetUp(-1, -7, nil, -3)
		                                     ->DefineOffsetDown(-3, -1);
	}
	return _inherited(by, ...);
}

/* --- Interface --- */

func GetAimAnimationOffset(object weapon, int angle, int precision)
{
	if (lib_aim_manager_shooter.position_hand)
	{
		var offset = lib_aim_manager_shooter.position_hand->GetPosition(angle, precision);
		offset.X += weapon->GetX() - GetX();
		offset.Y += weapon->GetY() - GetY();
		return offset;
	}
	else
	{
		return {X = 0, Y = 0,};
	}
}
