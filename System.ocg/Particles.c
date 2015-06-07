global func CreateImpactEffect(int size, int xoff, int yoff, string name, particles)
{
	if (particles == nil)
	{
		particles = Particles_Glimmer();
	}
	
	if (name == nil)
	{
		name = "Frazzle";
	}
	
	particles.Phase = PV_Random(0, 4);
	CreateParticle(name, PV_Random(xoff - 2, xoff + 2), PV_Random(yoff - 2, yoff + 2), PV_Random(-size, size), PV_Random(-size, size), PV_Random(20, 40), particles, size * 2);
}

/**
 Creates the effect for a projectile casing.
 @par particle The particle name.
 @par size The size, in pixels.
 @par x Position for the effect, local coordinates.
 @par y Position for the effect, local coordinates.
 @par xdir Velocity for the effect.
 @par ydir Velocity for the effect. The created particles have a gravity factor of 5,
           which has to be considered when calculating the velocity.
 @version 0.1.0
 */
global func CreateCartridgeEffect(string particle, int size, int x, int y, int xdir, int ydir)
{
	CreateParticle(particle, x, y, xdir, ydir, PV_Random(60, 120), Particles_Cartridge(size), 1);
}

global func Particles_Cartridge(int size)
{
	return
	{
		Alpha = PV_KeyFrames(0, 0, 255, 950, 255, 1000, 0),
		R = 255, G = 255, B = 255,
		Rotation = PV_Step(PV_Random(1, 4), PV_Random(60)),
		CollisionVertex = 0,
		OnCollision = PC_Bounce(500),
		ForceY = PV_Gravity(500),
		Size = size
	};
}

global func Particles_Blood(int size, int rotation)
{
	return
	{
		R = 200, G = 0, B = 0,
		Size = PV_KeyFrames(0, 0, 0, 100, size, 900, size, 1000, 0),
		Alpha = PV_KeyFrames(0, 0, 255, 700, 255, 1000, 0),
		DampingX = 900,  DampingY = 900,
		ForceY = PV_Gravity(400),
		Rotation = rotation,
		Attach = ATTACH_Front,
		CollisionVertex = 500,
		OnCollision = PC_Die(),
		Stretch = 1000,
		Phase = PV_Random(0, 4)
	};
}



