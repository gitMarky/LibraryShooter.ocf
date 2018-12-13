/**
	Library for randomization.

	@author Marky
 */


/* --- Global functions --- */

/**
	Defines a range for random values that can be sampled with
	{@link Library_Random#SampleValue}.

	@par min_value The lower end of the random value range.
	@par max_value The upper end of the random value range.
	@par steps [optional] The steps in which the values can occur.
	           For example, {@code RND_Random(2, 10, 2)} returns
	           values from [2, 4, 6, 8, 10].

	@return int The sampled value.
 */
global func RND_Random(int min_value, int max_value, int steps)
{
	AssertNotNil(min_value);
	AssertNotNil(max_value);

	var min = Min(min_value, max_value);
	var max = Max(min_value, max_value);
	steps = steps ?? 1;
	return [min, max, steps];
}

/* --- Definition calls --- */

/**
	Gets a sample of a random value.

	@par value The value. This can be a {@code C4V_Int}, or {@code C4V_Array}. 

	@return int The sampled value. This is either the {@code value}, if {@code C4V_Int}
         was passed, or if an array was passed: a random value between
         {@code value[0]} and {@code value[1]}, where the possible increments are
         {@code value[2]}.  
 */
func SampleValue(value)
{
	if (GetType(value) == C4V_Array)
	{
		var min = value[0];
		var max = value[1];
		var step = value[2] ?? 1;

		if (step == nil || step == 1)
		{
			return RandomX(min, max);
		}
		else
		{
			return min + step * Random((max - min) / step);
		}
	}
	else if (GetType(value) == C4V_Int)
	{
		return value;
	}
	else if (GetType(value) == C4V_Nil)
	{
		return nil;
	}
	else
	{
		FatalError(Format("Expected int or array, got %v", value));
	}
}

