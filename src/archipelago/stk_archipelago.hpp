//
// Created by tbgamer on 03.08.26.
//

#ifndef SUPERTUXKART_STK_ARCHIPELAGO_H
#define SUPERTUXKART_STK_ARCHIPELAGO_H
#include "irrString.h"
#include "challenges/challenge_data.hpp"
#include "challenges/challenge_status.hpp"
#include "race/race_manager.hpp"

using namespace irr::core;

void start_ap(const stringw& address, const stringw& slot_name, const stringw& password);

void challenge_completed(const RaceManager::Difficulty& difficulty, const std::string& challengeId);

void goal_nolok_completed();

bool is_unlocked_by_archipelago(const std::string& challengeId, int points);

void unlocked(const ChallengeData* challenge);

void clear_unlocked();

bool was_completed(RaceManager::Difficulty difficulty, const std::string& challengeId);

bool difficulty_unlocked(RaceManager::Difficulty difficulty);

void set_recent_points(int points);

void on_race_started();

int get_recent_points();

void in_race();

int get_required_keys();
int get_current_keys();
int get_fort_magma_points();
std::string get_key_display();

int get_speed_boosts();
float get_nitros();
int get_powerups();
int get_banana_traps();

bool can_use_nitro();
bool can_drift();

std::vector<std::string> get_recently_unlocked_tracks();

std::vector<std::string> get_recently_unlocked_gps();

enum ItemType
{
    SPEED = 17,
    NITRO,
    POWERUP,
    BANANA_TRAP,
    NITRO_ABILITY,
    DRIFT_ABILITY,
    PROGRESSIVE_DIFFICULTY,
    KEY
};

#endif //SUPERTUXKART_STK_ARCHIPELAGO_H
