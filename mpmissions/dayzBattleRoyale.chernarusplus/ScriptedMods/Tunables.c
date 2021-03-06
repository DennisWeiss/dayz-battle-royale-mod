/*
	The Following class allows you to enable/disable and modifiy mods.
*/
class ModTunables extends ModuleManager
{
	//============Mods============
	protected bool m_AdminTool         = false;   //Set to 'true' to enable admin tools
	protected bool m_AdvancedLoadouts  = false;	 //Set to true to Spawn Players with custom loadouts, set false to spawn with Vanilla DayZ gear ( check 'AdvancedLoadouts.c' for more settings )
	protected bool m_InfectedHordes	   = false;   //Random zombie hordes that spawn on the map (The tunables for this mod can be found in 'InfectedHordes.c')
	protected bool m_SafeZone		   = false;   //set 'true' to Enable Safe Zone
	//============================

	//==============Misc==========
	protected bool m_Debugmode 		   = false; //Set true to disable db for faster server loading and for testing purposes
	protected bool m_ProxyExportMode   = false; //Set to ture to export a new loot positions xml (export can be found in your Storage folder).
	protected bool m_StaminaStatus     = false;  //set to true to disable Stamina
	protected bool m_CustomBuildings   = false;  //Spawns the buildings you add in file BuildingSpawner.c
	protected bool m_SessionFeed	   = false;  //Set to true to enable Disconnect/Connect Messages
	//============================
	
	void ModTunables( DayZSurvival ServerMission )
	{
		
	}
	
	override void Init()
	{
		Print("ModTunables:: Init():: Loading Mod Settings...");
	}

	bool IsActiveMisc(string MiscName)
	{
		switch(MiscName)
		{
			case "Debugmode":
			return m_Debugmode;
			break;

			case "ProxyExportMode":
			return m_ProxyExportMode;
			break;

			case "StaminaStatus":
			return m_StaminaStatus;
			break;

			case "CustomBuildings":
			return m_CustomBuildings;
			break;

			case "SessionFeed":
			return m_SessionFeed;
			break;
		}
		return false;
	}
	
	bool IsActive(string ModName)
	{
		switch(ModName)
		{
			case "AdminTools":
			return m_AdminTool;
			break;

			case "AdvancedLoadouts":
			return m_AdvancedLoadouts;
			break;

			case "InfectedHordes":
			return m_InfectedHordes;
			break;

			case "SafeZone":
			return m_SafeZone;
			break;
		}
		return false;
	}
}
