/**
	Aim Position Offset

	Supports aiming position shifting up/forward/down, depending on angle.

	This is useful for effects and launch positions of projectiles, since
	the position changes with the aiming angle, and might even depend on
	the shooter stance (standing vs. prone).
*/


/**
	Defines a position offset that is caused by an object
	that is rotated, similarly to a vertex position.
	
	This can be used to model e.g. the muzzle of a firearm.
 */
static const PositionOffsetRotation = new Global
{
	DebugColor = 0xff0000ff,

	offset = nil,

	DefineOffset = func (int x, int y, int precision)
	{
		offset = {X = x, Y = y, Precision = precision ?? 1};
		return this;
	},

	GetPosition = func (int angle, int precision_angle, int precision_position)
	{
		if (nil == offset)
		{
			return {X = 0, Y = 0};
		}
		precision_angle = precision_angle ?? 1;
		precision_position = precision_position ?? 1;
		angle = Normalize(angle, -180 * precision_angle, precision_angle);
		
		var factor_y = 1;
		if (angle < 0)
		{
			factor_y = -1;
		}

		return 
		{
			X = (+Sin(angle, offset.X) + factor_y * Cos(angle, offset.Y)) * precision_position / offset.Precision,
			Y = (-Cos(angle, offset.X) + factor_y * Sin(angle, offset.Y)) * precision_position / offset.Precision,
			DebugColor = this.DebugColor,
		};
	},
};


/**
	Defines upward/forward/downward offset that is caused
	by an aiming animation.
	
	Positions are scaled linearly, because the animations blend
	linearly with the angle.
	
	This can be used to model e.g. the hand position.
 */
static const PositionOffsetAnimation = new Global
{
	DebugColor = 0xffffff00,

	aim_offset_forward = nil,
	aim_offset_up = nil,
	aim_offset_down = nil,

	DefineOffsetForward = func (int x, int y, int precision, int dir0_shift_x)
	{
		aim_offset_forward = {X = x, Y = y, Precision = precision ?? 1, X_left = dir0_shift_x};
		return this;
	},

	DefineOffsetUp = func (int x, int y, int precision, int dir0_shift_x)
	{
		aim_offset_up = {X = x, Y = y, Precision = precision ?? 1, X_left = dir0_shift_x};
		return this;
	},

	DefineOffsetDown = func (int x, int y, int precision, int dir0_shift_x)
	{
		aim_offset_down = {X = x, Y = y, Precision = precision ?? 1, X_left = dir0_shift_x};
		return this;
	},

	GetOffset = func (proplist offset, int precision)
	{
		if (nil == offset)
		{
			return {X = 0, Y = 0, X_left = 0, };
		}
		return {
			X = offset.X * precision / offset.Precision,
			Y = offset.Y * precision / offset.Precision,
			X_left = offset.X_left * precision / offset.Precision,
		};
	},

	GetPosition = func (int angle, int precision_angle, int precision_position)
	{
		precision_angle = precision_angle ?? 1;
		precision_position = precision_position ?? 1;
		angle = Normalize(angle, -180 * precision_angle, precision_angle);

		var angle_forward = 90 * precision_angle;
		var angle_down = 180 * precision_angle;

		var offset_up      = GetOffset(aim_offset_up, precision_position);
		var offset_forward = GetOffset(aim_offset_forward, precision_position);
		var offset_down    = GetOffset(aim_offset_down, precision_position);

		var x = InterpolateLinear(Abs(angle), 0, offset_up.X, angle_forward, offset_forward.X, angle_down, offset_down.X);
		var y = InterpolateLinear(Abs(angle), 0, offset_up.Y, angle_forward, offset_forward.Y, angle_down, offset_down.Y);

		if (angle < 0)
		{
			// Apply additional offset in case that the animation is not mirrored evenly
			x += InterpolateLinear(Abs(angle), 0, offset_up.X_left, angle_forward, offset_forward.X_left, angle_down, offset_down.X_left);
		}

		return { X = x, Y = y, DebugColor = this.DebugColor, };
	},

	InterpolateLinear = func (int progress, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3)
	{
		progress = BoundBy(progress, Min(Min(x0, x1), Min(x2, x3)), Max(Max(x0, x1), Max(x2, x3)));
	
		if (progress > x1 && x2 || x3)
		{
			if (x2 <= progress && progress <= x3)
			{
				return InterpolateLinear(progress, x2, y2, x3, y3);
			}
			else if (x1 <= progress && progress <= x2)
			{
				return InterpolateLinear(progress, x1, y1, x2, y2);
			}
		}
		else
		{
			var interval = x1 - x0;
			var factor = BoundBy(progress - x0, 0, interval);
	
			return (factor * y1 + (interval - factor) * y0) / Max(1, interval);
		}
	}
};
