/**
 This object contains a number of IDs.
 @br@br
 The IDs have no properties. For example the Hazard-Munition-System and the
 Alchemy-Ingredient-System uses these kind of ID bags.
 It is faster than storing a number of (icon)-objects in it but doesn't
 allow non-definition properties.

 @note Attention: The bag should only contain Items. All contents are considered as items.
 @author Hazard Team
 @version 0.1.0
 */

local Name = "DefinitionBag";

local content;

/**
 Is a bag.
*/
public func IsBag()
{
	return true;
}

/**
 Add one item.
 @par item An item of this type will be added.
 @return See {@link Library_DefinitionBag#AddItems}
 */
public func AddItem(id item)
{
	return AddItems(item, 1);
}

/**
 Delete one item.
 @par item An item of this type will be deleted.
 @return See {@link Library_DefinitionBag#RemoveItems}
 */
public func RemoveItem(id item)
{
	return RemoveItems(item, 1);
}

/**
 Adds a certain amount of items to the bag.
 @par item This type of object will be added.
 @par amount This many units will be added.
 @return The amount of items of that type that are in the bag.
 */
public func AddItems(id item, int amount)
{
	if (amount < 0)
	{
		FatalError("Cannot add negative amount of items");
	}
	if (item == nil)
	{
		FatalError("This function needs an item");
	}
	
	var current = GetItemCount(item);
	return SetItemCount(item, current + amount);
}

/**
 Deletes a certain amount of items from the bag.
 @par item This type of object will be deleted.
 @par amount This many units will be deleted.
 @return The amount of items of that type that are in the bag.
 */
func RemoveItems(id item, int amount)
{
	if (amount < 0)
	{
		FatalError("Cannot add negative amount of items");
	}
	if (item == nil)
	{
		FatalError("This function needs an item");
	}
	
	var current = GetItemCount(item);
	return SetItemCount(item, current - amount);
}

/**
 Removes all items of a type.
 @par item The type of item that will be removed. Passing {@c nil} empties the whole bag.
 */
func RemoveAllItems(id item)
{	
	if (item == nil)
	{
		content = {};
	}
	else
	{
		RemoveItems(item, GetItemCount(item));
	}
}

/**
 The amount of stored items.
 @par item Items of this type will be counted.
 @return The amount of items of the given type.
 */
public func GetItemCount(id item)
{
	if (content == nil) content = {};
	
	var property = Id2Property(item);
	return GetProperty(property, content);
}

private func SetItemCount(id item, int amount)
{
	if (content == nil) content = {};
	
	var property = Id2Property(item);
	SetProperty(property, amount, content);
	return amount;
}

/**
 Transfers items to another bag
 @par bag The target bag.
 @par item Items of this type will be transferred.
 @par amount Transfers this many items. If {@c amount = nil}, transfer all
 @par is_strict Only transfer if amount is not more than actually in bag?
 @return {@c true} if the desired amount of items could be transferred
*/
func TransferItems(object bag, id item, int amount, bool is_strict)
{
	// All parameters must be given 
	if (bag == nil)
	{
		FatalError("A destination bag is required.");
	}
	if (item == nil)
	{
		FatalError("This function needs an item");
	}
	if (amount < 0)
	{
		FatalError("Cannot add negative amount of items");
	}

	
	var top = GetItemCount(item);
	// no sense in transfer if it is 0
	if (top == 0) return false;
	
	// transfer all
	if (amount == nil)
	{
		amount = top;
	}
	// amount is too big?
	if (amount > top)
	{
		if (is_strict)
		{
			return false;
		}
		else
		{
			amount = top;
		}
	}
	
	// transfer...
	bag->AddItems(item, amount);
	RemoveItems(item, amount);
	return true;
}

// Transfer all items into another bag
func TransferAllItems(object bag)
{
	if (bag == nil)
	{
		FatalError("A destination bag is required.");
	}
	
	for (var property in GetProperties(content))
	{
		var mine = GetProperty(property, content);
		
		var theirs = GetProperty(property, bag.content);
		
		
	}
	
	return true;
}

private func Id2Property(id item)
{
	return Format("%v", item);

}