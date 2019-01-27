#include "ReplayParser.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include "helper.h"
#include "sc2api/sc2_data.h"
#include <filesystem>

#ifndef TRACK_HISTORY
#define TRACK_HISTORY 0
#endif

#define ADD_VALUE(PROPERTYNAME, VALUE) #PROPERTYNAME << ": \"" << VALUE << "\""

ReplayParser::ReplayParser(int id) : mId(id), mFirstStep(false), mEnded(false), mLastFeatures() {
    std::ostringstream stream;
    stream << ExePath() << "\\output\\data_" << id << "\\";
    mOutputFolder = stream.str();
    stream.str(std::string());
    stream << ExePath() << "\\output\\analized_" << id << ".txt";
    mAnalizedDataFile = stream.str();
	
	//---------------------------------------
	//Terran
	//---------------------------------------
	
	
    // mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_SCV);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_COMMANDCENTER);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_ORBITALCOMMAND);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_PLANETARYFORTRESS);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_BARRACKS);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_BARRACKSFLYING);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_FACTORY);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_FACTORYFLYING);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_STARPORT);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_STARPORTFLYING);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_ARMORY);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_FUSIONCORE);
    mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::TERRAN_GHOSTACADEMY);
	
	/*
	//---------------------------------------
	//Protoss
	//---------------------------------------
	
	
	// Protoss Structure Units
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_DARKSHRINE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_FLEETBEACON);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_FORGE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_GATEWAY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_NEXUS);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_PHOTONCANNON);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_ROBOTICSBAY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_SHIELDBATTERY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_STARGATE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_WARPGATE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_PYLON);
	*/
}

ReplayParser::~ReplayParser() {}

void ReplayParser::OnGameStart() {
    const auto obs = Observation();
    const GameInfo& gameInfo = obs->GetGameInfo();
    mCurrentMapName = gameInfo.map_name;
    mStream.str(std::string());
    mRawActionStream.str(std::string());
    mActionStream.str(std::string());
    mFirstStep = true;
    mEnded = false;
    mBuildOrdersFromActions.clear();
    mBuildOrdersFromOnCreated.clear();
    mSeenUnits.clear();
    auto replayPath = ReplayControl()->GetReplayInfo().replay_path;
    const std::string::size_type pos1 = replayPath.find_last_of("\\/");
    const std::string::size_type pos2 = replayPath.find_last_of('.');
    mCurrentReplayName = replayPath.substr(pos1 + 1, pos2 - pos1 - 1);
    mProcessedSteps = 0;
    memset(mCurrentFeatures, 0, sizeof(mCurrentFeatures));
    memset(mLastFeatures, 0, sizeof(mLastFeatures));
}

void ReplayParser::OnGameEnd() {
    mEnded = true;
    if (mStream.str().size() < 50 || mProcessedSteps < 5000) {
        std::cout << "Output too small" << mStream.str() << std::endl;
        const auto replayPath = ReplayControl()->GetReplayInfo().replay_path;
        std::ofstream infoFile;
        infoFile.open(mAnalizedDataFile, std::ios_base::app);
#ifdef _DEBUG
        //assert(infoFile.is_open());
#endif
        infoFile << replayPath << std::endl;
        infoFile.close();
        return;
    }

    std::ofstream myfile;
    myfile.open(mOutputFolder + mCurrentReplayName + ".json", std::ios_base::out);
#ifdef _DEBUG
    //assert(myfile.is_open());
#endif
    myfile << "{\n";
    myfile << ADD_VALUE("replayname", mCurrentReplayName) << ",\n";
    myfile << ADD_VALUE("mapname", mCurrentMapName) << ",\n";
    const ReplayInfo& replayInfo = ReplayControl()->GetReplayInfo();
    myfile << ADD_VALUE("replaylength", replayInfo.duration) << ",\n";
    myfile << ADD_VALUE("loopcount", replayInfo.duration_gameloops) << ",\n";
    myfile << ADD_VALUE("result_p0", Observation()->GetResults()[0].result) << ",\n";
    myfile << ADD_VALUE("result_p1", Observation()->GetResults()[1].result) << ",\n";
    myfile << ADD_VALUE("enemy_race", replayInfo.players[1].race) << ",\n";// 0->Terran, 1->Zerg, 2->Protoss 
	myfile << ADD_VALUE("player_1_race", replayInfo.players[0].race) << ",\n";
    // Insert BuildOrder
    myfile << "\"buildorder\": [";
    for (auto i = 0; i < mBuildOrdersFromOnCreated.size(); ++i) {
        const BuildOrderEntry& entry = mBuildOrdersFromOnCreated[i];
        entry.AppendToJSON(myfile);
        if (i < mBuildOrdersFromOnCreated.size() - 1)
            myfile << ",\n";
    }
    myfile << "],\n";

    // Insert ActionBuildOrder
    myfile << "\"buildorder_action\": [";
    for (auto i = 0; i < mBuildOrdersFromActions.size(); ++i) {
        const auto& entry = mBuildOrdersFromActions[i];
        entry.AppendToJSON(myfile);
        if (i < mBuildOrdersFromActions.size() - 1)
            myfile << ",\n";
    }
    myfile << "]\n";
    myfile << "}\n";
    myfile.close();

    myfile.open(mOutputFolder + mCurrentReplayName + ".data", std::ios_base::out);
#ifdef _DEBUG
    //assert(myfile.is_open());
#endif
    myfile << mStream.str() << std::endl;
    myfile.close();

    const std::string replayPath = ReplayControl()->GetReplayInfo().replay_path;
    std::ofstream infoFile;
    infoFile.open(mAnalizedDataFile, std::ios_base::app);
#ifdef _DEBUG
    //assert(infoFile.is_open());
#endif
    infoFile << replayPath << std::endl;
    infoFile.close();

    std::ofstream raw_action_file;
    raw_action_file.open(mOutputFolder + mCurrentReplayName + ".raw_actions", std::ios_base::out);
    raw_action_file << mRawActionStream.str() << std::endl;
    raw_action_file.close();

    std::ofstream action_file;
    action_file.open(mOutputFolder + mCurrentReplayName + ".actions", std::ios_base::out);
    action_file << mActionStream.str() << std::endl;
    action_file.close();
}

