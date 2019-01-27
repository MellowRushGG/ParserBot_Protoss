#pragma once
#include "MyEnums.h"

static bool NeedUnitToGetCorrectAbilits(const ABILITY_ID& ability_id) {
    switch (ability_id) {
        case ABILITY_ID::BUILD_REACTOR: return true;
        case ABILITY_ID::BUILD_TECHLAB: return true;
        default:                        return false;
    }
}

static ABILITY_ID GetActualAbility(const ABILITY_ID& ability_id, const UNIT_TYPEID& unit_type) {
    switch (ability_id) {
        case ABILITY_ID::BUILD_REACTOR:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ABILITY_ID::BUILD_REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ABILITY_ID::BUILD_REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ABILITY_ID::BUILD_REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ABILITY_ID::BUILD_REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ABILITY_ID::BUILD_REACTOR_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ABILITY_ID::BUILD_REACTOR_STARPORT;
            } break;
        case ABILITY_ID::BUILD_TECHLAB:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ABILITY_ID::BUILD_TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ABILITY_ID::BUILD_TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ABILITY_ID::BUILD_TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ABILITY_ID::BUILD_TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ABILITY_ID::BUILD_TECHLAB_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ABILITY_ID::BUILD_TECHLAB_STARPORT;
            } break;
    }
    return ability_id;
}

static ACTION GetActionFromAbility(const ABILITY_ID& ability_id, const UNIT_TYPEID& unit_type) {
    switch (ability_id) {
        case ABILITY_ID::BUILD_REACTOR:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ACTION::REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ACTION::REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ACTION::REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ACTION::REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ACTION::REACTOR_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ACTION::REACTOR_STARPORT;
            } break;
        case ABILITY_ID::BUILD_TECHLAB:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ACTION::TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ACTION::TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ACTION::TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ACTION::TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ACTION::TECHLAB_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ACTION::TECHLAB_STARPORT;
            } break;
    }
    return GetActionFromAbility(ability_id);
}

static ABILITY_ID GetActualAbility(const ABILITY_ID& ability_id, const Unit* unit) {
    return GetActualAbility(ability_id, unit->unit_type.ToType());
}

static int GetCurrentMaterialIndex(const MATERIAL& index) {
    if (index == MATERIAL::INVALID)
        return (int)index;
    return (int)index;
}

static int GetInProductionIndex(const MATERIAL& index) {
    if (index == MATERIAL::INVALID)
        return (int)index;
    return (int)SECTORS::START_IN_PRODUCTION + (int)index;
}

static int GetProgressIndex(const MATERIAL& index) {
    if (index == MATERIAL::INVALID)
        return (int)index;
    return (int)SECTORS::START_PROGRESS + (int)index;
}

static int GetGameDataIndex(const GAME_DATA& index) {
    if (index == GAME_DATA::INVALID)
        return (int)index;
    return (int)SECTORS::START_GAME_DATA + (int)index;
}

static int GetPossibleActionIndex(const ACTION& index) {
    if (index == ACTION::INVALID)
        return (int)index;
    return (int)SECTORS::START_POSSIBLE_ACTIONS + (int)index;
}

//static int GetPossibleActionIndex(const ABILITY_ID& abilityId) {
//    MATERIAL unit_idx = GetUnitIndexFromUnitType(GetUnitTypeFromAbility(abilityId));
//    if (unit_idx != MATERIAL::INVALID) {
//        return GetPossibleActionIndex(unit_idx);
//    } else return GetPossibleActionIndex(GetUpgradeFromAbilityId(abilityId));
//}

static int GetUpgradeIndex(const MATERIAL& idx) {
    if (idx == MATERIAL::INVALID)
        return (int)idx;
    return (int)SECTORS::START_CURRENT_MATERIAL + (int)idx;
}



static bool IsGameDataIdx(int idx) {
    if (idx < (int)SECTORS::START_GAME_DATA)
        return false;
    if (idx < GetGameDataIndex(GAME_DATA::MAX))
        return true;
    return false;
}

static bool IsProgressIdx(int idx) {
    if (idx < (int)SECTORS::START_PROGRESS)
        return false;
    if (idx < GetProgressIndex(MATERIAL::MAX))
        return true;
    return false;
}

static bool IsInProductionIdx(int idx) {
    if (idx < (int)SECTORS::START_IN_PRODUCTION)
        return false;
    if (idx < GetInProductionIndex(MATERIAL::MAX))
        return true;
    return false;
}

static bool IsCurrentUnitsIdx(int idx) {
    if (idx < (int)SECTORS::START_CURRENT_MATERIAL)
        return false;
    if (idx < (int)SECTORS::START_CURRENT_MATERIAL + (int)MATERIAL_SECTIONS::START_UPGRADES)
        return true;
    return false;
}

