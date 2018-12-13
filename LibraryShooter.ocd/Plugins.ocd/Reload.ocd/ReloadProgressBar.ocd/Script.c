/**
	Plugin for weapons: The weapon displays a progress bar when reloading.

	@author Marky
 */

local reload_progress_bar;

public func OnStartReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode);

	if (reload_progress_bar) reload_progress_bar->Close();
	reload_progress_bar = CreateReloadProgressBar(user, firemode);
}

public func OnFinishReload(object user, int x, int y, proplist firemode)
{
	_inherited(user, x, y, firemode, ...);

	if (reload_progress_bar) reload_progress_bar->Close();
}


public func OnCancelReload(object user, int x, int y, proplist firemode, bool requested_by_user)
{
	_inherited(user, x, y, firemode, requested_by_user);

	if (reload_progress_bar) reload_progress_bar->Close();
}

public func OnProgressReload(object user, int x, int y, proplist firemode, int current_percent, int change_percent)
{
	_inherited(user, x, y, firemode, current_percent, change_percent);

	if (reload_progress_bar)
	{
		reload_progress_bar->SetValue(current_percent);
		reload_progress_bar->Update();
	}
}

public func CreateReloadProgressBar(object user, proplist firemode)
{
	return user->CreateProgressBar(GUI_SimpleProgressBar, 100, 0, 0, user->GetOwner(), {x = 0, y = -15}, VIS_Owner, {color = RGB(200, 0, 0)});
}
