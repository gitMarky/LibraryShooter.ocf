/**
	Plugin for weapons: Basic reload logic for weapons.@br

	@author Marky
 */

/* --- Interface --- */

/**
	Checks if the weapon is currently reloading.

	@return The reloading effect.
*/
func IsReloading()
{
	return GetEffect("IntReloadEffect", this);
}


/**
	Callback: The weapon reload effect should be created.

	@par user The object that is using the weapon.
	@par x The x coordinate the user is aiming at. Relative to the user.
	@par y The y coordinate the user is aimint at. Relative to the user.
	@par firemode A proplist containing the fire mode information.
 */
func StartReloadProcess(object user, int x, int y, proplist firemode)
{
	CreateEffect(IntReloadEffect, 1, 1, user, x, y, firemode);
}

/* --- Internal --- */

local IntReloadEffect = new Effect
{
	Construction = func (object user, int x, int y, proplist firemode)
	{
		this.user = user;
		this.x = x; // x and y will be updated by StartReload
		this.y = y;
		this.firemode = firemode;

		this.percent_old = 0;
		this.percentage = 0;
		this.progress = 0;
		this.is_reloaded = false;
	},

	Timer = func (int time)
	{
		// Cancel if user cannot reload
		if (!this.Target->IsUserReadyToReload(this.user))
		{
			this.Target->CancelReload(this.user, this.x, this.y, this.firemode, false);
		}
	
		// Increase progress percentage depending on the reloading delay of the firemode
		this.percentage = BoundBy(time * 100 / this.firemode->GetReloadDelay(), 0, 100);
		// Save the progress (i.e. the difference between the current percentage and during the last update)
		this.progress = this.percentage - this.percent_old;

		// Check if the reloading process is finished based on the reloading delay of the firemode
		if (time > this.firemode->GetReloadDelay() && !this.is_reloaded)
		{
			this.is_reloaded = true;

			// Do the reload if anything is necessary and end the effect if successful
			if (this.Target->DoReload(this.user, this.x, this.y, this.firemode))
			{
				return FX_Execute_Kill;
			}
		}

		// Do a progress update if necessary
		if (this.progress > 0)
		{
			this.Target->OnProgressReload(this.user, this.x, this.y, this.firemode, this.percentage, this.progress);
			this.percent_old = this.progress;
		}
	},
	
	GetProgress = func ()
	{
		return this.percentage;
	},
};
