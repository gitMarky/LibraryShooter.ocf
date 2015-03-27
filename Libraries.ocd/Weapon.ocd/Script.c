#include Library_AmmoManager

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;


local is_selected = true; // bool: is the weapon currently selected?
local holding = true; // TODO: experimental

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

public func GetCarryMode(object user) {    if (is_selected) return CARRY_Hand; }
public func GetCarrySpecial(object user) { if (is_selected) return "pos_hand2"; }
public func GetCarryBone() { return "main"; }
public func GetCarryTransform()
{
	return Trans_Rotate(-90, 0, 1, 0);
}

local animation_set = {
		AimMode        = AIM_Position, // The aiming animation is done by adjusting the animation position to fit the angle
		AnimationAim   = "MusketAimArms",
		AnimationLoad  = "MusketLoadArms",
		LoadTime       = 80,
		AnimationShoot = nil,
		ShootTime      = 20,
		WalkSpeed      = nil,
		WalkBack       = nil,
	};


public func GetAnimationSet() { return animation_set; }


/**
 This is executed each time the user presses the fire button.@br@br

 The function does the following:@br
 - tell the user to start aiming@br
 - call {@link Library_Weapon#ControlUseHolding}@br
 - call {@link Library_Weapon#Fire}@br
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected func ControlUseStart(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

//	if(!Ready(user, x, y)) return true; // checks loading etc

//	if(!ReadyToFire())
//	{
//		CheckReload();
//		Sound("DryFire?");
//		return true;
//	}

//	AimStartSound();

//	fAiming = 1;
//	holding = true;
	user->StartAim(this);

	ControlUseHolding(user, x, y);
	//if(!weapon_properties.delay_shot && !weapon_properties.full_auto)
		Fire(user, user->GetAimPosition());
	return true;
}

/**
 This is executed while the user is holding the fire button.@br@br

 The function does the following:@br
 - update the aiming angle according to the parameters
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected func ControlUseHolding(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

	var angle = GetAngle(x, y);
	user->SetAimPosition(angle);
	
//	if(weapon_properties.delay_shot)
//		ResetAim(angle);
//	if (weapon_properties.full_auto)
//	{
//		if(!TryFire(user, angle))
//		{
//			ControlUseStop(user, x, y);
//			return false;
//		}
//	}
	return true;
}

/**
 This is executed when the user stops holding the fire button.@br@br

 The function does the following:@br
 - tell the user to stop aiming.
 @par user The object that is using the weapon.
 @par x The x coordinate the user is aiming at.
 @par y The y coordinate the user is aimint at.
 @version 0.1.0
 */
protected protected func ControlUseStop(object user, int x, int y)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}

//	holding = false;
	user->CancelAiming();
	return -1;
}

/**
 Converts coordinates to an aiming angle for the weapon.
 @par x The x coordinate, local.
 @par y The y coordinate, local.
 @return int The angle in degrees, normalized to the range of [-180°, 180°].
 @version 0.1.0
 */
private func GetAngle(int x, int y)
{
	var angle = Angle(0, 0, x, y); // - weapon_properties.gfx_off_y);
		angle = Normalize(angle, -180);
		
	return angle;
}

/**
 Fires the weapon.@br
 
 The function does the following:@br
 - write a message saying 'pew pew'
 @par user The object that is using the weapon.
 @par angle The angle the weapon is aimed at.
 @version 0.1.0
 */
private func Fire(object user, int angle)
{
	if(user == nil)
	{
		FatalError("The function expects a user that is not nil");
	}
	
	user->Message("Pew pew %d", angle);
}
