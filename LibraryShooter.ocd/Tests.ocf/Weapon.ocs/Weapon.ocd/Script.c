#include Library_Firearm

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

	position_hand = new PositionOffsetAnimation {};
	position_hand->DefineOffsetForward(0, -3, nil, -3)
	             ->DefineOffsetUp(-1, -7, nil, -3)
	             ->DefineOffsetDown(-3, -1);

	position_weapon = {};
	position_weapon.Fuse = new PositionOffsetRotation {};
	position_weapon.Fuse->DefineOffset(+2, -2);
	position_weapon.Muzzle = new PositionOffsetRotation {};
	position_weapon.Muzzle->DefineOffset(+12, -1);
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
	var off = GetPositionWeapon("Muzzle", angle);
	var off_x = off.X;
	var off_y = off.Y;

	var x = Sin(angle, 20);
	var y = -Cos(angle, 20);
	CreateParticle("Smoke", off_x, off_y, PV_Random(x - 20, x + 20), PV_Random(y - 20, y + 20), PV_Random(40, 60), Particles_Smoke(), 20);
	user->CreateMuzzleFlash(off_x, off_y, angle, 20);
	CreateParticle("Flash", 0, 0, 0, 0, 8, Particles_Flash());
}

/* --- Aim position --- */

local position_hand;

func GetPositionHand(int angle, int precision)
{
	return position_hand->GetPosition(angle, precision);
}

local position_weapon;

func GetPositionWeapon(string name, int angle, int precision)
{
	var hand = GetPositionHand(angle, precision);
	var weapon = position_weapon[name]->GetPosition(angle, precision);
	return { X = hand.X + weapon.X, Y = hand.Y + weapon.Y, DebugColor = weapon.DebugColor };
}

/* --- Debugging --- */

local FxDebugPositions = new Effect
{
	Timer = func ()
	{
		var user = this.Target->Contained();
		if (!user || !user->~IsAiming()) return;
		
		var angle = user->GetAimPosition();
		
		var debug_positions = 
		[
			this.Target->GetPositionHand(angle),
			this.Target->GetPositionWeapon("Fuse", angle),
			this.Target->GetPositionWeapon("Muzzle", angle)
		];
		
		for (var position in debug_positions)
		{
			var color = SplitRGBaValue(position.DebugColor);
			user->CreateParticle("SphereSpark", position.X, position.Y, 0, 0, this.Interval,
			{
				Size = 1, R = color.R, G = color.G, B = color.B,
				Attach = ATTACH_Front | ATTACH_MoveRelative,
			});
		}
	},
};
