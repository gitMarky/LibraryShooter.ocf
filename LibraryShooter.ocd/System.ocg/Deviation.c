/**
	Handles the deviation of projectiles.

	@author Marky
 */

static const Deviation = new Global
{
	// Properties
	
	Value = 0,
	Precision = 1,
	
	// Functions

	GetValue = func ()
	{
		return this.Value;
	},
	
	GetPrecision = func ()
	{
		return this.Precision;
	},

	SetValue = func (value)
	{
		if (GetType(value) == C4V_Int
		 || GetType(value) == C4V_Array)
		{
			this.Value = value;
			return this;
		}
		else
		{
			FatalError(Format("This function accepts arguments of type C4V_Int or C4V_Array for the parameter 'angle'. You passed %v: %v", GetType(value), value));
		}
	},
	
	SetPrecision = func(int value)
	{
		this.Precision = Max(1, value ?? 1);
		return this;
	},
};


/**
	Creates a proplist that contains the information for
	projectile deviation.

	@par angle The angle, in degrees.
	@par precision The precision factor, default precision is 1.
 */
global func Projectile_Deviation(angle, int precision)
{
	var deviation = new Deviation {};
	return deviation->SetValue(angle)->SetPrecision(precision);
}
 

/**
	Takes an array of projectile deviations, and
	normalizes it so that all deviations use the same
	precision.

	@par deviations The deviation definitions.
	@par min_precision The minimal precision.
 */ 
global func NormalizeDeviations(array deviations, int min_precision)
{
	RemoveHoles(deviations);

	var precision_max = min_precision ?? 1;
	for (var deviation in deviations)
	{
		if (deviation->GetPrecision() > precision_max)
		{
			precision_max = deviation->GetPrecision();
		}
	}
	
	var angles = [];
	for (var deviation in deviations)
	{
		var precision = deviation->GetPrecision();
		var value = deviation->GetValue();
		if (GetType(value) == C4V_Array)
		{
			var angles = [];
			for (var angle in value)
			{
				PushBack(angles, angle * precision_max / precision);
			}
		}
		else
		{
			PushBack(angles, value * precision_max / precision);
		}
	}
	
	return Projectile_Deviation(angles, precision_max);
}


/**
	Takes a projectile deviation and scales it to fit the target precision.

	@par deviations The deviation definitions.
	@par target_precision The target precision;
 */ 
global func ScaleDeviation(proplist deviation, int target_precision)
{
	var factor = target_precision ?? 1;
	var angles;
	if (GetType(deviation.angle) == C4V_Array)
	{
		angles = [];
		for (var i = 0; i < GetLength(deviation.angle); ++i)
		{
			angles[i] = factor * deviation.angle[i] / deviation.precision;
		}
	}
	else
	{
		angles = factor * deviation.angle / deviation.precision;
	}

	return Projectile_Deviation(angles, target_precision);
}
