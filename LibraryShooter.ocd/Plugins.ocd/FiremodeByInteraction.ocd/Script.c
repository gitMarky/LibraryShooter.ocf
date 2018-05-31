/**
	Allows fire mode selection via the object interaction menu.
 
	{@c public func HasInteractionMenu()} must be implemented in order for this to work.
 */

/* --- GUI --- */

static const GUI_PRIORITY_FIREMODE = 10;


public func GetInteractionMenus(object crew)
{
	var menus = _inherited(crew) ?? [];		

	var change_firemode_menu =
	{
		title = Format("%s: %s ({{%i}})", "$GUI_Change_Firemode$", this->GetName(), this->GetID()),
		entries_callback = this.GetGUIFiremodeMenuEntries,
		entries_callback_parameter = this,
		callback = "OnGUIChangeFiremode",
		callback_hover = "OnGUIHoverFiremode",
		callback_target = this,
		BackgroundColor = GetGUIFiremodeMenuColor(),
		Priority = GUI_PRIORITY_FIREMODE,
	};

	PushBack(menus, change_firemode_menu);
	return menus;
}


public func GetGUIFiremodeMenuColor(){ return RGB(0, 50, 50);}
public func GetGUIFiremodeActiveColor(){ return "ffffff";}
public func GetGUIFiremodeInactiveColor(){ return "bbbbbb";}

public func GetGUIFiremodeMenuEntries(object crew, object weapon)
{
	var menu_entries = [];
	
	// default design of a control menu item
	var custom_entry = 
	{
		Right = "100%", Bottom = "2em",
		BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
		image = {Right = "2em"},
		text = {Left = "2em"}
	};
	
		
	// Add info message for every defender
	var modes = weapon->~GetAvailableFiremodes();
	
	if (!modes) return menu_entries;
	
	var selected = weapon->~GetFiremode();
	
	for (var firemode in modes)
	{
		var firemode_symbol = firemode.icon ?? weapon->GetID();
		
		var text_color;
		
		if (firemode == selected)
		{
			text_color = GetGUIFiremodeActiveColor();
		}
		else
		{
			text_color = GetGUIFiremodeInactiveColor();
		}

		PushBack(menu_entries,
		{
		    Symbol = firemode_symbol,
		    extra_data = { weapon = weapon, firemode = weapon->GetFiremodeIndex(firemode)},
			custom = 
			{
				Prototype = custom_entry,
				Priority = GUI_PRIORITY_FIREMODE,
				text = {Prototype = custom_entry.text, Text = Format("<c %s>%s</c>", text_color, firemode.name)},
				image = {Prototype = custom_entry.image, Symbol = firemode_symbol},
			}
		});
	}

	return menu_entries;
}

public func OnGUIHoverFiremode(id symbol, proplist action, desc_menu_target, menu_id)
{
	// do nothing at the moment
}

public func OnGUIChangeFiremode(id symbol, proplist action, object crew)
{
	action.weapon->~ScheduleSetFiremode(action.firemode);
}

public func SetFiremode(int number, bool force)
{
	var return_value = _inherited(number, force, ...);
	
	if (Contained())
	{
		Contained()->UpdateInteractionMenus();
	}
	return return_value;
}
