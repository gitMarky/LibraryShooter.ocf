/*--
		HitCheck.c
		Authors: Newton, Boni
	
		Effect for hit checking.
		Facilitates any hit check of a projectile. The Projectile hits anything
		which is either alive or returns for IsProjectileTarget(object projectile,
		object shooter) true. If the projectile hits something, it calls
		HitObject(object target) in the projectile.
--*/

global func StartHitCheckCall(object shooter, bool never_shooter, bool limit_velocity)
{
	AssertObjectContext("StartHitCheckCall()");

	return AddEffect("HitCheck2", this, 1, nil, nil, nil, shooter, never_shooter, limit_velocity);
}

global func DoHitCheckCall()
{
	AssertObjectContext("DoHitCheckCall()");

	var e = GetHitCheck();
	if(!e) return;
	EffectCall(this, e, "DoCheck");
}

global func UpdateHitCheckCoordinates(int x_start, int y_start, int x_end, int y_end)
{
	AssertObjectContext("UpdateHitCheckCoordinates()");

	var e = GetHitCheck();
	if (!e) return;

	e.oldx = x_start;
	e.oldy = y_start;
	e.newx = x_end;
	e.newy = y_end;
}


global func ExcludedFromHitCheckCall()
{
	AssertObjectContext("ExcludedFromHitCheckCall()");
	
	var e = GetHitCheck();
	if (!e) return [];
	return e.excluded;
}


global func SetHitCheckCallCounter(int value)
{
	AssertObjectContext("ResetHitCheckCallCounter()");
	
	var e = GetHitCheck();
	if (!e) return;
	e.registered_hit = value ?? -1;
}


global func GetHitCheck()
{
	AssertObjectContext("GetHitCheck()");

	return GetEffect("HitCheck2", this);
}

global func FxHitCheck2Start(object target, proplist fx, int temp, object by_obj, bool never_shooter, bool limit_velocity)
{
	if (temp) return;
	
	fx.startx = target->GetX();
	fx.starty = target->GetY();
	fx.oldx = fx.startx;
	fx.oldy = fx.starty;
	
	if (!by_obj)
		by_obj = target;
	if (by_obj->Contained())
		by_obj = by_obj->Contained();

	fx.shooter = by_obj;
	fx.live = false;
	fx.never_shooter = never_shooter;
	fx.limit_velocity = limit_velocity;
	fx.registered_hit = -1;
	fx.excluded = [];
	
	// C4D_Object has a hitcheck too -> change to vehicle to supress that.
	if (target->GetCategory() & C4D_Object)
		target->SetCategory((target->GetCategory() - C4D_Object) | C4D_Vehicle);
	
	//fx.range = target.bulletRange;
	
	EffectCall(target, fx, "DoCheck");
	
	return;
}

global func FxHitCheck2Stop(object target, proplist fx, int reason, bool temp)
{
	if (temp)
		return;
	
	target->SetCategory(target->GetID()->GetCategory());
	return;
}

global func FxHitCheck2DoCheck(object target, proplist fx, int timer)
{
	if (fx.registered_hit >= FrameCounter())
	{
		return;
	}

	var obj;
	// rather search in front of the projectile, since a hit might delete the effect,
	// and clonks can effectively hide in front of walls.
	// NO WTF IS THIS SHIT
	var oldx = fx.oldx;
	var oldy = fx.oldy;
	var newx = target->GetX();
	var newy = target->GetY();
	fx.oldx = newx;
	fx.oldy = newy;
	var dist = Distance(oldx, oldy, newx, newy);
	
	var shooter = fx.shooter;
	var live = fx.live;
	
	if (live && !fx.never_shooter)
		shooter = target;
	
	if (!fx.limit_velocity || (dist <= Max(1, Max(Abs(target->GetXDir()), Abs(target->GetYDir()))) * 2))
	{
		// We search for objects along the line on which we moved since the last check
		// and sort by distance (closer first).
		for (obj in FindObjects(Find_OnLine(oldx, oldy, newx, newy),
								Find_NoContainer(),
								Find_Layer(target->GetObjectLayer()),
								//Find_PathFree(target),
								Sort_Distance(oldx, oldy)))
		{	
			// Excludes
			if (obj == target) continue;
			if (obj == shooter) continue;
			if (IsValueInArray(fx.excluded)) continue;
			// Unlike in hazard, there is no NOFF rule (yet)
			// CheckEnemy
			//if(!CheckEnemy(obj,target)) continue;

			// IsProjectileTarget or Alive will be hit
			if (obj->~IsProjectileTarget(target, shooter) || obj->GetOCF() & OCF_Alive)
			{
				var objdist = Distance(oldx, oldy, obj->GetX(), obj->GetY());
				var diffx = objdist * (newx - oldx) / Max(dist, 1);
				var diffy = objdist * (newy - oldy) / Max(dist, 1);

				target->SetPosition(oldx + diffx, oldy + diffy);

				if(target.trail)
					target.trail->~Travelling();

				if (target) target->~HitObject(obj, true, fx);

				if (fx.registered_hit >= FrameCounter() || !target)
				{
					return;
				}
			}
		}
	}
	
	return;
}

global func FxHitCheck2Effect(string newname)
{
	if (newname == "HitCheck2")
		return -2;
	return;
}

global func FxHitCheck2Add(object target, proplist fx, string neweffectname, int newtimer, by_obj, never_shooter, limit_velocity)
{
	fx.x = target->GetX();
	fx.y = target->GetY();
	if (!by_obj)
		by_obj = target;
	if (by_obj->Contained())
		by_obj = by_obj->Contained();
	fx.shooter = by_obj;
	fx.live = false;
	fx.never_shooter = never_shooter;
	fx.limit_velocity = limit_velocity;
	fx.registered_hit = -1;
	return;
}

global func FxHitCheck2Timer(object target, proplist fx, int time)
{
	EffectCall(target, fx, "DoCheck");
	// It could be that it hit something and removed itself. thus check if target is still there.
	// The effect will be deleted right after this.
	if (!target)
		return -1;
		
	//if(effect.range - 1 == time)
	//	return -1;
	
	fx.x = target->GetX();
	fx.y = target->GetY();
	var live = fx.live;
	var never_shooter = fx.never_shooter;
	var shooter = fx.shooter;

	// The projectile will be only switched to "live", meaning that it can hit the
	// shooter himself when the shot exited the shape of the shooter one time.
	if (!never_shooter)
	{
		if (!live)
		{
			var ready = true;
			// We search for all objects with the id of our shooter.
			for (var foo in FindObjects(Find_AtPoint(target->GetX(), target->GetY()), Find_ID(shooter->GetID())))
			{
				// If its the shooter...
				if(foo == shooter)
					// we may not switch to "live" yet.
					ready = false;
			}
			// Otherwise, the shot will be live.
			if (ready)
				fx.live = true;
		}
	}
	return;
}

