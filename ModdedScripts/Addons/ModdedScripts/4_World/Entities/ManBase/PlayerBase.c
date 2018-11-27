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

    override void EEKilled( Object killer )
    {
		super.EEKilled(killer);

		Print("1");

        if (m_InRoundGameStatus)
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

            string KillerName = SurvivorKiller.GetPlayerName();
            Print("7");
            string KilledName = SurvivorKilled.GetPlayerName();
            Print("8");

            if (GetGame().IsServer())
            {
                Print("9");
                if (KillerDude.IsMan() && SurvivorKiller.GetPlayerID() != SurvivorKilled.GetPlayerID())
                {
                    Print("10");
                    float distance = vector.Distance(SurvivorKilled.GetPosition(),SurvivorKiller.GetPosition());
                    Print("11");
                    string Message = KillerName + " killed " + KilledName + " with " + SurvivorKiller.GetHumanInventory().GetEntityInHands().GetDisplayName() + " [" + Math.Round(distance).ToString() + "m]";
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
                else if (KillerDude.IsMan() && SurvivorKiller.GetPlayerID() == SurvivorKilled.GetPlayerID())
                {
                    GetGame().ChatPlayer(0, KilledName + " managed to kill himself"); //Global Chat
                }
            }
        }
    }
	
	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);
		
		// Let stamina always be 100
		GetStatStamina().SetMax(100);
	}
}