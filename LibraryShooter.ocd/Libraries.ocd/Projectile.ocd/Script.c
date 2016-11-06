/**
 Library for projectiles.
 
 @author Marky
 @credits Hazard Team, Code Modern Combat as inspiration
 @version 0.1.0
 */

static const PROJECTILE_Default_Velocity_Precision = 10;
static const PROJECTILE_Range_Infinite = 0;

static const PROJECTILE_Deviation_Value = 0;
static const PROJECTILE_Deviation_Precision = 1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions


local damage;					// int - damage on hit
local damage_type;				// int - damage type
local range;					// int - max. travel distance, in pixels
local velocity;					// int - velocity
local velocity_x, velocity_y;	// int - velocity, as components
local weapon_ID;				// id
local user;						// object
local instant;					// bool - if true the projectile hits instantly

local is_launched;				// bool - true if the projectile has been launched

local remove_on_hit;			// bool - is the object removed when it hits an object or the landscape?

local lastX, lastY, nextX, nextY;// int - positions for hit checks
local trail;					 // object - for effects
local trail_width, trail_length; // int - trail dimensions, in pixels 

local lifetime;					// int - calculated from range and velocity
local rotation_by_rdir;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

/**
 Override of the default function for getting x dir, so that hitscan projectiles
 can provide a correct value.
 
 @par precision The precision value, default is 10.
 @version 0.2.0
 */
public func GetXDir(int precision)
{
	if (IsHitscan())
	{
		var velo = (precision ?? PROJECTILE_Default_Velocity_Precision) * velocity_x / PROJECTILE_Default_Velocity_Precision;
		Log("GetXDir for hitscan = %d", velo);
		return velo;
	}
	else
	{
		return _inherited(precision, ...);
	}
}

/**
 Override of the default function for getting y dir, so that hitscan projectiles
 can provide a correct value.

 @par precision The precision value, default is 10.
 @version 0.2.0
 */
