//
// Created by tbgamer on 03.08.26.
//

#include "archipelago/stk_archipelago.hpp"

#include <iostream>
#include <locale>
#include <freetype/freetype.h>

#include "../../lib/APCpp/Archipelago.h"
#include "challenges/story_mode_status.hpp"
#include "challenges/unlock_manager.hpp"
#include "config/player_manager.hpp"
#include "guiengine/message_queue.hpp"
#include "items/powerup.hpp"
#include "karts/abstract_kart.hpp"
#include "modes/world.hpp"
#include "scriptengine/script_engine.hpp"
#include "tracks/track.hpp"
#include "tracks/track_manager.hpp"
#include "tracks/track_object_manager.hpp"


static std::string convert_wchar(const stringw& wtext);
static int get_location_key(int difficulty, int trackId);
static void grant_item(const FillerId& item);
static void set_object_activity(const std::string& challengeId);
static int recent_points;
static int get_difficulty_from_location_key(int key);
static int get_track_from_location_key(int key);

static std::map<std::string, int> track_ids = {
    {"cornfield_crossing", 0},
    {"snowpeak", 1},
    {"volcano_island", 2},
    {"hacienda", 3},
    {"mansion", 4},
    {"abyss", 5},
    {"scotland", 6},
    {"olivermath", 7},
    {"granparadiso_island", 8},
    {"candela_city", 9},
    {"lighthouse", 10},
    {"snowmountain", 11},
    {"minigolf", 12},
    {"green_valley", 13},
    {"mines", 14},
    {"fortmagma", 15},
    {"sandtrack", 16},
    {"zengarden", 17},
    {"stk_enterprise", 18},
    {"xr591", 19},
    {"cocoa_temple", 20},
    {"gp1", 21},
    {"gp2", 22},
    {"gp3", 23},
    {"gp4", 24},
};

static std::map<std::string, std::string> challenge_to_track_object {
    {"olivermath", "Plane"},
    {"candela_city", "Plane.001"},
    {"cornfield_crossing", "Plane.002"},
    {"snowmountain", "Plane.003"},
    {"zengarden", "Plane.004"},
    {"snowpeak", "Plane.005"},
    {"cocoa_temple", "Plane.006"},
    {"sandtrack", "Plane.007"},
    {"hacienda", "Plane.008"},
    {"scotland", "Plane.009"},
    {"abyss", "Plane.010"},
    {"mines", "Plane.011"},
    {"stk_enterprise", "Plane.012"},
    {"volcano_island", "Plane.013"},
    {"lighthouse", "Sphere.000"},
    {"granparadiso_island", "Sphere.010"},
    {"green_valley", "Sphere.011"},
    {"minigolf", "Sphere.012"},
    {"fortmagma", "Sphere.013"},
    {"mansion", "Sphere.015"},
    {"xr591", "Sphere.020"},
    {"gp1", "Sphere.023"},
    {"gp2", "Sphere.024"},
    {"gp3", "Sphere.029"},
    {"gp4", "Sphere.030"}
};

static std::map<int, std::string> item_to_track = {
    {1, "olivermath"},
    {2, "granparadiso_island"},
    {3, "candela_city"},
    {4, "lighthouse"},
    {5, "snowmountain"},
    {6, "minigolf"},
    {7, "green_valley"},
    {8, "mines"},
    {9, "zengarden"},
    {10, "stk_enterprise"},
    {11, "xr591"},
    {12, "cocoa_temple"},
    {13, "gp1"},
    {14, "gp2"},
    {15, "gp3"},
    {16, "gp4"},
};

static std::vector<std::string> recently_unlocked_gps = {};
static std::vector<std::string> recently_unlocked_tracks = {};
static std::vector<std::string> unlocked_challenges = {};
static std::map<std::string, std::set<RaceManager::Difficulty>> completed_challenges = {};
static RaceManager::Difficulty unlocked_difficulty;
static int pending_speed_boosts = 0;
static float pending_nitro_bonus = 0;
static int pending_banana_traps = 0;
static int pending_power_ups = 0;
static bool nitro_ability = false;
static bool drift_ability = false;

