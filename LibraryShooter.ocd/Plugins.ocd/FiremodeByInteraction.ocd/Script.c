/**
 Allows fire mode selection via the object interaction menu.
 
 {@c public func HasInteractionMenu()} must be implemented in order for this to work.
 */

#include Plugin_Weapon_FiremodeBySelection

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// GUI

static const GUI_PRIORITY_FIREMODE = 10;


public func GetInteractionMenus(object crew)
{
	var menus = _inherited(crew) ?? [];		

	var change_firemode_menu =
	{
		title = "$GUI_Change_Firemode$",
		entries_callback = this.GetGUIFiremodeMenuEntries,
		callback = "OnGUIChangeFiremode",
		callback_hover = "OnGUIHoverFiremode",
		callback_target = this,
		BackgroundColor = GetGUIFiremodeMenuColor(),
		Priority = GUI_PRIORITY_FIREMODE
	};

	PushBack(menus, change_firemode_menu);
	return menus;
}


public func GetGUIFiremodeMenuColor(){ return RGB(0, 50, 50);}


public func GetGUIFiremodeMenuEntries(object crew)
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
	for (var firemode in this->~GetFiremodes())
	{
		var is_available = firemode.condition == nil || Call(firemode.condition);
		
		if (!is_available) continue;
		
		var firemode_symbol = firemode.icon ?? this;

		PushBack(menu_entries,
		{
		    symbol = firemode_symbol,
		    extra_data = firemode.name,
			custom = 
			{
				Prototype = custom_entry,
				Priority = GUI_PRIORITY_FIREMODE,
				text = {Prototype = custom_entry.text, Text = firemode.name},
				image = {Prototype = custom_entry.image, Symbol = firemode_symbol},
			}
		});
	}

	return menu_entries;
}

public func OnGUIHoverFiremode(id symbol, string action, desc_menu_target, menu_id)
{
	// do nothing at the moment
}

public func OnGUIChangeFiremode(id symbol, string action, bool alt)
{
	// do nothing on click, or maybe let the clonk play an idle animation :p
}
