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

	const float LOBBY_TIME = 10.0;
    const float INITIAL_RADIUS = 8000.0;
    ref array<ref array<float>> circleConf;

    float m_LastRoundTimeShown = 0.0;

	const vector SPAWN_ISLAND_CENTER = "2725 100 1275";
	const float SPAWN_ISLAND_MAX_DISTANCE = 200.0;

	vector center = "2700 100 10000";
	vector nextCenter = "2700 100 10000";
	int m_Phase = 1;
	GameStatus m_GameStatus = GameStatus.IN_LOBBY;
	float m_RoundTime = 0.0;
	int m_PlayersStartedRound;

	ref array<Man> m_PlayersInRound;
	ref set<Man> playersOutOfZone;

	const float PRINT_PLAYERS_ALIVE_INTERVAL = 30.0;
	float lastTimePlayersAlivePrinted = 0.0;

	void DayZSurvival()
	{
		Print("BATTLE ROYALE IS ALIVE!!");

		circleConf = {{480.0, 720.0},
                      {960.0, 1080.0},
                      {1320.0, 1500.0},
                      {1680.0, 1800.0},
                      {1920.0, 2040.0},
                      {2160.0, 2220.0},
                      {2280.0, 2340.0},
                      {2400.0, 2430.0},
                      {2520.0, 2550.0},
                      {2640.0, 2670.0},
                      {3000.0, 3012.0}};

		// DEBUG ZONES
        /*circleConf = {{8.0, 12.0},
                      {16.0, 18.0},
                      {22.0, 24.0},
                      {28.0, 30.0},
                      {32.0, 34.0},
                      {36.0, 37.0},
                      {38.0, 39.0},
                      {40.0, 40.5},
                      {42.0, 42.5},
                      {44, 44.5},
                      {50, 50.2}};*/
					  
		// ONLY FOR DEBUG PURPOSES, NEEDS TO BE REMOVED FOR PRODUCTION
		for (int i = 0; i < circleConf.Count(); i++)
		{
			ref array<float> subArr = circleConf.Get(i);
			for (int j = 0; j < circleConf.Get(i).Count(); j++)
			{
				subArr.Set(j, circleConf.Get(i).Get(j) / 12.0);
				
			}
			circleConf.Set(i, subArr);
		}

		for (int k = 0; k < 11; k++)
        {
		    Print("Zone " + (k + 1) + ": " + circleConf.Get(k).Get(0) + ", " + circleConf.Get(k).Get(1));
        }

		m_Modules = new set<ref ModuleManager>;
		widgetEventHandler = new CustomWidgetEventHandler;
		playersOutOfZone = new set<Man>;
		RegisterModules();
	}

	void ~DayZSurvival()
	{

	}

	void RegisterModules()
	{
		m_Modules.Insert(new ModTunables(this));

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
		}
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
			PlayerBase playerInList = m_PlayersInRound.Get(i);
			if (playerInList.GetIdentity().GetId() == player.GetIdentity().GetId())
			{
				return true;
			}
		}
		return false;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		ItemBase itemBs;
		EntityAI itemEnt;

		if (m_GameStatus == GameStatus.IN_LOBBY)
        {
            AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).SpawnGunIn(player, "M4A1", true, {"M68Optic"},
                    {"Mag_STANAG_30Rnd", "Mag_STANAG_30Rnd"});
        }
	}

	float Mod(float a, float b)
    {
	    while (a > b)
        {
	        a -= b;
        }
        return a;
    }

	private string FormatTime(float timeInSec)
    {
	    if (timeInSec < 60)
        {
            return Math.Round(timeInSec).ToString() + "s";
        }
        return Math.Floor(timeInSec / 60).ToString() + "m" + FormatTime(Mod(timeInSec, 60.0));
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

	// TODO: Zone should have 0 radius after last zone.
	private float GetRadius()
    {
	    if (m_RoundTime < circleConf.Get(m_Phase - 1).Get(0))
        {
            return Math.Pow(0.5, m_Phase - 1) * INITIAL_RADIUS;
        }
	    return Math.Lerp(Math.Pow(0.5, m_Phase - 1) * INITIAL_RADIUS, Math.Pow(0.5, m_Phase) * INITIAL_RADIUS,
	            (m_RoundTime - circleConf.Get(m_Phase - 1).Get(0)) / (circleConf.Get(m_Phase - 1).Get(1) - circleConf.Get(m_Phase - 1).Get(0)));
    }

	private float Get2dDistance(vector a, vector b)
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
		return (float) 2 * m_Phase * m_Phase;
	}

	private void DealDamageToPlayerOutsideOfZone(array<Man> players, float deltaTime)
	{
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = players.Get(i);
			
			if (PlayerIsInRound(player))
			{
				float radius = GetRadius();
				if (Get2dDistance(GetCenter(), players.Get(i).GetPosition()) > radius * radius)
				{
					players.Get(i).DecreaseHealth("GlobalHealth", "Blood", deltaTime * GetBloodDamagePerSec());
					if (!PlayerAlreadyOutOfZone(players.Get(i)))
					{
						Send(players.Get(i), "You are leaving the safe zone!")
						playersOutOfZone.Insert(players.Get(i));
					}
				}
				else
				{
					if (PlayerAlreadyOutOfZone(players.Get(i)))
					{
						Send(players.Get(i), "You have entered the safe zone again!")
						RemovePlayerFromOutOfZone(players.Get(i));
					}
				}
			}
		    else 
			{
				if (Get2dDistance(SPAWN_ISLAND_CENTER, players.Get(i).GetPosition()) > SPAWN_ISLAND_MAX_DISTANCE)
				{
					Send(players.Get(i), "Don't leave spawn island!");
					players.Get(i).SetHealth("GlobalHealth", "Health", 0);
				}
			}
		}
	}

	private void ShowLobbyStartsInMessage(float time)
    {
        GlobalMessage("Round starts in " + FormatTime(time));
        m_LastRoundTimeShown = m_RoundTime;
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

            if (m_StaminaStatus) {
                currentPlayer.GetStaminaHandler().SyncStamina(1000,1000);
                currentPlayer.GetStatStamina().Set(currentPlayer.GetStaminaHandler().GetStaminaCap());
            }
            if (GetModule(SafeZone)) { SafeZone.Cast(GetModule(SafeZone)).SafeZoneHandle(currentPlayer); }
            m_currentPlayer++;
        }


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
        }

        if (m_GameStatus == GameStatus.IN_ROUND)
        {
            DealDamageToPlayerOutsideOfZone(m_Players, timeslice);
            ManageCirclePhases();
			
			if (m_RoundTime - lastTimePlayersAlivePrinted > PRINT_PLAYERS_ALIVE_INTERVAL)
			{
				PrintPlayersAlive();
			}
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
            if (m_RoundTime - m_LastRoundTimeShown > 30) {
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
        offset[0] = offsetMultiplier * INITIAL_RADIUS * Math.Sin(randomAlpha);
		offset[2] = offsetMultiplier * INITIAL_RADIUS * Math.Cos(randomAlpha);

		vector pos;
		pos[0] = Math.Clamp(GetCenter()[0] + offset[0], 100, 12800);
		pos[2] = Math.Clamp(GetCenter()[2] + offset[2], 3000, 15000);
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		GlobalMessage(pos.ToString());
        return pos;
    }


	void StartRound()
    {
        m_GameStatus = GameStatus.IN_ROUND;
	    m_RoundTime = 0.0;
	    m_LastRoundTimeShown = 0.0;
	    m_PlayersStartedRound = m_Players.Count();
        center[0] = Math.RandomFloat(2000, 9000);
		center[2] = Math.RandomFloat(6000, 13000);
        nextCenter = center;
        GlobalMessage("Round has started. Center: <" + center[0].ToString() + ", " + center[2].ToString() + "> , Radius: " + GetRadius().ToString() + "m");
        for (int i = 0; i < m_PlayersStartedRound; i++)
        {
            PlayerBase player = m_Players.Get(i);
            SpawnRandomly(player);
            m_PlayersInRound.Insert(player);
        }
    }

	void Send(PlayerBase player, string message)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(message);
		rpc.Send(player, 1337133710, true, player.GetIdentity());
	}
}