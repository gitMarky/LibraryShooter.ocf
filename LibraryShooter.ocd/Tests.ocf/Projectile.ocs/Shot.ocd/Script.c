#include Library_Projectile

func Initialize()
{
	SetObjectBlitMode(GFX_BLIT_Additive);
}

func OnLaunch()
{
	SetAction("Travel");
}
