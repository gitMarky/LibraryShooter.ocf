#include Library_AmmoManager

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;


local is_selected; // bool: is the weapon currently selected?

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// non-functional and temporary stuff

public func GetCarryMode(clonk) {    if (is_selected) return CARRY_Hand; }
public func GetCarrySpecial(clonk) { if (is_selected) return "pos_hand2"; }
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



func ControlUseStart(object clonk, int x, int y)
{
//	if(!Ready(clonk, x, y)) return true; // checks loading etc

//	if(!ReadyToFire())
//	{
//		CheckReload();
//		Sound("DryFire?");
//		return true;
//	}

//	AimStartSound();

//	fAiming = 1;
//	holding = true;
	clonk->StartAim(this);

	ControlUseHolding(clonk, x, y);
	//if(!weapon_properties.delay_shot && !weapon_properties.full_auto)
		Fire(clonk, clonk->GetAimPosition());
	return true;
}

func ControlUseHolding(object clonk, ix, iy)
{
	var angle = GetAngle(ix, iy);
	clonk->SetAimPosition(angle);
	
//	if(weapon_properties.delay_shot)
//		ResetAim(angle);
//	if (weapon_properties.full_auto)
//	{
//		if(!TryFire(clonk, angle))
//		{
//			ControlUseStop(clonk, ix, iy);
//			return false;
//		}
//	}
	return true;
}

protected func ControlUseStop(object clonk, ix, iy)
{
//	holding = false;
	clonk->CancelAiming();
	return -1;
}


func GetAngle(x, y)
{
	var angle = Angle(0,0,x,y); // - weapon_properties.gfx_off_y);
		angle = Normalize(angle,-180);
	return angle;
}


func Fire(clonk, angle)
{
	if(!clonk) return;
	if(!Contained()) return;
	
	clonk->Message("Pew pew %d", angle);
}
