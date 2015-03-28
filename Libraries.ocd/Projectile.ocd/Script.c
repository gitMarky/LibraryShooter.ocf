/*-- Bullet --*/

local damage;
local from_ID;
local user;
local deviation;
local bulletRange;
local speed;

local lastX, lastY, nextX, nextY;
local trail;

local instant;

protected func Initialize()
{
	speed = 4000;
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
			var max = Max(Abs(GetXDir()/10),AbsY(GetYDir()/10));
			for(var cnt = 0; cnt < max; cnt += 2)
			{
				nextX = Sin(a, cnt);
				nextY = -Cos(a, cnt);
				if(GBackSolid(lastX + nextX - x, lastY + nextY - y))
				{
					SetPosition(lastX + nextX, lastY + nextY);
					if(trail)
						trail->Traveling();
					break;
				}
			}
		}
			
		DoHitCheckCall();
	}
	
	if(self)
	{
		Sound("BulletHitGround?");
		CreateImpactEffect(Max(5, damage*2/3));
	  	
	  	RemoveObject();
	}
}

func Remove()
{
	var self = this;
	DoHitCheckCall();
	if(self) RemoveObject();
}

public func Launch(object shooter, id weapon, int angle, int dev, int speed, int range, int dmg, int damage_type)
{
	from_ID = weapon;
	user = shooter;
	damage = dmg;
	deviation = dev;
	bulletRange = range;
	
	instant = true;
		
	SetController(shooter->GetController());

	angle *= 100;
	angle += RandomX(-deviation, deviation);
	
	// set position to final point
	var x_p = GetX();
	var y_p = GetY();
	
	var t_x = GetX() + Sin(angle, bulletRange, 100);
	var t_y = GetY() - Cos(angle, bulletRange, 100);
	
	var coords = PathFree2(x_p, y_p, t_x, t_y);
	
	if(!coords) // path is free
		SetPosition(t_x, t_y);
	else SetPosition(coords[0], coords[1]);
		
	// we are at the end position now, check targets
	var hit_object = false;
	for (var obj in FindObjects(
								Find_OnLine(x_p - GetX(), y_p - GetY(), 0, 0),
								Find_NoContainer(),
								//Find_Layer(GetObjectLayer()),
								//Find_PathFree(target),
								Find_Exclude(shooter),
								Sort_Distance(x_p - GetX(), y_p - GetY())
							))
	{
		if (obj->~IsProjectileTarget(this, shooter) || obj->GetOCF() & OCF_Alive)
		{
			var objdist = Distance(x_p, y_p, obj->GetX(), obj->GetY());
			SetPosition(x_p + Sin(angle, objdist, 100), y_p - Cos(angle, objdist, 100));
			var self = this;
			HitObject(obj, true);
			hit_object = true;
			break;
		}
	}
	
	// at end position now
	for(var obj in FindObjects(Find_OnLine(x_p - GetX(), y_p - GetY()), Find_Func("IsProjectileInteractionTarget")))
	{
		obj->~OnProjectileInteraction(x_p, y_p, angle, shooter, damage);
	}
	
	if(!shooter.silencer)
	{
		var t = CreateObject(Bullet_TrailEffect, 0, 0, NO_OWNER);
		t->Point({x = x_p, y = y_p}, {x = GetX(), y = GetY()});
		t->FadeOut();
		t->SetObjectBlitMode(GFX_BLIT_Additive);
	}
	
	var self = this;
	if(!hit_object)
	{
		var hit = GBackSolid(Sin(angle, 2, 100), -Cos(angle, 2, 100));
		
		if(hit)
			Hit();
	}
	if(self) RemoveObject();
}

