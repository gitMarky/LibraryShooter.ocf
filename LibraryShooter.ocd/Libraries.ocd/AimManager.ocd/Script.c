/**
	Custom aim manager for shooters.

	@author Marky
 */

/* --- Properties --- */

local lib_aim_manager_shooter;

local AimControl = CON_Aim;

/* --- Engine Callbacks --- */

func Construction(object by)
{
	lib_aim_manager_shooter = lib_aim_manager_shooter ?? {};

	// Currently defines the offsets for the musket aiming animation only
	// Has to be changed to depend on the animation.
	if (!lib_aim_manager_shooter.position_hand)
	{
		lib_aim_manager_shooter.position_hand = new PositionOffsetAnimation {};
		lib_aim_manager_shooter.position_hand->DefineOffsetForward(0, -3, nil, -3)
		                                     ->DefineOffsetUp(-1, -7, nil, -3)
		                                     ->DefineOffsetDown(-3, -1);
	}
	if (lib_aim_manager_shooter.aim_angle == nil)
	{
		lib_aim_manager_shooter.aim_angle = 0;
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
		return { X = 0, Y = 0, };
	}
}

func GetAimAnimationAngle()
{
	if (IsAimControlEnabled())
	{
		return lib_aim_manager_shooter.aim_angle;
	}
	else
	{
		return nil;
	}
}

func SetAimAnimationAngle(int value)
{
	lib_aim_manager_shooter.aim_angle = Normalize(value, -1800, 10);
}

func SetAimControlEnabled(bool enabled)
{
	SetPlayerControlEnabled(GetOwner(), this.AimControl, enabled);
}

func IsAimControlEnabled()
{
	return GetPlayerControlEnabled(GetOwner(), this.AimControl);
}

/**
	This function can be called in ObjectControl(),
	passing all the necessary parameters.
	
	Implement wherever you need it in your code:
	
	if (HandleAimControl( - parameters - ))
	{
		return true;
	}
 */
func HandleAimControl(int player, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	// Handle aiming?
	if (ctrl == this.AimControl)
	{
		// Save last mouse position:
		// If the using has to be canceled, no information about the current x,y
		// is available. Thus, the last x,y position needs to be saved
		this.control.mlastx = x;
		this.control.mlasty = y;

		var item = this->~GetHandItem(0);
		if ((item != nil && item->~ControlAimAt(x, y))
		||  this->~ControlAimAt(x, y))
		{
			return true;
		}
	}
	return false;
}
