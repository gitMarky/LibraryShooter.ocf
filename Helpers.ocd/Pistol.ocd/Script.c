func IsSecondaryWeapon()
{
	return IsWeapon() && !IsPrimaryWeapon();
}

func QueryRejectDeparture(object clonk)
{
	return IsPrimaryWeapon();
}

func Exclusive(){return true;}
func IsWeapon(){return true;}

func IsFirstHandItem(){return true;}
func IsPrimaryWeapon(){return true;}

public func IsArmoryProduct() { return true; }

func Definition(def) {
	SetProperty("PictureTransformation", Trans_Mul(Trans_Scale(2000, 2000, 2000), Trans_Translate(0000, 3000, 000), Trans_Rotate(270, 0, 0, 1), Trans_Rotate(270, 1 ,0, 0)), def);
}

local Name = "$Name$";
local Description = "$Description$";
local UsageHelp = "$UsageHelp$";
local Collectible = 1;
local Rebuy = true;

local total_ammo;
local rounds;

func Hit()
{
	Sound("GeneralHit?");
}

local fAiming;	

public func GetDescID() { return "PistolDesc"; }
public func GetCarryMode(clonk) { if(fAiming >= 0) return CARRY_Hand; }
public func GetCarrySpecial(clonk) { if(fAiming > 0) return "pos_hand2"; }
public func GetCarryBone()	{	return	"main";	}
public func GetCarryTransform()
{
	return Trans_Rotate(-90, 0, 1, 0);
}

local animation_set;

local weapon_properties =
{
	max_ammo = 50,
	clip = 10,
	recharge = 10,
	
	spread = 1,
	spread_factor = 100,
	
	bullet_id = NormalBullet,
	damage = 15,
	range = 160,
	
	bullet_distance = 10,
	full_auto = false,
	delay_shot = 0,
	
	gfx_length = 6,
	gfx_off_y = -6,
	
	silencer = true
};

func Construction()
{
	this.weapon_properties = {Prototype = this.Prototype.weapon_properties};
	
	rounds = weapon_properties.clip;
	total_ammo = weapon_properties.max_ammo;
	
	animation_set = {
		AimMode        = AIM_Position, // The aiming animation is done by adjusting the animation position to fit the angle
		AnimationAim   = "MusketAimArms",
		AnimationLoad  = "MusketLoadArms",
		LoadTime       = 80,
		AnimationShoot = nil,
		ShootTime      = 20,
		WalkSpeed      = nil,
		WalkBack       = nil,
	};
}

public func GetAnimationSet() { return animation_set; }

local reload;

local yOffset;
local iBarrel;

local holding;

local MuskUp; local MuskFront; local MuskDown; local MuskOffset;

protected func HoldingEnabled() { return true; }

func Entrance(object obj)
{
	return _inherited(obj, ...);
}

func Departure(object from)
{
	if(!Contained())
	{
		if(GetEffect("IsLoading", this))
			RemoveEffect("IsLoading", this);
		if(GetEffect("DelayShot", this))
			RemoveEffect("DelayShot", this);
	}
	return _inherited(from, ...);
}

func IsLoading()
{
	return !!GetEffect("IsLoading", this);
}

func Ready(clonk, x, y)
{
	if(IsLoading())
		return false;
		
	if(clonk->GetDir() == DIR_Right)
	{
		if(x < 0)
			AddEffect("HighDeviation", this, 1, 20, this);
	}
	else 
		if(x > 0)
			AddEffect("HighDeviation", this, 1, 20, this);
	
	clonk->CancelAiming();
	return true;
}

func RejectUse(object clonk)
{
	return IsLoading() || !clonk->HasHandAction();
}

func ControlUseStart(object clonk, int x, int y)
{
	if(!Ready(clonk, x, y)) return true; // checks loading etc
			
	if(!ReadyToFire())
	{
		CheckReload();
		Sound("DryFire?");
		return true;
	}
	
	AimStartSound();
	
	fAiming = 1;
	holding = true;
	clonk->StartAim(this);

	ControlUseHolding(clonk, x, y);
	if(!weapon_properties.delay_shot && !weapon_properties.full_auto)
		Fire(clonk, clonk->GetAimPosition());
	return true;
}

func AimStartSound(){}

func CheckReload(bool always_try)
{
	if(IsLoading()) return true;
	
	if((rounds <= 0) || (((rounds < weapon_properties.clip) && always_try)))
	{
		if(total_ammo > 0)
		{
			Reload();
			return true;
		}
	}
}