void ReplayParser::OnStep() {
    if (mEnded)
        return;
    const ObservationInterface* obs = Observation();
    Units playerUnits = obs->GetUnits(Unit::Self);

    memset(mCurrentFeatures, 0, sizeof(mCurrentFeatures));
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::GAME_LOOP)] = float(obs->GetGameLoop());
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::MINERALS)] = float(obs->GetMinerals());
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::VESPENE)] = float(obs->GetVespene());
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::FOOD_LEFT)] = float(obs->GetFoodCap() - obs->GetFoodUsed());
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::FOOD_CAP)] = float(obs->GetFoodCap());
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::FOOD_USED)] = float(obs->GetFoodUsed());
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_IDLE)] = float(obs->GetIdleWorkerCount());
    float workerRefineryCur = 0;
    float workerRefineryOpt = 0;
    float townHallAssignedHarvester = 0;
    float townHallIdealHarvester = 0;
    float Energy = 0;

    for (const Unit* unit : playerUnits) {
        // Save current Units
        switch (unit->unit_type.ToType()) {
            case sc2::UNIT_TYPEID::TERRAN_REFINERY:								//Terran
                workerRefineryCur += unit->assigned_harvesters;
                workerRefineryOpt += unit->ideal_harvesters;
                break;
            case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:						//Terran
                Energy = max(unit->energy, Energy);  // Not breaking here is intended
            case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:						//Terran
            case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:					//Terran
                townHallAssignedHarvester += unit->assigned_harvesters;
                townHallIdealHarvester += unit->ideal_harvesters;
                break;
        }

        auto index = GetMaterialFromUnitType(unit->unit_type);
        if (index != MATERIAL::INVALID) {
            if (unit->build_progress < 1) {
                const auto idx = GetProgressIndex(index);
                mCurrentFeatures[GetInProductionIndex(index)] += 1;
                mCurrentFeatures[idx] = max(mCurrentFeatures[idx], unit->build_progress);
            } else {
                mCurrentFeatures[GetCurrentMaterialIndex(index)] += 1;
            }
        }
        // Only take orders from production units e.g. structures
        if (mUnitsOrdersImInterestedIn.count(unit->unit_type) > 0) {
            for (const UnitOrder& order : unit->orders) {
                auto idx = GetMaterialFromAbility(order.ability_id);
                if (idx != MATERIAL::INVALID) {
                    mCurrentFeatures[GetInProductionIndex(idx)] += 1;
                    mCurrentFeatures[GetProgressIndex(idx)] = max(mCurrentFeatures[GetProgressIndex(idx)], order.progress);
                }
            }
        }
    }
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_MINERALS_CUR)] = townHallAssignedHarvester;
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_MINERALS_MAX)] = townHallIdealHarvester;
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_MINERALS_DIF)] = townHallIdealHarvester - townHallAssignedHarvester;
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_REFINERY_CUR)] = workerRefineryCur;
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_REFINERY_MAX)] = workerRefineryOpt;
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::WORKER_REFINERY_DIF)] = workerRefineryOpt - workerRefineryCur;
    mCurrentFeatures[GetGameDataIndex(GAME_DATA::ORBITAL_COMMAND_ENERGY)] = Energy;										//Terran

    for (const auto upgrade : obs->GetUpgrades()) {
        MATERIAL up = GetMaterialFromUpgradeId(upgrade);
        if (up != MATERIAL::INVALID) {
            mCurrentFeatures[GetUpgradeIndex(up)] = max(UpgradeProgessionFromUpgradeId(upgrade), mCurrentFeatures[GetUpgradeIndex(up)]);
        }
    }

