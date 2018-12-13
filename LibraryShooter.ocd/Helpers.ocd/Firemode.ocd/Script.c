﻿/**
	A dummy definition, used to provide various getters and setters for fire modes.
	Keep in mind that for fire modes to be writable, they must be created using {@link Library_Firearm#AddFiremode}.

	@note
	A fire mode is a proplist that can define the following properties:@br
	mode: Integer. Must be set. This defines the basic firing mode. Can simply be one of the following constants:@br
	- WEAPON_FM_Single: single shot style, only shot per click is fired (default).@br
	- WEAPON_FM_Burst: burst style, firing a set number of shot in short succession.@br
	- WEAPON_FM_Auto: auto style, firing as long as the use button is pressed.@br
	name: A string containing the name of this fire mode. Unnecessary if no GUI exists that displays the name (default: Standard).@br
	icon: ID of a definition icon for the fire mode. Unnecessary if no GUI exists that displays the icon (default: nil).@br
	condition: A string corresponding to a function name. The fire mode will not be marked as 'available' unless the condition functions return true. Example: An upgraded weapon could offer more fire modes (default: nil).@br
	ammo_id: A definition that represents ammunition for the fire mode (default: nil).@br
	ammo_usage: Integer. How much ammunition is needed per ammo_rate shots (default: 1).@br
	ammo_rate: Integer. See ammo_usage (default: 1). As ammo handling is not part the library, this has to be implemented (or include {@link Library_Firearm_AmmoLogic}).@br
	delay_charge: Integer. Charge duration in frames. If 0 or nil, no charge is required (default: 0).@br
	delay_recover: Integer. Recovery duration in frames. If 0 or nil, no recovery is required (default: 1).@br
	delay_cooldown: Integer. Cooldown duration in frames. If 0 or nil, no cooldown is required (default: 0).@br
	delay_reload: Integer. Reload duration in frames. If 0 or nil, reloading is instantaneous (default: 0).@br
	damage: Integer. Amount of damage a projectile does (default: 10).@br
	damage_type: Integer. Defining a damage type. Damage type handling is not done by this library and should be handled by any implementation (default: nil).@br
	projectile_id: A definition of the actual projectile that is being fired. These are created on the fly and must therefore not be created beforehand (default: NormalBullet).@br
	projectile_speed: Integer. Firing speed of a projectile (default: 100).@br
	projectile_range: Integer. Maximum range a projectile flies (default: 600).@br
	projectile_distance: Integer. Distance the projectile is being created away from the shooting object (default: 10).@br
	projectile_offset_y: Integer. Y offset when creating a projectile in case the barrel of the gun is not perfectly aligned to the firing object's center (default: -6).@br
	projectile_number: Integer. How many projectiles are fired in a single shot (default: 1).@br
	projectile_spread: Integer. Deviation of the projectiles. Default: 0@br
	spread: Integer. Deviation of the fire mode. Default: 0@br
	spread_per_shot: Integer. Deviation that is added to the user with each shot.@br
	burst: Integer. Number of shots being fired when using burst mode style (default: 0).@br
	auto_reload: Boolean. If true, the weapon reloads even if the use button is not held (default: false).@br
	anim_shoot_name: A string containing the animation name that is returned for the animation set (usually when being used by a Clonk) as general aim animation (default: nil).@br
	anim_load_name: A string containing the animation name that is returned for the animation set (usually when being used by a Clonk) as general reload animation (default: nil).@br
	walk_speed_front: Integer. Forwards walking speed to be returned for the animation set (usually when being used by a Clonk) (default: nil).@br
	walk_speed_back: Integer. Backwards walking speed to be returned for the animation set (usually when being used by a Clonk) (default: nil).@br

	@author Clonkonaut
*/


