#include Library_Firearm
#include Plugin_Firearm_ControlScheme_Legacy

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

public func GetCarryMode(object user) {    return CARRY_Hand; }
public func GetCarrySpecial(object user) { return "pos_hand2"; }
public func GetCarryBone() { return "main"; }
public func GetCarryTransform()
{
	return Trans_Mul(Trans_Rotate(90, 1, 0, 0), Trans_Rotate(4, 0, 0, 1));
}

public func Initialize()
{
	_inherited(...);

	ClearFiremodes();
	var firemode_default = DefaultFiremode();
	AddFiremode(firemode_default);
	SetFiremode(firemode_default->GetIndex());

	DefineWeaponOffset("Fuse", +2, -2);
	DefineWeaponOffset(WEAPON_POS_Muzzle, +12, -1);
	DefineWeaponOffset("AimPoint", +500, -1);
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
	mode->SetProjectileSpread(0);

	return mode;
}

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

public func Selection (object user)
{
	user->StartAim(this);
}

/* --- Effects --- */

public func FireSound(object user, proplist firemode)
{
	// Gun blast sound.
	Sound("Objects::Weapons::Blunderbuss::GunShoot?");
}

public func OnFireProjectile(object user, object projectile, proplist firemode)
{
	var test = CurrentTest();
	if (test && test.data)
	{
		test.data.projectiles_fired += 1;
	}
}

public func FireEffect(object user, int angle, proplist firemode)
{
	// Muzzle Flash & gun smoke.
	//var off_x = +Sin(angle, firemode->GetProjectileDistance() / 2);
	//var off_y = -Cos(angle, firemode->GetProjectileDistance() / 2) +  + firemode->GetYOffset();
	var off = GetWeaponPosition(user, WEAPON_POS_Muzzle, angle);

	var x = Sin(angle, 20);
	var y = -Cos(angle, 20);
	CreateParticle("Smoke", off.X, off.Y, PV_Random(x - 20, x + 20), PV_Random(y - 20, y + 20), PV_Random(40, 60), Particles_Smoke(), 20);
	user->CreateMuzzleFlash(off.X, off.Y, angle, 20);
	CreateParticle("Flash", 0, 0, 0, 0, 8, Particles_Flash());
}