#define ADD_AVAILABLE_TEC(MAT, RULE) mCurrentFeatures[GetPossibleActionIndex(ACTION::MAT)] = ((RULE) ? 1.f : 0.f);
#define P_ADD_AVAILABLE_TEC(MAT, RULE) mCurrentFeatures[GetPossibleActionIndex(P_ACTION::MAT)] = ((RULE) ? 1.f : 0.f);
#define GET_MAT_COUNT(MAT) mCurrentFeatures[GetCurrentMaterialIndex(MATERIAL::MAT)]
#define HAS_MAT(MAT) (GET_MAT_COUNT(MAT) > 0)
#define ADD_AVAILABLE_EXTENSIONS(TYPE, MAT) \
    ADD_AVAILABLE_TEC(TYPE##_##MAT, HAS_MAT(MAT) && GET_MAT_COUNT(MAT) > (GET_MAT_COUNT(TECHLAB_##MAT) + GET_MAT_COUNT(REACTOR_##MAT)))
#define HAS_UPGRADE_MAXED(MAT) (mCurrentFeatures[GetUpgradeIndex(MATERIAL::MAT)] == 1)
#define IS_RESEARCHING_UPGRADE(MAT) (mCurrentFeatures[GetInProductionIndex(MATERIAL::MAT)] > 0)
#define ADD_AVAILABLE_UP(MAT, REQ) ADD_AVAILABLE_TEC(MAT, HAS_MAT(REQ) && !HAS_UPGRADE_MAXED(MAT) && !IS_RESEARCHING_UPGRADE(MAT))
#define ADD_AVAILABLE_UP_TWO(MAT, REQ, REQ2) ADD_AVAILABLE_TEC(MAT, HAS_MAT(REQ) && HAS_MAT(REQ2) && !HAS_UPGRADE_MAXED(MAT) && !IS_RESEARCHING_UPGRADE(MAT))


	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Terran
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	
    // Available Technology, RuleBased:
    ADD_AVAILABLE_TEC(COMMANDCENTER, HAS_MAT(SCV));
    ADD_AVAILABLE_TEC(ORBITALCOMMAND, HAS_MAT(COMMANDCENTER) && HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(PLANETARYFORTRESS, HAS_MAT(COMMANDCENTER) && HAS_MAT(ENGINEERINGBAY));
    ADD_AVAILABLE_TEC(SUPPLYDEPOT, HAS_MAT(SCV));
    ADD_AVAILABLE_TEC(REFINERY, HAS_MAT(SCV));
    ADD_AVAILABLE_TEC(BARRACKS, HAS_MAT(SUPPLYDEPOT));
    ADD_AVAILABLE_EXTENSIONS(TECHLAB, BARRACKS);
    ADD_AVAILABLE_EXTENSIONS(REACTOR, BARRACKS);
    ADD_AVAILABLE_TEC(FACTORY, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_EXTENSIONS(TECHLAB, FACTORY);
    ADD_AVAILABLE_EXTENSIONS(REACTOR, FACTORY);
    ADD_AVAILABLE_TEC(BUNKER, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(STARPORT, HAS_MAT(FACTORY));
    ADD_AVAILABLE_EXTENSIONS(TECHLAB, STARPORT);
    ADD_AVAILABLE_EXTENSIONS(REACTOR, STARPORT);
    ADD_AVAILABLE_TEC(GHOSTACADEMY, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(ENGINEERINGBAY, HAS_MAT(COMMANDCENTER) || HAS_MAT(ORBITALCOMMAND) || HAS_MAT(PLANETARYFORTRESS));
    ADD_AVAILABLE_TEC(ARMORY, HAS_MAT(FACTORY));
    ADD_AVAILABLE_TEC(FUSIONCORE, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(SENSORTOWER, HAS_MAT(ENGINEERINGBAY));
    ADD_AVAILABLE_TEC(MISSILETURRET, HAS_MAT(ENGINEERINGBAY));
    // Units
    ADD_AVAILABLE_TEC(SCV, HAS_MAT(COMMANDCENTER) || HAS_MAT(ORBITALCOMMAND) || HAS_MAT(PLANETARYFORTRESS));
    ADD_AVAILABLE_TEC(MULE, HAS_MAT(ORBITALCOMMAND) && mCurrentFeatures[GetGameDataIndex(GAME_DATA::ORBITAL_COMMAND_ENERGY)] >= 50);
    // Barracks
    ADD_AVAILABLE_TEC(GHOST, HAS_MAT(BARRACKS) && HAS_MAT(GHOSTACADEMY));
    ADD_AVAILABLE_TEC(MARAUDER, HAS_MAT(BARRACKS) && HAS_MAT(TECHLAB_BARRACKS));
    ADD_AVAILABLE_TEC(REAPER, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(MARINE, HAS_MAT(BARRACKS));
    // Factory
    ADD_AVAILABLE_TEC(HELLION, HAS_MAT(FACTORY));
    ADD_AVAILABLE_TEC(SIEGETANK, HAS_MAT(FACTORY) && HAS_MAT(TECHLAB_FACTORY));
    ADD_AVAILABLE_TEC(WIDOWMINE, HAS_MAT(FACTORY));
    ADD_AVAILABLE_TEC(HELLIONTANK, HAS_MAT(FACTORY) && HAS_MAT(ARMORY));
    ADD_AVAILABLE_TEC(THOR, HAS_MAT(FACTORY) && HAS_MAT(TECHLAB_FACTORY) && HAS_MAT(ARMORY));
    ADD_AVAILABLE_TEC(CYCLONE, HAS_MAT(FACTORY));
    // Starport
    ADD_AVAILABLE_TEC(VIKINGFIGHTER, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(MEDIVAC, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(LIBERATOR, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(RAVEN, HAS_MAT(STARPORT) && HAS_MAT(TECHLAB_STARPORT));
    ADD_AVAILABLE_TEC(BANSHEE, HAS_MAT(STARPORT) && HAS_MAT(TECHLAB_STARPORT));
    ADD_AVAILABLE_TEC(BATTLECRUISER, HAS_MAT(STARPORT) && HAS_MAT(TECHLAB_STARPORT) && HAS_MAT(FUSIONCORE));
    // Upgrades
    ADD_AVAILABLE_UP(TERRANSHIPWEAPONS, ARMORY);
    ADD_AVAILABLE_UP(TERRANVEHICLEANDSHIPPLATING, ARMORY);
    ADD_AVAILABLE_UP(TERRANVEHICLEWEAPONS, ARMORY);
    ADD_AVAILABLE_UP(STIMPACK, TECHLAB_BARRACKS);
    ADD_AVAILABLE_UP(COMBATSHIELD, TECHLAB_BARRACKS);
    ADD_AVAILABLE_UP(CONCUSSIVESHELLS, TECHLAB_BARRACKS);
    ADD_AVAILABLE_UP(HISECAUTOTRACKING, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(TERRANSTRUCTUREARMORUPGRADE, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(NEOSTEELFRAME, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(TERRANINFANTRYARMOR, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(TERRANINFANTRYWEAPONS, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(INFERNALPREIGNITER, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(DRILLINGCLAWS, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(RAPIDFIRELAUNCHERS, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(SMARTSERVOS, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(BATTLECRUISERWEAPONREFIT, FUSIONCORE);
    ADD_AVAILABLE_UP(PERSONALCLOAKING, GHOSTACADEMY);
    ADD_AVAILABLE_UP(BANSHEECLOAKINGFIELD, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(RAVENCORVIDREACTOR, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(BANSHEEHYPERFLIGHTROTORS, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(HIGHCAPACITYFUELTANKS, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(ADVANCEDBALLISTICS, TECHLAB_STARPORT);

    ADD_AVAILABLE_TEC(SUPPLYDROP, HAS_MAT(ORBITALCOMMAND))
    // <- Avaiable Technology
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//End Terran
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Protoss
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
    /*
	
	// Available Technology, RuleBased:
	
	
	//------------
	//Structures
	//------------
	
	
    ADD_AVAILABLE_TEC(NEXUS, HAS_MAT(PROBE));
	ADD_AVAILABLE_TEC(PYLON, HAS_MAT(PROBE));
	ADD_AVAILABLE_TEC(ASSIMILATOR, HAS_MAT(PROBE));
	ADD_AVAILABLE_TEC(GATEWAY, HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(FORGE, HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(CYBERNETICS_CORE, HAS_MAT(GATEWAY) && HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(STARGATE, HAS_MAT(CYBERNETICS_CORE) && HAS_MAT(GATEWAY));
	ADD_AVAILABLE_TEC(ROBOTICS_FACILITY, HAS_MAT(CYBERNETICS_CORE) && HAS_MAT(GATEWAY));
	ADD_AVAILABLE_TEC(TWILIGHT_COUNCIL, HAS_MAT(CYBERNETICS_CORE) && HAS_MAT(GATEWAY));
	ADD_AVAILABLE_TEC(FLEET_BEACON, HAS_MAT(STARGATE) && HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(ROBOTICS_BAY, HAS_MAT(ROBOTICS_FACILITY) && HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(TEMPLAR_ARCHIVES, HAS_MAT(TWILIGHT_COUNCIL) && HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(DARK_SHRINE, HAS_MAT(TWILIGHT_COUNCIL) && HAS_MAT(PYLON));
	ADD_AVAILABLE_TEC(PHOTON_CANNON, HAS_MAT(FORGE) && HAS_MAT(PYLON));
	
	
	//------------
    // Units
	//------------
	
	
	// Nexus
    ADD_AVAILABLE_TEC(PROBE, HAS_MAT(NEXUS));
	*/
    /*ADD_AVAILABLE_TEC(MULE, HAS_MAT(ORBITALCOMMAND) && mCurrentFeatures[GetGameDataIndex(GAME_DATA::ORBITAL_COMMAND_ENERGY)] >= 50);*/ //NEED TO FIGURE OUT HOW TO CONSIDER CHRONOBOOST
    /*
    // Gateway
    ADD_AVAILABLE_TEC(ZEALOT, HAS_MAT(GATEWAY));
    ADD_AVAILABLE_TEC(STALKER, HAS_MAT(GATEWAY) && HAS_MAT(CYBERNETICS_CORE));
	ADD_AVAILABLE_TEC(SENTRY, HAS_MAT(GATEWAY) && HAS_MAT(CYBERNETICS_CORE));
	ADD_AVAILABLE_TEC(ADEPT, HAS_MAT(GATEWAY) && HAS_MAT(CYBERNETICS_CORE));
	ADD_AVAILABLE_TEC(HIGH_TEMPLAR, HAS_MAT(GATEWAY) && HAS_MAT(TEMPLAR_ARCHIVES));
	ADD_AVAILABLE_TEC(DARK_TEMPLAR, HAS_MAT(GATEWAY) && HAS_MAT(DARK_SHRINE));

    // Robotics Facility
    ADD_AVAILABLE_TEC(OBSERVER, HAS_MAT(ROBOTICS_FACILITY));
	ADD_AVAILABLE_TEC(WARP_PRISM, HAS_MAT(ROBOTICS_FACILITY));
	ADD_AVAILABLE_TEC(IMMORTAL, HAS_MAT(ROBOTICS_FACILITY));
	ADD_AVAILABLE_TEC(COLOSSUS, HAS_MAT(ROBOTICS_FACILITY) && HAS_MAT(ROBOTICS_BAY));
	ADD_AVAILABLE_TEC(DISRUPTOR, HAS_MAT(ROBOTICS_FACILITY) && HAS_MAT(ROBOTICS_BAY));
	
    // StarGATE SG-1
    ADD_AVAILABLE_TEC(PHOENIX, HAS_MAT(STARGATE));
	ADD_AVAILABLE_TEC(ORACLE, HAS_MAT(STARGATE));
	ADD_AVAILABLE_TEC(VOID_RAY, HAS_MAT(STARGATE));
	ADD_AVAILABLE_TEC(TEMPEST, HAS_MAT(STARGATE) && HAS_MAT(FLEET_BEACON));
	ADD_AVAILABLE_TEC(CARRIER, HAS_MAT(STARGATE) && HAS_MAT(FLEET_BEACON));

	
	//------------
    // Upgrades
	//------------
	
	
	// Forge
	ADD_AVAILABLE_UP(PROTOSSGROUNDWEAPONSLEVEL1, FORGE);
	ADD_AVAILABLE_UP(PROTOSSGROUNDARMORSLEVEL1, FORGE);
	ADD_AVAILABLE_UP(PROTOSSSHIELDSLEVEL1, FORGE);
	ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDWEAPONSLEVEL2, FORGE, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDARMORSLEVEL2, FORGE, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP_TWO(PROTOSSSHIELDSLEVEL2, FORGE, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDWEAPONSLEVEL3, FORGE, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDARMORSLEVEL3, FORGE, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP_TWO(PROTOSSSHIELDSLEVEL3, FORGE, TWILIGHT_COUNCIL);	
	
	// Twilight Council
	ADD_AVAILABLE_UP(CHARGE, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP(BLINKTECH, TWILIGHT_COUNCIL);
	ADD_AVAILABLE_UP(ADEPTPIERCINGATTACK, TWILIGHT_COUNCIL);	
	
	// Cybernetics Core
	ADD_AVAILABLE_UP(WARPGATERESEARCH, CYBERNETICS_CORE);
	ADD_AVAILABLE_UP(PROTOSSAIRWEAPONSLEVEL1, CYBERNETICS_CORE);
	ADD_AVAILABLE_UP(PROTOSSAIRARMORSLEVEL1, CYBERNETICS_CORE);
	ADD_AVAILABLE_UP_TWO(PROTOSSAIRWEAPONSLEVEL2, CYBERNETICS_CORE, FLEET_BEACON);
	ADD_AVAILABLE_UP_TWO(PROTOSSAIRARMORSLEVEL2, CYBERNETICS_CORE, FLEET_BEACON);
	ADD_AVAILABLE_UP_TWO(PROTOSSAIRARMORSLEVEL3, CYBERNETICS_CORE, FLEET_BEACON);
	ADD_AVAILABLE_UP_TWO(PROTOSSAIRWEAPONSLEVEL3, CYBERNETICS_CORE, FLEET_BEACON);
	


    //ADD_AVAILABLE_TEC(SUPPLYDROP, HAS_MAT(ORBITALCOMMAND))
    // <- Avaiable Technology
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//End Protoss
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	*/
	
    // Append Expected Result to last Step
    if (!mFirstStep) {
        auto writeAction = [this](ACTION resultAction, ABILITY_ID realAbilityId) {
            for (auto i = 0; i < int(SECTORS::MAX); ++i) {
                if (SaveAsFloat(i)) {
                    mStream << std::setprecision(3) << mLastFeatures[i];
                } else {
                    mStream << std::setprecision(0) << mLastFeatures[i];
                }
                mStream << (i < int(SECTORS::MAX) - 1 ? ";" : "");
            }
            mStream << "#" << ActionToName(resultAction) << std::endl;

            // Add Actions to BuildQueue
            if (int(resultAction) > int(ACTION::INVALID)) {
                if (int(resultAction) < int(MATERIAL_SECTIONS::START_UPGRADES)) {  // Is Unit
                    mBuildOrdersFromActions.emplace_back(Observation()->GetGameLoop(), GetUnitTypeFromAbility(realAbilityId));
                } else if (int(resultAction) < int(MATERIAL_SECTIONS::MAX)) {  // Is Upgrade
                    mBuildOrdersFromActions.emplace_back(Observation()->GetGameLoop(), GetUpgradeIdFromAbilityId(realAbilityId));
                }
            }
        };

        const auto& actions = obs->GetRawActions();
        for (const auto& action : actions) {
            mRawActionStream << obs->GetGameLoop() << ";" << action.ability_id.to_string() << ";" << AbilityTypeToName(action.ability_id) << "\n";
            ABILITY_ID realAbilityId = action.ability_id;
            if (NeedUnitToGetCorrectAbilits(action.ability_id)) {
                realAbilityId = GetActualAbility(action.ability_id, obs->GetUnit(action.unit_tags[0]));
            }
            auto resultAction = GetActionFromAbility(realAbilityId);
            if (int(resultAction) > int(ACTION::INVALID)) {
                mActionStream << obs->GetGameLoop() << ";" << ActionToName(resultAction) << "\n";
            }
            if (realAbilityId == ABILITY_ID::LIFT || realAbilityId == ABILITY_ID::LAND) {
                for (const auto& tag : action.unit_tags) {
                    auto unit = obs->GetUnit(tag);
                    auto addon = obs->GetUnit(unit->add_on_tag);
                    mActionStream << obs->GetGameLoop() << ";" << AbilityTypeToName(action.ability_id) << ";" << UnitTypeToName(unit->unit_type.ToType()) << ";"
                                  << (addon ? UnitTypeToName(addon->unit_type.ToType()) : "None") << "\n";
                }
            }
        }

        bool wroteSmth = false;
        if (!actions.empty()) {
            std::unordered_set<ABILITY_ID> actions_done;
            for (const auto& action : actions) {
                if (actions_done.count(action.ability_id.ToType()) > 0)
                    continue;
                actions_done.emplace(action.ability_id.ToType());
                ACTION resultAction = ACTION::INVALID;
                ABILITY_ID realAbilityId = action.ability_id;
                if (NeedUnitToGetCorrectAbilits(action.ability_id)) {
                    realAbilityId = GetActualAbility(action.ability_id, obs->GetUnit(action.unit_tags[0]));
                }
                resultAction = GetActionFromAbility(realAbilityId);

                if (int(resultAction) > int(ACTION::INVALID)) {
                    wroteSmth = true;
                    writeAction(resultAction, realAbilityId);
                }
            }
        }
#if TRACK_HISTORY
        if (!wroteSmth) {
            writeAction(ACTION::INVALID, ABILITY_ID::INVALID);
        }
#endif
    } else {
        mFirstStep = false;
    }

    memcpy(mLastFeatures, mCurrentFeatures, sizeof(mCurrentFeatures));
    mProcessedSteps = obs->GetGameLoop();
}

void ReplayParser::OnUnitCreated(const Unit* unit) {
    if (unit->alliance != Unit::Self) {
        return;
    }
    if (mSeenUnits.count(unit->tag) > 0)
        return;
    mSeenUnits.emplace(unit->tag);
    mBuildOrdersFromOnCreated.emplace_back(Observation()->GetGameLoop(), unit->unit_type.ToType());
}

void ReplayParser::OnUpgradeCompleted(UpgradeID upgrade) {
    mBuildOrdersFromOnCreated.emplace_back(Observation()->GetGameLoop(), upgrade.ToType());
}

bool ReplayParser::IgnoreReplay(const ReplayInfo& replay_info, uint32_t& player_id) {
    for (const auto player : replay_info.players) {
        if (player.race == Terran && uint32_t(player.player_id) == player_id) {
            // std::cout << replay_info.data_version << std::endl;
            return false;
        }
    }
    return true;
}

void BuildOrderEntry::AppendToJSON(std::ofstream& myfile) const {
    myfile << "{";
    myfile << ADD_VALUE("step", int(step)) << ", ";
    myfile << ADD_VALUE("type", (isUpgrade ? int(upgradeId) : int(unitType))) << ", ";
    myfile << ADD_VALUE("type_name", (isUpgrade ? UpgradeIDToName(upgradeId) : UnitTypeToName(unitType))) << ", ";
    myfile << ADD_VALUE("isUpgrade", int(isUpgrade));
    myfile << "}";
}


/*








*/



P_ReplayParser::P_ReplayParser(int id) : mId(id), mFirstStep(false), mEnded(false), mLastFeatures() {
    std::ostringstream stream;
    stream << ExePath() << "\\output\\data_" << id << "\\";
    mOutputFolder = stream.str();
    stream.str(std::string());
    stream << ExePath() << "\\output\\analized_" << id << ".txt";
    mAnalizedDataFile = stream.str();
	
	
	//---------------------------------------
	//Protoss
	//---------------------------------------
	
	
	// Protoss Structure Units
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_DARKSHRINE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_FLEETBEACON);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_FORGE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_GATEWAY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_NEXUS);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_PHOTONCANNON);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_ROBOTICSBAY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_SHIELDBATTERY);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_STARGATE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_WARPGATE);
	mUnitsOrdersImInterestedIn.emplace(UNIT_TYPEID::PROTOSS_PYLON);
	
}

P_ReplayParser::~P_ReplayParser() {}

void P_ReplayParser::OnGameStart() {
    const auto obs = Observation();
    const GameInfo& gameInfo = obs->GetGameInfo();
    mCurrentMapName = gameInfo.map_name;
    mStream.str(std::string());
    mRawActionStream.str(std::string());
    mActionStream.str(std::string());
    mFirstStep = true;
    mEnded = false;
    mBuildOrdersFromActions.clear();
    mBuildOrdersFromOnCreated.clear();
    mSeenUnits.clear();
    auto replayPath = ReplayControl()->GetReplayInfo().replay_path;
    const std::string::size_type pos1 = replayPath.find_last_of("\\/");
    const std::string::size_type pos2 = replayPath.find_last_of('.');
    mCurrentReplayName = replayPath.substr(pos1 + 1, pos2 - pos1 - 1);
    mProcessedSteps = 0;
    memset(mCurrentFeatures, 0, sizeof(mCurrentFeatures));
    memset(mLastFeatures, 0, sizeof(mLastFeatures));
}

void P_ReplayParser::OnGameEnd() {
    mEnded = true;
    if (mStream.str().size() < 50 || mProcessedSteps < 5000) {
        std::cout << "Output too small" << mStream.str() << std::endl;
        const auto replayPath = ReplayControl()->GetReplayInfo().replay_path;
        std::ofstream infoFile;
        infoFile.open(mAnalizedDataFile, std::ios_base::app);
#ifdef _DEBUG
        //assert(infoFile.is_open());
#endif
        infoFile << replayPath << std::endl;
        infoFile.close();
        return;
    }

    std::ofstream myfile;
    myfile.open(mOutputFolder + mCurrentReplayName + ".json", std::ios_base::out);
#ifdef _DEBUG
    //assert(myfile.is_open());
#endif
    myfile << "{\n";
    myfile << ADD_VALUE("replayname", mCurrentReplayName) << ",\n";
    myfile << ADD_VALUE("mapname", mCurrentMapName) << ",\n";
    const ReplayInfo& replayInfo = ReplayControl()->GetReplayInfo();
    myfile << ADD_VALUE("replaylength", replayInfo.duration) << ",\n";
    myfile << ADD_VALUE("loopcount", replayInfo.duration_gameloops) << ",\n";
    myfile << ADD_VALUE("result_p0", Observation()->GetResults()[0].result) << ",\n";
    myfile << ADD_VALUE("result_p1", Observation()->GetResults()[1].result) << ",\n";
    myfile << ADD_VALUE("enemy_race", replayInfo.players[1].race) << ",\n";// 0->Terran, 1->Zerg, 2->Protoss 
	myfile << ADD_VALUE("player_1_race", replayInfo.players[0].race) << ",\n";
    // Insert BuildOrder
    myfile << "\"p_buildorder\": [";
    for (auto i = 0; i < mBuildOrdersFromOnCreated.size(); ++i) {
        const P_BuildOrderEntry& entry = mBuildOrdersFromOnCreated[i];
        entry.AppendToJSON(myfile);
        if (i < mBuildOrdersFromOnCreated.size() - 1)
            myfile << ",\n";
    }
    myfile << "],\n";

    // Insert ActionBuildOrder
    myfile << "\"p_buildorder_action\": [";
    for (auto i = 0; i < mBuildOrdersFromActions.size(); ++i) {
        const auto& entry = mBuildOrdersFromActions[i];
        entry.AppendToJSON(myfile);
        if (i < mBuildOrdersFromActions.size() - 1)
            myfile << ",\n";
    }
    myfile << "]\n";
    myfile << "}\n";
    myfile.close();

    myfile.open(mOutputFolder + mCurrentReplayName + ".data", std::ios_base::out);
#ifdef _DEBUG
    //assert(myfile.is_open());
#endif
    myfile << mStream.str() << std::endl;
    myfile.close();

    const std::string replayPath = ReplayControl()->GetReplayInfo().replay_path;
    std::ofstream infoFile;
    infoFile.open(mAnalizedDataFile, std::ios_base::app);
#ifdef _DEBUG
    //assert(infoFile.is_open());
#endif
    infoFile << replayPath << std::endl;
    infoFile.close();

    std::ofstream raw_action_file;
    raw_action_file.open(mOutputFolder + mCurrentReplayName + ".raw_actions", std::ios_base::out);
    raw_action_file << mRawActionStream.str() << std::endl;
    raw_action_file.close();

    std::ofstream action_file;
    action_file.open(mOutputFolder + mCurrentReplayName + ".actions", std::ios_base::out);
    action_file << mActionStream.str() << std::endl;
    action_file.close();
}

void P_ReplayParser::OnStep() {
    if (mEnded)
        return;
    const ObservationInterface* obs = Observation();
    Units playerUnits = obs->GetUnits(Unit::Self);

    memset(mCurrentFeatures, 0, sizeof(mCurrentFeatures));
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::GAME_LOOP)] = float(obs->GetGameLoop());
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::MINERALS)] = float(obs->GetMinerals());
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::VESPENE)] = float(obs->GetVespene());
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::FOOD_LEFT)] = float(obs->GetFoodCap() - obs->GetFoodUsed());
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::FOOD_CAP)] = float(obs->GetFoodCap());
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::FOOD_USED)] = float(obs->GetFoodUsed());
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_IDLE)] = float(obs->GetIdleWorkerCount());
    float probeAssimilatorCur = 0;
    float probeAssimilatorOpt = 0;
    float townHallAssignedHarvester = 0;
    float townHallIdealHarvester = 0;
    float Energy = 0;

    for (const Unit* unit : playerUnits) {
        // Save current Units
        switch (unit->unit_type.ToType()) {
            case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:							//Protoss
                probeAssimilatorCur += unit->assigned_harvesters;
                probeAssimilatorOpt += unit->ideal_harvesters;
                break;
            case sc2::UNIT_TYPEID::PROTOSS_NEXUS:						//Protoss
                Energy = max(unit->energy, Energy);  // Not breaking here is intended
                townHallAssignedHarvester += unit->assigned_harvesters;
                townHallIdealHarvester += unit->ideal_harvesters;
                break;
        }

        auto index = P_GetMaterialFromUnitType(unit->unit_type);
        if (index != P_MATERIAL::INVALID) {
            if (unit->build_progress < 1) {
                const auto idx = P_GetProgressIndex(index);
                mCurrentFeatures[P_GetInProductionIndex(index)] += 1;
                mCurrentFeatures[idx] = max(mCurrentFeatures[idx], unit->build_progress);
            } else {
                mCurrentFeatures[P_GetCurrentMaterialIndex(index)] += 1;
            }
        }
        // Only take orders from production units e.g. structures
        if (mUnitsOrdersImInterestedIn.count(unit->unit_type) > 0) {
            for (const UnitOrder& order : unit->orders) {
                auto idx = P_GetMaterialFromAbility(order.ability_id);
                if (idx != P_MATERIAL::INVALID) {
                    mCurrentFeatures[P_GetInProductionIndex(idx)] += 1;
                    mCurrentFeatures[P_GetProgressIndex(idx)] = max(mCurrentFeatures[P_GetProgressIndex(idx)], order.progress);
                }
            }
        }
    }
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_MINERALS_CUR)] = townHallAssignedHarvester;
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_MINERALS_MAX)] = townHallIdealHarvester;
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_MINERALS_DIF)] = townHallIdealHarvester - townHallAssignedHarvester;
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_ASSIM_CUR)] = probeAssimilatorCur;
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_ASSIM_MAX)] = probeAssimilatorOpt;
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::WORKER_ASSIM_DIF)] = probeAssimilatorOpt - probeAssimilatorOpt;
    mCurrentFeatures[P_GetGameDataIndex(P_GAME_DATA::NEXUS_ENERGY)] = Energy;	// Might be problematic									//Terran

    for (const auto upgrade : obs->GetUpgrades()) {
        P_MATERIAL up = P_GetMaterialFromUpgradeId(upgrade);
        if (up != P_MATERIAL::INVALID) {
            mCurrentFeatures[P_GetUpgradeIndex(up)] = max(P_UpgradeProgessionFromUpgradeId(upgrade), mCurrentFeatures[P_GetUpgradeIndex(up)]);
        }
    }

