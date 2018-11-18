static string FolderLocation = "$profile:PlayerStats\\";

class ScoreTrackerHandle
{
    ref map<string, map<string, string>> JsonData = new map<string, map<string, string>>();
	
	//------------------------------------------
    map<string, map<string, string>> LoadData(string GUID)
    {
        ref map<string, map<string, string>> data = new map<string, map<string, string>>();
        string loadProfilePath = FolderLocation + GUID + ".json";

        if (FileExist("$profile:PlayerStats"))
        {
        	if (FileExist(loadProfilePath) )
			{
		        JsonFileLoader<map<string, map<string, string>>>.JsonLoadFile(loadProfilePath, data);
		    } 
			else 
			{
		        JsonFileLoader<map<string, map<string, string>>>.JsonSaveFile(loadProfilePath, data);
		    }
        }
        else
        {
        	MakeDirectory("$profile:PlayerStats");
        }
        return data;
		Print("LoadData:: "+data);
    }
	//------------------------------------------
	void PlayerConnectHandler(string GUID, string name)
    {
        JsonData = LoadData(GUID);

        ref map<string, string> KillData = new map<string, string>();

        KillData.Set("Name",name);

        JsonData.Insert(GUID, KillData);
        Save(JsonData, GUID);
    }
	//------------------------------------------

    void KillHandler(string GUID)
    {
        JsonData = LoadData(GUID);

        ref map<string, string> KillData = new map<string, string>();

        if (JsonData.Contains(GUID)) 
        {
            KillData = JsonData.Get(GUID);
        	if (KillData.Contains("Kills")) 
        	{
                int killCount = KillData.Get("Kills").ToInt();
                killCount = killCount + 1;
                KillData.Set("Kills", killCount.ToString());
            } 
            else 
            {
                KillData.Set("Kills", "1");
            }
        } 
        else 
        {
            KillData.Set("Kills", "1");
        }

        JsonData.Insert( GUID, KillData );
        Save( JsonData, GUID );
    }
	
    void DeathHandler(string GUID)
    {
        JsonData = LoadData(GUID);
        ref map<string, string> KillData = new map<string, string>();

        if (JsonData.Contains(GUID)) {

         KillData = JsonData.Get(GUID);
         if ( KillData.Contains("Deaths") ) {
             int DeathCount = KillData.Get("Deaths").ToInt();
             DeathCount = DeathCount + 1;
             KillData.Set("Deaths", DeathCount.ToString());
          } 
		  else 
		  {
             KillData.Set("Deaths", "1");
          }
        } 
		else 
		{
            KillData.Set("Deaths", "1");
        }

        JsonData.Insert(GUID, KillData);
        Save( JsonData, GUID);
    }

    void Save(map<string, map<string, string>> JsonData, string GUID)
    {
    	string saveProfilePath = FolderLocation + GUID + ".json";
        JsonFileLoader<map<string, map<string, string>>>.JsonSaveFile(saveProfilePath, JsonData);
    }
}
