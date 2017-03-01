
global func Control2Player(int plr, int ctrl, int x, int y, int strength, bool repeat, bool release)
{
	// cursor pos info - store in player values
	if (ctrl == CON_CursorPos)
	{
		if (!g_player_cursor_pos) g_player_cursor_pos = CreateArray(plr+1);
		g_player_cursor_pos[plr] = [x, y];
		return true;
	}
	
	if(ctrl == CON_CaedesAimingCursor)
	{
		var cursor = GetCursor(plr);
		if (cursor && ! cursor.aim_stop)
		{
			cursor->~SetAimPosition(Normalize(Angle(cursor->GetX(), cursor->GetY(), x, y), -180));
		}
		return true;
	}
	
	return inherited(plr, ctrl, x, y, strength, repeat, release);
}