void start_ap(const stringw& address, const stringw& slot_name, const stringw& password)
{
    const std::string address_char = convert_wchar(address);
    const std::string slot_char = convert_wchar(slot_name);
    const std::string password_char = convert_wchar(password);
    AP_Init(address_char.c_str(), "Super Tux Kart", slot_char.c_str(), password_char.c_str());

    unlocked_challenges.clear();
    unlocked_difficulty = RaceManager::DIFFICULTY_EASY;

    AP_SetItemClearCallback([]()
    {
        unlocked_challenges.clear();
        completed_challenges.clear();
        nitro_ability = false;
        drift_ability = false;
    });

    AP_SetItemRecvCallback([](int item, bool notify)
    {
        if (item != 0 && item <= 16) {
            unlocked_challenges.push_back(item_to_track[item]);
            if (item < 13)
            {
                recently_unlocked_tracks.push_back(item_to_track[item]);
            }
            else
            {
                recently_unlocked_gps.push_back(item_to_track[item]);
            }
            PlayerManager::getCurrentPlayer()->computeActive();
            set_object_activity(item_to_track[item]);
        }
        else if (item <= 22)
        {
            grant_item(static_cast<FillerId>(item));
        }
        else if (item == 23)
        {
            if (unlocked_difficulty < RaceManager::DIFFICULTY_LAST)
            {
                unlocked_difficulty = static_cast<RaceManager::Difficulty>(static_cast<int>(unlocked_difficulty) + 1);
                if (unlocked_difficulty == RaceManager::DIFFICULTY_BEST)
                {
                    PlayerManager::getCurrentPlayer()->computeActive();
                }
            }
        }
    });

    AP_SetLocationCheckedCallback([](const int location)
    {
        if (location >= 100) // target challenge completed locations only
        {
            for (auto & track_id : track_ids)
            {
                if (track_id.second == get_track_from_location_key(location))
                {
                    completed_challenges[track_id.first].insert(
                        static_cast<RaceManager::Difficulty>(get_difficulty_from_location_key(location)));
                    PlayerManager::getCurrentPlayer()->getStoryModeStatus()->unlockFeature(
                        const_cast<ChallengeStatus*>(PlayerManager::getCurrentPlayer()->getChallengeStatus(track_id.first)),
                        static_cast<RaceManager::Difficulty>(get_difficulty_from_location_key(location)), true);
                }
            }
            PlayerManager::getCurrentPlayer()->computeActive();
        }
    });

    AP_Start();
}

void grant_item(const FillerId& item)
{
    switch (item)
    {
    case SPEED:
        {
            pending_speed_boosts++;
            break;
        }
    case NITRO:
        {
            pending_nitro_bonus += 5;
            if (RaceManager::get()->isLinearRaceMode())
            {
                AbstractKart* kart = World::getWorld()->getLocalPlayerKart(0);
                kart->setEnergy(kart->getEnergy() + pending_nitro_bonus);
                pending_nitro_bonus = 0;
            }
            break;
        }
    case POWERUP:
        {
            if (RaceManager::get()->isLinearRaceMode() && World::getWorld()->getPlayerKart(0)->getNumPowerup() <= 0)
            {
                std::srand(time(nullptr));
                World::getWorld()->getLocalPlayerKart(0)->setPowerup(
                    static_cast<PowerupManager::PowerupType>(std::rand() % PowerupManager::POWERUP_LAST), 1);
            }
            else pending_power_ups++;
            break;
        }
    case BANANA_TRAP:
        {
            pending_banana_traps++;
            break;
        }
    case NITRO_ABILITY:
        {
            nitro_ability = true;
            break;
        }
    case DRIFT_ABILITY:
        {
            drift_ability = true;
            break;
        }
    }
}

void challenge_completed(const RaceManager::Difficulty& difficulty, const std::string& challengeId)
{
    completed_challenges[challengeId].insert(difficulty);
    AP_SendItem(get_location_key(difficulty + 1, track_ids[challengeId]));
}

