
public func Initialize()
{
	_inherited(...);
	// Reloading, the state saves some custom infos
	this.Reload_Magazine_StashStart = new Reload_Magazine_StashStart{};
}


/* --- Reload animations --- */

// 	Gets the default reload state that the weapon starts reloading from.
func GetReloadStartState(proplist firemode)
{
	var ammo_type = firemode->GetAmmoID();
	var ammo = this->GetAmmo(ammo_type);
	if (this->~AmmoChamberCapacity(ammo_type)
	&& !this->~AmmoChamberIsLoaded(ammo_type))
	{
		if (ammo >= firemode->GetAmmoAmount())
		{
			Log("Reload: Start from manual, because no bullet chambered");
			return Reload_Magazine_LoadAmmoChamber;
		}
		else
		{
			return nil;
		}
	}
	else
	{
		return Reload_Magazine_Grab;
	}
}

// Grab the magazine that is currently in the weapon
local Reload_Magazine_Grab = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Grab mag] - Start");
	},

	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Grab mag] - Finish");
		var ammo_type = firemode->GetAmmoID();
		var ammo = firearm->GetAmmo(ammo_type);
		if (ammo > firearm->~AmmoChamberCapacity(ammo_type))
		{
			firearm->SetReloadState(firemode, firearm.Reload_Magazine_StashStart);
		}
		else
		{
			firearm->SetReloadState(firemode, firearm.Reload_Magazine_Drop);
		}
	},

	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		// Repeat the same action
		Log("Reload [Grab mag] - Cancel");
	},
};

// Drop an empty mag to the ground
local Reload_Magazine_Drop = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, drop it] - Start");
		firearm->~PlaySoundEjectMagazine();
		
		// Lose current ammo
		firearm->ReloadRemoveAmmo(firemode, false);
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, drop it] - Finish");
		
		// Put a magazine in next
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_Insert);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, drop it] - Cancel");
		
		// Put a magazine in next
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_Insert);
	},
};

// Insert a new mag into the weapon
local Reload_Magazine_Insert = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Finish");
		firearm->~PlaySoundInsertMagazine();
		firearm->ReloadRefillAmmo(firemode);
		
		// Load a bullet now?
		var ammo_type = firemode->GetAmmoID();
		if (this->~AmmoChamberCapacity(ammo_type)
		&& !this->~AmmoChamberIsLoaded(ammo_type))
		{
			firearm->SetReloadState(firemode, firearm.Reload_Magazine_LoadAmmoChamber);
		}
		else
		{
			firearm->SetReloadState(firemode, firearm.Reload_Magazine_ReadyWeapon);
		}
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag insert] - Cancel");
		
		// Stay in the same state, be fair and keep magazine ;)
	},
};

// Bring the weapon to ready stance
local Reload_Magazine_ReadyWeapon = new Firearm_ReloadState
{
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		firearm->SetReloadState(nil); // Done!
	},
};


/* --- Support adding spare ammo back to the user --- */

// Take out a partially filled magazine and stash it
local Reload_Magazine_StashStart = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Start");
		firearm->~PlaySoundEjectMagazine();
		// Take out ammo now, because the previous version where ammo state is changed only on finish looked strange ingame
		SetTemporaryAmmo(firemode->GetAmmoID(), firearm->ReloadRemoveAmmo(firemode, false));
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Finish");

		// Fill ammo belt of the user
		var ammo_type = firemode->GetAmmoID();
		firearm->GetAmmoReloadContainer()->DoAmmo(ammo_type, GetTemporaryAmmo(ammo_type));
		SetTemporaryAmmo(ammo_type, 0);

		// Finish with stashing sound, symbolizes that ammo is added
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_StashFinish);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Mag out, stash it] - Cancel");

		// Put a magazine in next
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_Insert);
	},
	
	SetTemporaryAmmo = func (id ammo_type, int amount)
	{
		if (!this.temp_ammo) this.temp_ammo = {};
		this.temp_ammo[Format("%i", ammo_type)] = amount;
	},
	
	GetTemporaryAmmo = func (id ammo_type)
	{
		return this.temp_ammo[Format("%i", ammo_type)];
	},
};

// Short delay and sound while stashing the magazine, merely cosmetic
local Reload_Magazine_StashFinish = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Start");
		firearm->~PlaySoundResupplyAmmo();
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Finish");

		// Put a magazine in next
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_Insert);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Stashing] - Cancel");

		// Put a magazine in next
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_Insert);
	},
};

/* --- Support for an extra ammo chamber --- */


// Manually load a new bullet to the chamber
local Reload_Magazine_LoadAmmoChamber = new Firearm_ReloadState
{
	OnStart = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Start");
	},
	
	OnFinish = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Finish");
		firearm->~PlaySoundChamberBullet();
		firearm->~AmmoChamberInsert(firemode->GetAmmoID());
		firearm->SetReloadState(firemode, firearm.Reload_Magazine_ReadyWeapon);
	},
	
	OnCancel = func (object firearm, object user, int x, int y, proplist firemode)
	{
		Log("Reload [Manual load] - Cancel");
	},
};
