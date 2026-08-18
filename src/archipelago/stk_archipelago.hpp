//
// Created by Batknight21 on 03.08.26.
//

#ifndef SUPERTUXKART_STK_ARCHIPELAGO_H
#define SUPERTUXKART_STK_ARCHIPELAGO_H
#include "irrString.h"
#include "challenges/challenge_data.hpp"
#include "challenges/challenge_status.hpp"
#include "race/race_manager.hpp"

using namespace irr::core;

namespace APClient
{
    enum APTrack
    {
        CORNFIELD_CROSSING = 1,
        SNOW_PEAK,
        VOLCANO_ISLAND,
        HACIENDA,
        MANSION,
        ABYSS,
        SCOTLAND,
        OLIVER_MATH,
        GRAND_PARADISO_ISLAND,
        CANDELA_CITY,
        LIGHTHOUSE,
        SNOW_MOUNTAIN,
        MINIGOLF,
        GREEN_VALLEY,
        MINES,
        SAND_TRACK,
        ZEN_GARDEN,
        STK_ENTERPRISE,
        XR591,
        COCA_TEMPLE,
        PENGUIN_PLAYGROUND,
        OFF_THE_BEATEN_TRACK,
        TO_THE_MOON_AND_BACK,
        AT_WORLDS_END,
        FORT_MAGMA,
    };

    enum GoalType
    {
        DEFEAT_NOLOK,
        BEAT_GPS
    };

    enum DeathLinkMode
    {
        KNOCKOUT,
        LOOSE_CHALLENGE,
        BOTH // only in send mode
    };

    enum ItemType
    {
        SPEED = 26,
        NITRO,
        POWERUP,
        BANANA_TRAP,
        NITRO_ABILITY,
        DRIFT_ABILITY,
        LOOK_BACK_ABILITY,
        KEY
    };

    void start_ap(const stringw& address, const stringw& slot_name, const stringw& password);

    void challenge_completed(const RaceManager::Difficulty& difficulty, const std::string& challengeId);

    void goal_nolok_completed();

    bool is_unlocked_by_archipelago(const std::string& challengeId, int points);

    void unlocked(const ChallengeData* challenge);

    void clear_unlocked();

    void death_detected(DeathLinkMode mode);

    bool was_completed(RaceManager::Difficulty difficulty, const std::string& challengeId);

    bool difficulty_unlocked(RaceManager::Difficulty difficulty, const std::string& challengeId);

    RaceManager::Difficulty best_difficulty_unlocked(const std::string& challengeId);

    bool difficulty_best_unlocked();

    void set_recent_points(int points);

    void on_race_started();

    int get_recent_points();

    void in_race();

    void nolok_defeated(RaceManager::Difficulty difficulty);

    void gp_finished();

    int get_required_keys();
    int get_current_keys();
    bool has_enough_keys();
    int get_fort_magma_points();
    std::string get_key_display();

    int get_speed_boosts();
    float get_nitros();
    int get_powerups();
    int get_banana_traps();

    bool can_use_nitro();
    bool can_drift();
    bool can_look_back();

    std::vector<std::string> get_recently_unlocked_tracks();

    std::vector<std::string> get_recently_unlocked_gps();
}

#endif //SUPERTUXKART_STK_ARCHIPELAGO_H
