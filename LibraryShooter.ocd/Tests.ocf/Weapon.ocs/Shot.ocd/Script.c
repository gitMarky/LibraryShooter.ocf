#include Library_Projectile

protected func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);
}

private func OnLaunch()
{
	SetAction("Travel");
}

private func OnLaunched()
{
	CreateTrail(0, 0);
	if (trail) trail->SetGraphics("Red");
}


private func ProjectileColor(int time)
{
	var progress = 100 * time / lifetime;
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}

private func TrailColor(int time)
{
	var progress = 100 * time / lifetime;
	var value = Max(0, 255 - progress * 2);
	
	return RGBa(255, value, value, value);
}
