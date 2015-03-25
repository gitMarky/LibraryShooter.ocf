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

static const IDBAG_Capacity_Infinite = nil;
 

local Name = "DefinitionBag";

local content;
local capacity;

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
	
	var current = GetItems(item);
	var property = Id2Property(item);
	return SetItemCount(property, current + amount);
}

/**
 Deletes a certain amount of items from the bag.
 @par item This type of object will be deleted.
 @par amount This many units will be deleted.
 @return int The amount of items of that type that are in the bag.
 */
public func RemoveItems(id item, int amount)
{
	if (amount < 0)
	{
		FatalError("Cannot add negative amount of items");
	}
	if (item == nil)
	{
		FatalError("This function needs an item");
	}
	
	var current = GetItems(item);
	var property = Id2Property(item);
	return SetItemCount(property, current - amount);
}

/**
 Sets the amount of items in the bag.
 @par item This type of object will be changed.
 @par amount This many units will be in the bag after the change.
 @return int The amount of items of that type that are in the bag.
 */
public func SetItems(id item, int amount)
{
	if (item == nil)
	{
		FatalError("This function needs an item");
	}
	
	var property = Id2Property(item);
	return SetItemCount(property, amount);
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
		RemoveItems(item, GetItems(item));
	}
}

/**
 The amount of stored items.
 @par item Items of this type will be counted.
 @return int The amount of items of the given type.
 */
public func GetItems(id item)
{
	var property = Id2Property(item);
	return GetItemCount(property, content);
}

private func GetItemCount(string item)
{
	if (content == nil) content = {};
	
	return GetProperty(item, content);
}

private func SetItemCount(string item, int amount)
{
	if (content == nil) content = {};
	
	var max_items = GetItemCapacity(item);
	amount = BoundBy(amount, 0, max_items);
	
	SetProperty(item, amount, content);
	return amount;
}

/**
 The maximum amount of stored items.
 @par item Capacity for items of this type will considered.
 @return int the number of items of this type that can be contained in the bag.
 */
public func GetCapacity(id item)
{
	return GetItemCapacity(Id2Property(item));
}

/**
 Defihes the maximum amount of stored items.
 @par item Capacity for items of this type will considered.
 @par amount This values is the new maximum of stored items. The default value is {@c IDBAG_Capacity_Infinite}.
 */
public func SetCapacity(id item, int amount)
{
	return SetItemCapacity(Id2Property(item), amount);
}

private func GetItemCapacity(string item, int amount)
{
	if (capacity == nil) capacity = {};
	
	var max = GetProperty(item, capacity);
	
	if (max == IDBAG_Capacity_Infinite)
	{
		return 2147483647;
	}
	else
	{
		return max;
	}
}

private func SetItemCapacity(string item, int amount)
{
	if (capacity == nil) capacity = {};
	
	SetProperty(item, amount, capacity);
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
	if (bag->~IsBag() == false)
	{
		FatalError("The destination object must return true in IsBag()");
	}
	if (item == nil)
	{
		FatalError("This function needs an item");
	}
	if (amount < 0)
	{
		FatalError("Cannot add negative amount of items");
	}

	
	var top = GetItems(item);
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

/**
 Transfer all items into another bag.
 @par bag The destination object.
 @par is_strict if this is set to true, then the items that could not be transferred
      stay in this bag.
 */
func TransferAllItems(object bag, bool is_strict)
{
	if (bag == nil)
	{
		FatalError("A destination bag is required.");
	}
	if (bag->~IsBag() == false)
	{
		FatalError("The destination object must return true in IsBag()");
	}
	
	for (var property in GetProperties(content))
	{
		var mine = GetItemCount(property);
		var theirs = bag->GetItemCount(property);
		
		var theirs_then = bag->SetItemCount(property, theirs + mine);
		var mine_then = mine + theirs - theirs_then;
		
		if (!is_strict) mine_then = 0;
		
		SetItemCount(property, mine_then);
	}
}

private func Id2Property(id item)
{
	return Format("%v", item);

}