/**
	Draws a trail to any object. That object should fly straight or so fast
	that it looks like it because the trail itself is only drawn straight.
	Any projectile can use the trail by calling (after creating it):
	
	Set(int width, int length, object projectile)
	
	while width is the width of the trail, length the length of the trail and
	projectile itself. The trail will be removed when the projectile is gone.
	The color is always a light-grey. However each frame, ~TrailColor(int time)
	is called in the projectile which can return the color modulation.

	@author Newton (original author), Marky (modified script)
*/

local do_fade, fade_speed, move_speed, projectile, w, l, r, x, y;

local precision = 1000;
local fade_speed_precision = 100;
local fade_speed_factor = 100;
local fade_speed_add = 0;

/**
	Sets the trail to follow the projectile.

	@par shot [optional] This is the projectile. May be {@c nil} and in this
						 case the trail will fade out immediately.

	@par width The trail will be this wide, in pixels.

	@par length The trail will be this long, in pixels.
				Note that it stretches to this length from its original position.

	@par x_pos [optional] If provided, this specifies a starting position for the trail.
			    Otherwise this is the position of the projectile, if provided.

	@par y_pos [optional] If provided, this specifies a starting position for the trail.
			    Otherwise this is the position of the projectile, if provided.

	@par x_dir [optional] If provided, this specifies a velocity for the trail.
				Otherwise this is the velocity of the projectile, if provided.

	@par y_dir [optional] If provided, this specifies a velocity for the trail.
				Otherwise this is the velocity of the projectile, if provided.

	@par x_end [optional] Overrides the end position of the trail if not projectile was provided.

	@par y_end [optional] Overrides the end position of the trail if not projectile was provided.
 */
public func Set(object shot, int width, int length, int x_pos, int y_pos, int x_dir, int y_dir, int x_end, int y_end)
{
	projectile = shot;
	
	// collect data, set local variables
	
	if (projectile != nil)
	{
		if (x_dir == nil) x_dir = projectile->GetXDir(precision);
		if (y_dir == nil) y_dir = projectile->GetYDir(precision);
		if (x_pos == nil) x_pos = projectile->GetX();
		if (y_pos == nil) y_pos = projectile->GetY();
		if (x_end == nil) x_end = projectile->GetX();
		if (y_end == nil) y_end = projectile->GetY();
		
		AddEffect("IntProjectile", this, 1, 1, this);
	}
	else
	{
		if (x_end == nil) x_end = GetX();
		if (y_end == nil) y_end = GetY();
	}
	
	if (x_pos == nil) x_pos = GetX();
	if (y_pos == nil) y_pos = GetY();
	
	w = precision * width / ActMap.Travel.Wdt;
	l = precision * length / ActMap.Travel.Hgt;
	
	r = Angle(0, 0, x_dir, y_dir);
	x = GetX();
	y = GetY();
	
	move_speed = Sqrt(x_dir * x_dir/ precision + y_dir * y_dir / precision);
	SetFadeSpeed(fade_speed_factor);
	
	// do the magic!

	SetPosition(x_end, y_end);
	SetAction("Travel");
}


/**
	Specifies a color modulation for the trail, based on the time it exists.

	@par time The time that the trail exists, in frames.

	@return {@c nil}, which means that the color is not adjusted.
         You can override this function if you want a custom color modulation
         for trails.@br
         If the trail is set to a projectile, then this function is also called
         in the projectile. The return value of the projectile function has a
         higher priority than the local return value.
 */
public func TrailColor(int time)
{
	return nil;
}


/* --- Timer --- */

public func Travelling()
{
	// The shot is gone -> remove
	if (!do_fade && !projectile) Remove();
	
	// on the borders
	if (GetX() <= 0 && GetXDir() < 0 || GetX() >= LandscapeWidth() && GetXDir() > 0 || GetY() <= 0 && GetYDir() < 0 || GetY() >= LandscapeHeight() && GetYDir() > 0)
		Remove();
		
	DrawColorModulation();	
	DrawTransform();
	
	// Destroy
	
	if (l == 0)
		RemoveObject();
}

public func ProjectileUpdate()
{
	if (projectile)
	{
		SetPosition(projectile->GetX(), projectile->GetY());
		r = projectile->GetR();
		
	}
}

func Hit()
{
	Remove();
}

func Remove()
{
	l = Min(l, GetRelativeLength());
	
	do_fade = true;
	projectile = nil;
	move_speed = 0;
}

func GetRelativeLength()
{
	var distance = Distance(x, y, GetX(), GetY());
	var relative_length = precision * distance / ActMap.Travel.Hgt;
	return relative_length;
}

func DrawColorModulation()
{
	var trail_color;
	
	if (projectile != nil) trail_color = projectile->~TrailColor(GetActTime());
	if (trail_color == nil) trail_color = TrailColor(GetActTime());
	
	if (trail_color != nil)
	{
		SetClrModulation(trail_color);
	}
}

func DrawTransform()
{
	var relative_length = GetRelativeLength();
	
	// stretch <->
	var h = Max(0, Min(l + move_speed - fade_speed, relative_length));
	
	if (do_fade)
	{
		l = Min(l, h);
	}
	else
	{
		l = Max(l, h);
	}
	
	var fsin = -Sin(r, precision), fcos = Cos(r, precision);
	
	var xoff = -(ActMap.Travel.Wdt * w / precision) / 2;
	var yoff = 0;
	
	var width = +fcos * w / precision;
	var height = +fcos * h / precision;
	var xskew = +fsin * h / precision;
	var yskew = -fsin * w / precision;
	
	var xadjust = +fcos * xoff + fsin * yoff;
	var yadjust = -fsin * xoff + fcos * yoff;
	
	// set matrix values
	SetObjDrawTransform(width, xskew, xadjust, yskew, height, yadjust);
}

func SaveScenarioObject()
{
	return false;
}

local ActMap = 
{
	Travel =
	{
		Prototype = Action,
		Name = "Travel",
		Procedure = DFA_FLOAT,
		Speed = 100000,
		NextAction = "Travel",
		Length = 1,
		Delay = 1,
		X = 0,
		Y = 0,
		Wdt = 5,
		Hgt = 28,
		OffX = 0,
		OffY = 2,
		StartCall = "Travelling"
	},
	Laser =
	{
		Prototype = Action,
		Name = "Laser",
		Procedure = DFA_FLOAT,
		Speed = 100000,
		NextAction = "Laser",
		Length = 4,
		Delay = 1,
		X = 0,
		Y = 0,
		Wdt = 5,
		Hgt = 28,
		OffX = 0,
		OffY = 2,
		PhaseCall = "Travelling"
	}
};

func FxIntProjectileTimer(object target, proplist effect, int time)
{
	if (target != this)
	{
		FatalError("This should be used internally only, target and calling object are different.");
	}
	
	ProjectileUpdate();
}

public func SetFadeSpeed(int factor)
{
	fade_speed_factor = factor;
	fade_speed = move_speed * fade_speed_factor / fade_speed_precision;
}