func ReadyToFire()
{
	return (rounds > 0) && !IsLoading();
}

func Reload()
{
	Contained()->CancelAiming();
	//SetProperty("PictureTransformation",Trans_Mul(Trans_Translate(1500,0,-1500),Trans_Rotate(170,0,1,0),Trans_Rotate(30,0,0,1)));
	var old_load_time = animation_set.LoadTime;
	
	if(Contained().faster_reload)
		animation_set.LoadTime = (2 * animation_set.LoadTime) / 3;
		
	Contained()->StartLoad(this);
	AddEffect("IsLoading", this, 1, 5, this, nil, animation_set.LoadTime);
	
	animation_set.LoadTime = old_load_time;
	
	Sound("LongReload");
}

func FxIsLoadingStart(target, effect, temp, p1)
{
	if(temp) return;
	effect.max = p1;
	effect.bar = Contained()->CreateProgressBar(GUI_SimpleProgressBar, p1, 0, 20, Contained()->GetOwner(), {x = 0, y = -20}, VIS_Owner | VIS_Allies, {width = 20, height = 4});
}

func FxIsLoadingTimer(target, effect, time)
{
	if(!effect.bar) return -1;
	
	effect.bar->SetValue(time);
	if(time >= effect.max)
	{
		FinishReload();
		Sound("Click");
		return -1;
	}
	return 1;
}

func FinishReload()
{
	total_ammo += rounds;
	rounds = Min(weapon_properties.clip, total_ammo);
	total_ammo -= rounds;
	Contained()->~UpdateHUD();
	Sound("SniperAim");
}

func FireSound()
{
	Sound("GunShoot*");
}

func TryFire(clonk, angle)
{
	if(!ReadyToFire())
	{
		CheckReload();
		Sound("DryFire?");
		return false;
	}
	if(IsRecharging(true)) return;
	Fire(clonk, angle);
	//clonk->StartShoot(this);
	return true;
}

func Fire(clonk, angle)
{
	if(!clonk) return;
	if(!Contained()) return;
	
	var x = Sin(angle, weapon_properties.gfx_length);
	var y = -Cos(angle, weapon_properties.gfx_length) + weapon_properties.gfx_off_y;
	
	var range = weapon_properties.range;
	if(clonk.accuracy)
		range = (range * 3) / 2;
	
	if (clonk.silencer && weapon_properties.silencer)
	{
		Sound("Silenced?");
	}
	else
	{
		FireSound();
		FireEffect(clonk, angle);
	}
	FireShot(clonk, angle, x / 2, y / 2, range);
	
	AddDeviation();
	--rounds;
	
	var recharge_time;
	if(clonk.faster_reload)
	{
		recharge_time = Min(recharge_time, (2 * animation_set.LoadTime) / 3);
	} else recharge_time = weapon_properties.recharge;
	
	AddEffect("Recharge", clonk, 1, 2, nil, Pistol, recharge_time);
	clonk->~UpdateHUD();
	
	if(rounds == 0)
		clonk->CancelAiming(this);
	
	CheckReload();
}

func FxRechargeStart(target, effect, temp, par1)
{
	if(temp) return;
	effect.max = par1;
}

func FxRechargeTimer(target, effect, time)
{
	if(effect.draw)
	{
		if(!effect.bar)
			effect.bar = target->CreateProgressBar(GUI_SimpleProgressBar, effect.max, time, 0, target->GetOwner(), {x = 0, y = -17}, VIS_Owner, {width = 15, height = 3, back_color = RGBa(100, 0, 0)});
		else effect.bar->SetValue(time);
	}
	if(time >= effect.max)
	{
		return -1;
	}
}

func FxRechargeStop(target, effect, reason, temp)
{
	if(temp) return;
	if(effect.bar)
			effect.bar->Close();
}

func GetDeviation()
{
	var factor = 1;
	var c = Contained();
	if(!c) return;
	if(c.accuracy)
		factor = 3;
	var s = Abs(Contained()->GetXDir()) + Abs(Contained()->GetYDir());
	s = (s * weapon_properties.deviation_factor) / 100;
	s += weapon_properties.spread;
	if(GetEffect("HighDeviation", this))
		s += 2000;
	return s / factor;
	/*var e = GetEffect("Deviate", this);
	if(!e) return s / factor;
	
	s += e.spread;
	Message("%d", s/factor);
	return s / factor;*/
}

