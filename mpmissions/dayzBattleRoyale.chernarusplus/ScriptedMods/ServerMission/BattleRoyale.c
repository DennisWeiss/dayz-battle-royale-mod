
class BattleRoyale : MissionServer {

    vector center = "2700 100 10000";
    float radius = 40.0;

    ref set<Man>
    playersOutOfZone;

    void BattleRoyale() {
        Print("BATTLE ROYALE IS ALIVE!!");
        playersOutOfZone = new set<Man>;
    }

    void ~BattleRoyale() {

    }

    override void OnInit() {
        super.OnInit();
    }

    override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
    {
        Entity playerEnt;

        playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
        Class.CastTo(m_player, playerEnt);

        GetGame().SelectPlayer(identity, m_player);
        return m_player;
    }

    void GlobalMessage(string Message) {
        if (Message != "") {
            GetGame().ChatPlayer(0, Message);
        }
    }

    override void StartingEquipSetup(PlayerBase player, bool clothesChosen) {
        ItemBase itemBs;
        EntityAI itemEnt;

//        AdvancedLoadouts.Cast(GetModule(AdvancedLoadouts)).SpawnGunIn(player, "M4A1", true, {"M68Optic"},
//                                                                      {"Mag_STANAG_30Rnd", "Mag_STANAG_30Rnd"});
    }

    private float Get2dDistance(vector a, vector b) {
        return (a[0] - b[0]) * (a[0] - b[0]) + (a[2] - b[2]) * (a[2] - b[2]);
    }

    private bool PlayerAlreadyOutOfZone(Man player) {
        for (int i = 0; i < playersOutOfZone.Count(); i++) {
            if (playersOutOfZone.Get(i).GetID() == player.GetID()) {
                return true;
            }
        }
        return false;
    }

    private bool RemovePlayerFromOutOfZone(Man player) {
        for (int i = 0; i < playersOutOfZone.Count(); i++) {
            if (playersOutOfZone.Get(i).GetID() == player.GetID()) {
                playersOutOfZone.Remove(i);
                return true;
            }
        }
        return false;
    }

    private void DealDamageToPlayerOutsideOfZone(array <Man> players, float deltaTime) {
        for (int i = 0; i < players.Count(); i++) {
            if (Get2dDistance(center, players.Get(i).GetPosition()) > radius * radius) {
                players.Get(i).DecreaseHealth("GlobalHealth", "Blood", deltaTime * 20);
				GlobalMessage(players.Get(i).GetName() + " has " + players.Get(i).GetHealth("GlobalHealth", "Blood") + " blood.");
                if (!PlayerAlreadyOutOfZone(players.Get(i))) {
                    GlobalMessage(players.Get(i).GetName() + " is leaving zone!");
                    playersOutOfZone.Insert(players.Get(i));
                }
            } else {
                if (PlayerAlreadyOutOfZone(players.Get(i))) {
                    RemovePlayerFromOutOfZone(players.Get(i));
                }
            }
        }
    }

    override void TickScheduler(float timeslice) {
        GetGame().GetWorld().GetPlayerList(m_Players);

        DealDamageToPlayerOutsideOfZone(m_Players, timeslice);
    }
}