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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

/**
 Identifies the object as a projectile.
 @version 0.1.0
 */
public func IsProjectile(){ return true;}



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
 Configures how far the projectile will travel. 
 @par value The approximate distance in pixels. The projectile calculates its lifetime in frames with {@c distance / velocity}. 
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

	range = value;
	return this;
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
		OnHitLandscape();
		
		if (remove_on_hit && self) RemoveObject();
	}
}

func Remove()
{
	var self = this;
	DoHitCheckCall();
	if(self) RemoveObject();
}

public func Launch(int angle, array deviation)
{
	
	lifetime = PROJECTILE_Default_Velocity_Precision * range / velocity;

	this.is_launched = true;
	this.remove_on_hit = true;

	SetController(user->GetController());
	
	var precision = 100;
	var precisions = [];
	var precision_max = 0;
	
	// get common precision
	
	if (GetType(deviation) == C4V_Array)
	{
		if (GetType(deviation[PROJECTILE_Deviation_Value]) == C4V_Array)
		{
			for (var dev in deviation)
			{
				var pre = dev[PROJECTILE_Deviation_Precision];
				PushBack(precisions, pre);

				if (pre > precision_max) precision_max = pre;
			}
			
			var min_exponent = GetExponent(precision_max);

			precision = 1;
			for (var pre in precisions)
			{
				precision *= pre;

				var exponent = GetExponent(precision);
				if (exponent > min_exponent)
				{
					precision /= 10 ** (exponent - min_exponent);
				}
			}

			angle *= precision;

			for (var i = 0; i < GetLength(deviation); i++)
			{
				var rnd = deviation[i][PROJECTILE_Deviation_Value] * precision / deviation[i][PROJECTILE_Deviation_Precision];
				angle += RandomX(-rnd, +rnd);
			}
		}
		else
		{
			precision = deviation[PROJECTILE_Deviation_Precision];
			angle *= precision;
		    angle += RandomX(-deviation[PROJECTILE_Deviation_Value], deviation[PROJECTILE_Deviation_Value]);
		}
	}
	else if (GetType(deviation) == C4V_Nil)
	{
		angle *= precision;
	}
	else
	{
		FatalError(Format("Unexpected parameter %v for deviation. Expected array", deviation));
	}

	var self = this;

	OnLaunch();

	if (!instant)
	{
		velocity_x = +Sin(angle, velocity, precision);
		velocity_y = -Cos(angle, velocity, precision);

		SetXDir(velocity_x); SetYDir(velocity_y);

		StartHitCheckCall(user, true, true);
	}
	else
	{
		this.remove_on_hit = false;
		StartHitCheckCall(user, true, false);

		if (self)
		{
			range *= precision;
	
			// set position to final point
			var x_p = GetX();
			var y_p = GetY();
	
			var d_x = + Sin(angle, range, precision);
			var d_y = - Cos(angle, range, precision);
			var t_x = GetX() + d_x;
			var t_y = GetY() + d_y;
	
			// cap to landscape bounds
			var current_length = Distance(0, 0, d_x, d_y);
			for (var desired_length = current_length;
			    (desired_length > 0)
			 && (t_x < 0 || t_x > LandscapeWidth() || t_y < 0 || t_y > LandscapeHeight());
			     desired_length--)
				{
					t_x = GetX() + desired_length * d_x / current_length;
					t_y = GetY() + desired_length * d_y / current_length;
				}
	
			var coords = PathFree2(x_p, y_p, t_x, t_y);
	
			if(!coords) // path is free
			{
				SetPosition(t_x, t_y);
			}
			else
			{
				SetPosition(coords[0], coords[1]);
			}
	
			// we are at the end position now, check targets
			DoHitCheckCall();
		}

		if (self) OnHitScan(x_p, y_p, GetX(), GetY());

		if (self != nil && self.remove_on_hit)
		{
			RemoveObject();
		}
		else if (coords && self)
		{
			Hit();
		}
//		var hit_object = false;
//		for (var obj in FindObjects(Find_OnLine(x_p - GetX(), y_p - GetY(), 0, 0),
//									Find_NoContainer(),
//									//Find_Layer(GetObjectLayer()),
//									//Find_PathFree(target),
//									Find_Exclude(user),
//									Sort_Distance(x_p - GetX(), y_p - GetY()) ))
//		{
//			if (obj->~IsProjectileTarget(this, user) || obj->GetOCF() & OCF_Alive)
//			{
//				var objdist = Distance(x_p, y_p, obj->GetX(), obj->GetY());
//				SetPosition(x_p + Sin(angle, objdist, precision), y_p - Cos(angle, objdist, precision));
//				var self = this;
//				HitObject(obj, true);
//				hit_object = true;
//				break;
//			}
//		}
//		
//		// at end position now
//		for(var obj in FindObjects(Find_OnLine(x_p - GetX(), y_p - GetY()), Find_Func("IsProjectileInteractionTarget")))
//		{
//			obj->~OnProjectileInteraction(x_p, y_p, angle, user, damage);
//		}
//		
//		
//		/*if(!user.silencer)
//		{
//			var t = CreateObject(Bullet_TrailEffect, 0, 0, NO_OWNER);
//			t->Point({x = x_p, y = y_p}, {x = GetX(), y = GetY()});
//			t->FadeOut();
//			t->SetObjectBlitMode(GFX_BLIT_Additive);
//		}*/
//		
//		if(!hit_object)
//		{
//			var hit = GBackSolid(Sin(angle, 2, 100), -Cos(angle, 2, 100));
//			
//			if(hit)
//			{
//				Hit();
//			}
//		}

		if(self) RemoveObject();
	}
	
	if (self)
	{
		OnLaunched();
	}
}