public func Fire(object shooter, int angle, int dev, int dist, int dmg, id weapon, range)
{
	from_ID = weapon;
	user = shooter;
	damage = dmg;
	deviation = dev;
	bulletRange = range;
	
	instant = true;
		
	SetController(shooter->GetController());

	angle *= 100;
	angle += RandomX(-deviation, deviation);
	
	// set position to final point
	var x_p = GetX();
	var y_p = GetY();
	
	var t_x = GetX() + Sin(angle, bulletRange, 100);
	var t_y = GetY() - Cos(angle, bulletRange, 100);
	
	var coords = PathFree2(x_p, y_p, t_x, t_y);
	
	if(!coords) // path is free
		SetPosition(t_x, t_y);
	else SetPosition(coords[0], coords[1]);
		
	// we are at the end position now, check targets
	var hit_object = false;
	for (var obj in FindObjects(
								Find_OnLine(x_p - GetX(), y_p - GetY(), 0, 0),
								Find_NoContainer(),
								//Find_Layer(GetObjectLayer()),
								//Find_PathFree(target),
								Find_Exclude(shooter),
								Sort_Distance(x_p - GetX(), y_p - GetY())
							))
	{
		if (obj->~IsProjectileTarget(this, shooter) || obj->GetOCF() & OCF_Alive)
		{
			var objdist = Distance(x_p, y_p, obj->GetX(), obj->GetY());
			SetPosition(x_p + Sin(angle, objdist, 100), y_p - Cos(angle, objdist, 100));
			var self = this;
			HitObject(obj, true);
			hit_object = true;
			break;
		}
	}
	
	// at end position now
	for(var obj in FindObjects(Find_OnLine(x_p - GetX(), y_p - GetY()), Find_Func("IsProjectileInteractionTarget")))
	{
		obj->~OnProjectileInteraction(x_p, y_p, angle, shooter, damage);
	}
	
	if(!shooter.silencer)
	{
		var t = CreateObject(Bullet_TrailEffect, 0, 0, NO_OWNER);
		t->Point({x = x_p, y = y_p}, {x = GetX(), y = GetY()});
		t->FadeOut();
		t->SetObjectBlitMode(GFX_BLIT_Additive);
	}
	
	var self = this;
	if(!hit_object)
	{
		var hit = GBackSolid(Sin(angle, 2, 100), -Cos(angle, 2, 100));
		
		if(hit)
			Hit();
	}
	if(self) RemoveObject();
}

func CreateTrail()
{
	// neat trail
	trail = CreateObject(BulletTrail,0,0);
	trail->SetObjectBlitMode(GFX_BLIT_Additive);
	trail->Set(2, 125, this);
}

func FxPositionCheckTimer(target, effect, time)
{
	lastX = GetX();
	lastY = GetY();
	nextX = lastX + GetXDir()/10;
	nextY = lastY + GetYDir()/10;
}

func Traveling()
{
}

func TrailColor(int acttime){/*return 0x88fffffff;*/ return RGBa(255,255 - Min(150, acttime*20) ,75,255);}

public func HitObject(object obj, bool no_remove)
{
	if(obj.receive_crits > 0)
	{
		damage = (3 * damage) / 2;
		this.crit = true;
	}
	
	DoDmg(damage, nil, obj, nil, nil, this, from_ID);
	CreateImpactEffect(Max(5, damage*2/3));
	
	if(!no_remove) RemoveObject();
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
		Procedure = DFA_FLOAT,
		NextAction = "Travel",
		Length = 1,
		Delay = 1,
		FacetBase = 1,
		FacetCall="Traveling",
	},
};
local Name = "$Name$";
local Description = "$Description$";



/////////////////////////////////////////////////////////////////////+
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/* Schuss */

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

public func Initialize()
{
	SetObjectBlitMode(1);
}