static bool IsUpgradeIdx(int idx) {
    if (idx < (int)SECTORS::START_CURRENT_MATERIAL + (int)MATERIAL_SECTIONS::START_UPGRADES)
        return false;
    if (idx < GetUpgradeIndex(MATERIAL::MAX))
        return true;
    return false;
}


static bool SaveAsFloat(int index) {
    if (IsProgressIdx(index))
        return true;
    if (IsUpgradeIdx(index))
        return true;
    return false;
}

#define CASE_TO_NAME(ENUM_NAME) case ACTION::ENUM_NAME: return #ENUM_NAME;


static const char* ActionToName(ACTION action) {
    switch (action) {
        CASE_TO_NAME(INVALID);
        CASE_TO_NAME(COMMANDCENTER);
        CASE_TO_NAME(ORBITALCOMMAND);
        CASE_TO_NAME(PLANETARYFORTRESS);
        CASE_TO_NAME(SUPPLYDEPOT);
        CASE_TO_NAME(REFINERY);
        CASE_TO_NAME(BUNKER);
        CASE_TO_NAME(ARMORY);
        CASE_TO_NAME(ENGINEERINGBAY);
        CASE_TO_NAME(BARRACKS);
        CASE_TO_NAME(REACTOR_BARRACKS);
        CASE_TO_NAME(TECHLAB_BARRACKS);
        CASE_TO_NAME(FACTORY);
        CASE_TO_NAME(REACTOR_FACTORY);
        CASE_TO_NAME(TECHLAB_FACTORY);
        CASE_TO_NAME(FUSIONCORE);
        CASE_TO_NAME(GHOSTACADEMY);
        CASE_TO_NAME(STARPORT);
        CASE_TO_NAME(REACTOR_STARPORT);
        CASE_TO_NAME(TECHLAB_STARPORT);
        CASE_TO_NAME(SENSORTOWER);
        CASE_TO_NAME(MISSILETURRET);
        CASE_TO_NAME(SCV);
        CASE_TO_NAME(MULE);
        CASE_TO_NAME(MARINE);
        CASE_TO_NAME(REAPER);
        CASE_TO_NAME(MARAUDER);
        CASE_TO_NAME(GHOST);
        CASE_TO_NAME(CYCLONE);
        CASE_TO_NAME(HELLION);
        CASE_TO_NAME(HELLIONTANK);
        CASE_TO_NAME(SIEGETANK);
        CASE_TO_NAME(THOR);
        CASE_TO_NAME(WIDOWMINE);
        CASE_TO_NAME(VIKINGFIGHTER);
        CASE_TO_NAME(MEDIVAC);
        CASE_TO_NAME(BANSHEE);
        CASE_TO_NAME(BATTLECRUISER);
        CASE_TO_NAME(LIBERATOR);
        CASE_TO_NAME(RAVEN);
        CASE_TO_NAME(TERRANSHIPWEAPONS);
        CASE_TO_NAME(TERRANVEHICLEANDSHIPPLATING);
        CASE_TO_NAME(TERRANVEHICLEWEAPONS);
        CASE_TO_NAME(STIMPACK);
        CASE_TO_NAME(COMBATSHIELD);
        CASE_TO_NAME(CONCUSSIVESHELLS);
        CASE_TO_NAME(HISECAUTOTRACKING);
        CASE_TO_NAME(TERRANSTRUCTUREARMORUPGRADE);
        CASE_TO_NAME(NEOSTEELFRAME);
        CASE_TO_NAME(TERRANINFANTRYARMOR);
        CASE_TO_NAME(TERRANINFANTRYWEAPONS);
        CASE_TO_NAME(INFERNALPREIGNITER);
        CASE_TO_NAME(DRILLINGCLAWS);
        CASE_TO_NAME(RAPIDFIRELAUNCHERS);
        CASE_TO_NAME(SMARTSERVOS);
        CASE_TO_NAME(BATTLECRUISERWEAPONREFIT);
        CASE_TO_NAME(PERSONALCLOAKING);
        CASE_TO_NAME(BANSHEECLOAKINGFIELD);
        CASE_TO_NAME(RAVENCORVIDREACTOR);
        CASE_TO_NAME(BANSHEEHYPERFLIGHTROTORS);
        CASE_TO_NAME(HIGHCAPACITYFUELTANKS);
        CASE_TO_NAME(ADVANCEDBALLISTICS);
        CASE_TO_NAME(SUPPLYDROP);
        CASE_TO_NAME(MAX);
        default:
            return "INVALID";
    }
}


/*





*/

