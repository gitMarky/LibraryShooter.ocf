/*--
		HitCheck.c
		Authors: Newton, Boni
	
		Effect for hit checking.
		Facilitates any hit check of a projectile. The Projectile hits anything
		which is either alive or returns for IsProjectileTarget(object projectile,
		object shooter) true. If the projectile hits something, it calls
		HitObject(object target) in the projectile.
--*/

global func DoHitCheckCall()
{
	var e = GetEffect("HitCheck2", this);
	if(!e) return;
	EffectCall(this, e, "DoCheck");
}

global func FxHitCheck2Start(object target, proplist effect, int temp, object by_obj, bool never_shooter)
{
	if (temp)
		return;
	effect.startx = target->GetX();
	effect.starty = target->GetY();
	effect.oldx = effect.startx;
	effect.oldy = effect.starty;
	if (!by_obj)
		by_obj = target;
	if (by_obj->Contained())
		by_obj = by_obj->Contained();
	effect.shooter = by_obj;
	effect.live = false;
	effect.never_shooter = never_shooter;
	
	// C4D_Object has a hitcheck too -> change to vehicle to supress that.
	if (target->GetCategory() & C4D_Object)
		target->SetCategory((target->GetCategory() - C4D_Object) | C4D_Vehicle);
	
	effect.range = target.bulletRange;
	
	EffectCall(target, effect, "DoCheck");
	
	return;
}

global func FxHitCheck2Stop(object target, proplist effect, int reason, bool temp)
{
	if (temp)
		return;
	
	target->SetCategory(target->GetID()->GetCategory());
	return;
}

global func FxHitCheck2DoCheck(object target, proplist effect)
{
	var obj;
	// rather search in front of the projectile, since a hit might delete the effect,
	// and clonks can effectively hide in front of walls.
	// NO WTF IS THIS SHIT
	var oldx = effect.oldx;
	var oldy = effect.oldy;
	var newx = target->GetX();
	var newy = target->GetY();
	effect.oldx = newx;
	effect.oldy = newy;
	var dist = Distance(oldx, oldy, newx, newy);
	
	var shooter = effect.shooter;
	var live = effect.live;
	
	if (live)
		shooter = target;
	
	if (dist <= Max(1, Max(Abs(target->GetXDir()), Abs(target->GetYDir()))) * 2)
	{
		// We search for objects along the line on which we moved since the last check
		// and sort by distance (closer first).
		for (obj in FindObjects(Find_OnLine(oldx, oldy, newx, newy),
								Find_NoContainer(),
								Find_Layer(target->GetObjectLayer()),
								Find_PathFree(target),
								Sort_Distance(oldx, oldy)))
		{
			// Excludes
			if(obj == target) continue;
			if(obj == shooter) continue;
			// Unlike in hazard, there is no NOFF rule (yet)
			// CheckEnemy
			//if(!CheckEnemy(obj,target)) continue;

			// IsProjectileTarget or Alive will be hit
			if (obj->~IsProjectileTarget(target, shooter) || obj->GetOCF() & OCF_Alive)
			{
				var objdist = ObjectDistance(target, obj);
	
				target->SetPosition(oldx + Cos(target->GetR()-90, objdist), oldy + Sin(target->GetR()-90, objdist));
				if(target.trail)
					target.trail->Traveling();
				
				target->~HitObject(obj);
				if (!target)
					return;
			}
		}
	}
	
	return;
}
	
/*global func FxHitCheck2DoCheck(object target, proplist effect)
{
	var obj;
	// rather search in front of the projectile, since a hit might delete the effect,
	// and clonks can effectively hide in front of walls.
	// NO WTF IS THIS SHIT
	var oldx = effect.x;
	var oldy = effect.y;
	var newx = target->GetX();
	var newy = target->GetY();
	var dist = Distance(oldx, oldy, newx, newy);
	
	var shooter = effect.shooter;
	var live = effect.live;
	
	if (live)
		shooter = target;
	
	if (dist <= Max(1, Max(Abs(target->GetXDir()), Abs(target->GetYDir()))) * 2)
	{
		// We search for objects along the line on which we moved since the last check
		// and sort by distance (closer first).
		for (obj in FindObjects(Find_OnLine(oldx, oldy, newx, newy),
								Find_NoContainer(),
								Find_Layer(target->GetObjectLayer()),
								Find_PathFree(target),
								Sort_Distance(oldx, oldy)))
		{
			// Excludes
			if(obj == target) continue;
			if(obj == shooter) continue;

			// Unlike in hazard, there is no NOFF rule (yet)
			// CheckEnemy
			//if(!CheckEnemy(obj,target)) continue;

			// IsProjectileTarget or Alive will be hit
			if (obj->~IsProjectileTarget(target, shooter) || obj->GetOCF() & OCF_Alive)
			{
				target->~HitObject(obj);
				if (!target)
					return;
			}
		}
	}
	return;
}*/

global func FxHitCheck2Effect(string newname)
{
	if (newname == "HitCheck2")
		return -2;
	return;
}

global func FxHitCheck2Add(object target, proplist effect, string neweffectname, int newtimer, by_obj, never_shooter)
{
	effect.x = target->GetX();
	effect.y = target->GetY();
	if (!by_obj)
		by_obj = target;
	if (by_obj->Contained())
		by_obj = by_obj->Contained();
	effect.shooter = by_obj;
	effect.live = false;
	effect.never_shooter = never_shooter;
	return;
}

global func FxHitCheck2Timer(object target, proplist effect, int time)
{
	EffectCall(target, effect, "DoCheck");
	// It could be that it hit something and removed itself. thus check if target is still there.
	// The effect will be deleted right after this.
	if (!target)
		return -1;
		
	if(effect.range - 1 == time)
		return -1;
	
	effect.x = target->GetX();
	effect.y = target->GetY();
	var live = effect.live;
	var never_shooter = effect.never_shooter;
	var shooter = effect.shooter;

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
				effect.live = true;
		}
	}
	return;
}

