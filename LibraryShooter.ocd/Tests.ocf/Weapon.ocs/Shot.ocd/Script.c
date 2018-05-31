#include Library_Projectile

func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);
}

func OnLaunch()
{
	SetAction("Travel");
}

func OnLaunched()
{
	CreateTrail(0, 0);
	if (trail) trail->SetGraphics("Red");
}


func ProjectileColor(int time)
{
	var progress = 100 * time / lifetime;
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}

func TrailColor(int time)
{
	var progress = 100 * time / lifetime;
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}
