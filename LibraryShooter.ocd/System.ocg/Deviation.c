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
	
	
	/**
		Sets the precision, without changing the value.
	 */
	SetPrecision = func(int value)
	{
		this.Precision = Max(1, value ?? 1);
		return this;
	},
	
	
	/**
		Returns a new deviation with the current value
		scaled to a specific precision
	 */
	ScalePrecision = func (int value)
	{
		var factor = value ?? 1;
		var angles;
		if (GetType(this.Value) == C4V_Array)
		{
			angles = [];
			for (var i = 0; i < GetLength(this->GetValue()); ++i)
			{
				angles[i] = factor * this.Value[i] / this.Precision;
			}
		}
		else
		{
			angles = factor * this.Value / this.Precision;
		}
	
		return Projectile_Deviation(angles, factor);
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
