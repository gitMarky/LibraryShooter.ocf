#include Library_Firearm

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

public func GetCarryMode(object user) {    return CARRY_Hand; }
public func GetCarrySpecial(object user) { return "pos_hand2"; }
public func GetCarryBone() { return "main"; }
public func GetCarryTransform()
{
	return Trans_Rotate(90, 1, 0, 0);
}

public func Initialize()
{
	_inherited(...);
	ClearFiremodes();
	var firemode_default = DefaultFiremode();
	AddFiremode(firemode_default);
	SetFiremode(firemode_default->GetIndex());
}

/* --- Fire modes --- */

local Collectible = 1;

func DefaultFiremode()
{
	var mode = new Library_Firearm_Firemode {};

	mode->SetName("Standard");
	mode->SetAmmoID(Dummy);
	mode->SetRecoveryDelay(50);
	mode->SetReloadDelay(100);
	mode->SetMode(WEAPON_FM_Single);
	mode->SetProjectileID(Bullet);
	mode->SetProjectileSpeed(50);
	mode->SetProjectileRange(1000);
	mode->SetProjectileDistance(10);
	mode->SetYOffset(-2);
	mode->SetProjectileSpread(0);

	return mode;
}

local weapon_properties = 
{
		gfx_distance = 6,
		gfx_offset_y = 0,
};

func Hit()
{
	Sound("Hits::GeneralHit?");
}

public func GetAmmoSource(id ammo)
{
	return AMMO_Source_Container;
}

public func GetAmmoContainer()
{
	return Contained();
}

public func Setting_AimOnUseStart()
{
	return this.AimOnUseStart;
}

local AimOnUseStart = false;

/* --- Effects --- */


public func FireSound(object user, proplist firemode)
{
	// Gun blast sound.
	Sound("Objects::Weapons::Blunderbuss::GunShoot?");
}

public func OnFireProjectile(object user, object projectile, proplist firemode)
{
}

public func FireEffect(object user, int angle, proplist firemode)
{
	// Muzzle Flash & gun smoke.
	var off_x = +Sin(angle, firemode->GetProjectileDistance() / 2);
	var off_y = -Cos(angle, firemode->GetProjectileDistance() / 2) +  + firemode->GetYOffset();
	var x = Sin(angle, 20);
	var y = -Cos(angle, 20);
	CreateParticle("Smoke", off_x, off_y, PV_Random(x - 20, x + 20), PV_Random(y - 20, y + 20), PV_Random(40, 60), Particles_Smoke(), 20);
	user->CreateMuzzleFlash(off_x, off_y, angle, 20);
	CreateParticle("Flash", 0, 0, 0, 0, 8, Particles_Flash());
}