func FireShot(clonk, angle, out_x, out_y, range)
{
	var c = CreateObject(weapon_properties.bullet_id, out_x, out_y, GetController());
	ManipulateBullet(c);
	var dmg = weapon_properties.damage;
	if(clonk.crits)
		{
			dmg = (dmg * 3) / 2;
			c.crit = true;
		}
	
	c->~Fire(clonk, angle, GetDeviation(), weapon_properties.bullet_distance /* distance */, dmg, GetID(), range);
}

func ManipulateBullet(object bullet){ }

func FireEffect(clonk, angle)
{
	clonk->CreateMuzzleFlash(0, 0, angle, 10);
	//CreateCartridgeEffect(6, Sin(-angle, 10), -RandomX(6, 12));
}

func AddDeviation()
{
	/*var e = GetEffect("Deviate", this);
	if(!e)
	{
		e = AddEffect("Deviate", this, 1, 10, this);
		e.spread = weapon_properties.spread_factor;
	}
	else
	EffectCall(this, e, "SetSpread", (e.spread + weapon_properties.spread_factor));*/
	//Log("%d derp %d <--- %d", e.spread, (e.spread * (100 + weapon_properties.spread_factor)) / 100, weapon_properties.spread_factor);
}

func FxDeviateTimer(target, effect, time)
{
	effect.spread = Max(effect.spread - weapon_properties.spread_decrease, 0);
	if(effect.spread == 0) return -1;
	return 1;
}

func FxDeviateSetSpread(target, effect, p1)
{
	effect.spread = p1;
}

func GetAngle(x, y)
{
	var angle = Angle(0,0,x,y - weapon_properties.gfx_off_y);
		angle = Normalize(angle,-180);
	return angle;
}

// Callback from the clonk when loading is finished
public func FinishedLoading(object clonk)
{
	//SetProperty("PictureTransformation",Trans_Mul(Trans_Translate(500,1000,-000),Trans_Rotate(130,0,1,0),Trans_Rotate(20,0,0,1)));
	if(holding) clonk->StartAim(this);
	return holding; // false means stop here and reset the clonk
}

func ControlUseHolding(object clonk, ix, iy)
{
	var angle = GetAngle(ix, iy);
	clonk->SetAimPosition(angle);
	
	if(weapon_properties.delay_shot)
		ResetAim(angle);
	if (weapon_properties.full_auto)
	{
		if(!TryFire(clonk, angle))
		{
			ControlUseStop(clonk, ix, iy);
			return false;
		}
	}
	return true;
}

protected func ControlUseStop(object clonk, ix, iy)
{
	holding = false;
	clonk->CancelAiming();
	return -1;
}

func IsRecharging(bool draw)
{
	var e = GetEffect("Recharge", Contained());
	if(e && draw)
		if(e.max > 20) e.draw = true;
	return !!e;
}

// Callback from the clonk, when he actually has stopped aiming
public func FinishedAiming(object clonk, int angle)
{
	if(weapon_properties.delay_shot)
	{
		Unaim();
		clonk->CancelAiming(this);
		return true;
	}
	return true;
}

protected func ControlUseCancel(object clonk, int x, int y)
{
	clonk->CancelAiming(this);
	return true;
}

public func Reset(clonk)
{
	fAiming = 0;
}

func OnNewRound()
{
	this.rounds = weapon_properties.clip;
	this.total_ammo = weapon_properties.max_ammo;
	Contained()->CancelAiming();
}

func ResetAim(angle)
{
	var e = GetEffect("DelayShot", this);
	
	if(e)
	{
		if(Abs(e.angle - angle) <= 2)
			return;
	}
	else
	{
		e = AddEffect("DelayShot", this, 1, 4, this);
		e.max_time = weapon_properties.delay_shot;
	}
	
	e.time = weapon_properties.delay_shot;
	e.angle = angle;
}

func FxDelayShotTimer(target, effect, time)
{
	if(!fAiming) return -1;
	var clonk;
	if(!(clonk = Contained())) return -1;
	
	effect.time -= 4;
	
	if(effect.bar)
		effect.bar->SetValue(effect.max_time - effect.time);
	
	if(effect.time > 0) return 1;
	
	if(rounds <= 0) return -1;
	if(IsRecharging(true)) return -1;
	
	Fire(clonk, effect.angle);
	//Contained()->StartShoot(this);
	return -1;
}

func FxDelayShotStop(target, effect, reason, temp)
{
	if(temp) return;
	if(effect.bar) effect.bar->Close();
}

func Unaim()
{
	if(GetEffect("DelayShot", this))
		RemoveEffect("DelayShot", this);
}