static bool P_NeedUnitToGetCorrectAbilits(const ABILITY_ID& ability_id) {
    switch (ability_id) {
        case ABILITY_ID::BUILD_REACTOR: return true;
        case ABILITY_ID::BUILD_TECHLAB: return true;
        default:                        return false;
    }
}

static ABILITY_ID P_GetActualAbility(const ABILITY_ID& ability_id, const UNIT_TYPEID& unit_type) {
    switch (ability_id) {
        case ABILITY_ID::BUILD_REACTOR:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ABILITY_ID::BUILD_REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ABILITY_ID::BUILD_REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ABILITY_ID::BUILD_REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ABILITY_ID::BUILD_REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ABILITY_ID::BUILD_REACTOR_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ABILITY_ID::BUILD_REACTOR_STARPORT;
            } break;
        case ABILITY_ID::BUILD_TECHLAB:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ABILITY_ID::BUILD_TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ABILITY_ID::BUILD_TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ABILITY_ID::BUILD_TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ABILITY_ID::BUILD_TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ABILITY_ID::BUILD_TECHLAB_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ABILITY_ID::BUILD_TECHLAB_STARPORT;
            } break;
    }
    return ability_id;
}

static P_ACTION P_GetActionFromAbility(const ABILITY_ID& ability_id, const UNIT_TYPEID& unit_type) {
    /*switch (ability_id) {
        case ABILITY_ID::BUILD_REACTOR:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ACTION::REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ACTION::REACTOR_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ACTION::REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ACTION::REACTOR_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ACTION::REACTOR_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ACTION::REACTOR_STARPORT;
            } break;
        case ABILITY_ID::BUILD_TECHLAB:
            switch (unit_type) {
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:    return ACTION::TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_BARRACKS:          return ACTION::TECHLAB_BARRACKS;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:     return ACTION::TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_FACTORY:           return ACTION::TECHLAB_FACTORY;
                case UNIT_TYPEID::TERRAN_STARPORTFLYING:    return ACTION::TECHLAB_STARPORT;
                case UNIT_TYPEID::TERRAN_STARPORT:          return ACTION::TECHLAB_STARPORT;
            } break;
    }*/
    return P_GetActionFromAbility(ability_id);
}

static ABILITY_ID P_GetActualAbility(const ABILITY_ID& ability_id, const Unit* unit) {
    return P_GetActualAbility(ability_id, unit->unit_type.ToType());
}

static int P_GetCurrentMaterialIndex(const P_MATERIAL& index) {
    if (index == P_MATERIAL::INVALID)
        return (int)index;
    return (int)index;
}

static int P_GetInProductionIndex(const P_MATERIAL& index) {
    if (index == P_MATERIAL::INVALID)
        return (int)index;
    return (int)P_SECTORS::START_IN_PRODUCTION + (int)index;
}

static int P_GetProgressIndex(const P_MATERIAL& index) {
    if (index == P_MATERIAL::INVALID)
        return (int)index;
    return (int)P_SECTORS::START_PROGRESS + (int)index;
}

static int P_GetGameDataIndex(const P_GAME_DATA& index) {
    if (index == P_GAME_DATA::INVALID)
        return (int)index;
    return (int)P_SECTORS::START_GAME_DATA + (int)index;
}

static int P_GetPossibleActionIndex(const P_ACTION& index) {
    if (index == P_ACTION::INVALID)
        return (int)index;
    return (int)P_SECTORS::START_POSSIBLE_ACTIONS + (int)index;
}

//static int GetPossibleActionIndex(const ABILITY_ID& abilityId) {
//    MATERIAL unit_idx = GetUnitIndexFromUnitType(GetUnitTypeFromAbility(abilityId));
//    if (unit_idx != MATERIAL::INVALID) {
//        return GetPossibleActionIndex(unit_idx);
//    } else return GetPossibleActionIndex(GetUpgradeFromAbilityId(abilityId));
//}

static int P_GetUpgradeIndex(const P_MATERIAL& idx) {
    if (idx == P_MATERIAL::INVALID)
        return (int)idx;
    return (int)P_SECTORS::START_CURRENT_MATERIAL + (int)idx;
}



static bool P_IsGameDataIdx(int idx) {
    if (idx < (int)P_SECTORS::START_GAME_DATA)
        return false;
    if (idx < P_GetGameDataIndex(P_GAME_DATA::MAX))
        return true;
    return false;
}

static bool P_IsProgressIdx(int idx) {
    if (idx < (int)P_SECTORS::START_PROGRESS)
        return false;
    if (idx < P_GetProgressIndex(P_MATERIAL::MAX))
        return true;
    return false;
}

static bool P_IsInProductionIdx(int idx) {
    if (idx < (int)P_SECTORS::START_IN_PRODUCTION)
        return false;
    if (idx < P_GetInProductionIndex(P_MATERIAL::MAX))
        return true;
    return false;
}

