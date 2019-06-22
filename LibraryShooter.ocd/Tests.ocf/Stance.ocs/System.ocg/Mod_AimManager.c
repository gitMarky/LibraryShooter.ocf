/**
	The OC Aim Manager should be compatible with the
	shooter library aim manager interface.
 */

#appendto Library_AimManager

local AimControl = CON_LibShooter_Aim;

func Definition(id def)
{
	def.HandleAimControl = Library_AimManager_Shooter.HandleAimControl;
	def.SetAimControlEnabled = Library_AimManager_Shooter.SetAimControlEnabled;
	def.IsAimControlEnabled = Library_AimManager_Shooter.IsAimControlEnabled;
	_inherited(def, ...);
}

func GetAimAnimationOffset(object weapon, int angle, int precision)
{
	return { X = 0, Y = 0, };
}

func GetAimAnimationAngle()
{
	return GetAimPosition() * 10;
}

func SetAimAnimationAngle(int value)
{
	return SetAimPosition(value / 10);
}

func ControlAimAt(int x, int y, object with_item)
{
	SetAimAnimationAngle(Angle(0, 0, x, y, 10));
	return true;
}