bool was_completed(const RaceManager::Difficulty difficulty, const std::string& challengeId)
{
    for(std::map<std::string, std::set<RaceManager::Difficulty>>::const_iterator i = completed_challenges.begin();
        i != completed_challenges.end();  i++)
    {
        if (i->first == challengeId)
        {
            for (const RaceManager::Difficulty set_difficulty : i->second)
            {
                if (set_difficulty == difficulty)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool difficulty_unlocked(const RaceManager::Difficulty difficulty)
{
    return difficulty <= unlocked_difficulty;
}

void unlocked(const ChallengeData* challenge)
{
    for(std::map<int, std::string>::const_iterator i = item_to_track.begin();
        i != item_to_track.end();  i++)
    {
        if (i->second == challenge->getChallengeId())
        {
            AP_SendItem(i->first);
        }
    }
}

void goal_nolok_completed()
{
    
}

void set_object_activity(const std::string& challengeId)
{
    Track* track = Track::getCurrentTrack();
    if (RaceManager::get()->getTrackName() == "overworld")
    {
        track->getTrackObjectManager()
        ->getTrackObject("", challenge_to_track_object[challengeId])->setEnabled(false);
    }
}

void set_recent_points(int points)
{
    recent_points = points;
}

int get_recent_points()
{
    return recent_points;
}

void clear_unlocked()
{
    recently_unlocked_tracks.clear();
    recently_unlocked_gps.clear();
}

void on_race_started()
{
    if (pending_power_ups > 0)
    {
        std::srand(time(nullptr));
        World::getWorld()->getLocalPlayerKart(0)->setPowerup(
            static_cast<PowerupManager::PowerupType>(std::rand() % PowerupManager::POWERUP_LAST), 1);
        pending_power_ups--;
    }
    if (pending_nitro_bonus > 0)
    {
        AbstractKart* kart = World::getWorld()->getLocalPlayerKart(0);
        kart->setEnergy(kart->getEnergy() + pending_nitro_bonus);
        pending_nitro_bonus = 0;
    }
}

void in_race()
{
    if (get_powerups() > 0 && World::getWorld()->getLocalPlayerKart(0)->getNumPowerup() == 0)
    {
        std::srand(time(nullptr));
        World::getWorld()->getLocalPlayerKart(0)->setPowerup(
            static_cast<PowerupManager::PowerupType>(std::rand() % PowerupManager::POWERUP_LAST), 1);
        pending_power_ups--;
    }
}

std::vector<std::string> get_recently_unlocked_tracks()
{
    return recently_unlocked_tracks;
}

std::vector<std::string> get_recently_unlocked_gps()
{
    return recently_unlocked_gps;
}

std::string convert_wchar(const stringw& wtext)
{
    std::string s;
    s.reserve(wtext.size());

    for (u32 i = 0; i < wtext.size(); ++i)
    {
        s += static_cast<char>(wtext[i]);
    }

    if (s.empty())
    {
        return "";
    }
    return s;
}

bool is_unlocked_by_archipelago(const std::string& challengeId)
{
    return std::any_of(unlocked_challenges.begin(), unlocked_challenges.end(), [challengeId](const std::string& track)
    {
        return track == challengeId;
    });
}

int get_location_key(const int difficulty, const int trackId)
{
    return difficulty * 100 + trackId;
}

int get_difficulty_from_location_key(const int key)
{
    return std::stoi(std::to_string(key).substr(0, 1)) - 1;
}

int get_track_from_location_key(const int key)
{
    return std::stoi(std::to_string(key).substr(1, 2));
}

int get_speed_boosts()
{
    return pending_speed_boosts;
}

int get_powerups()
{
    return pending_power_ups;
}

float get_nitros()
{
    return pending_nitro_bonus;
}

int get_banana_traps()
{
    return pending_banana_traps;
}

bool can_use_nitro()
{
    return nitro_ability;
}

bool can_drift()
{
    return drift_ability;
}
