#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\ModuleManager.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Tunables.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\AdminTool\\AdminTool.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\AdvancedLoadouts\\AdvancedLoadouts.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\Misc\\BuildingSpawner.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\SafeZone\\SafeZoneFunctions.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\ServerEvents\\InfectedHordes.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\UI\\CustomWidgetEventHandler.c"
#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\ServerMission\\GameStatus.c"
//#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\ServerMission\\CirclePhase.c"

//#include "$CurrentDir:\\mpmissions\\DayZBattleRoyale.chernarusplus\\ScriptedMods\\Modules\\Misc\\MOTDMessages.c"

class DayZSurvival : MissionServer
{
	private ref set<ref ModuleManager> m_Modules;
	ref InfectedHordes m_ZombieEvents;
	protected float m_LogInTimerLength = 1;     //in seconds the spawn timer when players login!
	bool m_StaminaStatus = false;
	ref CustomWidgetEventHandler widgetEventHandler;

	const float LOBBY_TIME = 60.0;
    const float INITIAL_RADIUS = 8000.0;
    ref array<ref array<float>> circleConf;

    float m_LastRoundTimeShown = 0.0;

	const vector SPAWN_ISLAND_CENTER = "2725 0 1275";
	const float SPAWN_ISLAND_MAX_DISTANCE = 200.0;

	vector center = "2700 0 10000";
	vector nextCenter = "2700 0 10000";
	int m_Phase = 8;
	GameStatus m_GameStatus = GameStatus.IN_LOBBY;
	float m_RoundTime = 0.0;
	int m_PlayersStartedRound;

	ref array<string> m_PlayersInRound;
	ref set<Man> playersOutOfZone;

	const float PRINT_PLAYERS_ALIVE_INTERVAL = 30.0;
	float lastTimePlayersAlivePrinted = 0.0;
	bool m_WinnerMessageShown = false;
	
	const float m_PhysicalZoneObjectDistance = 2.0;
	ref array<Object> m_PhysicalZone;

	void DayZSurvival()
	{
		Print("BATTLE ROYALE IS ALIVE!!");

		circleConf = {{480.0, 720.0},
                      {960.0, 1080.0},
                      {1320.0, 1410.0},
                      {1680.0, 1740.0},
                      {1920.0, 1965.0},
                      {2100.0, 2130.0},
                      {2220.0, 2250.0},
                      {2310.0, 2325.0},
                      {2370.0, 2380.0},
                      {2425.0, 2435.0},
                      {2500.0, 2508.0}};
					  
		// ONLY FOR DEBUG PURPOSES, NEEDS TO BE REMOVED FOR PRODUCTION
		/*for (int i = 0; i < circleConf.Count(); i++)
		{
			ref array<float> subArr = circleConf.Get(i);
			for (int j = 0; j < circleConf.Get(i).Count(); j++)
			{
				subArr.Set(j, circleConf.Get(i).Get(j) / 12.0);
				
			}
			circleConf.Set(i, subArr);
		}*/

		m_Modules = new set<ref ModuleManager>;
		widgetEventHandler = new CustomWidgetEventHandler;
		playersOutOfZone = new set<Man>;
		m_PlayersInRound = new array<string>;
		RegisterModules();
		
		m_PhysicalZone = new array<Object>;
	}

	void ~DayZSurvival()
	{

	}

	void RegisterModules()
	{
		/*m_Modules.Insert(new ModTunables(this));

		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("AdminTools"))
		{
			m_Modules.Insert(new AdminTool(this));
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("AdvancedLoadouts"))
		{
			m_Modules.Insert(new AdvancedLoadouts(this));
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("SafeZone"))
		{
			m_Modules.Insert(new SafeZone(this));
		}*/
	}

	void InitModules()
	{
		for ( int i = 0; i < m_Modules.Count(); ++i)
		{
			m_Modules.Get(i).Init();
		}
	}

	ModuleManager GetModule(typename moduleType)
	{
		for ( int i = 0; i < m_Modules.Count(); ++i)
		{
			ModuleManager module = m_Modules.Get(i);
			if (module.GetModuleType() == moduleType)
			{
				return module;
			}
		}
		return NULL;
	}


