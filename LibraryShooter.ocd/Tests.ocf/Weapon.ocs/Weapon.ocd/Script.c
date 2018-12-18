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
	position_weapon.AimPoint = new PositionOffsetRotation {};
	position_weapon.AimPoint->DefineOffset(+500, -1);
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

/**
	The actual firing function.@br@br

	The function will create new bullet objects, as many as the firemode defines. Since no actual ammo objects are taken or consumed, this should be handled in {@link Library_Firearm#HandleAmmoUsage}.@br
	Each time a single projectile is fired, {@link Library_Firearm#OnFireProjectile} is called.@br
	{@link Library_Firearm#GetProjectileAmount} and {@link Library_Firearm#GetSpread} can be used for custom behaviour.@br

	@par user The object that is using the weapon.
	@par angle The firing angle.
	@par firemode A proplist containing the fire mode information.
*/
func FireProjectiles(object user, int angle, proplist firemode)
{
	AssertNotNil(user);
	AssertNotNil(firemode);

	var user_x = user->~GetWeaponX(this); if (user_x) user_x -= GetX();
	var user_y = user->~GetWeaponY(this); if (user_y) user_y -= GetY();

	//var x = +Sin(angle, firemode->GetProjectileDistance()) + user_x;
	//var y = -Cos(angle, firemode->GetProjectileDistance()) + user_y + firemode->GetYOffset();
	var launch_position = GetPositionWeapon("Muzzle", angle);
	var x = launch_position.X;
	var y = launch_position.Y;

	// launch the single projectiles
	for (var i = 0; i < Max(1, firemode->GetProjectileAmount()); i++)
	{
		var projectile = CreateObject(firemode->GetProjectileID(), x, y, user->GetController());

		projectile->Shooter(user)
		          ->Weapon(this)
		          ->DamageAmount(firemode->GetDamage())
		          ->DamageType(firemode->GetDamageType())
		          ->Velocity(Library_Random->SampleValue(firemode->GetProjectileSpeed()))
		          ->Range(Library_Random->SampleValue(firemode->GetProjectileRange()));

		this->OnFireProjectile(user, projectile, firemode);
		projectile->Launch(angle, ComposeSpread(user, firemode));
	}

	shot_counter[firemode->GetIndex()]++;

	HandleAmmoUsage(firemode);
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
