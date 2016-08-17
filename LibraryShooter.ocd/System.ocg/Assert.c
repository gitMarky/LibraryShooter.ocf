global func AssertObjectContext(string function_name)
{
	if (!this)
	{
		FatalError(Format("%s must be called from object context!", function_name ?? "The function"));
	}
}
