#appendto BulletTrail

/**
 Used for updating the rotation of the trail.
 */
func UpdateRot(int angle)
{
	r = angle;
}


func Set(int iWidth, int iLength, object pSht)
{
	_inherited(iWidth, iLength, pSht);
	
	SetXDir();
	SetYDir();
}


func DrawTransform()
{
	
	var distance = Distance(x, y, GetX(), GetY());
	var relative_length = 1000 * distance / ActMap.Travel.Hgt;
	
	// skip because nothing has to be transformed
	// if(!fRemove && l < relative_length) return;
	
	// stretch >-<
	if (fRemove)
		l = Max(0, l - iSpeed);
	
	// stretch <->
	var h = Min(l, relative_length);
	
	var fsin = -Sin(r, 1000), fcos = Cos(r, 1000);
	
	var xoff = -(ActMap.Travel.Wdt * w / 1000) / 2;
	var yoff = 0;
	
	var width = +fcos * w / 1000, height = +fcos * h / 1000;
	var xskew = +fsin * h / 1000, yskew = -fsin * w / 1000;
	
	var xadjust = +fcos * xoff + fsin * yoff;
	var yadjust = -fsin * xoff + fcos * yoff;
	
	// set matrix values
	SetObjDrawTransform(width, xskew, xadjust, yskew, height, yadjust);
}