public func GetYDir(int precision)
{
	if (IsHitscan())
	{
		var velo = (precision ?? PROJECTILE_Default_Velocity_Precision) * velocity_y / PROJECTILE_Default_Velocity_Precision;
		Log("GetYDir for hitscan = %d", velo);
		return velo;
	}
	else
	{
		return _inherited(precision, ...);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

/**
 Identifies the object as a projectile.
 @version 0.1.0
 */
public func IsProjectile(){ return true;}


/**
 Identifies the object as a hitscan projectile.
 @version 0.2.0
 */
public func IsHitscan()
{
	return instant;
}


/**
 Configures the object that controls the weapon. 
 @par shooter An object. 
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func Shooter(object shooter)
{
	ProhibitedWhileLaunched();
	
	if (shooter == nil)
	{
		FatalError(Format("Parameter 'shooter' expects an object, got nil"));
	}

	user = shooter;
	return this;
}


/**
 Gets the shooter that this projectile was configured with.
 @return object Returns the shooter, usually the object that fired the projectile.
 @version 0.2.0
 */
public func GetShooter()
{
	return user;
}


/**
 Configures the weapon ID that shot the projectile.
 @par value The weapon that launches the projectile. This can be either an id, or an object. 
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func Weapon(value)
{
	ProhibitedWhileLaunched();
	
	if (GetType(value) == C4V_Def)
	{
		weapon_ID = value;
	}
	else if (GetType(value) == C4V_C4Object)
	{
		weapon_ID = value->GetID();
	}
	else
	{
		FatalError(Format("Expected either an object or an ID, got %v: %v", GetType(value), value));
	}

	return this;
}


/**
 Gets a weapon ID that can be used for kill messages, for example.
 @return The weapon ID that this object was configured with.
 @version 0.2.0
 */
public func GetWeaponID()
{
	return weapon_ID;
}


/**
 Configures how far the projectile will travel. 
 @par value The approximate distance in pixels. The projectile calculates its lifetime in frames with {@c distance / velocity}. 
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func Range(int value)
{
	ProhibitedWhileLaunched();
	
	if (value < 0)
	{
		FatalError(Format("Cannot set negative range - the function received %d", value));
	}
	
	if (GetLifetime() > 0)
	{
		FatalError(Format("Cannot set range, because a lifetime of %d was specified already", GetLifetime()));
	}
	
	range = value;
	return this;
}


/**
 Gets the range of the projectile.
 @return int the range of the projectile, in pixels.
 @version 0.2.0
 */
public func GetRange()
{
	return range;
}


/**
 Configures how long the projectile will travel. 
 @par value The lifetime in frames.
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.2.0
 */
public func Lifetime(int value)
{
	ProhibitedWhileLaunched();
	
	if (value <= 0)
	{
		FatalError(Format("Must sef positive lifetime - the function received %d", value));
	}
	
	if (GetRange() > 0)
	{
		FatalError(Format("Cannot set lifetime, because a range of %d was specified already", GetRange()));
	}

	lifetime = value;
	return this;
}


/**
 Gets the lifetime of the projectile.
 @return int the lifetime of the projectile, in frames.
 @version 0.2.0
 */
public func GetLifetime()
{
	return lifetime;
}


/**
 Configures how much damage the projectile will deal when it hits. 
 @par value The amount of damage that the projectile deals to the target. 
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func DamageAmount(int value)
{
	ProhibitedWhileLaunched();
	
	// may receive negative damage! healing projectiles :D
	
	damage = value;
	return this;
}


/**
 Gets the amount of damage that this projectile was configured with.
 @return int The amount of damage that this projectile should inflict.
 @version 0.2.0
 */
public func GetDamageAmount()
{
	return damage;
}


/**
 Configures the damage type. 
 @par value A damage type code that exists in your project. This is a custom value. For existing damage types see {@link TODO}. 
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func DamageType(int value)
{
	ProhibitedWhileLaunched();
	
	damage_type = value;
	return this;
}


/**
 Gets the damage type that this projectile was configured with.
 @return int The damage type for this projectile.
 @version 0.2.0
 */
public func GetDamageType()
{
	return damage_type;
}


/**
 Sets the velocity of the projectile.
 @par value The new velocity.
 @return object The projectile object, so that it can be modified further.
 */
public func Velocity(int value)
{
	ProhibitedWhileLaunched();
		
	if (value < 0)
	{
		FatalError(Format("Cannot set negative velocity - the function received %d", value));
	}

	velocity = value;
	return this;
}


/**
 Configures the projectile to do a hit check immediately when launched. This means that the projectile will not fly,
 it rather hits instantly. 
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func HitScan()
{
	ProhibitedWhileLaunched();
	
	instant = true;
	return this;
}


/**
 Configures the projectile trail dimensions.
 @par width The trail width, in pixels.
 @par length The trail length, in pixels.
 @return object Returns the projectile object, so that further function calls can be issued.
 @version 0.1.0
 */
public func Trail(int width, int length, string gfx, int speed)
{
	ProhibitedWhileLaunched();
	
	if (width < 0 || length < 0)
	{
		FatalError(Format("The trail dimensions must be positive. Got: %d/%d", width, length));
	}
	
	trail_width = width;
	trail_length = length;
	return this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// temporary stuff






protected func Initialize()
{
	velocity = 100;
	damage = 0;
	range = 500;
	instant = false;
}

protected func Hit()
{
	var self = this;
	
	if(!instant)
	{
		SetXDir(0);
		SetYDir(0);
	
		if(nextX)
		{
			var x = GetX(), y = GetY();
			var a = Angle(lastX, lastY, nextX, nextY);
			var max = Max(Abs(GetXDir()/10), Abs(GetYDir()/10));
			for(var cnt = 0; cnt < max; cnt += 2)
			{
				nextX = Sin(a, cnt);
				nextY = -Cos(a, cnt);
				if(GBackSolid(lastX + nextX - x, lastY + nextY - y))
				{
					SetPosition(lastX + nextX, lastY + nextY);
					break;
				}
			}
		}

		
		if (trail)
		{
			trail->ProjectileUpdate();
			trail->Travelling();
		}

		DoHitCheckCall();
	}
	
	if(self)
	{	
		self->OnHitLandscape();
		
		if (self && self->ShouldRemoveOnHit()) RemoveObject();
	}
}

func Remove()
{
	var self = this;
	DoHitCheckCall();
	if (self) this->OnHitNothing();
	if (self) RemoveObject();
}

func RemoveOnHit()
{
	this.remove_on_hit = true;
}

func StayOnHit()
{
	this.remove_on_hit = false;
}

func ShouldRemoveOnHit()
{
	return this.remove_on_hit;
}

public func Launch(int angle, proplist deviation)
{
	lifetime = lifetime ?? (PROJECTILE_Default_Velocity_Precision * GetRange() / Max(velocity, 1));

	this.is_launched = true;
	RemoveOnHit();

	SetController(user->GetController());
	
	var precision = 100;
	
	// get correct precision
	if (deviation == nil)
	{
		// everything ok
	}
	else if (deviation.precision > precision)
	{
		precision = deviation.precision;
	}
	else
	{
		deviation = ScaleDeviation(deviation, precision);
	}
	
	// get angle and velocity
	angle = GetLaunchAngle(angle, precision, deviation);
	velocity_x = +Sin(angle, velocity, precision);
	velocity_y = -Cos(angle, velocity, precision);

	var self = this;

	this->OnLaunch();

	if (instant)
	{
		LaunchHitscan(angle, precision);
	}
	else
	{
		LaunchAsProjectile(angle, precision);
	}
	
	if (self)
	{
		this->OnLaunched();
	}
}


private func LaunchAsProjectile(int angle, int precision)
{
	//Log("Launching projectile at angle %d (%d with precision %d), v_x = %d, v_y = %d", angle / precision, angle, precision, velocity_x, velocity_y);

	SetXDir(velocity_x); SetYDir(velocity_y);

	StartHitCheckCall(user, true, true);
}


private func LaunchHitscan(int angle, int precision)
{
	//-- initialize position

	var self = this;
	var x_start = GetX();
	var y_start = GetY();

	StayOnHit();
	StartHitCheckCall(user, true, false);

	if (!self) return;

	//-- set position to final point

	var d_x = + Sin(angle, range, precision);
	var d_y = - Cos(angle, range, precision);
	var x_end = GetX() + d_x;
	var y_end = GetY() + d_y;

	// cap to landscape bounds
	var current_length = Distance(0, 0, d_x, d_y);
	for (var desired_length = current_length;
	    (desired_length > 0)
	 && (x_end < 0 || x_end > LandscapeWidth() || y_end < 0 || y_end > LandscapeHeight());
	     desired_length--)
		{
			x_end = GetX() + desired_length * d_x / current_length;
			y_end = GetY() + desired_length * d_y / current_length;
		}

	// cap to landscape itself
	var coords = PathFree2(x_start, y_start, x_end, y_end);

	if(!coords) // path is free
	{
		SetPosition(x_end, y_end);
	}
	else
	{
		SetPosition(coords[0], coords[1]);
	}

	//-- actual hit detection
	
	// we are at the end position now, check targets
	DoHitCheckCall();

	//-- cleanup

	if (self) this->OnHitScan(x_start, y_start, GetX(), GetY());

	if (self)
	{
		if (self->ShouldRemoveOnHit())
		{
			RemoveObject();
		}
		else if (coords)
		{
			Hit();
		}
	}

	if(self) RemoveObject();
}


/**
 Callback that happens before the projectile is launched.
 @version 0.1.0
 */
public func OnLaunch()
{
}


/**
 Callback that happens after the projectile was launched.
 The projectile may be gone before the call happens, in
 that case the call will not be executed at all.
 @note This call will not be executed in hitscan projectiles,
       see {@link Library_Projectile#HitScan}.
 @version 0.1.0
 */
public func OnLaunched()
{
}


/**
 Callback that happens after a hitscan projectile determines the
 projectile end position. This is typically used to create a
 line of hit effects.

 @par x_start The starting X coordinate of the projectile, in global
              coordinates. 
 @par y_start The starting Y coordinate of the projectile, in global
              coordinates. 
 @par x_end   The final X coordinate of the projectile, in global
              coordinates. 
 @par y_end   The final Y coordinate of the projectile, in global
              coordinates. 
 @version 0.1.0
 */
public func OnHitScan(int x_start, int y_start, int x_end, int y_end)
{
}

/*
public func Launch(int iAngle, int iSpeed, int iDist, int iSize, int iTrail, int iDmg, int iDmgType, int iGlowSize, int iAPrec, int iReflections)
{
	if(MOD_FastBullets())
		//return LaunchInstant(iAngle, iSpeed, iDist, iSize, iTrail, iDmg, iDmgType, iGlowSize, iAPrec, iReflections);
		iSpeed*=2;

	// Standardwerte setzen
	if(!iSize)		iSize = 8;
	//if(!iGlowSize)
	//	iGlowSize = iSize;
	if(!iTrail)	 iTrail = 300;
	if(!iDmg)		 iDamage = 3;
	else iDamage = iDmg;
	if(!iDmgType) DMG_Type = DMG_Projectile;
	else DMG_Type = iDmgType;

	bGlow = false;
	// Tolles Leuchten erzeugen
	if(iGlowSize)
	{
		bGlow = true;
		SetGraphics(0,this(),LIGH,1,GFXOV_MODE_Base, 0, 1);
		SetObjDrawTransform(100+35*iGlowSize,0,0, 0,100+35*iGlowSize,0, this(),1);
		SetClrModulation(GlowColor(1),this(),1);
	}

	// Werte für Reflektionen speichern
	iRefl = iReflections;

		iTrailSize = iSize;
		iTrailLength = iTrail;
		iXDir = GetXDir(0, 100);
		iYDir = GetYDir(0, 100);


}
*/


func CreateTrail(int x, int y)
{
	// neat trail
	trail = CreateObject(Bullet_Trail, x, y);
	trail->SetObjectBlitMode(GFX_BLIT_Additive);
	trail->Set(this, trail_width, trail_length);
}

func FxPositionCheckTimer(target, effect, time)
{
	lastX = GetX();
	lastY = GetY();
	nextX = lastX + GetXDir()/10;
	nextY = lastY + GetYDir()/10;
}

protected func Travelling()
{
	ControlSpeed();
	
	DrawColorModulation();
	
	if (trail)
	{
		trail->ProjectileUpdate();
	}
	
	this->OnTravelling();
	
	var self = this;
	DoHitCheckCall();
	if (self && GetLifetime() > 0 && GetActTime() >= GetLifetime()) Remove();
}

protected func ControlSpeed()
{
	if (GetAction() == "Travel")
	{
		SetXDir(velocity_x);
		SetYDir(velocity_y);
	}
	
	if (!rotation_by_rdir) SetR(Angle(0, 0, GetXDir(), GetYDir()));
}

private func DrawColorModulation()
{
	var color = ProjectileColor(GetActTime());
	
	if (color != nil)
	{
		SetClrModulation(color);
	}
}


/**
 Callback while the projectile travels.
 Overload this, if you want to have custom
 effects for the projectile while it travels.
 */
public func OnTravelling()
{
}


/**
 Specifies a color modulation for the projectile, based on the time it exists.
 @par time The time that the projectile exists, in frames.
 @return {@c nil}, which means that the color is not adjusted.
         You can override this function if you want a custom color modulation
         for projectile.
@version 0.1.0
 */
public func ProjectileColor(int time)
{
	return nil;
}

public func TrailColor(int acttime)
{
	return RGBa(255, Max(0, 255 - Min(150, acttime * 20)), 75, 255);
}

public func HitObject(object obj, bool remove, proplist effect)
{
	if (effect)
	{
		effect.registered_hit = FrameCounter();
	}

	var self = this;

	obj->~OnProjectileHit(this);
	this->DoDamageObject(obj);
	this->OnHitObject(obj);

	if (!self) return;

	// remove object if removal is requested
	if(remove && self->ShouldRemoveOnHit())
	{
		RemoveObject();
	}
	// TODO: workaround for hitscan projectiles: the removal happens later
	else if (instant && !self->ShouldRemoveOnHit())
	{
		RemoveOnHit();
	}
}


/**
 Callback when a target should be damaged.
 @par target The object that should be damaged.
 @version 0.2.0 
 */
public func DoDamageObject(object target)
{
	WeaponDamageShooter(target, GetDamageAmount(), GetDamageType(), nil, false, GetWeaponID());
}


/**
 Callback if the projectile was removed because
 it did not hit anything before its lifetime
 expired.
 @version 0.2.0
 */
public func OnHitNothing()
{
}


/**
 Callback if the projectile hits another object.
 @par target This is the object that was hit.
 @par effect
 @version 0.1.0
 */
public func OnHitObject(object target, proplist effect)
{
}


/**
 Callback if the projectile collides with the landscape.
 @version 0.1.0
 */
public func OnHitLandscape()
{
}


private func SquishVertices(bool squish)
{
	if(squish)
	{
		SetVertex(1, VTX_X, 0, 2);
		SetVertex(1, VTX_Y, 0, 2);
		SetVertex(2, VTX_X, 0, 2);
		SetVertex(2, VTX_Y, 0, 2);
		return true;
	}
	else
	{
		SetVertex(1, VTX_X,-3, 2);
		SetVertex(1, VTX_Y, 1, 2);
		SetVertex(2, VTX_X, 3, 2);
		SetVertex(2, VTX_Y, 1, 2);
		return false;
	}
}

local ActMap = {

	Travel = {
		Prototype = Action,
		Name = "Travel",
		Procedure = DFA_NONE,
		NextAction = "Travel",
		Length = 1,
		Delay = 1,
		FacetBase = 1,
		StartCall="Travelling",
	},
	
	TravelBallistic = {
		Prototype = Action,
		Name = "TravelBallistic",
		Procedure = DFA_NONE,
		Length = 1,
		Delay = 1,
		FacetBase = 1,
		NextAction = "TravelBallistic",
		StartCall = "Travelling",
	},
};

local Name = "$Name$";
local Description = "$Description$";



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*

local iTime, lx, ly, pTrail, iDamage, iPrec, iRefl, iTrailLength, iTrailSize, iXDir, iYDir, DMG_Type;
local shooter; // Dingens/Clonk das den Schuss abgefeuert hat.
local bGlow, bNoRemove;
local shooter_weapon; // ID des Schützen oder seiner Waffe

public func GetDamageType(){ return DMG_Type; }
public func DoCorrectPosition(){ return false; }

// Ist ein Schuss! Nicht warpen!
func NoWarp() { return true; }
// Ist ein Schuss!
func IsBullet() { return true; }
//IsDodgeBullet(){ return(1);}


func Construction(object byObj)
{
	// nichts? :C
	if(!byObj)
		return;
	// Waffe?
	shooter = GetShooter(byObj);
	shooter_weapon = GetShooter(byObj,true);
	// Team?
	if(shooter->GetTeam())
		SetTeam(shooter->GetTeam());
}

public func GetWeaponID()
{
	return shooter_weapon;
}


public func LaunchSpeed(int iXD, int iYD)
{
	var iSpeed = 1;
	if(MOD_FastBullets()) iSpeed=2;

	SetXDir(iXD);
	SetYDir(iYD);
	SetR(Angle(0,0,iXD,iYD));

		iXDir = GetXDir(0, 100);
		iYDir = GetYDir(0, 100);
}


public func LaunchInstant(int iAngle, int iSpeed, int iDist, int iSize, int iTrail, int iDmg, int iDmgType, int iGlowSize, int iAPrec, int iReflections)
{
	// Standardwerte setzen
	if(!iSize)		iSize = 8;
	//if(!iGlowSize)
	//	iGlowSize = iSize;
	if(!iTrail) iTrail = 300;
	if(!iDmg)	iDamage = 3;
	else iDamage = iDmg;
	if(!iDmgType) DMG_Type = DMG_Projectile;
	else DMG_Type = iDmgType;

	HitCheck(GetX(),GetY(),GetX()+Sin(iAngle,iDist, iAPrec),GetY()-Cos(iAngle,iDist,iAPrec),shooter);

	RemoveObject();
}


// Extern für Überladung
private func CreateTrail(int iSize, int iTrail) {
	pTrail = CreateObject(TrailID(),0,0,-1);
	if(pTrail) {
		pTrail->Set(iSize-2,iTrail,this());
		SetObjectBlitMode(GetObjectBlitMode(),pTrail);
	}
}

// Timer 

private func Traveling()
{
	if(pTrail) HandleTrail();

	var iATime = GetActTime();

	// ausfaden
	SetClrModulation(Color(iATime));
	if(bGlow) {
		SetClrModulation(GlowColor(iATime),0,1);
	}
	// löschen
	if(iATime >= iTime) return(Remove());

	// außerhalb der Landschaft: löschen
	if(GetY()<0) return(Remove());
}

// Treffer

private func Hit()
{
	// Reflektion an Wänden (Eintrittswinkel = Austrittwinkel)
	if(iRefl)
	{
		if(!iXDir && !iYDir)
		{
			iXDir = GetXDir();
			iYDir = GetYDir();
		}
		if(iXDir != 0)
		{
			// Positive XDir -> Rechts auf Material prüfen
			if(iXDir > 0)
			{
				if(GBackSolid(1,0))
					iXDir *= -1;
			// Und andersrum das gleiche Spiel
			}
			else
				if(GBackSolid(-1,0))
					iXDir *= -1;
		}
		if(iYDir != 0)
		{
			// Positive YDir -> Unten auf Material prüfen
			if(iYDir > 0)
			{
				if(GBackSolid(0,1))
					iYDir *= -1;
			}
			// Und andersrum das gleiche Spiel
			else
				if(GBackSolid(0,-1))
					iYDir *= -1;
		}
		// Geschwindigkeit gehört neu gesetzt
		SetXDir(iXDir, 0, 100);
		SetYDir(iYDir, 0, 100);
		SetR(Angle(0, 0, iXDir, iYDir));
		// Trail wird entfernt!
		if(pTrail)
		{
			pTrail->SetPosition(GetX(), GetY());
			pTrail->SetSpeed(0,0);
			pTrail->Remove();
		}
		// Und neu erzeugt
		CreateTrail(iTrailSize, iTrailLength);

		// Eine Reflektion weniger
		iRefl--;
		// Und erstmal nicht auftreffen
		return;
	}

	// umliegende Objekte beschädigen.
	var objs = FindObjects(	Find_AtPoint(),
							Find_NoContainer(),
							Find_Or(
								Find_Func("IsBulletTarget",GetID(),this(),shooter),
								Find_OCF(OCF_Alive)
							),
							Find_Func("CheckEnemy",this())
							);
 
	for(var pTarget in objs) {
		BulletStrike(pTarget);
	}

	HitObject();
}

private func Color(int iATime) {
	var iPrg = 100*iATime/iTime;
	return(RGBa(255,255-iPrg*2,255-iPrg*2,iPrg*2));
}

public func TrailColor(int iATime) {
	var iPrg = 100*iATime/iTime;
	return(RGBa(255,255-iPrg*2,255-iPrg*2,iPrg*2));
}

public func TrailID(){ return TRAI; }

public func GlowColor(int iATime) {
	return(RGBa(255,190,0,50));
}

// Dummy-func
func HitExclude()
{

}

public func Remove() {
	if(pTrail) {
		pTrail->SetPosition(GetX(),GetY());
		pTrail->Remove();
	}

	RemoveObject();
}

// Effekt für Trefferüberprüfung

// EffectVars:
// 0 - alte X-Position
// 1 - alte Y-Position
// 2 - Schütze (Objekt, das die Waffe abgefeuert hat, üblicherweise ein Clonk)
// 3 - ID des Schützen
// 4 - Scharf? Wenn true wird der Schütze vom Projektil getroffen 
// 5 - niemals den Schützen treffen

public func FxHitCheckStart(object target, int effect, int temp, object byObj, bool neverShooter)
{
	if(temp) return;
	EffectVar(0, target, effect) = GetX(target);
	EffectVar(1, target, effect) = GetY(target);
	if(!byObj)
		byObj = target;
	if(byObj->Contained())
		byObj = (byObj->Contained());
	EffectVar(2, target, effect) = byObj;
	EffectVar(3, target, effect) = GetID(byObj);
	EffectVar(4, target, effect) = false;
	EffectVar(5, target, effect) = neverShooter;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// neue Treffer-Routine

private func HitCheck(int x1, int y1, int x2, int y2, object pExclude )
{

	// Endposition bestimmen
	var dx, dy, dmax, dist;

	dx = x2-x1;
	dy = y2-y1;
	dmax = Distance(dx,dy);
	dist = 0;

	SetPosition(x1,y1);

	// Landschaftskollision prüfen
	while(++dist)
	{
		if(GBackSolid(dx*dist/dmax,dy*dist/dmax))
			break;
		if(dist>=dmax)
			break;
	}

	// neue Endposition
	dx = dx*dist/dmax;
	dy = dy*dist/dmax;

	//for(var i = 0; i < dist; i++)
	//{
		var pObj, pTargets;

		//SetPosition(x1,y1);

		pTargets = FindObjects(	 Find_OnLine(0,0,dx,dy),//Find_AtPoint( dx*i/dist,dy*i/dist),
								 Find_Exclude(this),
								 Find_Exclude(pExclude),
								 Find_NoContainer(),
								 Find_Or(Find_Func("IsBulletTarget",GetID(),this,shooter), Find_OCF(OCF_Alive)),
								 Find_Func("CheckEnemy",this),
								 Find_Not(Find_Func("HitExclude")),
								 Sort_Distance(x1,y1));

		for(pObj in pTargets)
		{
			var i = Distance(x1,y1,GetX(pObj),GetY(pObj));
			lx = dx*i/dist;
			ly = dy*i/dist;
			SetPosition( x1 + dx*i/dist, y1 + dy*i/dist);

			if(HitObject(pObj))
			{
				return pObj;
			}
		}
	//}

	lx = x1+dx;
	ly = x2+dy;


	if(dist < dmax)//Nicht in der Luft. :O
	{
		SetPosition(x1+dx,x2+dy);
		HitObject(); //HitLandscape(mx,my);
		return 1;
	}
	else
	{
		SetPosition(x2,y2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Geändertes

private func HitObject(object pObject)
{
	var ret = BulletStrike(pObject);

	if(!ret) DoHitEffect();

	if(!bNoRemove) Remove();

	return ret;
}

public func DoHitEffect()
{
	//Sparks(70*GetCon()/100,Color(GetActTime()));
	Sound("ProjectileHit*");
	CastParticles("ColSpark",10,20,0,0,20,40,RGBa(255,100,0,80),RGBa(255,200,0,130));
}

public func BulletStrike(object pObj)
{
	if(pObj)
	{
		DoDmg(iDamage,DMG_Type,pObj,iPrec);
		return true;
	}
}

public func HandleTrail()
{
	 //SetPosition(GetX(), GetY(), pTrail);
	 if(pTrail) pTrail->~Update(this);

	 if(GetAction() == "TravelBallistic")
		  SetR(Angle(0,0,GetXDir(),GetYDir()));
}

public func SetBallistic()
{
	SetAction("TravelBallistic");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// properties

local Name = "$Name$";
local Description = "$Description$";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// actions

local ActMap = {
Travel = {
	Prototype = Action,
	Name = "Travel",
	Procedure = DFA_FLOAT,
	Length = 1,
	Delay = 1,
	FacetBase = 1,
	NextAction = "Travel",
	StartCall = "Traveling",
},

TravelBallistic = {
	Prototype = Action,
	Name = "TravelBallistic",
	Procedure = DFA_NONE,
	Length = 1,
	Delay = 1,
	FacetBase = 1,
	NextAction = "TravelBallistic",
	StartCall = "Traveling",
},

};

*/

private func ProhibitedWhileLaunched()
{
	if (is_launched)
	{
		FatalError("This function may only be called before the projectile is launched.");
	}
}


private func GetLaunchAngle(int angle, int precision, deviation)
{
	var launch_angle = angle * precision;
	// handle correct deviation
	if (GetType(deviation) == C4V_PropList)
	{
		if (GetType(deviation.angle) == C4V_Int)
		{
			deviation.angle = [deviation.angle];
		}

		for (var i = 0; i < GetLength(deviation.angle); ++i)
		{
			var rnd = deviation.angle[i];
			launch_angle += RandomX(-rnd, +rnd);
		}
	}
	else if (GetType(deviation) != C4V_Nil)
	{
		FatalError(Format("Unexpected parameter %v for deviation. Expected array, proplist, or nil.", deviation));
	}
	return launch_angle;
}
