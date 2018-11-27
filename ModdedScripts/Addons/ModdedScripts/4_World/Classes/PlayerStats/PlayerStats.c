modded class PlayerStats
{
	override void Init(Man player)
	{
		m_PlayerStats 	= new array<ref PlayerStatBase>;
		m_Player 		= player;
		
		//																	min						max						initial					name				flag
		RegisterStat(EPlayerStats.HEATCOMFORT,  	new PlayerStat<float>	(-1,					1,						0,						"HeatComfort",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.TREMOR,		  	new PlayerStat<float>	(0,						1,						0,						"Tremor",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.WET,  			new PlayerStat<int>		(0,						1,						0,						"Wet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.ENERGY,  			new PlayerStat<float>	(0,						20000,					10000,					"Energy",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.WATER,  			new PlayerStat<float>	(0,						5000,					2500,					"Water",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.STOMACH_SOLID,  	new PlayerStat<float>	(0,						5000,					0,						"StomachSolid",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.STOMACH_ENERGY,  	new PlayerStat<float>	(0,						40000,					0,						"StomachEnergy",	EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.STOMACH_WATER,  	new PlayerStat<float>	(0,						5000,					0,						"StomachWater",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.DIET,  			new PlayerStat<float>	(0,						5000,					2500,					"Diet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.STAMINA,  		new PlayerStat<float>	(0,						STAMINA_MAX,			100,					"Stamina",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.SPECIALTY,  		new PlayerStat<float>	(-1,					1,						0,						"Specialty",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats.BLOODTYPE,  		new PlayerStat<int>		(0,						128,					BloodTypes.GenerateBloodType(),	"BloodType",		EPSstatsFlags.EMPTY) );
	}
};