public func OnLaunch()
{
}


public func OnLaunched()
{
}

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

	// Werte f�r Reflektionen speichern
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
	
	var self = this;
	DoHitCheckCall();
	if (self && lifetime > 0 && GetActTime() >= lifetime) Remove();
}

protected func ControlSpeed()
{
	if (GetAction() == "Travel")
	{
		SetXDir(velocity_x);
		SetYDir(velocity_y);
	}
	
	SetR(Angle(0, 0, GetXDir(), GetYDir()));
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
	return RGBa(255, 255 - Min(150, acttime*20), 75, 255);
}

public func HitObject(object obj, bool remove, proplist effect)
{
	DoDmg(damage, nil, obj, nil, nil, this, weapon_ID);
	
	OnHitObject(obj);
	
	if(remove && remove_on_hit)
	{
		RemoveObject();
	}

	if (instant && !remove_on_hit)
	{
		remove_on_hit = true;
	}
	
	if (effect)
	{
		effect.registered_hit = FrameCounter();
	}
}

public func OnHitObject(object target, proplist effect)
{
	CreateImpactEffect(this.damage);
}

public func OnHitLandscape()
{
		Sound("BulletHitGround?");
		CreateImpactEffect(this.damage);
}

// called by successful hit of object after from ProjectileHit(...)
public func OnStrike(object obj)
{
	if(obj->GetAlive())
		Sound("ProjectileHitLiving?");
	else
		Sound("BulletHitGround?");
}



private func SquishVertices(bool squish)
{
	if(squish==true)
	{
		SetVertex(1,VTX_X,0,2);
		SetVertex(1,VTX_Y,0,2);
		SetVertex(2,VTX_X,0,2);
		SetVertex(2,VTX_Y,0,2);
	return 1;
	}

	if(squish!=true)
	{
		SetVertex(1,VTX_X,-3,2);
		SetVertex(1,VTX_Y,1,2);
		SetVertex(2,VTX_X,3,2);
		SetVertex(2,VTX_Y,1,2);
	return 0;
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
local shooter_weapon; // ID des Sch�tzen oder seiner Waffe

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


// Extern f�r �berladung
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
	// l�schen
	if(iATime >= iTime) return(Remove());

	// au�erhalb der Landschaft: l�schen
	if(GetY()<0) return(Remove());
}

// Treffer

private func Hit()
{
	// Reflektion an W�nden (Eintrittswinkel = Austrittwinkel)
	if(iRefl)
	{
		if(!iXDir && !iYDir)
		{
			iXDir = GetXDir();
			iYDir = GetYDir();
		}
		if(iXDir != 0)
		{
			// Positive XDir -> Rechts auf Material pr�fen
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
			// Positive YDir -> Unten auf Material pr�fen
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
		// Geschwindigkeit geh�rt neu gesetzt
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

	// umliegende Objekte besch�digen.
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

// Effekt f�r Treffer�berpr�fung

// EffectVars:
// 0 - alte X-Position
// 1 - alte Y-Position
// 2 - Sch�tze (Objekt, das die Waffe abgefeuert hat, �blicherweise ein Clonk)
// 3 - ID des Sch�tzen
// 4 - Scharf? Wenn true wird der Sch�tze vom Projektil getroffen 
// 5 - niemals den Sch�tzen treffen

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

	// Landschaftskollision pr�fen
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
// Ge�ndertes

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

private func GetExponent(int value, int base)
{
	var exponent = 0;
	
	if (base == nil) base = 10;
	
	for (var test = value; test%base == 0; test = test/base)
	{
	}
	
	return exponent;
}