/**
 A temporary light source.
 @author Marky
 @version 0.1.0
 */

static const TEMPLIGHT_Effect = "IntLight";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// definitions

local Name = "Light";

local light_range_0 = nil;
local light_range_1 = nil;
local lifetime = 1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// global functions


/**
 Creates a temporary light at the given coordinates. The coordinates are relative to object coordinates in local context.
 @par x The x coordinate.
 @par y The y coordinate.
 @return object Returns the spawn point object, so that further function calls can be issued.
 @version 0.1.0
 */
global func CreateTemporaryLight(int x, int y)
{
	if (!this && (x == nil || y == nil))
	{
		FatalError("You have to specify x and y values in global context");
	}
	
	var point = CreateObject(TemporaryLight, x, y, NO_OWNER);
	return point;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// finished functions

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// temporary functions


public func LightRangeStart(int value)
{
	ProhibitedWhileActive();
	
	light_range_0 = value;
	return this;
}

public func LightRangeEnd(int value)
{
	ProhibitedWhileActive();
	
	if (light_range_0 == nil)
	{
		FatalError("Set a starting range with LightRangeStart() first!");
	}
	
	light_range_1 = value;

	return this;
}

public func SetLifetime(int frames)
{
	ProhibitedWhileActive();
	
	if (lifetime < 1)
	{
		FatalError(Format("The lifetime has to be at least 1, you specified %d", frames));
	}

	lifetime = frames;
	
	return this;
}

public func Color(int color)
{
	ProhibitedWhileActive();
	
	SetLightColor(color);
	
	return this;
}

public func Activate()
{
	if (!IsActive())
	{
		AddEffect(TEMPLIGHT_Effect, this, 1, 1, this);
	}
	
	if (light_range_0 != nil)
	{
		SetLightRange(light_range_0);
	}
}

private func IsActive()
{
	return GetEffect(TEMPLIGHT_Effect, this) != nil;
}

private func ProhibitedWhileActive()
{
	if (IsActive())
	{
		FatalError("This function should be used for configuring the light- it is not to be called while the light is active");
	}
}

private func FxIntLightTimer(object target, proplist fx, int timer)
{
	if (target != this)
	{
		FatalError(Format("The effect \"%s\" may only be applied to the temporary light", TEMPLIGHT_Effect));
	}
	
	if (timer > lifetime)
	{
		target->RemoveObject();
		
		return FX_Execute_Kill;
	}
	
	if (light_range_1 != nil)
	{
		var range = (timer * light_range_0 + Max(0, lifetime - timer) * light_range_1)/lifetime;

		SetLightRange(range);
	}
	
	return FX_OK;
}