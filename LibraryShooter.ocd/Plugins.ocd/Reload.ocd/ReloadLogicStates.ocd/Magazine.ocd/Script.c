

/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);

	// Special situations?
	if (ammo >= firemode->GetAmmoAmount())
	{
		if (this->~AmmoChamberCapacity(ammo_type))
		{
			var extended_amount = firemode->GetAmmoAmount() + this->AmmoChamberCapacity(ammo_type);
			if (!this->~AmmoChamberIsLoaded(ammo_type))
			{
				return "Reload_Magazine_LoadAmmoChamber";
			}
			else if (ammo >= extended_amount)
			{
				return nil;
			}
		}
		else
		{
			return nil;
		}
	}

	// Default
	return "Reload_Magazine_Prepare";
}

local ReloadStateMap = 
{
	/* --- Default sequence --- */


	Magazine_Prepare = // Grab the magazine that is currently in the weapon
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Magazine_Prepare",
		NextAction = "#Reload_Magazine_Prepare_NextAction",
	},

	Magazine_Drop = // Drop an empty mag to the ground
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Magazine_Drop",
		NextAction  = "Reload_Magazine_Insert",
		StartCall   = "~PlaySoundEjectMagazine",
	},

	Magazine_Insert = // Insert a new mag into the weapon
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Magazine_Insert",
		NextAction = "#Reload_Magazine_Insert_NextAction",
		EndCall    = "~PlaySoundInsertMagazine",
	},

	Magazine_ReadyWeapon = // Bring the weapon to ready stance
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Magazine_ReadyWeapon",
		NextAction  = "Idle",
	},

	/* --- Support adding spare ammo back to the user --- */

	Magazine_StashStart = // Take out a partially filled magazine and stash it
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Magazine_StashStart",
		NextAction = "Reload_Magazine_StashFinish",
		StartCall  = "~PlaySoundEjectMagazine",
		EndCall    = "~PlaySoundResupplyAmmo",
	},

	Magazine_StashFinish = // Short delay while stashing the magazine, merely cosmetic
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Magazine_StashFinish",
		NextAction  = "Reload_Magazine_Insert",
		AbortAction = "Reload_Magazine_Insert", // This serves as a delay only, so if it gets aborted no harm is done
	},

	/* --- Support for an extra ammo chamber --- */

	Magazine_LoadAmmoChamber = // Manually load a new bullet to the chamber
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Magazine_LoadAmmoChamber",
		NextAction = "Reload_Magazine_ReadyWeapon",
		EndCall    = "~PlaySoundChamberBullet",
	},
};


//---------------------------------------------------------------------------------
func Reload_Magazine_Prepare_NextAction(object user, int x, int y, proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo > this->~AmmoChamberCapacity(ammo_type))
	{
		// Take out ammo now, because the previous version where ammo state is changed only on finish looked strange ingame
		this->DoTemporaryAmmo(firemode->GetAmmoID(), this->ReloadRemoveAmmo(firemode, false));
		return "Reload_Magazine_StashStart";
	}
	else
	{
		return "Reload_Magazine_Drop";
	}
}

//---------------------------------------------------------------------------------
func Reload_Magazine_Drop_OnEnd(object user, int x, int y, proplist firemode)
{
	// Lose current ammo
	this->ReloadRemoveAmmo(firemode, false);
}

//---------------------------------------------------------------------------------
func Reload_Magazine_Insert_OnEnd(object user, int x, int y, proplist firemode)
{
	this->ReloadRefillAmmo(firemode);
}

func Reload_Magazine_Insert_NextAction(object user, int x, int y, proplist firemode)
{
	// Load a bullet now?
	var ammo_type = firemode->GetAmmoID();
	if (this->~AmmoChamberCapacity(ammo_type)
	&& !this->~AmmoChamberIsLoaded(ammo_type))
	{
		return "Reload_Magazine_LoadAmmoChamber";
	}
	else
	{
		return "Reload_Magazine_ReadyWeapon";
	}
}


/* --- Support adding spare ammo back to the user --- */

//---------------------------------------------------------------------------------
func Reload_Magazine_StashStart_OnEnd(object user, int x, int y, proplist firemode)
{
	// Fill ammo belt of the user
	var ammo_type = firemode->GetAmmoID();
	this->GetAmmoReloadContainer()->DoAmmo(ammo_type, this->GetTemporaryAmmo(ammo_type));
	this->SetTemporaryAmmo(ammo_type, 0);
}


/* --- Support for an extra ammo chamber --- */

//---------------------------------------------------------------------------------
func Reload_Magazine_LoadAmmoChamber_OnEnd(object user, int x, int y, proplist firemode)
{
	this->~AmmoChamberInsert(firemode->GetAmmoID());
}
