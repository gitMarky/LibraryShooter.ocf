/* Schadenssystem */


global func DoDmg(int iDmg, int iType, object pTarget, int iPrecision, empty, object pFrom, id idWeapon) {
  if(!pTarget)
    if(!(pTarget = this))
      return(0);
	if(!pFrom)
		pFrom = this;
  if(!iPrecision)
    iPrecision = 1000;

  var dmg;

  var red = pTarget->~OnDmg(iDmg, iType); //reduction
  // reduction
  dmg = (iDmg*(100-red)*iPrecision) / 100;

	if(dmg <= 0) return;
	pTarget->~SetLastDamagingWeapon(idWeapon);
  // Killer setzen
  /*if(this && (pTarget->GetOCF() & OCF_CrewMember))
  {
    //DebugLog("DoDmg sets killer: %d, owner of %s", "damage", this->GetOwner(), this->GetName());
   
    if(iDmg) pTarget->~SetLastDamagingObject(pFrom);
    if(iDmg) pTarget->~SetLastDamagingWeapon(idWeapon);
  }*/

  //Schaden machen
  //if(pTarget != this())
    
  //if(pFrom)
  //	pTarget->SetKiller(pFrom->GetController());
  //pTarget->~LastDamageType(iType);
  /*{
  var n = pFrom->GetName();
  if(!n) n = "???";
  Log("%s deals %d dmg to %s", n, dmg, pTarget->GetName());
  }*/
  pTarget->~OnHit(dmg/1000, iType, pFrom, idWeapon);
	if(pTarget->GetCategory() & C4D_Living)
	{
		pTarget->DoEnergy(-dmg, true, nil, pFrom->GetController());
		
		
		
		return 1;
	}
  pTarget->DoDamage(dmg/1000, nil, pFrom->GetController());
  return 1;
}
