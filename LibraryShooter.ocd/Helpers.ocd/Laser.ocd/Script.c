/* --- Properties --- */

local phase;	// int - phase of the animation
local width;	// int - the laser beam is this wide, in pixels
local length;	// int - the laser beam is this long, in pixels
local angle;	// int - the rotation of the laser, in degrees
local lifetime;	// int - the time until the laser is removed, in frames
local timer;	// int - the time that has already passed, in frames
local alpha;    // int - the alpha modulation

local pAttach;
local dx;
local dy;

local BorderBound = 7;


local ActMap = {
Laser = {
	Prototype = Action,
	Name = "Laser",
	Procedure = DFA_FLOAT,
	Length = 4,
	Delay = 1,
	X = 0,
	Y = 0,
	Wdt = 32,
	Hgt = 96,
	NextAction = "Laser",
	PhaseCall = "Laser",
},

LaserEnd = {
	Prototype = Action,
	Name = "LaserEnd",
	Procedure = DFA_FLOAT,
	Length = 4,
	Delay = 1,
	X = 0,
	Y = 0,
	Wdt = 32,
	Hgt = 128,
	NextAction = "LaserEnd",
	PhaseCall = "Laser",
},
};


/* --- Engine callbacks --- */

func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);

	// default values
	SetClrModulation(RGB(255,0,0));
	width = 3;
	length = 300;
	timer = 0;
	alpha = 200;
}


/* --- Interface --- */

public func SetPosi(int iPos)
{
	phase = iPos;
	return this;
}

public func SetRotation(int rotation)
{
	angle = rotation - 180;
	return this;
}

public func SetWidth(int pixels)
{
	width = 1000 * pixels / this.ActMap["Laser"].Wdt;
	return this;
}

public func SetLifetime(int frames)
{
	lifetime = frames;
	return this;
}

public func Attach(object pAtt)
{
	pAttach = pAtt;
	return this;
}

public func Color(int rgba)
{
	alpha = GetRGBaValue(GetClrModulation(), RGBA_ALPHA);
	SetClrModulation(rgba);
	return this;
}

public func Line(int x_start, int y_start, int x_end, int y_end)
{
	SetPosition(x_start, y_start);
	SetRotation(Angle(x_start, y_start, x_end, y_end));
	SetRange(Distance(x_start, y_start, x_end, y_end));
	return this;
}

public func SetRange(int pixels)
{
	length = pixels;
	return this;
}

public func GetRotation()
{
	return angle + 180;
}

public func GetLaserWidth()
{
	return width * this.ActMap["Laser"] / 1000;
}

public func GetLaserLength()
{ 
	var name = GetAction();
	if (name == "Idle") name = "Laser";

	return length * this.ActMap[name].Hgt / 1000;
}

public func LaserEnd()
{
	return [GetX() + Sin(GetRotation(), GetLaserLength()),
	        GetY() - Cos(GetRotation(), GetLaserLength())];
}

func Activate()
{
	if(GetAction() != "Laser")
	{
		SetAction("Laser");
	}

	DrawTransform();
}

func Update()
{
	DrawTransform();
}

func Laser()
{
	DrawTransform();

	// attach to obj
	// if(pAttach) SetPosition(dx + pAttach->GetX(), dy + pAttach->GetY());

	// animation & effect
	SetPhase(Random(4));

	// fade out (only if lifetime != 0)
	if(!lifetime) return;

	var a = alpha * (lifetime - timer) / lifetime;
	var rgba = SetRGBaValue(GetClrModulation(), a, RGBA_ALPHA);
	SetClrModulation(rgba);

	++timer;
	if(timer >= lifetime) RemoveObject();
}

func DrawTransform()
{
	// draw line

	var current_length = BoundBy(length - phase, 0, length);
	current_length = 1000 * length / this.ActMap[GetAction()].Hgt;

	var fsin = -Sin(angle, 1000);
	var fcos = +Cos(angle, 1000);

	var xoff = -this.ActMap[GetAction()].Wdt * width / 2000;
	var yoff = 0;

	var width2 = +fcos * width / 1000;
	var height = +fcos * current_length /1000;
	var xskew = +fsin * current_length / 1000;
	var yskew = -fsin * width / 1000;

	var xadjust = +fcos*xoff + fsin*yoff +fsin*phase;
	var yadjust = -fsin*xoff + fcos*yoff +fcos*phase;

	// set matrix values
	SetObjDrawTransform(width2, xskew, xadjust, yskew, height, yadjust);
}