public func Launch(int iAngle, int iSpeed, int iDist, int iSize, int iTrail, int iDmg, int iDmgType/*, int iDmgPrec*/, int iGlowSize, int iAPrec, int iReflections)
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

	// und zuweisen
	iSize = Min(iSize+2,GetDefWidth());
	iPrec = 0;//iDmgPrec;

	// Positionieren
	//SetPosition(GetX(),GetY()+GetDefWidth()/2);
	lx = GetX(); ly = GetY()+GetDefWidth()/2;

	DoCon(100*iSize/GetDefWidth()-100);

	if(DoCorrectPosition()) SetPosition(lx,ly);

	//lx = GetX();
	//ly = GetY();

	iTime = 10*iDist/iSpeed;

	if(!iTime)
		return(RemoveObject());

	var self = this();
	SetAction("Travel");
	if(!self) return;	 // Kleiner Sicherheitscheck, ob die Kugel nicht sofort verschwindet

	SetXDir(+Sin(iAngle,iSpeed, iAPrec));
	SetYDir(-Cos(iAngle,iSpeed, iAPrec));
	SetR(+iAngle);

	// Trail erzeugen
	CreateTrail(iSize, iTrail);

	bGlow = false;
	// Tolles Leuchten erzeugen
	if(iGlowSize)
	{
		bGlow = true;
		SetGraphics(0,this(),LIGH,1,GFXOV_MODE_Base, 0, 1);
		SetObjDrawTransform(100+35*iGlowSize,0,0, 0,100+35*iGlowSize,0, this(),1);
		SetClrModulation(GlowColor(1),this(),1);
	}

	AddEffect("HitCheck", this(), 1,1, 0,GetID(),shooter);
	//AddEffect("HitCheck", this, 1,1, this,0,shooter);

	// Werte für Reflektionen speichern
	iRefl = iReflections;
	//if(iRefl)
	//{
		iTrailSize = iSize;
		iTrailLength = iTrail;
		iXDir = GetXDir(0, 100);
		iYDir = GetYDir(0, 100);
	//}

	// Werte für Reflektionen speichern
	/*iRefl = iReflections;
	if(iRefl)
	{
		iTrailSize = iSize;
		iTrailLength = iTrail;
		iXDir = GetXDir(0, 100);
		iYDir = GetYDir(0, 100);
	}*/
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


public func LaunchInstant(int iAngle, int iSpeed, int iDist, int iSize, int iTrail, int iDmg, int iDmgType/*, int iDmgPrec*/, int iGlowSize, int iAPrec, int iReflections)
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

	// und zuweisen
	iSize = Min(iSize+2,GetDefWidth());
	iPrec = 0;//iDmgPrec;

	// Positionieren
	SetPosition(GetX(),GetY()+GetDefWidth()/2);

	DoCon(100*iSize/GetDefWidth()-100);

	lx = GetX();
	ly = GetY();

	iTime = 10*iDist/iSpeed;

	if(!iTime)
		return(RemoveObject());

	var self = this();
	SetAction("Travel");
	if(!self) return;	 // Kleiner Sicherheitscheck, ob die Kugel nicht sofort verschwindet

	SetXDir(+Sin(iAngle,iSpeed, iAPrec));
	SetYDir(-Cos(iAngle,iSpeed, iAPrec));
	SetR(+iAngle);

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

/* Timer */

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

/* Treffer */

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
							/*Find_Not(Find_Func("HitExclude"))*/);
 
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

/* Effekt für Trefferüberprüfung */

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
/*
public func FxHitCheckTimer(object target, int effect, int time)
{
	var obj;
	// Oh man. :O
	var oldx = EffectVar(0, target, effect);
	var oldy = EffectVar(1, target, effect);
	var newx = GetX(target);
	var newy = GetY(target);
	var dist = Distance(oldx, oldy, newx, newy);
	EffectVar(0, target, effect) = GetX(target);
	EffectVar(1, target, effect) = GetY(target);

	// Schuss schon Scharf?
	var exclude = EffectVar(2, target, effect);
	// Ja, wir treffen nur uns selbst nicht (ja, Workaround .)
	if(EffectVar(4, target, effect)) exclude = target;

	//DrawParticleLine("NoGravSpark",newx-oldx, newy-oldy,0,0,1,25,RGB(255,0,0),RGB(0,0,255));
	// Wir suchen nach Objekten entlang der Linie die wir uns seit dem letzten Check
	// bewegt haben. Und sortieren sie nach Distanz (nähere zuerst)
	for(obj in FindObjects(Find_OnLine(oldx,oldy,newx,newy),
												 Find_NoContainer(),
												 Sort_Distance(oldx, oldy)))
	{
		// Excludes
		if(obj == target) continue;
		if(obj == exclude) continue;
		
		// CheckEnemy
		if(!CheckEnemy(obj,target)) continue;

		// IsBulletTarget oder Alive
		if(obj->~IsBulletTarget(GetID(target),target,EffectVar(2, target, effect)) || GetOCF(obj) & OCF_Alive) {
			DebugLog("%s IsBulletTarget: %i, %s, %s","HitCheck",GetName(obj),GetID(target),GetName(target),GetName(EffectVar(2, target, effect)));
			return(target-> ~HitObject(obj));
		}
	}

	EffectVar(0, target, effect) = GetX(target);
	EffectVar(1, target, effect) = GetY(target);

	//verdammt, kommentier doch mal... Also:
	// Der Schuss wird erst "scharf gemacht", d.h. kann den Schützen selbst treffen, wenn
	// der Schuss einmal die Shape des Schützen verlassen hat.

	// OKOK, Ich hab sogar das Restzeug kommentiert. :P
	if(!EffectVar(5,target,effect)) {
		if(!EffectVar(4, target, effect)) {
			// ready gibt an, ob wir schon "scharf" sind. True = Scharf
			var ready = true;
			// Wir suchen alle Objekte mit der ID unseres Schützens an unserer momentanen Stelle
			for(var foo in FindObjects(Find_AtPoint(GetX(target),GetY(target)),Find_ID(EffectVar(3, target, effect))))
				// und schauen, ob es der Schütze ist.
				if(foo == EffectVar(2, target, effect))
					// wir haben den Schützen gefunden -> Er ist noch an unserer Position
					ready = false;
			// wir haben den Schützen nicht gefunden
			if(ready)
				// -> Wir treffen ihn ab jetzt.
				EffectVar(4, target, effect) = true;
		}
	}
}
*/


