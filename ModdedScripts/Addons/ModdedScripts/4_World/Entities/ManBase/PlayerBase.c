modded class PlayerBase
{
	
	bool m_InRoundGameStatus = false;
	bool m_StillInRound = true;
	int m_Kills = 0;
	func m_OnKilledInRound;
	
	private void Send(string message)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(message);
		rpc.Send(this, 1337133710, true, GetIdentity());
	}
	
	void SetGameStatus(bool inRound)
	{
		m_InRoundGameStatus = inRound;
	}
	
	bool IsStillInRound()
	{
		return m_StillInRound;
	}
	
	void KilledPlayer(string killedName)
	{
		m_Kills++;
		string killString;
		if (m_Kills = 1)
		{
			killString = " kill";
		}
		else {
			killString = " kills";
		}
		Send("You killed " + killedName + " - " + m_Kills.ToString() + killString);
	}
	
	void SetOnKilledInRound(func OnKilledInRound)
	{
		m_OnKilledInRound = OnKilledInRound;
	}
	
    override void EEKilled( Object killer )
    {
		super.EEKilled(killer);
		
        ref Man KillerDude = killer;
		ref PlayerBase KillerPlayerBase = killer;
		ref PlayerIdentity killerIdentity = KillerPlayerBase.GetIdentity();
		ref PlayerIdentity killedIdentity = GetIdentity();
		
		string KillerName = killerIdentity.GetName();
		string KilledName = killedIdentity.GetName();
		
		if (m_InRoundGameStatus)
		{
			m_StillInRound = false;
			m_OnKilledInRound(killer, this);
		}
		
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