/**
	A dummy definition, used to provide various getters and setters for fire modes.
	Keep in mind that for fire modes to be writable, they must be created using {@link Library_Firearm#AddFiremode}.

	@author Clonkonaut
*/


/*-- Getters --*/

/**
 Get the mode (firing style) of this fire mode.
 @return An integer (see {@link Library_Firearm} fire modes description).
*/
public func GetMode()
{
	return this.mode;
}

/**
 Get the name of this fire mode.
 @return A string.
*/
public func GetName()
{
	return this.name;
}

/**
 Get the icon of this fire mode.
 @return An ID.
*/
public func GetIcon()
{
	return this.icon;
}

/**
 Get the condition callback of this fire mode.
 @return A string.
*/
public func GetCondition()
{
	return this.condition;
}

/**
 Get the ammunition ID of this fire mode.
 @return An ID.
*/
public func GetAmmoID()
{
	return this.ammo_id;
}

/**
 Get the ammunition usage of this fire mode.
 @return An integer.
*/
public func GetAmmoUsage()
{
	return this.ammo_usage ?? 1;
}

/**
 Get the ammunition rate of this fire mode.
 @return An integer.
*/
public func GetAmmoRate()
{
	return this.ammo_rate ?? 1;
}

/**
 Get the charging delay of this fire mode.
 @return An integer.
*/
public func GetChargeDelay()
{
	return this.delay_charge;
}

/**
 Get the recovery delay of this fire mode.
 @return An integer.
*/
public func GetRecoveryDelay()
{
	return this.delay_recover;
}

/**
 Get the cooldown delay of this fire mode.
 @return An integer.
*/
public func GetCooldownDelay()
{
	return this.delay_cooldown;
}

/**
 Get the reloading delay of this fire mode.
 @return An integer.
*/
public func GetReloadDelay()
{
	return this.delay_reload;
}

/**
 Get the damage of this fire mode.
 @return An integer.
*/
public func GetDamage() // yes, this shadows an engine function!
{
	return this.damage;
}

/**
 Get the damage type of this fire mode.
 @return An integer.
*/
public func GetDamageType()
{
	return this.damage_type;
}

/**
 Get the projectile ID of this fire mode.
 @return An ID.
*/
public func GetProjectileID()
{
	return this.projectile_id;
}

/**
 Get the projectile speed of this fire mode.
 @return An integer.
*/
public func GetProjectileSpeed()
{
	return this.projectile_speed;
}

/**
 Get the projectile spread of this fire mode.
 @return An array of two integers, spread and precision.
*/
public func GetProjectileSpread()
{
	return this.projectile_spread;
}

/**
 Get the projectile range of this fire mode.
 @return An integer.
*/
public func GetProjectileRange()
{
	return this.projectile_range;
}

/**
 Get the projectile distance of this fire mode.
 @return An integer.
*/
public func GetProjectileDistance()
{
	return this.projectile_distance;
}

/**
 Get the Y offset of this fire mode.
 @return An integer.
*/
public func GetYOffset()
{
	return this.projectile_offset_y;
}

/**
 Get the spread of this fire mode.
 @return An array of two integers, spread and precision.
*/
public func GetSpread()
{
	return this.spread;
}

/**
 Get the amount of projectiles firing in a burst of this fire mode.
 @return An integer.
*/
public func GetBurstAmount()
{
	return this.burst;
}

/**
 Get the automatic reload setting of this fire mode.
 @return A boolean.
*/
public func GetAutoReload()
{
	return this.auto_reload;
}

/**
 Get the shooting animation name of this fire mode.
 @return A string.
*/
public func GetShootingAnimation()
{
	return this.anim_shoot_name;
}

/**
 Get the loading animation name of this fire mode.
 @return A string.
*/
public func GetReloadAnimation()
{
	return this.anim_load_name;
}

/**
 Get the forward walking speed of this fire mode.
 @return An integer.
*/
public func GetForwardWalkingSpeed()
{
	return this.walk_speed_front;
}

/**
 Get the backward walking speed of this fire mode.
 @return An integer.
*/
public func GetBackwardWalkingSpeed()
{
	return this.walk_speed_back;
}

/*-- Setters --*/

