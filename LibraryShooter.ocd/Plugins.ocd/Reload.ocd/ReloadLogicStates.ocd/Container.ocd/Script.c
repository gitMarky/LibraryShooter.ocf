
static const RELOAD_CONTAINER_PREPARE = "Reload_Container_Prepare";

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
				return "Reload_Container_LoadAmmoChamber";
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
	return RELOAD_CONTAINER_PREPARE;
}

local ReloadStateMap = 
{
	/* --- Default sequence --- */

	Container_Prepare = // Get ready to reload
	{
		Prototype  = Firearm_ReloadState,
		Name       = RELOAD_CONTAINER_PREPARE,
		NextAction = "Reload_Container_EjectAmmo",
		StartCall  = "~PlaySoundOpenAmmoContainer",
	},
	
	Container_EjectAmmo = // Remove an ammo container from the weapon
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Container_EjectAmmo",
		NextAction = "#Reload_Container_EjectAmmo_NextAction", // Evaluate function
		EndCall    = "~PlaySoundEjectAmmo",
	},
	
	Container_InsertAmmo = // Insert an ammo container into the weapon
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Container_InsertAmmo",
		NextAction = "Reload_Container_Close",
		EndCall    = "~PlaySoundInsertAmmo",
	},
	
	Container_Close = 
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Container_Close",
		NextAction = "#Reload_Container_Close_NextAction", // Evaluate the function
		StartCall  = "~PlaySoundCloseAmmoContainer",
	},
	
	Container_ReadyWeapon = // Bring the weapon to ready stance
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Container_ReadyWeapon",
		NextAction = "Idle",
		StartCall  = "~PlaySoundCloseAmmoContainer",
	},
	
	/* --- Support adding spare ammo back to the user --- */
	
	Container_StashStart = // Take out a partially filled magazine and stash it
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Container_StashStart",
		NextAction  = "Reload_Container_StashFinish",
		AbortAction = "Reload_Container_InsertAmmo", // FIXME: Violates design guideline
	},
	
	Container_StashFinish = // Short delay and sound while stashing the magazine, merely cosmetic
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Container_StashFinish",
		NextAction  = "Reload_Container_InsertAmmo",
		AbortAction = "Reload_Container_InsertAmmo", // FIXME: Violates design guideline
		StartCall   = "~PlaySoundResupplyAmmo",
	},
	
	/* --- Support for an extra ammo chamber --- */
	
	Container_LoadAmmoChamber = // Manually load a new shell to the chamber (open and close in one)
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Container_LoadAmmoChamber",
		NextAction = "Reload_Container_ReadyWeapon",
		EndCall    = "~PlaySoundLoadAmmoChamber",
	},
};

//---------------------------------------------------------------------------------
func Reload_Container_Prepare_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Prepare] - Start");
}

func Reload_Container_Prepare_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Prepare] - Finish");
}

func Reload_Container_Prepare_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Prepare] - Cancel");
}


//---------------------------------------------------------------------------------
func Reload_Container_EjectAmmo_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Ammo eject] - Start");
}

func Reload_Container_EjectAmmo_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Ammo eject] - Finish");
	this->~Reload_Container_EjectCasings(user, firemode);
}
	
func Reload_Container_EjectAmmo_NextAction(object user, int x, int y, proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	
	if (this->GetAmmo(ammo_type) > this->~AmmoChamberCapacity(ammo_type))
	{
		return "Reload_Container_StashStart";
	}
	else
	{
		return "Reload_Container_InsertAmmo";
	}
}

func Reload_Container_EjectAmmo_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Ammo eject] - Cancel");
	// Stay in the same state, be fair and keep magazine ;)
}

//---------------------------------------------------------------------------------
func Reload_Container_InsertAmmo_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Ammo insert] - Start");
}

func Reload_Container_InsertAmmo_OnEnd(object user, int x, int y, proplist firemode)
{
	this->ReloadRefillAmmo(firemode);
}

func Reload_Container_InsertAmmo_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Ammo insert] - Cancel");
	// Stay in the same state, be fair and keep magazine ;)
}

//---------------------------------------------------------------------------------
func Reload_Container_Close_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Close] - Start");
}

func Reload_Container_Close_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Close] - Finish");
}

func Reload_Container_Close_NextAction(object user, int x, int y, proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	Log("Reload [Close] - Capacity: %d, loaded: %v, ammo %d %i", this->~AmmoChamberCapacity(ammo_type), this->~AmmoChamberIsLoaded(ammo_type), this->~GetAmmo(ammo_type), ammo_type);
	if (this->~AmmoChamberCapacity(ammo_type)
	&& !this->~AmmoChamberIsLoaded(ammo_type))
	{
		return "Reload_Container_LoadAmmoChamber";
	}
	else
	{
		return "Reload_Container_ReadyWeapon";
	}
}

func Reload_Container_Close_OnAbort(object user, int x, int y, proplist firemode)
{
	// Repeat the same action
	Log("Reload [Close] - Cancel");
}

/* --- Support adding spare ammo back to the user --- */

//---------------------------------------------------------------------------------
func Reload_Container_StashStart_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, stash it] - Start");
	// Take out ammo now, because the previous version where ammo state is changed only on finish looked strange ingame
	this->SetTemporaryAmmo(firemode->GetAmmoID(), this->ReloadRemoveAmmo(firemode, false));
}

func Reload_Container_StashStart_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, stash it] - Finish");

	// Fill ammo belt of the user
	var ammo_type = firemode->GetAmmoID();
	this->GetAmmoReloadContainer()->DoAmmo(ammo_type, this->GetTemporaryAmmo(ammo_type));
	this->SetTemporaryAmmo(ammo_type, 0);
}

func Reload_Container_StashStart_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Mag out, stash it] - Cancel");
}

//---------------------------------------------------------------------------------
func Reload_Container_StashFinish_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Stashing] - Start");
}

func Reload_Container_StashFinish_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Stashing] - Finish");
}

func Reload_Container_StashFinish_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Stashing] - Cancel");
}

/* --- Support for an extra ammo chamber --- */

//---------------------------------------------------------------------------------
func Reload_Container_LoadAmmoChamber_OnStart(object user, int x, int y, proplist firemode)
{
	Log("Reload [Manual load] - Start");
}

func Reload_Container_LoadAmmoChamber_OnEnd(object user, int x, int y, proplist firemode)
{
	Log("Reload [Manual load] - Finish");
	this->~AmmoChamberInsert(firemode->GetAmmoID());
}

func Reload_Container_LoadAmmoChamber_OnAbort(object user, int x, int y, proplist firemode)
{
	Log("Reload [Manual load] - Cancel");
}
