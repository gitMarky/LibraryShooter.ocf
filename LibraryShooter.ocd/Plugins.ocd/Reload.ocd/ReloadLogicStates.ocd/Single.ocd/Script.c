
/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (ammo >= firemode->GetAmmoAmount())
	{
		if (this->~AmmoChamberCapacity(ammo_type)
		&& !this->~AmmoChamberIsLoaded(ammo_type))
		{
			return "Reload_Single_LoadAmmoChamber";
		}
		else
		{
			return nil;
		}
	}
	else
	{
		return "Reload_Single_Prepare";
	}
}

local ReloadStateMap = 
{
	/* --- Default sequence --- */

	Single_Prepare = // Get ready to reload
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Single_Prepare",
		NextAction = "#Reload_Single_Prepare_NextAction",
		StartCall  = "~PlaySoundOpenAmmoContainer",
	},

	Single_InsertAmmo = // Insert a single shell into the tube
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Single_InsertAmmo",
		NextAction = "#Reload_Single_InsertAmmo_NextAction",
	},

	Single_ReadyWeapon = // Bring the weapon to ready stance
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Single_ReadyWeapon",
		StartCall  = "~PlaySoundCloseAmmoContainer",
		NextAction = "Idle",
	},

	/* --- Support for an extra ammo chamber --- */

	Single_LoadAmmoChamber = // Manually load a new shell to the chamber (open and close in one)
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Single_LoadAmmoChamber",
		NextAction = "Reload_Single_ReadyWeapon",
		EndCall    = "~PlaySoundLoadAmmoChamber",
	},

	Single_OpenAmmoChamber = // Open the chamber, for manually inserting a shell 
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Single_OpenAmmoChamber",
		NextAction = "Reload_Single_InsertAmmo",
		StartCall  = "~PlaySoundOpenAmmoChamber",
		EndFunc    = "OpenChamber",
	},

	Single_CloseAmmoChamber = // Close the chamber, after inserting a single shell
	{
		Prototype  = Firearm_ReloadState,
		Name       = "Reload_Single_CloseAmmoChamber",
		NextAction = "#CloseChamber",
		StartCall  = "~PlaySoundCloseAmmoChamber",
	},
};

//---------------------------------------------------------------------------------
func Reload_Single_Prepare_OnEnd(object user, int x, int y, proplist firemode)
{
	this->~Reload_Single_EjectCasings(user, firemode);
}

func Reload_Single_Prepare_NextAction(object user, int x, int y, proplist firemode)
{
	if (this->~AmmoChamberCapacity(firemode->GetAmmoID())
	&& !this->~AmmoChamberIsLoaded(firemode->GetAmmoID()))
	{
		return "Reload_Single_OpenAmmoChamber";
	}
	else
	{
		return "Reload_Single_InsertAmmo";
	}
}

//---------------------------------------------------------------------------------
func Reload_Single_InsertAmmo_OnStart(object user, int x, int y, proplist firemode)
{
	// Do everything at the beginning here and count the next rest of the process
	// as a delay for getting the next shell ready - if that fails, start from
	// the beginning
	var is_done = false;
	var source = this->GetAmmoReloadContainer();
	if (source)
	{
		var info = this->ReloadGetAmmoInfo(firemode);
		var ammo_requested = BoundBy(info.ammo_max + info.ammo_chambered - info.ammo_available, 0, firemode.ammo_usage ?? 1);
		var ammo_received = Abs(source->DoAmmo(firemode->GetAmmoID(), -ammo_requested)); // see how much you can get
		var ammo_spare = (info.ammo_available + ammo_received) % (firemode.ammo_usage ?? 1); // get ammo only in increments of ammo_usage

		source->DoAmmo(info.ammo_type, ammo_spare); // give back the unnecessary ammo
		if (ammo_received > 0)
		{
			this->~PlaySoundInsertShell();
			this->DoAmmo(info.ammo_type, ammo_received);
		}
		else
		{
			is_done = true;
		}

		// Finish condition?
		var is_full = this->GetAmmo(info.ammo_type) >= (info.ammo_max + info.ammo_chambered);
		var no_ammo = source->GetAmmo(info.ammo_type) == 0;
		if (is_full || no_ammo)
		{
			is_done = true;
		}			
	}
	else
	{
		is_done = true;
	}

	this.firearm_reload.data_store.single_insert_ammo_is_done = is_done;
}

func Reload_Single_InsertAmmo_NextAction(object user, int x, int y, proplist firemode)
{
	// Finish condition?		
	if (this.firearm_reload.data_store.single_insert_ammo_do_chamber_bullet)
	{
		this.firearm_reload.data_store.single_insert_ammo_do_chamber_bullet = false;
		return "Reload_Single_CloseAmmoChamber";
	}
	else if (this.firearm_reload.data_store.single_insert_ammo_is_done)
	{
		if (this->~AmmoChamberCapacity(firemode->GetAmmoID())
		&& !this->~AmmoChamberIsLoaded(firemode->GetAmmoID()))
		{
			return "Reload_Single_LoadAmmoChamber";
		}
		else
		{
			return "Reload_Single_ReadyWeapon";
		}
	}
}


/* --- Support for an extra ammo chamber --- */

//---------------------------------------------------------------------------------
func Reload_Single_LoadAmmoChamber_OnEnd(object user, int x, int y, proplist firemode)
{
	this->~AmmoChamberInsert(firemode->GetAmmoID());
}

//---------------------------------------------------------------------------------
func OpenChamber(object user, int x, int y, proplist firemode)
{
	this.firearm_reload.data_store.single_insert_ammo_do_chamber_bullet = true;
}


//---------------------------------------------------------------------------------
func Reload_Single_CloseAmmoChamber_OnStart(object user, int x, int y, proplist firemode)
{
	this->~AmmoChamberInsert(firemode->GetAmmoID());
}


func CloseChamber(object user, int x, int y, proplist firemode)
{
	var source = this->GetAmmoReloadContainer();
	var ammo_requested = 0;
	if (source)
	{
		var info = this->ReloadGetAmmoInfo(firemode);
		ammo_requested = BoundBy(info.ammo_max + info.ammo_chambered - info.ammo_available, 0, firemode.ammo_usage ?? 1);
	}

	if (ammo_requested > 0)
	{
		return "Reload_Single_InsertAmmo";
	}
	else
	{
		return "Reload_Single_ReadyWeapon";
	}
}
