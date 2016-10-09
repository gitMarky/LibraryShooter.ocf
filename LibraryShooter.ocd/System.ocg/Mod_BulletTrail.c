#appendto BulletTrail

/**
 Used for updating the rotation of the trail.
 */
func UpdateRot(int angle)
{
	rotation = angle;
}


//func Set(int iWidth, int iLength, object pSht)
public func Set(object bullet, int wdt, int lgt)
{
	_inherited(bullet, wdt, lgt);
//	SetXDir();
//	SetYDir();
}

/*
func DrawTransform()
{
	
	var distance = Distance(orig_x, orig_y, GetX(), GetY());
	var relative_length = 1000 * distance / ActMap.Travel.Hgt;
	
	// skip because nothing has to be transformed
	// if(!fRemove && l < relative_length) return;
	
	// stretch >-<
	if (fRemove)
		length = Max(0, length - speed);
	
	// stretch <->
	var h = Min(length, relative_length);
	
	var fsin = -Sin(rotation, 1000), fcos = Cos(rotation, 1000);
	
	var xoff = -(ActMap.Travel.Wdt * width / 1000) / 2;
	var yoff = 0;
	
	var draw_width = +fcos * width / 1000;
	var draw_height = +fcos * h / 1000;
	var xskew = +fsin * h / 1000;
	var yskew = -fsin * width / 1000;

	var xadjust = +fcos * xoff + fsin * yoff;
	var yadjust = -fsin * xoff + fcos * yoff;
	
	// set matrix values
	SetObjDrawTransform(draw_width, xskew, xadjust, yskew, draw_height, yadjust);
}
*/