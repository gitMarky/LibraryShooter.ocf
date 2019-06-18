#appendto Clonk

// Handle controls to the clonk
public func ObjectControl(int plr, int ctrl, int x, int y, int strength, bool repeat, int status)
{
	if (!this)
	{
		return false;
	}

	// Handle aiming?
	if (this->~HandleAimControl(plr, ctrl, x, y, strength, repeat, status))
	{
		return true;
	}

	return _inherited(plr, ctrl, x, y, strength, repeat, status);
}
