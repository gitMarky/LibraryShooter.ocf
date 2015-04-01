#appendto BulletTrail

/**
 Used for updating the rotation of the trail.
 */
public func UpdateRot(int angle)
{
	r = angle;
}


public func Set(int iWidth, int iLength, object pSht)
{
	_inherited(iWidth, iLength, pSht);
	
	SetXDir();
	SetYDir();
}