public func FxHitCheckTimer(object target, int effect, int time)
{
	var obj;
	var oldx = EffectVar(0, target, effect);
	var oldy = EffectVar(1, target, effect);
	var newx = GetX(target);
	var newy = GetY(target);
	EffectVar(0, target, effect) = GetX(target);
	EffectVar(1, target, effect) = GetY(target);

	//Schuss schon Scharf?
	var exclude = EffectVar(2, target, effect);
	//Nicht selber treffen
	if(EffectVar(4, target, effect)) exclude = target;

	if(target->~HitCheck(oldx,oldy,newx,newy,exclude)) return -1;
/*
	//DrawParticleLine("NoGravSpark",newx-oldx, newy-oldy,0,0,1,25,RGB(255,0,0),RGB(0,0,255));
	//Wir suchen nach Objekten entlang der Linie die wir uns seit dem letzten Check
	//bewegt haben. Und sortieren sie nach Distanz (nähere zuerst)
	for(obj in FindObjects(Find_OnLine(oldx,oldy,newx,newy),
				Find_NoContainer(),
				Sort_Distance(oldx, oldy)))
	{
		//Excludes
		if(obj == target) continue;
		if(obj == exclude) continue;

		//CheckEnemy
		if(!CheckEnemy(obj,target)) continue;

		// IsBulletTarget oder Alive
		if(obj->~IsBulletTarget(GetID(target),target,EffectVar(2, target, effect),oldx,oldy) || GetOCF(obj) & OCF_Alive)
		{
			DebugLog("%s IsBulletTarget: %i, %s, %s","HitCheck",GetName(obj),GetID(target),GetName(target),GetName(EffectVar(2, target, effect)));
			return(target-> ~HitObject(obj));
		}
	}
*/
	if(!target) return -1;
	if(!EffectVar(0, target, effect )) return -1;
	EffectVar(0, target, effect) = GetX(target);
	EffectVar(1, target, effect) = GetY(target);

	//Der Schuss wird erst "scharf gemacht", d.h. kann den Schützen selbst treffen, wenn
	//der Schuss einmal die Shape des Schützen verlassen hat.

	if(!EffectVar(5,target,effect))
	{
		if(!EffectVar(4, target, effect))
		{
			//Ready gibt an, ob wir schon "scharf" sind. True = Scharf
			var ready = true;
			//Wir suchen alle Objekte mit der ID unseres Schützens an unserer momentanen Stelle
			for(var foo in FindObjects(Find_AtPoint(GetX(target),GetY(target)),Find_ID(EffectVar(3, target, effect))))
			//Und schauen, ob es der Schütze ist.
			if(foo == EffectVar(2, target, effect))
				//Wir haben den Schützen gefunden -> Er ist noch an unserer Position
				ready = false;
			//Wir haben den Schützen nicht gefunden
			if(ready)
				//Wir treffen ihn ab jetzt
				EffectVar(4, target, effect) = true;
		}
	}
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
