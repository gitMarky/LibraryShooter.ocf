/**
 Handles the deviation of projectiles.

 @author Marky
 @version 0.2.0
 */


/**
 Creates a proplist that contains the information for
 projectile deviation.

 @par angle The angle, in degrees.
 @par precision The precision factor, default precision is 1.
 @version 0.2.0
 */
global func Projectile_Deviation(angle, int precision)
{
	if (GetType(angle) != C4V_Int
	 && GetType(angle) != C4V_Array)
	{
		FatalError(Format("This function accepts arguments of type C4V_Int or C4V_Array for the parameter 'angle'. You passed %v", GetType(angle)));
	}

	return {angle = angle, precision = precision ?? 1};
}
 

/**
 Takes an array of projectile deviations, and
 normalizes it so that all deviations use the same
 precision.
 
 @par deviations The deviation definitions.
 @version 0.2.0
 */ 
global func NormalizeDeviations(array deviations, int min_precision)
{
	var precision_max = min_precision ?? 1;
	var precisions = [];
	for (var deviation in deviations)
	{
		PushBack(precisions, deviation.precision);
		if (deviation.precision > precision_max) precision_max = deviation.precision;
	}
	
	var angles = [];
	for (var deviation in deviations)
	{
		PushBack(angles, deviation.angle * precision_max / deviation.precision);
	}
	
	return Projectile_Deviation(angles, precision_max);
}
