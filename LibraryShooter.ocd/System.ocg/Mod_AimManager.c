/**
	The OC Aim Manager should be compatible with the
	shooter library aim manager interface.
 */

#appendto Library_AimManager

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
