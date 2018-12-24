modded class PlayerBase
{

	bool m_InRoundGameStatus = false;
	bool m_StillInRound = true;
	PlayerBase m_KilledBy;

	void SetGameStatus(bool inRound)
	{
		m_InRoundGameStatus = inRound;
	}

	bool IsStillInRound()
	{
		return m_StillInRound;
	}

	PlayerBase GetKilledBy()
	{
		return m_KilledBy;
	}
	
	override bool IsMan()
	{
		return true;
	}

    override void EEKilled( Object killer )
    {
		super.EEKilled(killer);

		Print("1");

		// || true for debug purposes
        if (m_InRoundGameStatus || true)
        {
		
            Print("2");
            m_StillInRound = false;
            Print("3");

            ref SurvivorBase SurvivorKilled = this;
            Print("4");
            ref SurvivorBase SurvivorKiller = killer;
            Print("5");
            ref Man KillerDude = killer;
            Print("6");

			Print("Killer is man: " + killer.IsMan().ToString());

            if (killer.IsMan() && GetGame().IsServer())
            {
				Print("7");
				string KillerName = KillerDude.GetIdentity().GetName();
				Print("8");
				string KilledName = GetIdentity().GetName();
                Print("9");
                if (SurvivorKiller.GetPlayerID() != SurvivorKilled.GetPlayerID())
                {
                    Print("10");
                    float distance = vector.Distance(SurvivorKilled.GetPosition(),SurvivorKiller.GetPosition());
                    Print("11");
                    string Message = KillerName + " killed " + KilledName + " [" + Math.Round(distance).ToString() + "m]";
                    Print("12");
                    GetGame().ChatPlayer(0, Message);
                    Print("13");
                    if (m_InRoundGameStatus)
                    {
                        Print("14");
                        ref PlayerBase KillerPlayerBase = killer;
                        Print("15");
                        m_KilledBy = KillerPlayerBase;
                        Print("16");
                    }
                }
                else if (SurvivorKiller.GetPlayerID() == SurvivorKilled.GetPlayerID())
                {
                    GetGame().ChatPlayer(0, KilledName + " managed to kill himself"); //Global Chat
                }
            }
        }
    }
	
	/*override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);
		
		// Let stamina always be 100
		GetStatStamina().SetMax(100);
	}*/
}