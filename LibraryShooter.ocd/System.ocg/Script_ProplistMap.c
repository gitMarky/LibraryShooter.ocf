/**
	Provides a generic implementation for getting
	a proplist from a proplist map, such as ActMap.

	@author Marky
 */


/**
	Gets a property from a proplist, filtered by an attribute value.

	@par attribute_value The function should search for this attribute value.
	                     The first property where the attribute value matches
	                     is returned. This is modified by the optional parameter
	                     {@code type_check}.

	@par list The function will search properties from this proplist.

	@par attribute (optional) This is the name of the attribute that should be
	               inspected. For example, {@code GetPropertyByAttribute("Hello", this.MyProplist, "Message")}
	               will return the first property in {@code MyProplist} that has the property {@code Message = "Hello"}.
	               Default value for this parameter is {@code "Name"}. In that case only the property name
	               is also inspected, making it equal to {@code GetProperty(attribute_value, list)}.

	@par list_name (optional) In case that {@code list} is {@code nil} this name will be logged
	                          in an error message, so that you have an idea which proplist was
	                          inspected but not defined.
 */
global func GetPropertyByAttribute(any attribute_value, proplist list, string attribute, string list_name)
{
	if (list == nil)
	{
		FatalError("%s not defined", list_name ?? "Proplist");
	}
	else
	{
		attribute = attribute ?? "Name";

		for (var property_name in GetProperties(list))
		{
			var found = GetProperty(property_name, list);
			if (found && found[attribute] == attribute_value)
			{
				return found;
			}
			else if (attribute == "Name" && property_name == attribute_value)
			{
				return found;
			}
		}
		return nil;
	}
}
