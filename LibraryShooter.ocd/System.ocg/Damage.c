
/*
	Deals damage to an object, draining either energy for living things or dealing damage otherwise.
	CatchBlow is called on the target if it's alive.
*/
global func WeaponDamageShooter(object target, int damage, int damage_type, int engine_damage_type, bool exact_damage, id weapon)
{
	AssertObjectContext("WeaponDamageShooter");
	
	if (!target)
	{
		FatalError("This function needs a target, got nil.");
	}

	engine_damage_type = engine_damage_type ?? FX_Call_EngObjHit;
	damage = target->~ModifyWeaponDamageShooter(damage, damage_type, engine_damage_type) ?? damage;
	var true_damage = damage;
	if (exact_damage) true_damage = damage / 1000;
	
	if (exact_damage < 0) return;
	
	// TODO: pTarget->~SetLastDamagingWeapon(idWeapon);
	/*
	// Killer setzen
	if(this && (pTarget->GetOCF() & OCF_CrewMember))
	{
		//DebugLog("WeaponDamageShooter sets killer: %d, owner of %s", "damage", this->GetOwner(), this->GetName());
		   
		if(amount) pTarget->~SetLastDamagingObject(pFrom);
		if(amount) pTarget->~SetLastDamagingWeapon(idWeapon);
	}
	*/
	//Schaden machen
	//if(pTarget != this())
	
	//if(pFrom)
	//	pTarget->SetKiller(pFrom->GetController());
	//pTarget->~LastDamageType(engine_damage_type);
	/*{
		var n = pFrom->GetName();
		if(!n) n = "???";
		DebugLog("%s deals %d dmg to %s", n, dmg, pTarget->GetName());
	}*/


	if (target->GetAlive())
	{
		target->DoEnergy(-damage, exact_damage, engine_damage_type, GetController());
		if (!target) return;

		target->~CatchBlow(-true_damage, this);
	}
	else
	{
		target->DoDamage(true_damage, engine_damage_type, GetController());
	}
}
