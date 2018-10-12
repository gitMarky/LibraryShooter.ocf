

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
				Log("Reload: Start from manual, because no bullet chambered");
				return "Reload_Magazine_LoadAmmoChamber";
			}
			else if (ammo >= extended_amount)
			{
				Log("Reload: Do nothing, because ammo amount %d >= %d", ammo, extended_amount);
				return nil;
			}
		}
		else
		{
			Log("Reload: Do nothing, because ammo amount %d >= %d", ammo, firemode->GetAmmoAmount());
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
		AbortAction = "Reload_Magazine_Insert", // FIXME: Violates design guideline
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
		AbortAction = "Idle",  // FIXME: Violates design guideline
	},

	/* --- Support adding spare ammo back to the user --- */

	Magazine_StashStart = // Take out a partially filled magazine and stash it
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Magazine_StashStart",
		NextAction = "Reload_Magazine_StashFinish",
		AbortAction = "Reload_Magazine_Insert", // FIXME: Violates design guideline
		StartCall  = "~PlaySoundEjectMagazine",
	},


	Magazine_StashFinish = // Short delay and sound while stashing the magazine, merely cosmetic
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Magazine_StashFinish",
		NextAction  = "Reload_Magazine_Insert",
		AbortAction = "Reload_Magazine_Insert", // FIXME: Violates design guideline
		StartCall   = "~PlaySoundResupplyAmmo",
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
func Reload_Magazine_Prepare_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Grab mag] - Start");
}

func Reload_Magazine_Prepare_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Grab mag] - Finish");
}

func Reload_Magazine_Prepare_NextAction(object user, int x, int y, proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo > this->~AmmoChamberCapacity(ammo_type))
	{
		return "Reload_Magazine_StashStart";
	}
	else
	{
		return "Reload_Magazine_Drop";
	}
}

func Reload_Magazine_Prepare_OnAbort(object user, int x, int y, proplist firemode)
{
	// Repeat the same action
	Log("Reload [Grab mag] - Cancel");
}

//---------------------------------------------------------------------------------
func Reload_Magazine_Drop_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, drop it] - Start");
	
	// Lose current ammo
	this->ReloadRemoveAmmo(firemode, false);
}

func Reload_Magazine_Drop_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, drop it] - Finish");
}

func Reload_Magazine_Drop_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, drop it] - Cancel");
}

//---------------------------------------------------------------------------------
func Reload_Magazine_Insert_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag insert] - Start");
}

func Reload_Magazine_Insert_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag insert] - Finish");
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

func Reload_Magazine_Insert_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag insert] - Cancel");
	
	// Stay in the same state, be fair and keep magazine ;)
}

/* --- Support adding spare ammo back to the user --- */

//---------------------------------------------------------------------------------
func Reload_Magazine_StashStart_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, stash it] - Start");

	// Take out ammo now, because the previous version where ammo state is changed only on finish looked strange ingame
	SetTemporaryAmmo(firemode->GetAmmoID(), this->ReloadRemoveAmmo(firemode, false));
}

func Reload_Magazine_StashStart_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, stash it] - Finish");

	// Fill ammo belt of the user
	var ammo_type = firemode->GetAmmoID();
	this->GetAmmoReloadContainer()->DoAmmo(ammo_type, GetTemporaryAmmo(ammo_type));
	SetTemporaryAmmo(ammo_type, 0);
}

func Reload_Magazine_StashStart_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, stash it] - Cancel");
}

func SetTemporaryAmmo(id ammo_type, int amount) // FIXME: Globalize
{
	if (!this.temp_ammo) this.temp_ammo = {};
	this.temp_ammo[Format("%i", ammo_type)] = amount;
}

func GetTemporaryAmmo(id ammo_type) // FIXME: Globalize
{
	return this.temp_ammo[Format("%i", ammo_type)];
}

//---------------------------------------------------------------------------------
func Reload_Magazine_StashFinish_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Stashing] - Start");
}

func Reload_Magazine_StashFinish_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Stashing] - Finish");
}

func Reload_Magazine_StashFinish_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Stashing] - Cancel");
}

/* --- Support for an extra ammo chamber --- */

//---------------------------------------------------------------------------------
func Reload_Magazine_LoadAmmoChamber_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Manual load] - Start");
}

func Reload_Magazine_LoadAmmoChamber_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Manual load] - Finish");
	this->~AmmoChamberInsert(firemode->GetAmmoID());
}

func Reload_Magazine_LoadAmmoChamber_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Manual load] - Cancel");
}
