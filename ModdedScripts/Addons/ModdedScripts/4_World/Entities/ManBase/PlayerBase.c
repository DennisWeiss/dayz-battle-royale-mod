modded class PlayerBase
{
	
	bool m_InRoundGameStatus = false;
	bool m_StillInRound = true;
	
	void SetGameStatus(bool inRound)
	{
		m_InRoundGameStatus = inRound;
	}
	
	bool IsStillInRound()
	{
		return m_StillInRound;
	}
	
    override void EEKilled( Object killer )
    {
		super.EEKilled(killer);
		
		if (m_InRoundGameStatus)
		{
			m_StillInRound = false;
		}
		
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
				Message = KillerName + " killed " + KilledName + " with " + "["+ KillerDude.GetHumanInventory().GetEntityInHands().GetDisplayName() +"]" + " ("+ Rounded.ToString() + "m" +")";
				GetGame().ChatPlayer(0, Message); //Global Chat
		    }
			else if (KillerDude.IsMan() && killerIdentity.GetId() == killedIdentity.GetId())  //SUICIDE
		    {
				Message = KilledName + " managed to kill himself";
				GetGame().ChatPlayer(0, Message); //Global Chat
		    }
        }
    }
}