/**
 Set the mode (firing style) of this fire mode.
 
 @par value An integer (see {@link Library_Firearm} fire modes description). Can simply be one of the following constants:@br
	- WEAPON_FM_Single: single shot style, only shot per click is fired (default).@br
	- WEAPON_FM_Burst: burst style, firing a set number of shot in short succession.@br
	- WEAPON_FM_Auto: auto style, firing as long as the use button is pressed.@br
	
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetMode(int value)
{
	this.mode = value;
	return this;
}

/**
 Set the name of this fire mode.
 
 @par value The name of this fire mode.
            Unnecessary if no GUI exists that displays the name.
 
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetName(string value)
{
	this.name = value;
	return this;
}

/**
 Set the icon of this fire mode.
 
 @par value The ID of a definition icon for the fire mode.
            Unnecessary if no GUI exists that displays the icon.
            
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetIcon(id value)
{
	this.icon = value;
	return this;
}

/**
 Set the condition callback of this fire mode.
 
 @par value A string corresponding to a function name, or a function pointer.
            The fire mode will not be marked as 'available' unless the condition functions return true. 
            Example: An upgraded weapon could offer more fire modes.

 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetCondition(value)
{
	this.condition = value;
	return this;
}

/**
 Set the ammunition ID of this fire mode.
 
 @par value A definition that represents ammunition for the fire mode.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetAmmoID(id value)
{
	this.ammo_id = value;
	return this;
}

/**
 Set the ammunition usage of this fire mode.
 
 @par value How much ammunition is needed per {@link Library_Firearm_Firemode#GetAmmoRate} shots.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetAmmoUsage(int value)
{
	this.ammo_usage = value;
	return this;
}

/**
 Set the ammunition rate of this fire mode.
 
 @par value An integer.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetAmmoRate(int value)
{
	this.ammo_rate = value;
	return this;
}

/**
 Set the charging delay of this fire mode.
 
 @par value Charge duration in frames before the weapon can fire.
            If 0 or nil, no charge is required.

 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetChargeDelay(int value)
{
	this.delay_charge = value;
	return this;
}

/**
 Set the recovery delay of this fire mode.
 
 @par value Recovery duration between consecutive shots in frames.
            If 0 or nil, no recovery is required
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetRecoveryDelay(int value)
{
	this.delay_recover = value;
	return this;
}

/**
 Set the cooldown delay of this fire mode.
 
 @par value Cooldown duration after firing the last shot in frames.
            The last shot is the last shot in a series of consecutive
            shots (as opposed to the last shot before ammo runs out).
            If 0 or nil, no cooldown is required.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetCooldownDelay(int value)
{
	this.delay_cooldown = value;
	return this;
}

/**
 Set the reloading delay of this fire mode.
 
 @par value Reload duration in frames. If 0 or nil, reloading is instantaneous.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetReloadDelay(int value)
{
	this.delay_reload = value;
	return this;
}

/**
 Set the damage of this fire mode.
 
 @par value The amount of damage that a projectile does.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetDamage(int value) // yes, this shadows an engine function!
{
	this.damage = value;
	return this;
}

/**
 Set the damage type of this fire mode.
 
 @par value Defining a damage type.
            Damage type handling is not done by this library
            and should be handled by any implementation.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetDamageType(int value)
{
	this.damage_type = value;
	return this;
}

/**
 Set the projectile ID of this fire mode.
 
 @par value A definition of the actual projectile that is being fired.
            These are created on the fly and must therefore not be created beforehand.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetProjectileID(id value)
{
	this.projectile_id = value;
	return this;
}

/**
 Set the projectile speed of this fire mode.
 
 @par value Firing speed of a projectile.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetProjectileSpeed(int value)
{
	this.projectile_speed = value;
	return this;
}

/**
 Set the spread of the projectiles for this fire mode. Refers to 
 the basic inaccuracy of the projectiles.
 
 @par value A proplist with the properties "angle" and "precision",
            see {@link Global#Projectile_Deviation}.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetProjectileSpread(proplist value)
{
	this.projectile_spread = value;
	return this;
}

/**
 Set the projectile range of this fire mode.
 
 @par value Maximum range a projectile flies.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetProjectileRange(int value)
{
	this.projectile_range = value;
	return this;
}

/**
 Set the projectile distance of this fire mode.
 
 @par value Distance the projectile is being created away from the shooting object.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetProjectileDistance(int value)
{
	this.projectile_distance = value;
	return this;
}

/**
 Set the Y offset of this fire mode.
 
 @par value Y offset when creating a projectile in case
            the barrel of the gun is not perfectly aligned
            to the firing object's center.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetYOffset(int value)
{
	this.projectile_offset_y = value;
	return this;
}

/**
 Set the spread of this fire mode. Refers to 
 additional deviation added by certain effects (e.g. continuous firing).
 
 @par value A proplist with the properties "angle" and "precision",
            see {@link Global#Projectile_Deviation}.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetSpread(proplist value)
{
	this.spread = value;
	return this;
}

/**
 Set the amount of shots being fired in a burst of this fire mode.
 
 @par value An integer.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetBurstAmount(int value)
{
	this.burst = value;
	return this;
}

/**
 Set the automatic reload setting of this fire mode.
 
 @par value If true, the weapon reloads even if the use button
            is not being held while reloading.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetAutoReload(bool value)
{
	this.auto_reload = value;
	return this;
}

/**
 Set the shooting animation name of this fire mode.
 
 @par value The animation name that is returned for
            the animation set (usually when being used by a Clonk)
            as general aim animation.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetShootingAnimation(string value)
{
	this.anim_shoot_name = value;
	return this;
}

/**
 Set the loading animation name of this fire mode.
 
 @par value The animation name that is returned for
      the animation set (usually when being used by a Clonk)
      as general reload animation.
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetReloadAnimation(string value)
{
	this.anim_load_name = value;
	return this;
}

/**
 Set the forward walking speed of this fire mode.
 
 @par value Forwards walking speed to be returned for
      the animation set (usually when being used by a Clonk).
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetForwardWalkingSpeed(int value)
{
	this.walk_speed_front = value;
	return this;
}

/**
 Set the backward walking speed of this fire mode.
 
 @par value Backwards walking speed to be returned for
      the animation set (usually when being used by a Clonk).
 @return proplist Returns the fire mode,
         so that further function calls can be issued.
*/
public func SetBackwardWalkingSpeed(int value)
{
	this.walk_speed_back = value;
	return this;
}