/* --- Getters --- */

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
	Get the index of this fire mode, for {@link Library_Firemode#GetFiremode()}.

	@return int The fire mode index.
*/
public func GetIndex()
{
	return this.index;
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
	Get the ammunition maximum of this fire mode.

	@return An integer.
*/
public func GetAmmoAmount()
{
	return this.ammo_load;
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
	Gets the number of projectiles to be fired by a single shot.

	@return int The number of projectiles per shot.
*/
func GetProjectileAmount()
{
	return this.projectile_number;
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
	This spread value is static and defines how inaccurate
	the projectiles are by themselves.

	@return int The spread {@code x} in millidegrees, will be added to the
	            as a cone {@code +/- x} to the aiming angle.
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

	This spread value is static and defines how inaccurate
	the weapon or firemode is by itself in this fire mode.

	@return int The spread {@code x} in millidegrees, will be added to the
	            as a cone {@code +/- x} to the aiming angle.
*/
public func GetSpread()
{
	return this.spread;
}


/**
	Get the spread that is passed to the user after
	each shot.

	This spread value is static and defines how inaccurate
	the weapon or firemode is by itself in this fire mode.

	@note This depends entirely on how the user handles
	      spread. Can be handley for example by
	      including {@link Plugin_Firearm_DynamicSpread}
	      in the user.

	@return int The spread {@code x} in millidegrees, will be added to the
	            as a cone {@code +/- x} to the aiming angle.
*/
public func GetSpreadPerShot()
{
	return this.spread_per_shot;
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


/* --- Setters --- */

/**
	Set the mode (firing style) of this fire mode.

	@par value An integer (see {@link Library_Firearm} fire modes description). Can simply be one of the following constants:
	<ul>
		<li>WEAPON_FM_Single: single shot style, only shot per click is fired (default).</li>
		<li>WEAPON_FM_Burst: burst style, firing a set number of shot in short succession.</li>
		<li>WEAPON_FM_Auto: auto style, firing as long as the use button is pressed.</li>
	</ul>

	@return proplist Returns the fire mode, so that further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetIcon(id value)
{
	this.icon = value;
	return this;
}


/**
	Set the index of this fire mode, for {@link Library_Firearm#GetFiremode}.

	@note This is automatically called by {@link Library_Firearm#AddFiremode}.

	@par value The index of the firemode.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetIndex(int value)
{
	this.index = value;
	return this;
}


/**
	Set the condition callback of this fire mode.

	@par value A string corresponding to a function name, or a function pointer.
               The fire mode will not be marked as 'available' unless the condition functions return true. 
               Example: An upgraded weapon could offer more fire modes.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetCondition(value)
{
	this.condition = value;
	return this;
}


/**
	Set the ammunition ID of this fire mode.

	@par value A definition that represents ammunition for the fire mode.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetAmmoID(id value)
{
	this.ammo_id = value;
	return this;
}


/**
	Set the ammunition maximum of this fire mode.

	@par value This fire mode can have this many ammunition.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetAmmoAmount(int value)
{
	this.ammo_load = value;
	return this;
}


/**
	Set the ammunition usage of this fire mode.

	@par value How much ammunition is needed per {@link Library_Firearm_Firemode#GetAmmoRate} shots.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetAmmoUsage(int value)
{
	this.ammo_usage = value;
	return this;
}


/**
	Set the ammunition rate of this fire mode.

	@par value An integer. The firearm can fire this many shots after
	           using up {@link Library_Firearm_Firemode#GetAmmoUsage}
	           ammunition.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

    @note Use this to model a (semi-)automatic loading cycle of the
          weapon. The recovery from recoil, etc. should be modeled
          via {@link Library_Firearm_Firemode#SetCooldownDelay} 

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

    @note Use this to model recoil, letting a gatling gun roll out, etc.
          Time for loading a new shot should be modeled
          via {@link Library_Firearm_Firemode#SetRecoveryDelay} 

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetCooldownDelay(int value)
{
	this.delay_cooldown = value;
	return this;
}


/**
	Set the reloading delay of this fire mode.

	@par value Reload duration in frames. If 0 or nil, reloading is instantaneous.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetReloadDelay(int value)
{
	this.delay_reload = value;
	return this;
}


/**
	Set the damage of this fire mode.

	@par value The amount of damage that a projectile does.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetDamageType(int value)
{
	this.damage_type = value;
	return this;
}


/**
	Sets the number of projectiles to be fired by a single shot.

	@par value The number of projectiles per shot.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
func SetProjectileAmount(int value)
{
	this.projectile_number = value;
	return this;
}


/**
	Set the projectile ID of this fire mode.

	@par value A definition of the actual projectile that is being fired.
               These are created on the fly and must therefore not be created beforehand.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetProjectileID(id value)
{
	this.projectile_id = value;
	return this;
}


/**
	Set the projectile speed of this fire mode.

	@par value Firing speed of a projectile. May
	           be an integer, or an array for use with
	           {@link Library_Random#SampleValue}.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetProjectileSpeed(value)
{
	if (GetType(value) == C4V_Int
	 || GetType(value) == C4V_Array)
	{
		this.projectile_speed = value;
		return this;
	}
	else
	{
		FatalError(Format("You have to pass an array or int, got %v", GetType(value)));
	}
}


/**
	Set the spread of the projectiles for this fire mode. Refers to 
    the basic inaccuracy of the projectiles.

	@par value The spread {@code x} in millidegrees, will be added to the
	           as a cone {@code +/- x} to the aiming angle.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetProjectileSpread(int value)
{
	this.projectile_spread = value;
	return this;
}


/**
	Set the projectile range of this fire mode.

	@par value Maximum range a projectile flies. May
	           be an integer, or an array for use with
	           {@link Library_Random#SampleValue}.


	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetProjectileRange(value)
{
	if (GetType(value) == C4V_Int
	 || GetType(value) == C4V_Array)
	{
		this.projectile_range = value;
		return this;
	}
	else
	{
		FatalError(Format("You have to pass an array or int, got %v", GetType(value)));
	}
}


/**
	Set the projectile distance of this fire mode.

	@par value Distance the projectile is being created away from the shooting object.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetYOffset(int value)
{
	this.projectile_offset_y = value;
	return this;
}


/**
	Set the spread of this fire mode. Refers to 
    additional deviation added by certain effects (e.g. continuous firing).

	@par value The spread {@code x} in millidegrees, will be added to the
	           as a cone {@code +/- x} to the aiming angle.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetSpread(int value)
{
	this.spread = value;
	return this;
}


/**
	Set the spread that is passed to the user after
	each shot.

	@note This depends entirely on how the user handles
	      spread. Can be handley for example by
	      including {@link Plugin_Firearm_DynamicSpread}
	      in the user.

	@par value The spread {@code x} in millidegrees, will be added to the
	           as a cone {@code +/- x} to the aiming angle.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetSpreadPerShot(int value)
{
	this.spread_per_shot = value;
	return this;
}


/**
	Set the amount of shots being fired in a burst of this fire mode.

	@par value An integer.

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
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

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetForwardWalkingSpeed(int value)
{
	this.walk_speed_front = value;
}


/**
	Set the backward walking speed of this fire mode.

	@par value Backwards walking speed to be returned for
               animation set (usually when being used by a Clonk).

	@return proplist Returns the fire mode, so that 
	                 further function calls can be issued.
*/
public func SetBackwardWalkingSpeed(int value)
{
	this.walk_speed_back = value;
	return this;
}

/* --- Misc --- */


/**
	Find out whether the firemode is available.

	@return {@code true} if the firemode has no condition,
	        or the return value of the function defined in
	        {@link Library_Firearm_Firemode#GetCondition}.
 */
public func IsAvailable()
{
	return this.condition == nil || this->Call(this->GetCondition());
}
