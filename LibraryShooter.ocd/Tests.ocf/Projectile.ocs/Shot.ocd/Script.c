#include Library_Projectile

protected func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);
}

private func OnLaunch()
{
	SetAction("Travel");
}