#define P_ADD_AVAILABLE_TEC(MAT, RULE) mCurrentFeatures[P_GetPossibleActionIndex(P_ACTION::MAT)] = ((RULE) ? 1.f : 0.f);
#define P_GET_MAT_COUNT(MAT) mCurrentFeatures[P_GetCurrentMaterialIndex(P_MATERIAL::MAT)]
#define P_HAS_MAT(MAT) (P_GET_MAT_COUNT(MAT) > 0)
#define P_ADD_AVAILABLE_EXTENSIONS(TYPE, MAT) \
    P_ADD_AVAILABLE_TEC(TYPE##_##MAT, P_HAS_MAT(MAT) && P_GET_MAT_COUNT(MAT) > (P_GET_MAT_COUNT(TECHLAB_##MAT) + P_GET_MAT_COUNT(REACTOR_##MAT)))
#define P_HAS_UPGRADE_MAXED(MAT) (mCurrentFeatures[P_GetUpgradeIndex(P_MATERIAL::MAT)] == 1)
#define P_IS_RESEARCHING_UPGRADE(MAT) (mCurrentFeatures[P_GetInProductionIndex(P_MATERIAL::MAT)] > 0)
#define P_ADD_AVAILABLE_UP(MAT, REQ) P_ADD_AVAILABLE_TEC(MAT, P_HAS_MAT(REQ) && !P_HAS_UPGRADE_MAXED(MAT) && !P_IS_RESEARCHING_UPGRADE(MAT))
#define P_ADD_AVAILABLE_UP_TWO(MAT, REQ, REQ2) P_ADD_AVAILABLE_TEC(MAT, P_HAS_MAT(REQ) && P_HAS_MAT(REQ2) && !P_HAS_UPGRADE_MAXED(MAT) && !P_IS_RESEARCHING_UPGRADE(MAT))


	/*
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Terran
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	
    // Available Technology, RuleBased:
    ADD_AVAILABLE_TEC(COMMANDCENTER, HAS_MAT(SCV));
    ADD_AVAILABLE_TEC(ORBITALCOMMAND, HAS_MAT(COMMANDCENTER) && HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(PLANETARYFORTRESS, HAS_MAT(COMMANDCENTER) && HAS_MAT(ENGINEERINGBAY));
    ADD_AVAILABLE_TEC(SUPPLYDEPOT, HAS_MAT(SCV));
    ADD_AVAILABLE_TEC(REFINERY, HAS_MAT(SCV));
    ADD_AVAILABLE_TEC(BARRACKS, HAS_MAT(SUPPLYDEPOT));
    ADD_AVAILABLE_EXTENSIONS(TECHLAB, BARRACKS);
    ADD_AVAILABLE_EXTENSIONS(REACTOR, BARRACKS);
    ADD_AVAILABLE_TEC(FACTORY, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_EXTENSIONS(TECHLAB, FACTORY);
    ADD_AVAILABLE_EXTENSIONS(REACTOR, FACTORY);
    ADD_AVAILABLE_TEC(BUNKER, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(STARPORT, HAS_MAT(FACTORY));
    ADD_AVAILABLE_EXTENSIONS(TECHLAB, STARPORT);
    ADD_AVAILABLE_EXTENSIONS(REACTOR, STARPORT);
    ADD_AVAILABLE_TEC(GHOSTACADEMY, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(ENGINEERINGBAY, HAS_MAT(COMMANDCENTER) || HAS_MAT(ORBITALCOMMAND) || HAS_MAT(PLANETARYFORTRESS));
    ADD_AVAILABLE_TEC(ARMORY, HAS_MAT(FACTORY));
    ADD_AVAILABLE_TEC(FUSIONCORE, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(SENSORTOWER, HAS_MAT(ENGINEERINGBAY));
    ADD_AVAILABLE_TEC(MISSILETURRET, HAS_MAT(ENGINEERINGBAY));
    // Units
    ADD_AVAILABLE_TEC(SCV, HAS_MAT(COMMANDCENTER) || HAS_MAT(ORBITALCOMMAND) || HAS_MAT(PLANETARYFORTRESS));
    ADD_AVAILABLE_TEC(MULE, HAS_MAT(ORBITALCOMMAND) && mCurrentFeatures[GetGameDataIndex(GAME_DATA::ORBITAL_COMMAND_ENERGY)] >= 50);
    // Barracks
    ADD_AVAILABLE_TEC(GHOST, HAS_MAT(BARRACKS) && HAS_MAT(GHOSTACADEMY));
    ADD_AVAILABLE_TEC(MARAUDER, HAS_MAT(BARRACKS) && HAS_MAT(TECHLAB_BARRACKS));
    ADD_AVAILABLE_TEC(REAPER, HAS_MAT(BARRACKS));
    ADD_AVAILABLE_TEC(MARINE, HAS_MAT(BARRACKS));
    // Factory
    ADD_AVAILABLE_TEC(HELLION, HAS_MAT(FACTORY));
    ADD_AVAILABLE_TEC(SIEGETANK, HAS_MAT(FACTORY) && HAS_MAT(TECHLAB_FACTORY));
    ADD_AVAILABLE_TEC(WIDOWMINE, HAS_MAT(FACTORY));
    ADD_AVAILABLE_TEC(HELLIONTANK, HAS_MAT(FACTORY) && HAS_MAT(ARMORY));
    ADD_AVAILABLE_TEC(THOR, HAS_MAT(FACTORY) && HAS_MAT(TECHLAB_FACTORY) && HAS_MAT(ARMORY));
    ADD_AVAILABLE_TEC(CYCLONE, HAS_MAT(FACTORY));
    // Starport
    ADD_AVAILABLE_TEC(VIKINGFIGHTER, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(MEDIVAC, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(LIBERATOR, HAS_MAT(STARPORT));
    ADD_AVAILABLE_TEC(RAVEN, HAS_MAT(STARPORT) && HAS_MAT(TECHLAB_STARPORT));
    ADD_AVAILABLE_TEC(BANSHEE, HAS_MAT(STARPORT) && HAS_MAT(TECHLAB_STARPORT));
    ADD_AVAILABLE_TEC(BATTLECRUISER, HAS_MAT(STARPORT) && HAS_MAT(TECHLAB_STARPORT) && HAS_MAT(FUSIONCORE));
    // Upgrades
    ADD_AVAILABLE_UP(TERRANSHIPWEAPONS, ARMORY);
    ADD_AVAILABLE_UP(TERRANVEHICLEANDSHIPPLATING, ARMORY);
    ADD_AVAILABLE_UP(TERRANVEHICLEWEAPONS, ARMORY);
    ADD_AVAILABLE_UP(STIMPACK, TECHLAB_BARRACKS);
    ADD_AVAILABLE_UP(COMBATSHIELD, TECHLAB_BARRACKS);
    ADD_AVAILABLE_UP(CONCUSSIVESHELLS, TECHLAB_BARRACKS);
    ADD_AVAILABLE_UP(HISECAUTOTRACKING, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(TERRANSTRUCTUREARMORUPGRADE, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(NEOSTEELFRAME, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(TERRANINFANTRYARMOR, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(TERRANINFANTRYWEAPONS, ENGINEERINGBAY);
    ADD_AVAILABLE_UP(INFERNALPREIGNITER, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(DRILLINGCLAWS, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(RAPIDFIRELAUNCHERS, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(SMARTSERVOS, TECHLAB_FACTORY);
    ADD_AVAILABLE_UP(BATTLECRUISERWEAPONREFIT, FUSIONCORE);
    ADD_AVAILABLE_UP(PERSONALCLOAKING, GHOSTACADEMY);
    ADD_AVAILABLE_UP(BANSHEECLOAKINGFIELD, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(RAVENCORVIDREACTOR, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(BANSHEEHYPERFLIGHTROTORS, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(HIGHCAPACITYFUELTANKS, TECHLAB_STARPORT);
    ADD_AVAILABLE_UP(ADVANCEDBALLISTICS, TECHLAB_STARPORT);

    ADD_AVAILABLE_TEC(SUPPLYDROP, HAS_MAT(ORBITALCOMMAND))
    // <- Avaiable Technology
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//End Terran
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	*/
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Protoss
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
    
	
	// Available Technology, RuleBased:
	
	
	//------------
	//Structures
	//------------
	
	
    P_ADD_AVAILABLE_TEC(NEXUS, P_HAS_MAT(PROBE));
	P_ADD_AVAILABLE_TEC(PYLON, P_HAS_MAT(PROBE));
	P_ADD_AVAILABLE_TEC(ASSIMILATOR, P_HAS_MAT(PROBE));
	P_ADD_AVAILABLE_TEC(GATEWAY, P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(FORGE, P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(CYBERNETICS_CORE, P_HAS_MAT(GATEWAY) && P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(STARGATE, P_HAS_MAT(CYBERNETICS_CORE) && P_HAS_MAT(GATEWAY));
	P_ADD_AVAILABLE_TEC(ROBOTICS_FACILITY, P_HAS_MAT(CYBERNETICS_CORE) && P_HAS_MAT(GATEWAY));
	P_ADD_AVAILABLE_TEC(TWILIGHT_COUNCIL, P_HAS_MAT(CYBERNETICS_CORE) && P_HAS_MAT(GATEWAY));
	P_ADD_AVAILABLE_TEC(FLEET_BEACON, P_HAS_MAT(STARGATE) && P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(ROBOTICS_BAY, P_HAS_MAT(ROBOTICS_FACILITY) && P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(TEMPLAR_ARCHIVE, P_HAS_MAT(TWILIGHT_COUNCIL) && P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(DARK_SHRINE, P_HAS_MAT(TWILIGHT_COUNCIL) && P_HAS_MAT(PYLON));
	P_ADD_AVAILABLE_TEC(PHOTON_CANNON, P_HAS_MAT(FORGE) && P_HAS_MAT(PYLON));
	
	
	//------------
    // Units
	//------------
	
	
	// Nexus
    P_ADD_AVAILABLE_TEC(PROBE, P_HAS_MAT(NEXUS));
	
    /*ADD_AVAILABLE_TEC(MULE, HAS_MAT(ORBITALCOMMAND) && mCurrentFeatures[GetGameDataIndex(GAME_DATA::ORBITAL_COMMAND_ENERGY)] >= 50);*/ //NEED TO FIGURE OUT HOW TO CONSIDER CHRONOBOOST
    
    // Gateway
    P_ADD_AVAILABLE_TEC(ZEALOT, P_HAS_MAT(GATEWAY));
    P_ADD_AVAILABLE_TEC(STALKER, P_HAS_MAT(GATEWAY) && P_HAS_MAT(CYBERNETICS_CORE));
	P_ADD_AVAILABLE_TEC(SENTRY, P_HAS_MAT(GATEWAY) && P_HAS_MAT(CYBERNETICS_CORE));
	P_ADD_AVAILABLE_TEC(ADEPT, P_HAS_MAT(GATEWAY) && P_HAS_MAT(CYBERNETICS_CORE));
	P_ADD_AVAILABLE_TEC(HIGH_TEMPLAR, P_HAS_MAT(GATEWAY) && P_HAS_MAT(TEMPLAR_ARCHIVE));
	P_ADD_AVAILABLE_TEC(DARK_TEMPLAR, P_HAS_MAT(GATEWAY) && P_HAS_MAT(DARK_SHRINE));

    // Robotics Facility
    P_ADD_AVAILABLE_TEC(OBSERVER, P_HAS_MAT(ROBOTICS_FACILITY));
	P_ADD_AVAILABLE_TEC(WARP_PRISM, P_HAS_MAT(ROBOTICS_FACILITY));
	P_ADD_AVAILABLE_TEC(IMMORTAL, P_HAS_MAT(ROBOTICS_FACILITY));
	P_ADD_AVAILABLE_TEC(COLOSSUS, P_HAS_MAT(ROBOTICS_FACILITY) && P_HAS_MAT(ROBOTICS_BAY));
	P_ADD_AVAILABLE_TEC(DISRUPTOR, P_HAS_MAT(ROBOTICS_FACILITY) && P_HAS_MAT(ROBOTICS_BAY));
	
    // StarGATE SG-1
    P_ADD_AVAILABLE_TEC(PHOENIX, P_HAS_MAT(STARGATE));
	P_ADD_AVAILABLE_TEC(ORACLE, P_HAS_MAT(STARGATE));
	P_ADD_AVAILABLE_TEC(VOID_RAY, P_HAS_MAT(STARGATE));
	P_ADD_AVAILABLE_TEC(TEMPEST, P_HAS_MAT(STARGATE) && P_HAS_MAT(FLEET_BEACON));
	P_ADD_AVAILABLE_TEC(CARRIER, P_HAS_MAT(STARGATE) && P_HAS_MAT(FLEET_BEACON));

	
	//------------
    // Upgrades
	//------------
	
	
	// Forge
	P_ADD_AVAILABLE_UP(PROTOSSGROUNDWEAPONS, FORGE);
	P_ADD_AVAILABLE_UP(PROTOSSGROUNDARMOR, FORGE);
	P_ADD_AVAILABLE_UP(PROTOSSSHIELDS, FORGE);
	/*P_ADD_AVAILABLE_UP(PROTOSSGROUNDWEAPONSLEVEL1, FORGE);
	P_ADD_AVAILABLE_UP(PROTOSSGROUNDARMORSLEVEL1, FORGE);
	P_ADD_AVAILABLE_UP(PROTOSSSHIELDSLEVEL1, FORGE);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDWEAPONSLEVEL2, FORGE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDARMORSLEVEL2, FORGE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSSHIELDSLEVEL2, FORGE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDWEAPONSLEVEL3, FORGE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSGROUNDARMORSLEVEL3, FORGE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSSHIELDSLEVEL3, FORGE, TWILIGHT_COUNCIL);*/	
	
	// Twilight Council
	P_ADD_AVAILABLE_UP(CHARGE_UPGRADE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP(BLINK_UPGRADE, TWILIGHT_COUNCIL);
	P_ADD_AVAILABLE_UP(ADEPT_UPGRADE, TWILIGHT_COUNCIL);	
	
	// Cybernetics Core
	P_ADD_AVAILABLE_UP(WARPGATE_RESEARCH, CYBERNETICS_CORE);
	P_ADD_AVAILABLE_UP(PROTOSSAIRWEAPONS, CYBERNETICS_CORE);
	P_ADD_AVAILABLE_UP(PROTOSSAIRARMOR, CYBERNETICS_CORE);
	
	/*P_ADD_AVAILABLE_UP(PROTOSSAIRWEAPONSLEVEL1, CYBERNETICS_CORE);
	P_ADD_AVAILABLE_UP(PROTOSSAIRARMORSLEVEL1, CYBERNETICS_CORE);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSAIRWEAPONSLEVEL2, CYBERNETICS_CORE, FLEET_BEACON);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSAIRARMORSLEVEL2, CYBERNETICS_CORE, FLEET_BEACON);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSAIRARMORSLEVEL3, CYBERNETICS_CORE, FLEET_BEACON);
	P_ADD_AVAILABLE_UP_TWO(PROTOSSAIRWEAPONSLEVEL3, CYBERNETICS_CORE, FLEET_BEACON);*/
	


    //ADD_AVAILABLE_TEC(SUPPLYDROP, HAS_MAT(ORBITALCOMMAND))
    // <- Avaiable Technology
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	//End Protoss
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	
    // Append Expected Result to last Step
    if (!mFirstStep) {
        auto writeAction = [this](P_ACTION resultAction, ABILITY_ID realAbilityId) {
            for (auto i = 0; i < int(P_SECTORS::MAX); ++i) {
                if (SaveAsFloat(i)) {
                    mStream << std::setprecision(3) << mLastFeatures[i];
                } else {
                    mStream << std::setprecision(0) << mLastFeatures[i];
                }
                mStream << (i < int(SECTORS::MAX) - 1 ? ";" : "");
            }
            mStream << "#" << P_ActionToName(resultAction) << std::endl;

            // Add Actions to BuildQueue
            if (int(resultAction) > int(P_ACTION::INVALID)) {
                if (int(resultAction) < int(P_MATERIAL_SECTIONS::START_UPGRADES)) {  // Is Unit
                    mBuildOrdersFromActions.emplace_back(Observation()->GetGameLoop(), P_GetUnitTypeFromAbility(realAbilityId));
                } else if (int(resultAction) < int(P_MATERIAL_SECTIONS::MAX)) {  // Is Upgrade
                    mBuildOrdersFromActions.emplace_back(Observation()->GetGameLoop(), P_GetUpgradeIdFromAbilityId(realAbilityId));
                }
            }
        };

        const auto& actions = obs->GetRawActions();
        for (const auto& action : actions) {
            mRawActionStream << obs->GetGameLoop() << ";" << action.ability_id.to_string() << ";" << AbilityTypeToName(action.ability_id) << "\n";
            ABILITY_ID realAbilityId = action.ability_id;
            if (P_NeedUnitToGetCorrectAbilits(action.ability_id)) {
                realAbilityId = P_GetActualAbility(action.ability_id, obs->GetUnit(action.unit_tags[0]));
            }
            auto resultAction = P_GetActionFromAbility(realAbilityId);
            if (int(resultAction) > int(P_ACTION::INVALID)) {
                mActionStream << obs->GetGameLoop() << ";" << P_ActionToName(resultAction) << "\n";
            }
            if (realAbilityId == ABILITY_ID::LIFT || realAbilityId == ABILITY_ID::LAND) {
                for (const auto& tag : action.unit_tags) {
                    auto unit = obs->GetUnit(tag);
                    auto addon = obs->GetUnit(unit->add_on_tag);
                    mActionStream << obs->GetGameLoop() << ";" << AbilityTypeToName(action.ability_id) << ";" << UnitTypeToName(unit->unit_type.ToType()) << ";"
                                  << (addon ? UnitTypeToName(addon->unit_type.ToType()) : "None") << "\n";
                }
            }
        }

        bool wroteSmth = false;
        if (!actions.empty()) {
            std::unordered_set<ABILITY_ID> actions_done;
            for (const auto& action : actions) {
                if (actions_done.count(action.ability_id.ToType()) > 0)
                    continue;
                actions_done.emplace(action.ability_id.ToType());
                P_ACTION resultAction = P_ACTION::INVALID;
                ABILITY_ID realAbilityId = action.ability_id;
                if (P_NeedUnitToGetCorrectAbilits(action.ability_id)) {
                    realAbilityId = P_GetActualAbility(action.ability_id, obs->GetUnit(action.unit_tags[0]));
                }
                resultAction = P_GetActionFromAbility(realAbilityId);

                if (int(resultAction) > int(P_ACTION::INVALID)) {
                    wroteSmth = true;
                    writeAction(resultAction, realAbilityId);
                }
            }
        }
#if TRACK_HISTORY
        if (!wroteSmth) {
            writeAction(P_ACTION::INVALID, ABILITY_ID::INVALID);
        }
#endif
    } else {
        mFirstStep = false;
    }

    memcpy(mLastFeatures, mCurrentFeatures, sizeof(mCurrentFeatures));
    mProcessedSteps = obs->GetGameLoop();
}

void P_ReplayParser::OnUnitCreated(const Unit* unit) {
    if (unit->alliance != Unit::Self) {
        return;
    }
    if (mSeenUnits.count(unit->tag) > 0)
        return;
    mSeenUnits.emplace(unit->tag);
    mBuildOrdersFromOnCreated.emplace_back(Observation()->GetGameLoop(), unit->unit_type.ToType());
}

void P_ReplayParser::OnUpgradeCompleted(UpgradeID upgrade) {
    mBuildOrdersFromOnCreated.emplace_back(Observation()->GetGameLoop(), upgrade.ToType());
}

bool P_ReplayParser::IgnoreReplay(const ReplayInfo& replay_info, uint32_t& player_id) {
    for (const auto player : replay_info.players) {
        if (player.race == Protoss && uint32_t(player.player_id) == player_id) {
            // std::cout << replay_info.data_version << std::endl;
            return false;
        }
    }
    return true;
}

void P_BuildOrderEntry::AppendToJSON(std::ofstream& myfile) const {
    myfile << "{";
    myfile << ADD_VALUE("step", int(step)) << ", ";
    myfile << ADD_VALUE("type", (isUpgrade ? int(upgradeId) : int(unitType))) << ", ";
    myfile << ADD_VALUE("type_name", (isUpgrade ? UpgradeIDToName(upgradeId) : UnitTypeToName(unitType))) << ", ";
    myfile << ADD_VALUE("isUpgrade", int(isUpgrade));
    myfile << "}";
}