static bool P_IsCurrentUnitsIdx(int idx) {
    if (idx < (int)P_SECTORS::START_CURRENT_MATERIAL)
        return false;
    if (idx < (int)P_SECTORS::START_CURRENT_MATERIAL + (int)P_MATERIAL_SECTIONS::START_UPGRADES)
        return true;
    return false;
}

static bool P_IsUpgradeIdx(int idx) {
    if (idx < (int)P_SECTORS::START_CURRENT_MATERIAL + (int)P_MATERIAL_SECTIONS::START_UPGRADES)
        return false;
    if (idx < P_GetUpgradeIndex(P_MATERIAL::MAX))
        return true;
    return false;
}


static bool P_SaveAsFloat(int index) {
    if (P_IsProgressIdx(index))
        return true;
    if (P_IsUpgradeIdx(index))
        return true;
    return false;
}

#define P_CASE_TO_NAME(ENUM_NAME) case P_ACTION::ENUM_NAME: return #ENUM_NAME;

static const char* P_ActionToName(P_ACTION p_action) {
    switch (p_action) {
        P_CASE_TO_NAME(INVALID);
        P_CASE_TO_NAME(NEXUS);
        P_CASE_TO_NAME(PYLON);
        P_CASE_TO_NAME(ASSIMILATOR);
        P_CASE_TO_NAME(FORGE);
        P_CASE_TO_NAME(GATEWAY);
        P_CASE_TO_NAME(WARPGATE);
        P_CASE_TO_NAME(CYBERNETICS_CORE);
        P_CASE_TO_NAME(DARK_SHRINE);
        P_CASE_TO_NAME(PHOTON_CANNON);
        P_CASE_TO_NAME(ROBOTICS_BAY);
        P_CASE_TO_NAME(ROBOTICS_FACILITY);
        P_CASE_TO_NAME(SHIELD_BATTERY);
        P_CASE_TO_NAME(STARGATE);
        P_CASE_TO_NAME(TEMPLAR_ARCHIVE);
        P_CASE_TO_NAME(TWILIGHT_COUNCIL);
        P_CASE_TO_NAME(FLEET_BEACON);
        P_CASE_TO_NAME(PROBE);
        P_CASE_TO_NAME(MOTHERSHIP);
        P_CASE_TO_NAME(ZEALOT);
        P_CASE_TO_NAME(ADEPT);
        P_CASE_TO_NAME(STALKER);
        P_CASE_TO_NAME(HIGH_TEMPLAR);
        P_CASE_TO_NAME(DARK_TEMPLAR);
        P_CASE_TO_NAME(SENTRY);
        P_CASE_TO_NAME(ARCHON);
        P_CASE_TO_NAME(OBSERVER);
        P_CASE_TO_NAME(IMMORTAL);
        P_CASE_TO_NAME(WARP_PRISM);
        P_CASE_TO_NAME(COLOSSUS);
        P_CASE_TO_NAME(DISRUPTOR);
        P_CASE_TO_NAME(PHOENIX);
        P_CASE_TO_NAME(ORACLE);
        P_CASE_TO_NAME(VOID_RAY);
        P_CASE_TO_NAME(TEMPEST);
        P_CASE_TO_NAME(CARRIER);
        P_CASE_TO_NAME(PROTOSSAIRWEAPONS);
        P_CASE_TO_NAME(PROTOSSAIRARMOR);
        P_CASE_TO_NAME(PROTOSSGROUNDWEAPONS);
        P_CASE_TO_NAME(PROTOSSGROUNDARMOR);
        P_CASE_TO_NAME(PROTOSSSHIELDS);
        P_CASE_TO_NAME(BLINK_UPGRADE);
        P_CASE_TO_NAME(ADEPT_UPGRADE);
        P_CASE_TO_NAME(CHARGE_UPGRADE);
        P_CASE_TO_NAME(WARPGATE_RESEARCH);
        P_CASE_TO_NAME(PHOENIX_RANGE_UPGRADE);
        P_CASE_TO_NAME(GRAVITIC_BOOSTERS);
        P_CASE_TO_NAME(GRAVITIC_DRIVE);
        P_CASE_TO_NAME(EXTENDED_THERMAL_LANCE);
        P_CASE_TO_NAME(PSIONIC_STORM);
        P_CASE_TO_NAME(GRAVITON_CATAPULT);
        P_CASE_TO_NAME(GRAVITIC_BOOSTER); //MIGHT BE A PROBLEM THAT THIS IS HERE TWICE & IN MYENUMS ;w;
        P_CASE_TO_NAME(SHADOW_STRIDE);
        P_CASE_TO_NAME(MAX);
        default:
            return "INVALID";
    }
}












































