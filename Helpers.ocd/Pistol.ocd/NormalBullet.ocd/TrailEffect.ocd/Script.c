/**
	TrailEffect
	Visual.
*/

local Name = "$Name$";
local Description = "$Description$";

local last_from, last_to;

func Initialize()
{
	this.Plane = 500;
	SetClrModulation(RGBa(255, 255, 255, 100));
}

func Point(from, to)
{
	if(from == nil) from = last_from;
	else last_from = from;
	
	last_to = to;
	
	var my_size = 10;
	var dis = Distance(from.x, from.y, to.x, to.y);
	var angle = Angle(from.x, from.y, to.x, to.y);
	var midpoint_x = (from.x + to.x)/2;
	var midpoint_y = (from.y + to.y)/2;
	var s = (dis * 100) / my_size;
	var stretch = (1100 * s) / 100;
	
	SetObjDrawTransform(500, 0, 0, 0, stretch, 0, 0);
	SetR(angle);
	SetPosition(midpoint_x, midpoint_y);
}

func FadeOut()
{
	AddEffect("QuickFade", this, 1, 1, this);
}

func FxQuickFadeTimer(target, effect, time)
{
	
	var x = (last_from.x + last_to.x) / 2;
	var y = (last_from.y + last_to.y) / 2;
	
	Point({x = x, y = y}, last_to);
	
	var fade = time * 10;
	if(fade > 90 || ((x == last_to.x) && (y == last_to.y)))
	{
		RemoveObject();
		return -1;
	}
	
	SetClrModulation(RGBa(255, 255, 255, 100 - fade));
}