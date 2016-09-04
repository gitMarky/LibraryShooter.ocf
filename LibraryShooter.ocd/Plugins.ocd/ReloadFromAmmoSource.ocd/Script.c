
public func OnFinishReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode, ...);

	var source = this->GetAmmoReloadContainer();
	if (source)
	{	
		var ammo_type = firemode.ammo_id;
		var ammo_max = firemode.ammo_load ?? 1;
		var ammo_available = this->GetAmmo(ammo_type); // how many do I have
		
		var ammo_requested = ammo_max - ammo_available; // receive only as much as you need

		Log("Reloaded: ");
		Log(" * ammo in source before = %d", source->GetAmmo(ammo_type));
		Log(" * ammo_type = %d", ammo_type);
		Log(" * ammo_max = %d", ammo_max);
		Log(" * ammo_available = %d", ammo_available);
		Log(" * ammo_requested = %d", ammo_requested);

		var ammo_received = Abs(source->DoAmmo(ammo_type, -ammo_requested)); // see how much you can get
		Log(" * ammo_received = %d", ammo_received);
		var ammo_spare = (ammo_available + ammo_received) % (firemode.ammo_usage ?? 1); // get ammo only in increments of ammo_usage

		source->DoAmmo(ammo_type, ammo_spare); // give back the unecessary ammo
		ammo_received -= ammo_spare;           // adjust the counter accordingly
		Log(" * ammo_spare = %d", ammo_spare);
		Log(" * ammo_inserted = %d", ammo_received);
		Log(" * ammo in source after = %d", source->GetAmmo(ammo_type));
	
		this->DoAmmo(ammo_type, ammo_received);
	}
	
	if (firemode.progress_bar) firemode.progress_bar->Close();
}

public func CanReload(object user, proplist firemode)
{
	var source = this->GetAmmoReloadContainer();
	if (source)
	{
		Log("Can reload? %d", source->GetAmmo(firemode.ammo_id));
		return source->GetAmmo(firemode.ammo_id) > 0;
	}
	else
	{
		return false;
	}
}

public func OnStartReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode);
	
	Log("Reloading started");

	firemode.progress_bar = user->CreateProgressBar(GUI_SimpleProgressBar, 100, 0, 0, user->GetOwner(), {x = 0, y = -15}, VIS_Owner, {color = RGB(200, 0, 0)});	
}

public func OnCancelReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode);
	
	Log("Reloading cancelled");

	if (firemode.progress_bar) firemode.progress_bar->Close();
}

public func OnProgressReload(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
{
	_inherited(user, x, y, firemode, current_percent, change_percent);
	
	if (firemode.progress_bar)
	{
		firemode.progress_bar->SetValue(current_percent);
		firemode.progress_bar->Update();
	}
}

public func GetAmmoReloadContainer()
{
	// TODO;
}
