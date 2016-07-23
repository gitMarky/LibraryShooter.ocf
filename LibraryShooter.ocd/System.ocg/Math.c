
/**
 Gets the exponent of a value.
 @par value The value.
 @par base The base.
 @return int The exponent, so that {@code value = base ** exponent}.
 */
global func GetExponent(int value, int base)
{
	var exponent = 0;
	
	base = base ?? 10;
	
	for (var test = value; test%base == 0; test = test/base)
	{
		exponent += 1;
	}
	
	return exponent;
}
