
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
				return "Reload_Container_LoadAmmoChamber";
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
		EndCall     = "~PlaySoundResupplyAmmo",
	},

	Container_StashFinish = // Short delay while stashing the magazine, merely cosmetic
	{
		Prototype   = Firearm_ReloadState,
		Name        = "Reload_Container_StashFinish",
		NextAction  = "Reload_Container_InsertAmmo",
		AbortAction = "Reload_Container_InsertAmmo", // This serves as a delay only, so if it gets aborted no harm is done
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
func Reload_Container_EjectAmmo_OnEnd(object user, int x, int y, proplist firemode)
{
	this->~Reload_Container_EjectCasings(user, firemode);
}

func Reload_Container_EjectAmmo_NextAction(object user, int x, int y, proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();

	if (this->GetAmmo(ammo_type) > this->~AmmoChamberCapacity(ammo_type)) // FIXME: Is bugged if the chamber is not loaded, but there is ammo left. Needs a separate function
	{
		// Take out ammo now, because the previous version where ammo state is changed only on finish looked strange ingame
		this->DoTemporaryAmmo(firemode->GetAmmoID(), this->ReloadRemoveAmmo(firemode, false));
		return "Reload_Container_StashStart";
	}
	else
	{
		return "Reload_Container_InsertAmmo";
	}
}

//---------------------------------------------------------------------------------
func Reload_Container_InsertAmmo_OnEnd(object user, int x, int y, proplist firemode)
{
	this->ReloadRefillAmmo(firemode);
}

//---------------------------------------------------------------------------------
func Reload_Container_Close_NextAction(object user, int x, int y, proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
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

/* --- Support adding spare ammo back to the user --- */

//---------------------------------------------------------------------------------
func Reload_Container_StashStart_OnEnd(object user, int x, int y, proplist firemode)
{
	// Fill ammo belt of the user
	var ammo_type = firemode->GetAmmoID();
	this->GetAmmoReloadContainer()->DoAmmo(ammo_type, this->GetTemporaryAmmo(ammo_type));
	this->SetTemporaryAmmo(ammo_type, 0);
}

/* --- Support for an extra ammo chamber --- */

//---------------------------------------------------------------------------------
func Reload_Container_LoadAmmoChamber_OnEnd(object user, int x, int y, proplist firemode)
{
	this->~AmmoChamberInsert(firemode->GetAmmoID());
}
