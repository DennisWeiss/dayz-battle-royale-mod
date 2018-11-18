modded class PlayerBase
{
	
    override void EEKilled( Object killer )
    {
		super.EEKilled(killer);
		
        ref Man KillerDude = killer;
		ref PlayerBase KillerPlayerBase = killer;
		ref PlayerIdentity killerIdentity = KillerPlayerBase.GetIdentity();
		ref PlayerIdentity killedIdentity = GetIdentity();
		
		string KillerName = killerIdentity.GetName();
		string KilledName = killedIdentity.GetName();
		
		float distance;
		int Rounded;
		string Message;
		
        if (GetGame().IsServer()) {
		    if (KillerDude.IsMan() && killerIdentity.GetId() != killedIdentity.GetId()) 
		    {
				distance = vector.Distance(GetPosition(), KillerDude.GetPosition());
				Rounded = Math.Round(distance);
				Message = KillerName + " Killed " + KilledName + " with " + "["+ KillerDude.GetHumanInventory().GetEntityInHands().GetDisplayName() +"]" + " ("+ Rounded.ToString() + "m" +")";
				GetGame().ChatPlayer(1, Message); //Global Chat
		    }
			else if (KillerDude.IsMan() && killerIdentity.GetId() == killedIdentity.GetId())  //SUICIDE
		    {
				Message = KilledName + " managed to kill himself";
				GetGame().ChatPlayer(1, Message); //Global Chat
		    }
        }
    }

}