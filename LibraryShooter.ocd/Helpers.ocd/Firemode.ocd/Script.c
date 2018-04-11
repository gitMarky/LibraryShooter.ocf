/**
	A dummy definition, used to provide various getters and setters for fire modes.@br
	Keep in mind that for fire modes to be writable, they must be created using {@link Library_Firearm#AddFiremode}.@br

	@author Clonkonaut
	@version 0.3.0
*/

/*-- Getters --*/

/**
 Get the mode (firing style) of this fire mode.@br
 @return An integer (see {@link Library_Firearm} fire modes description).
 @version 0.3.0
*/
public func GetMode()
{
	return this.mode;
}

/**
 Get the name of this fire mode.@br
 @return A string.
 @version 0.3.0
*/
public func GetName()
{
	return this.name;
}

/**
 Get the icon of this fire mode.@br
 @return An ID.
 @version 0.3.0
*/
public func GetIcon()
{
	return this.mode;
}

/**
 Get the condition callback of this fire mode.@br
 @return A string.
 @version 0.3.0
*/
public func GetCondition()
{
	return this.condition;
}

/**
 Get the ammunition ID of this fire mode.@br
 @return An ID.
 @version 0.3.0
*/
public func GetAmmoID()
{
	return this.ammo_id;
}

/**
 Get the ammunition usage of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetAmmoUsage()
{
	return this.ammo_usage;
}

/**
 Get the ammunition rate of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetAmmoRate()
{
	return this.ammo_rate;
}

/**
 Get the charging delay of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetChargeDelay()
{
	return this.delay_charge;
}

/**
 Get the recovery delay of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetRecoveryDelay()
{
	return this.delay_recover;
}

/**
 Get the cooldown delay of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetCooldownDelay()
{
	return this.delay_cooldown;
}

/**
 Get the reloading delay of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetReloadDelay()
{
	return this.delay_reload;
}

/**
 Get the damage of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetDamage() // yes, this shadows an engine function!
{
	return this.damage;
}

/**
 Get the damage type of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetDamageType()
{
	return this.damage_type;
}

/**
 Get the projectile ID of this fire mode.@br
 @return An ID.
 @version 0.3.0
*/
public func GetProjectileID()
{
	return this.projectile_id;
}

/**
 Get the projectile speed of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetProjectileSpeed()
{
	return this.projectile_speed;
}

/**
 Get the projectile range of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetProjectileRange()
{
	return this.projectile_range;
}

/**
 Get the projectile distance of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetProjectileDistance()
{
	return this.projectile_distance;
}

/**
 Get the Y offset of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetYOffset()
{
	return this.projectile_offset_y;
}

/**
 Get the spread of this fire mode.@br
 @return An array of two integers, spread and precision.
 @version 0.3.0
*/
public func GetSpread()
{
	return this.spread;
}

/**
 Get the amount of projectiles firing in a burst of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetBurstAmount()
{
	return this.burst;
}

/**
 Get the automatic reload setting of this fire mode.@br
 @return A boolean.
 @version 0.3.0
*/
public func GetAutoReload()
{
	return this.auto_reload;
}

/**
 Get the shooting animation name of this fire mode.@br
 @return A string.
 @version 0.3.0
*/
public func GetShootingAnimation()
{
	return this.anim_shoot_name;
}

/**
 Get the loading animation name of this fire mode.@br
 @return A string.
 @version 0.3.0
*/
public func GetReloadAnimation()
{
	return this.anim_load_name;
}

/**
 Get the forward walking speed of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetForwardWalkingSpeed()
{
	return this.walk_speed_front;
}

/**
 Get the backward walking speed of this fire mode.@br
 @return An integer.
 @version 0.3.0
*/
public func GetBackwardWalkingSpeed()
{
	return this.walk_speed_back;
}

/*-- Setters --*/

/**
 Set the mode (firing style) of this fire mode.@br
 @par value An integer (see {@link Library_Firearm} fire modes description).
 @version 0.3.0
*/
public func SetMode(int value)
{
	return this.mode = value;
}

/**
 Set the name of this fire mode.@br
 @par value A string.
 @version 0.3.0
*/
public func SetName(string value)
{
	return this.name = value;
}

/**
 Set the icon of this fire mode.@br
 @par value An ID.
 @version 0.3.0
*/
public func SetIcon(id value)
{
	return this.mode = value;
}

/**
 Set the condition callback of this fire mode.@br
 @par value A string.
 @version 0.3.0
*/
public func SetCondition(string value)
{
	return this.condition = value;
}

/**
 Set the ammunition ID of this fire mode.@br
 @par value An ID.
 @version 0.3.0
*/
public func SetAmmoID(id value)
{
	return this.ammo_id = value;
}

/**
 Set the ammunition usage of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetAmmoUsage(int value)
{
	return this.ammo_usage = value;
}

/**
 Set the ammunition rate of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetAmmoRate(int value)
{
	return this.ammo_rate = value;
}

/**
 Set the charging delay of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetChargeDelay(int value)
{
	return this.delay_charge = value;
}

/**
 Set the recovery delay of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetRecoveryDelay(int value)
{
	return this.delay_recover = value;
}

/**
 Set the cooldown delay of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetCooldownDelay(int value)
{
	return this.delay_cooldown = value;
}

/**
 Set the reloading delay of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetReloadDelay(int value)
{
	return this.delay_reload = value;
}

/**
 Set the damage of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetDamage(int value) // yes, this shadows an engine function!
{
	return this.damage = value;
}

/**
 Set the damage type of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetDamageType(int value)
{
	return this.damage_type = value;
}

/**
 Set the projectile ID of this fire mode.@br
 @par value An ID.
 @version 0.3.0
*/
public func SetProjectileID(id value)
{
	return this.projectile_id = value;
}

/**
 Set the projectile speed of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetProjectileSpeed(int value)
{
	return this.projectile_speed = value;
}

/**
 Set the projectile range of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetProjectileRange(int value)
{
	return this.projectile_range = value;
}

/**
 Set the projectile distance of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetProjectileDistance(int value)
{
	return this.projectile_distance = value;
}

/**
 Set the Y offset of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetYOffset(int value)
{
	return this.projectile_offset_y = value;
}

/**
 Set the spread of this fire mode.@br
 @par value An array of two integers, spread and precision.
 @version 0.3.0
*/
public func SetSpread(int value)
{
	return this.spread = value;
}

/**
 Set the amount of projectiles firing in a burst of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetBurstAmount(int value)
{
	return this.burst = value;
}

/**
 Set the automatic reload setting of this fire mode.@br
 @par value A boolean.
 @version 0.3.0
*/
public func SetAutoReload(bool value)
{
	return this.auto_reload = value;
}

/**
 Set the shooting animation name of this fire mode.@br
 @par value A string.
 @version 0.3.0
*/
public func SetShootingAnimation(string value)
{
	return this.anim_shoot_name = value;
}

/**
 Set the loading animation name of this fire mode.@br
 @par value A string.
 @version 0.3.0
*/
public func SetReloadAnimation(string value)
{
	return this.anim_load_name = value;
}

/**
 Set the forward walking speed of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetForwardWalkingSpeed(int value)
{
	return this.walk_speed_front = value;
}

/**
 Set the backward walking speed of this fire mode.@br
 @par value An integer.
 @version 0.3.0
*/
public func SetBackwardWalkingSpeed(int value)
{
	return this.walk_speed_back = value;
}