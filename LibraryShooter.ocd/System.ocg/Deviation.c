/**
	Handles the deviation of projectiles.

	@author Marky
 */

static const Deviation = new Global
{
	// Properties
	
	Value_Default = 0,
	Value_Current = 0,
	Precision = 1,
	Dynamic = false,
	
	// Functions

	/**
		Gets the current deviation value.
	 */
	GetValue = func ()
	{
		return this.Value_Current;
	},


	/**
		Gets the deviation precision.
	 */
	GetPrecision = func ()
	{
		return this.Precision;
	},


	/**
		Sets the current and default deviation value.
	 */
	SetValue = func (value)
	{
		if (GetType(value) == C4V_Int
		 || GetType(value) == C4V_Array)
		{
			this.Value_Default = value;
			this.Value_Current = value;
			return this;
		}
		else
		{
			FatalError(Format("This function accepts arguments of type C4V_Int or C4V_Array for the parameter 'angle'. You passed %v: %v", GetType(value), value));
		}
	},
	
	
	DoValue = func (int change)
	{
		this.Value_Current = Max(0, this.Value_Current + change);
		return this;
	},
	
	
	/**
		Sets the precision, without changing the value.
	 */
	SetPrecision = func (int value)
	{
		this.Precision = Max(1, value ?? 1);
		return this;
	},
	
	
	/**
		Determines whether the deviation can be changed.
		
		@par dynamic {@code true} means that the deviation
		             can be changed, otherwise it is
		             write protected.
	 */
	SetDynamic = func (bool dynamic)
	{
		this.Dynamic = dynamic;
		return this;
	},

	
	/**
		Returns a new deviation with the current value
		scaled to a specific precision
	 */
	ScalePrecision = func (int precision)
	{
		precision = precision ?? 1;
		var values;
		if (GetType(this.Value_Current) == C4V_Array)
		{
			values = [];
			for (var i = 0; i < GetLength(this->GetValue()); ++i)
			{
				values[i] = precision * this.Value_Current[i] / this.Precision;
			}
		}
		else
		{
			values = precision * this.Value_Current / this.Precision;
		}

		var deviation = new Deviation {};
		return deviation->SetValue(values)->SetPrecision(precision);
	},
	
	
	/**
		Returns a new deviation with the current value
		modified by the given parameter.
	 */
	AddDeviation = func (proplist deviation)
	{
		// Adjust a copy of this, or the deviation, to a common precision
		var self = this;
		if (deviation->GetPrecision() > this->GetPrecision())
		{
			self = ScalePrecision(deviation->GetPrecision());
		}
		else
		{
			deviation = deviation->ScalePrecision(this->GetPrecision());
		}
		var added = new Deviation{};
		return added->SetValue(self->GetValue() + deviation->GetValue())->SetPrecision(deviation->GetPrecision());
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