	override void OnInit()
	{
		super.OnInit();
		InitModules();

		if (!ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("Debugmode"))
		{
			Hive ce = CreateHive();
			if (ce)
			ce.InitOffline();
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("ProxyExportMode"))
		{
			CETesting TestHive = GetTesting();
			TestHive.ExportProxyProto();
			TestHive.ExportProxyData( "7500 0 7500", 15000 );
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("SessionFeed"))
		{
			g_Game.SetProfileString("SessionFeed", "true");
		}
		else
		{
			g_Game.SetProfileString("SessionFeed", "false");
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("CustomBuildings"))
		{
			ref BuildingSpawner bldspnwer = new BuildingSpawner;
			bldspnwer.Init();
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActiveMisc("StaminaStatus"))
		{
			m_StaminaStatus = true; //Disable Stamina
		}

		if (ModTunables.Cast(GetModule(ModTunables)).IsActive("InfectedHordes"))
		{
			m_ZombieEvents = new InfectedHordes;
		}

		//-----------
		//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.PlayerCounter, 110000, true);  //Default 120000 2 mins Looped
		//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.CustomMOTD, TIME_INTERVAL, true);  //Default 120000 2 mins Looped
		//-----------
	}

	override void OnPreloadEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int queueTime)
	{
		if (GetHive())
		{
			useDB = true;
			queueTime = m_LogInTimerLength;
		}
		else
		{
			useDB = false;
			pos = "7500 0 7500";
			yaw = 0;
			queueTime = m_LogInTimerLength;
		}
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;

		playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
		Class.CastTo(m_player, playerEnt);

		GetGame().SelectPlayer(identity, m_player);
		return m_player;
	}

	override void OnEvent(EventType eventTypeId, Param params)
	{
		super.OnEvent(eventTypeId,params);

		/*switch(eventTypeId)
		{
			case ChatMessageEventTypeID:
				 ChatMessageEventParams chat_params = ChatMessageEventParams.Cast(params);
				 if (chat_params.param1 == 0 && chat_params.param2 != "") //trigger only when channel is Global == 0 and Player Name does not equal to null
					{
						Param4<int,string,string,string> request_info = new Param4<int,string,string,string>(chat_params.param1, chat_params.param2, chat_params.param3, chat_params.param4);
						AdminTool.Cast(GetModule(AdminTool)).RequestHandler(request_info); //Send the param to Admintools
					}
			break;
		}*/
	}

	void GlobalMessage(string Message)
	{
	    if (Message != "")
		{
			GetGame().ChatPlayer(0, Message);
		}
	}

	void PlayerCounter()
	{
		array<Man> players = new array<Man>;
	    GetGame().GetPlayers( players );
	    int numbOfplayers = players.Count();
	    GlobalMessage("Online Players: "+ numbOfplayers.ToString());
	}
	
	bool PlayerIsInRound(PlayerBase player)
	{
		for (int i = 0; i < m_PlayersInRound.Count(); i++)
		{
			if (m_PlayersInRound.Get(i) == player.GetIdentity().GetId())
			{
				return true;
			}
		}
		return false;
	}

	PlayerBase GetPlayerById(string id)
    {
	    for (int i = 0; i < m_Players.Count(); i++)
        {
	        PlayerBase player = PlayerBase.Cast(m_Players.Get(i));
	        PlayerIdentity playerIdentity = player.GetIdentity();
	        if (id == playerIdentity.GetId())
            {
	            return player;
            }
        }
        return NULL;
    }
	
	private void UpdatePlayersInRound()
	{
		for (int i = 0; i < m_PlayersInRound.Count(); i++)
		{
			PlayerBase player = GetPlayerById(m_PlayersInRound.Get(i));
			if (!player.IsStillInRound())
			{
				// THIS STILL NEEDS TO BE TESTED!!!
				m_PlayersInRound.RemoveOrdered(i);
				i--;
			}
		}
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		ItemBase itemBs;
		EntityAI itemEnt;

		player.GetInventory().CreateInInventory("ChernarusMap");

		/*if (m_GameStatus == GameStatus.IN_LOBBY)
        {
            AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).SpawnGunIn(player, "M4A1", true, {"M68Optic"},
                    {"Mag_STANAG_30Rnd", "Mag_STANAG_30Rnd"});
        }*/
	}

	int Mod(int a, int b)
    {
	    while (a >= b)
        {
	        a -= b;
        }
        return a;
    }

	private string FormatTime(int timeInSec)
    {
	    if (timeInSec < 10)
        {
	        return "0" + timeInSec.ToString() + "s";
        }
	    if (timeInSec < 60)
        {
            return timeInSec.ToString() + "s";
        }
        return Math.Floor(timeInSec / 60).ToString() + "m" + FormatTime(Mod(timeInSec, 60));
    }

    private vector LerpVector(vector a, vector b, float time)
    {
	    vector c = a;
	    for (int i = 0; i < 3; i++)
        {
            c[i] = Math.Lerp(a[i], b[i], time);
        }
	    return c;
    }

    private vector GetCenter()
    {
        if (m_RoundTime > circleConf.Get(m_Phase - 1).Get(0))
        {
            return LerpVector(center, nextCenter,
				(m_RoundTime - circleConf.Get(m_Phase - 1).Get(0)) / (circleConf.Get(m_Phase - 1).Get(1) - circleConf.Get(m_Phase - 1).Get(0)));
        }
        return center;
    }

	private float GetRadius()
    {
	    if (m_RoundTime < circleConf.Get(m_Phase - 1).Get(0))
        {
            return Math.Pow(0.5, m_Phase - 1) * INITIAL_RADIUS;
        }
		if (m_Phase == circleConf.Count())
		{
			if (m_RoundTime > circleConf.Get(m_Phase - 1).Get(1))
			{
				return 0;
			}
			return Math.Lerp(Math.Pow(0.5, m_Phase - 1) * INITIAL_RADIUS, 0,
				(m_RoundTime - circleConf.Get(m_Phase - 1).Get(0)) / (circleConf.Get(m_Phase - 1).Get(1) - circleConf.Get(m_Phase - 1).Get(0)));
		}
	    return Math.Lerp(Math.Pow(0.5, m_Phase - 1) * INITIAL_RADIUS, Math.Pow(0.5, m_Phase) * INITIAL_RADIUS,
	            (m_RoundTime - circleConf.Get(m_Phase - 1).Get(0)) / (circleConf.Get(m_Phase - 1).Get(1) - circleConf.Get(m_Phase - 1).Get(0)));
    }

	private float Get2dDistanceSquared(vector a, vector b)
	{
		return (a[0] - b[0]) * (a[0] - b[0]) + (a[2] - b[2]) * (a[2] - b[2]);
	}

	private bool PlayerAlreadyOutOfZone(Man player)
	{
		for (int i = 0; i < playersOutOfZone.Count(); i++)
		{
			if (playersOutOfZone.Get(i).GetID() == player.GetID())
			{
				return true;
			}
		}
		return false;
	}

	private bool RemovePlayerFromOutOfZone(Man player)
	{
		for (int i = 0; i < playersOutOfZone.Count(); i++)
		{
			if (playersOutOfZone.Get(i).GetID() == player.GetID())
			{
				playersOutOfZone.Remove(i);
				return true;
			}
		}
		return false;
	}

	private float GetBloodDamagePerSec()
	{
		return (float) m_Phase * m_Phase;
	}

	private void DealDamageToPlayerOutsideOfZone(array<Man> players, float deltaTime)
	{
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = players.Get(i);
			
			if (PlayerIsInRound(player))
			{
				float radius = GetRadius();
				if (Get2dDistanceSquared(GetCenter(), players.Get(i).GetPosition()) > radius * radius)
				{
					players.Get(i).DecreaseHealth("GlobalHealth", "Blood", deltaTime * GetBloodDamagePerSec());
					if (!PlayerAlreadyOutOfZone(players.Get(i)))
					{
						Send(players.Get(i), "You are leaving the safe zone!", "colorImportant");
						playersOutOfZone.Insert(players.Get(i));
						SendOutOfZone(player, true);
					}
				}
				else
				{
					if (PlayerAlreadyOutOfZone(players.Get(i)))
					{
						Send(players.Get(i), "You have entered the safe zone again!", "colorFriendly");
						RemovePlayerFromOutOfZone(players.Get(i));
						SendOutOfZone(player, false);
					}
				}
			}
		}
	}
	
	private bool IsOutOfSpawnZone(PlayerBase player)
	{
		return Get2dDistanceSquared(SPAWN_ISLAND_CENTER, player.GetPosition()) > SPAWN_ISLAND_MAX_DISTANCE * SPAWN_ISLAND_MAX_DISTANCE;
	}
	
	private bool ShouldBeKilledBecauseOutOfSpawnZone(PlayerBase player)
	{
		return IsOutOfSpawnZone(player) && (m_GameStatus == GameStatus.IN_LOBBY || m_GameStatus == GameStatus.IN_ROUND && !PlayerIsInRound(player)) && player.GetHealth("GlobalHealth", "Health") > 0;
	}
	
	private void KillPlayersOutOfSpawnZone(array<Man> players)
	{
		for (int i = 0; i < players.Count(); i++)
		{
			if (ShouldBeKilledBecauseOutOfSpawnZone(players.Get(i)))
			{
				Send(players.Get(i), "Don't leave spawn island!", "colorImportant");
				players.Get(i).SetHealth("GlobalHealth", "Health", 0);
			}
		}
	}

	private void ShowLobbyStartsInMessage(float time)
    {
        GlobalMessage("Round starts in " + FormatTime(Math.Round(time)));
        m_LastRoundTimeShown = m_RoundTime;
    }
	
	private void SetGameStatusOfPlayers()
	{
		for (int i = 0; i < m_Players.Count(); i++)
		{
			PlayerBase currentPlayer = PlayerBase.Cast(m_Players.Get(i));
			currentPlayer.SetGameStatus(m_GameStatus == GameStatus.IN_ROUND);
		}
	}

	void GivePlayerChickenDinner(PlayerBase player)
	{
	    for (int i = 0; i < 5; i++)
        {
            player.GetInventory().CreateInInventory("ChickenBreastMeat");
        }
	}
	
	void CheckIfWon()
	{
		if (m_PlayersInRound.Count() == 1 && !m_WinnerMessageShown)
		{
			PlayerBase player = GetPlayerById(m_PlayersInRound.Get(0));
		    GlobalMessage(player.GetIdentity().GetName() + " - WINNER, WINNER, CHICKEN DINNER!!!");
		    GivePlayerChickenDinner(player);
			m_WinnerMessageShown = true;
		}
	}

	private void CheckIfKilled()
	{
		for (int i = 0; i < m_PlayersInRound.Count(); i++)
		{
			PlayerBase player = GetPlayerById(m_PlayersInRound.Get(i));
			PlayerBase killedBy = player.GetKilledBy();
			if (killedBy != NULL)
			{
				SendKillMessage(killedBy);
				m_PlayersInRound.RemoveOrdered(i);
				i--;
			}
		}
	}

	private void SendZoneToAllPlayersInRound()
	{
		for (int i = 0; i < m_Players.Count(); i++)
		{
			PlayerBase player = m_Players.Get(i);
			SendZone(player);
			SendNextZone(player);
		}
	}

	override void TickScheduler(float timeslice)
    {

        GetGame().GetWorld().GetPlayerList(m_Players);
        if( m_Players.Count() == 0 )
        {
            m_RoundTime = 0.0;
            return;
        }

        m_RoundTime += timeslice;

        for(int i = 0; i < SCHEDULER_PLAYERS_PER_TICK; i++)
        {
            if(m_currentPlayer >= m_Players.Count() )
            {
                m_currentPlayer = 0;
            }

            PlayerBase currentPlayer = PlayerBase.Cast(m_Players.Get(m_currentPlayer));
            currentPlayer.OnTick();

            if (m_StaminaStatus)
            {
                currentPlayer.GetStaminaHandler().SyncStamina(1000,1000);
                currentPlayer.GetStatStamina().Set(currentPlayer.GetStaminaHandler().GetStaminaCap());
            }
            if (GetModule(SafeZone)) { SafeZone.Cast(GetModule(SafeZone)).SafeZoneHandle(currentPlayer); }
            m_currentPlayer++;
        }

		SetGameStatusOfPlayers();

	    if (m_GameStatus == GameStatus.IN_LOBBY)
        {
            if (m_RoundTime - m_LastRoundTimeShown > 10)
            {
                ShowLobbyStartsInMessage(LOBBY_TIME - m_RoundTime);
            }
	        if (m_RoundTime > LOBBY_TIME)
            {
                StartRound();
            }
			KillPlayersOutOfSpawnZone(m_Players);
        }

        if (m_GameStatus == GameStatus.IN_ROUND)
        {
            UpdatePlayersInRound();
            DealDamageToPlayerOutsideOfZone(m_Players, timeslice);
            ManageCirclePhases();
			
			if (m_RoundTime - lastTimePlayersAlivePrinted > PRINT_PLAYERS_ALIVE_INTERVAL)
			{
				PrintPlayersAlive();
			}

			CheckIfKilled();
			CheckIfWon();
			SendZoneToAllPlayersInRound();
			UpdatePhysicalZone();
        }
	}
	
	void PrintPlayersAlive()
	{
		GlobalMessage("Players Alive: " + m_PlayersInRound.Count() + "/" + m_PlayersStartedRound);
		lastTimePlayersAlivePrinted = m_RoundTime;
	}

	private vector GenerateNextCenter(int circlePhase)
    {
	    float randomAlpha = Math.RandomFloat(0, 2 * Math.PI);
	    float randomOffsetMultiplier = Math.RandomFloat(0, Math.Pow(0.5, circlePhase) * INITIAL_RADIUS);
	    vector offset = "0 0 0";
	    offset[0] = randomOffsetMultiplier * Math.Sin(randomAlpha);
	    offset[2] = randomOffsetMultiplier * Math.Cos(randomAlpha);
	    Print("offset: " + offset.ToString());
	    return center + offset;
    }

	void NextCirclePhase()
    {
        m_Phase++;
        center = nextCenter;
        nextCenter = GenerateNextCenter(m_Phase);
        GlobalMessage("Zone has been updated!");
    }

	private void ManageCirclePhases()
    {
	    if (m_RoundTime > circleConf.Get(m_Phase - 1).Get(1))
        {
	        if (m_Phase < circleConf.Count())
            {
	            NextCirclePhase();
            }
        }
        else if (m_RoundTime < circleConf.Get(m_Phase - 1).Get(0))
        {
            if (m_RoundTime - m_LastRoundTimeShown > 30)
            {
                GlobalMessage("Zone starts to shrink in " + FormatTime(Math.Round(circleConf.Get(m_Phase - 1).Get(0) - m_RoundTime)));
                m_LastRoundTimeShown = m_RoundTime;
            }
        }
    }

	private void SpawnRandomly(PlayerBase player)
    {
	    player.SetPosition(GenerateRandomVectorBasedOnZone());
	    player.SetHealth("GlobalHealth", "Blood", 5000);
	    player.SetHealth("GlobalHealth", "Health", 5000);
	    player.SetHealth("GlobalHealth", "Shock", 5000);
    }

    private vector GenerateRandomVectorBasedOnZone()
    {
        float randomAlpha = Math.RandomFloat(0, 2 * Math.PI);
        float offsetMultiplier = Math.RandomFloat(0, 0.75);

        vector offset;
        offset[0] = offsetMultiplier * GetRadius() * Math.Sin(randomAlpha);
		offset[2] = offsetMultiplier * GetRadius() * Math.Cos(randomAlpha);

		vector pos;
		pos[0] = Math.Clamp(GetCenter()[0] + offset[0], 100, 12800);
		pos[2] = Math.Clamp(GetCenter()[2] + offset[2], 3000, 15000);
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]) + 0.3;
        return pos;
    }
	
	private vector GetPhysicalZoneObjectPosition(vector center, float radius, int index, float numericalTotalObjects)
	{
		vector offset = "0 0 0";
		float alpha = (float) index / numericalTotalObjects * 2 * Math.PI
		offset[0] = radius * Math.Sin(alpha);
		offset[2] = radius * Math.Cos(alpha);
		vector pos = "0 0 0";
		pos = center + offset;
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]) + 5;
		return pos;
	}
	
	int GetNumberOfPhysicalZoneObjects(float radius)
	{
		return (int) Math.Round(2 * Math.PI * radius / m_PhysicalZoneObjectDistance);
	}
	
	float GetNumerialcNumberOfPhysicalObjects(float radius)
	{
		return 2 * Math.PI * radius / m_PhysicalZoneObjectDistance;
	}
	
	void PlacePhysicalZone()
	{
		float radius = GetRadius();
		int numberOfObjects = GetNumberOfPhysicalZoneObjects(radius);
		float numericalNumberOfObjects = GetNumerialcNumberOfPhysicalObjects(radius);
		for (int i = 0; i < numberOfObjects; i++)
		{
			vector pos = GetPhysicalZoneObjectPosition(GetCenter(), radius, i, numericalNumberOfObjects);
			m_PhysicalZone.Insert(GetGame().CreateObject("LargeTent", pos));
		}
	}
	
	void UpdatePhysicalZone()
	{
		float radius = GetRadius();
		int numberOfObjects = GetNumberOfPhysicalZoneObjects(radius);
		float numericalNumberOfObjects = GetNumerialcNumberOfPhysicalObjects(radius);
		
		if (numberOfObjects < m_PhysicalZone.Count())
		{
			for (int i = m_PhysicalZone.Count() - 1; i >= numberOfObjects; i--)
			{
				GetGame().ObjectDelete(m_PhysicalZone.Get(i));
				m_PhysicalZone.Remove(i);
			}
		} 
		else if (numberOfObjects > m_PhysicalZone.Count())
		{
			for (int j = m_PhysicalZone.Count(); j < numberOfObjects; j++)
			{
				vector pos = GetPhysicalZoneObjectPosition(GetCenter(), radius, j, numericalNumberOfObjects);
				m_PhysicalZone.Insert(GetGame().CreateObject("LargeTent", pos));
			}
		}
		
		for (int k = 0; k < m_PhysicalZone.Count(); k++)
		{
			m_PhysicalZone.Get(k).SetPosition(GetPhysicalZoneObjectPosition(GetCenter(), radius, k, numericalNumberOfObjects));
		}
	}

	void StartRound()
    {
        m_GameStatus = GameStatus.IN_ROUND;
	    m_RoundTime = 2330.0;
	    m_LastRoundTimeShown = 0.0;
	    m_PlayersStartedRound = m_Players.Count();
        center[0] = Math.RandomFloat(4000, 9000);
		center[2] = Math.RandomFloat(6000, 13000);
        nextCenter = center;
        GlobalMessage("Round has started.");
        for (int i = 0; i < m_PlayersStartedRound; i++)
        {
            PlayerBase player = m_Players.Get(i);
            SpawnRandomly(player);
            m_PlayersInRound.Insert(player.GetIdentity().GetId());
        }
		PlacePhysicalZone();
    }

	void Send(PlayerBase player, string message, string type)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(message);
		rpc.Write(type);
		rpc.Send(player, 1337133710, true, player.GetIdentity());
	}

	void SendKillMessage(PlayerBase player)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Send(player, 1337133711, true, player.GetIdentity());
	}

	void SendZone(PlayerBase player)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(GetCenter());
		rpc.Write(GetRadius());
		rpc.Send(player, 1337133712, true, player.GetIdentity());
	}

	void SendNextZone(PlayerBase player)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(nextCenter);
		rpc.Write(Math.Pow(0.5, m_Phase) * INITIAL_RADIUS);
		rpc.Send(player, 1337133713, true, player.GetIdentity());
	}

	void SendOutOfZone(PlayerBase player, bool outOfZone)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(outOfZone);
		rpc.Send(player, 1337133714, true, player.GetIdentity());
	}

}