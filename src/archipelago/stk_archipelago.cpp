//
// Created by Batknight21 on 03.08.26.
//

#include "archipelago/stk_archipelago.hpp"

#include <iostream>
#include <locale>

#include "../../lib/APCpp/Archipelago.h"
#include "challenges/story_mode_status.hpp"
#include "challenges/unlock_manager.hpp"
#include "config/player_manager.hpp"
#include "guiengine/message_queue.hpp"
#include "items/powerup.hpp"
#include "karts/abstract_kart.hpp"
#include "karts/explosion_animation.hpp"
#include "modes/world.hpp"
#include "scriptengine/script_engine.hpp"
#include "tracks/track.hpp"
#include "tracks/track_object_manager.hpp"


namespace APClient
{
    static std::string convert_wchar(const stringw& wtext);
    static int get_location_key(int difficulty, int trackId);
    static void grant_item(const ItemType& item);
    static void set_object_activity(const std::string& challengeId);
    static int recent_points;
    static int get_difficulty_from_location_key(int key);
    static int get_track_from_location_key(int key);
    static void reset_unlocks();
    static bool is_gp(APTrack track);
    static void open_doors();

    // static std::map<std::string, int> track_ids = {
    //     {"cornfield_crossing", 0},
    //     {"snowpeak", 1},
    //     {"volcano_island", 2},
    //     {"hacienda", 3},
    //     {"mansion", 4},
    //     {"abyss", 5},
    //     {"scotland", 6},
    //     {"olivermath", 7},
    //     {"granparadiso_island", 8},
    //     {"candela_city", 9},
    //     {"lighthouse", 10},
    //     {"snowmountain", 11},
    //     {"minigolf", 12},
    //     {"green_valley", 13},
    //     {"mines", 14},
    //     {"fortmagma", 15},
    //     {"sandtrack", 16},
    //     {"zengarden", 17},
    //     {"stk_enterprise", 18},
    //     {"xr591", 19},
    //     {"cocoa_temple", 20},
    //     {"gp1", 21},
    //     {"gp2", 22},
    //     {"gp3", 23},
    //     {"gp4", 24},
    // };

    static std::map<int, std::string> item_to_name = {
        {1, "Cornfield Crossing"},
        {2, "Snow Peak"},
        {3, "Volcan Island"},
        {4, "Hacienda"},
        {5, "Ravenbridge Mansion"},
        {6, "Antediluvian Abyss"},
        {7, "Nessie's Pond"},
        {8, "Oliver Math"},
        {9, "Gran Paradiso Island"},
        {10, "Candela City"},
        {11, "Light House"},
        {12, "Snow Mountain"},
        {13, "Minigolf"},
        {14, "Black Forest"},
        {15, "Mines"},
        {16, "Shifting Sands"},
        {17, "Zen Garden"},
        {18, "STK Enterprise"},
        {19, "xr591"},
        {20, "Cocoa Temple"},
        {21, "Penguin Playground"},
        {22, "Off the Beaten Track"},
        {23, "To the Moon and back"},
        {24, "At World's End"},
        {25, "Fort Magma"},
        {26, "Speed Boost"},
        {27, "Nitro Canister"},
        {28, "Random Powerup"},
        {29, "Banana Trap"},
        {30, "Nitro Ability"},
        {31, "Skid Ability"},
        {32, "Look Back Ability"},
        {33, "Key"}
    };

    static std::map<APTrack, std::string> track_to_id = {
        {CORNFIELD_CROSSING, "cornfield_crossing"},
        {SNOW_PEAK, "snowpeak"},
        {VOLCANO_ISLAND, "volcano_island"},
        {HACIENDA, "hacienda"},
        {MANSION, "mansion"},
        {ABYSS, "abyss"},
        {SCOTLAND, "scotland"},
        {OLIVER_MATH, "olivermath"},
        {GRAND_PARADISO_ISLAND, "granparadiso_island"},
        {CANDELA_CITY, "candela_city"},
        {LIGHTHOUSE, "lighthouse"},
        {SNOW_MOUNTAIN, "snowmountain"},
        {MINIGOLF, "minigolf"},
        {GREEN_VALLEY, "green_valley"},
        {MINES, "mines"},
        {SAND_TRACK, "sandtrack"},
        {ZEN_GARDEN, "zengarden"},
        {STK_ENTERPRISE, "stk_enterprise"},
        {XR591, "xr591"},
        {COCA_TEMPLE, "cocoa_temple"},
        {PENGUIN_PLAYGROUND, "gp1"},
        {OFF_THE_BEATEN_TRACK, "gp2"},
        {TO_THE_MOON_AND_BACK, "gp3"},
        {AT_WORLDS_END, "gp4"},
        {FORT_MAGMA, "fortmagma"}
    };

    static std::map<std::string, APTrack> id_to_track = {
        {"cornfield_crossing", CORNFIELD_CROSSING},
        {"snowpeak", SNOW_PEAK},
        {"volcano_island", VOLCANO_ISLAND},
        {"hacienda", HACIENDA},
        {"mansion", MANSION},
        {"abyss", ABYSS},
        {"scotland", SCOTLAND},
        {"olivermath", OLIVER_MATH},
        {"granparadiso_island", GRAND_PARADISO_ISLAND},
        {"candela_city", CANDELA_CITY},
        {"lighthouse", LIGHTHOUSE},
        {"snowmountain", SNOW_MOUNTAIN},
        {"minigolf", MINIGOLF},
        {"green_valley", GREEN_VALLEY},
        {"mines", MINES},
        {"sandtrack", SAND_TRACK},
        {"zengarden", ZEN_GARDEN},
        {"stk_enterprise", STK_ENTERPRISE},
        {"xr591", XR591},
        {"cocoa_temple", COCA_TEMPLE},
        {"gp1", PENGUIN_PLAYGROUND},
        {"gp2", OFF_THE_BEATEN_TRACK},
        {"gp3", TO_THE_MOON_AND_BACK},
        {"gp4", AT_WORLDS_END},
        {"fortmagma", FORT_MAGMA}
    };

    static std::map<std::string, std::string> challenge_to_track_object = {
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

    // static std::map<int, std::string> item_to_track = {
    //     {1, "olivermath"},
    //     {2, "granparadiso_island"},
    //     {3, "candela_city"},
    //     {4, "lighthouse"},
    //     {5, "snowmountain"},
    //     {6, "minigolf"},
    //     {7, "green_valley"},
    //     {8, "mines"},
    //     {9, "zengarden"},
    //     {10, "stk_enterprise"},
    //     {11, "xr591"},
    //     {12, "cocoa_temple"},
    //     {13, "gp1"},
    //     {14, "gp2"},
    //     {15, "gp3"},
    //     {16, "gp4"},
    // };

    static std::map<APTrack, int> unlocked_challenges = {
        {CORNFIELD_CROSSING, 0},
        {SNOW_PEAK, 0},
        {VOLCANO_ISLAND, 0},
        {HACIENDA, 0},
        {MANSION, 0},
        {ABYSS, 0},
        {SCOTLAND, 0},
        {OLIVER_MATH, 0},
        {GRAND_PARADISO_ISLAND, 0},
        {CANDELA_CITY, 0},
        {LIGHTHOUSE, 0},
        {SNOW_MOUNTAIN, 0},
        {MINIGOLF, 0},
        {GREEN_VALLEY, 0},
        {MINES, 0},
        {SAND_TRACK, 0},
        {ZEN_GARDEN, 0},
        {STK_ENTERPRISE, 0},
        {XR591, 0},
        {COCA_TEMPLE, 0},
        {PENGUIN_PLAYGROUND, 0},
        {OFF_THE_BEATEN_TRACK, 0},
        {TO_THE_MOON_AND_BACK, 0},
        {AT_WORLDS_END, 0},
        {FORT_MAGMA, 0}
    };

    static const std::vector<APTrack> PRE_UNLOCKED_TRACKS = {
        CORNFIELD_CROSSING,
        SNOW_PEAK,
        VOLCANO_ISLAND,
        HACIENDA,
        MANSION,
        ABYSS,
        SCOTLAND,
        SAND_TRACK
    };

    static std::vector<APTrack> recently_unlocked_tracks = {};
    static std::map<APTrack, std::set<RaceManager::Difficulty>> completed_challenges = {};
    static int pending_speed_boosts = 0;
    static float pending_nitro_bonus = 0;
    static int pending_banana_traps = 0;
    static int pending_power_ups = 0;
    static bool nitro_ability = false;
    static bool drift_ability = false;
    static bool look_back_ability = false;
    static bool recent_knockout = false;

    static DeathLinkMode death_link_receive = KNOCKOUT;
    static DeathLinkMode death_link_send = LOOSE_CHALLENGE;

    static RaceManager::Difficulty goal_difficulty;
    static GoalType goal_type;
    static int required_keys;
    static int collected_keys;
    static int fort_magma_points;
    static bool keys_initialized = false;

    void start_ap(const stringw& address, const stringw& slot_name, const stringw& password)
    {
        const std::string address_char = convert_wchar(address);
        const std::string slot_char = convert_wchar(slot_name);
        const std::string password_char = convert_wchar(password);
        AP_Init(address_char.c_str(), "Super Tux Kart", slot_char.c_str(), password_char.c_str());

        unlocked_challenges.clear();
#ifdef DEBUG
        unlocked_challenges[CORNFIELD_CROSSING] = 1;
        unlocked_challenges[SNOW_PEAK] = 1;
        unlocked_challenges[VOLCANO_ISLAND] = 1;
        unlocked_challenges[HACIENDA] = 1;
        unlocked_challenges[MANSION] = 1;
        unlocked_challenges[ABYSS] = 1;
        unlocked_challenges[SCOTLAND] = 1;
        unlocked_challenges[OLIVER_MATH] = 1;
        unlocked_challenges[GRAND_PARADISO_ISLAND] = 1;
        unlocked_challenges[CANDELA_CITY] = 1;
        unlocked_challenges[LIGHTHOUSE] = 1;
        unlocked_challenges[SNOW_MOUNTAIN] = 1;
        unlocked_challenges[MINIGOLF] = 1;
        unlocked_challenges[GREEN_VALLEY] = 1;
        unlocked_challenges[MINES] = 1;
        unlocked_challenges[SAND_TRACK] = 1;
        unlocked_challenges[ZEN_GARDEN] = 1;
        unlocked_challenges[STK_ENTERPRISE] = 1;
        unlocked_challenges[XR591] = 1;
        unlocked_challenges[COCA_TEMPLE] = 1;
        unlocked_challenges[PENGUIN_PLAYGROUND] = 1;
        unlocked_challenges[OFF_THE_BEATEN_TRACK] = 1;
        unlocked_challenges[TO_THE_MOON_AND_BACK] = 1;
        unlocked_challenges[AT_WORLDS_END] = 1;

        completed_challenges[CORNFIELD_CROSSING] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[SNOW_PEAK] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[VOLCANO_ISLAND] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[HACIENDA] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[MANSION] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[ABYSS] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[SCOTLAND] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[OLIVER_MATH] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[GRAND_PARADISO_ISLAND] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[CANDELA_CITY] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[LIGHTHOUSE] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[SNOW_MOUNTAIN] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[MINIGOLF] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[GREEN_VALLEY] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[MINES] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[SAND_TRACK] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[ZEN_GARDEN] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[STK_ENTERPRISE] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[XR591] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[COCA_TEMPLE] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[PENGUIN_PLAYGROUND] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[OFF_THE_BEATEN_TRACK] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[TO_THE_MOON_AND_BACK] = {RaceManager::DIFFICULTY_EASY};
        completed_challenges[AT_WORLDS_END] = {RaceManager::DIFFICULTY_EASY};

        for (auto i = completed_challenges.begin(); i != completed_challenges.end(); i++)
        {
            PlayerManager::getCurrentPlayer()->getStoryModeStatus()->unlockFeature(
                const_cast<ChallengeStatus*>(PlayerManager::getCurrentPlayer()->getChallengeStatus(
                    track_to_id[i->first])), RaceManager::DIFFICULTY_EASY, true);
        }
        PlayerManager::getCurrentPlayer()->computeActive();
#endif

        AP_SetItemClearCallback([]
        {
#ifdef NDEBUG
            reset_unlocks();
            completed_challenges.clear();
            nitro_ability = false;
            drift_ability = false;
#endif
        });

        AP_SetItemRecvCallback([](int item, bool notify)
        {
            if (item <= 25)
            {
#ifdef NDEBUG
                const auto track = static_cast<APTrack>(item);
                if (unlocked_challenges[track] == 0 && std::find(PRE_UNLOCKED_TRACKS.begin(),
                    PRE_UNLOCKED_TRACKS.end(), track) != PRE_UNLOCKED_TRACKS.end())
                {
                    recently_unlocked_tracks.push_back(track);
                }
                unlocked_challenges[track]++;
                PlayerManager::getCurrentPlayer()->computeActive();
                set_object_activity(track_to_id[track]);
#endif
            }
            else
            {
                grant_item(static_cast<ItemType>(item));
            }

            if (notify)
            {
                const std::string message = "Received: " + item_to_name[item];
                MessageQueue::add(MessageQueue::MT_ARCHIPELAGO, message.data());
            }
        });

        AP_SetLocationCheckedCallback([](const int location)
        {
#ifdef NDEBUG
            if (location >= 100) // target challenge completed locations only
            {
                const auto track = static_cast<APTrack>(get_track_from_location_key(location));
                completed_challenges[track].insert(
                    static_cast<RaceManager::Difficulty>(get_difficulty_from_location_key(location)));

                PlayerManager::getCurrentPlayer()->getStoryModeStatus()->unlockFeature(
                    const_cast<ChallengeStatus*>(PlayerManager::getCurrentPlayer()->getChallengeStatus(
                        track_to_id[track])),
                    static_cast<RaceManager::Difficulty>(get_difficulty_from_location_key(location)), true);

                PlayerManager::getCurrentPlayer()->computeActive();
            }
#endif
        });

        AP_RegisterSlotDataIntCallback("goal", [](const int goal)
        {
            goal_type = static_cast<GoalType>(goal);
        });

        AP_RegisterSlotDataIntCallback("goal_difficulty", [](const int difficulty)
        {
            goal_difficulty = static_cast<RaceManager::Difficulty>(difficulty);
        });

        AP_RegisterSlotDataIntCallback("required_keys", [](const int keys)
        {
            required_keys = keys;
            keys_initialized = true;
        });

        AP_RegisterSlotDataIntCallback("required_points", [](const int points)
        {
            fort_magma_points = points;
        });

        AP_RegisterSlotDataIntCallback("nitro", [](const bool nitro)
        {
            if (!nitro)
            {
                nitro_ability = true;
            }
        });

        AP_RegisterSlotDataIntCallback("skid", [](const bool skid)
        {
            if (!skid)
            {
                drift_ability = true;
            }
        });

        AP_RegisterSlotDataIntCallback("look_back", [](const bool look_back)
        {
            if (!look_back)
            {
                look_back_ability = true;
            }
        });

        AP_RegisterSlotDataIntCallback("death_link_send_mode", [](int send_mode)
        {
            death_link_send = static_cast<DeathLinkMode>(send_mode);
        });

        AP_RegisterSlotDataIntCallback("death_link_receive_mode", [](int receive_mode)
        {
            death_link_receive = static_cast<DeathLinkMode>(receive_mode);
        });

        AP_SetDeathLinkSupported(true);

        AP_SetDeathLinkRecvCallback([](const std::string& source, const std::string& cause)
        {
            const std::string message = source + " died: " + cause;
            MessageQueue::add(MessageQueue::MT_ARCHIPELAGO, message.data());
            switch (death_link_receive)
            {
            case KNOCKOUT:
                {
                    if (RaceManager::get()->isLinearRaceMode())
                    {
                        recent_knockout = true;
                        AbstractKart* kart = World::getWorld()->getPlayerKart(0);
                        ExplosionAnimation::create(kart, kart->getXYZ(), true);
                    }
                    break;
                }
            case LOOSE_CHALLENGE:
                {
                    if (RaceManager::get()->getMajorMode() == RaceManager::MAJOR_MODE_GRAND_PRIX)
                        RaceManager::get()->addSkippedTrackInGP();
                    World::getWorld()->getRaceGUI()->removeReferee();
                    World::getWorld()->endRaceEarly();
                    break;
                }
            }
        });

        AP_EnableQueueItemRecvMsgs(false);

        AP_Start();
    }

    void grant_item(const ItemType& item)
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
        case LOOK_BACK_ABILITY:
            {
                look_back_ability = true;
                break;
            }
        case KEY:
            {
                collected_keys++;
                if (has_enough_keys())
                {
                    open_doors();
                }
                break;
            }
        }
    }

    void challenge_completed(const RaceManager::Difficulty& difficulty, const std::string& challengeId)
    {
        const APTrack track = id_to_track[challengeId];
        completed_challenges[track].insert(difficulty);
        AP_SendItem(get_location_key(difficulty + 1, track));
    }

    bool was_completed(const RaceManager::Difficulty difficulty, const std::string& challengeId)
    {
        for (std::map<APTrack, std::set<RaceManager::Difficulty>>::const_iterator i = completed_challenges.begin();
             i != completed_challenges.end(); i++)
        {
            if (i->first == id_to_track[challengeId])
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

    bool difficulty_best_unlocked()
    {
        for (const auto & unlocked_challenge : unlocked_challenges)
        {
            if (unlocked_challenge.first == FORT_MAGMA) continue;
            if (unlocked_challenge.second >= 4)
                return true;
        }
        return false;
    }

    bool difficulty_unlocked(const RaceManager::Difficulty difficulty, const std::string& challengeId)
    {
        return unlocked_challenges[id_to_track[challengeId]] >= difficulty + 1;
    }

    void unlocked(const ChallengeData* challenge)
    {
        AP_SendItem(id_to_track[challenge->getChallengeId()]);
    }

    void death_detected(const DeathLinkMode mode)
    {
        if (mode == KNOCKOUT && recent_knockout)
        {
            recent_knockout = false;
            return;
        }

        if (mode == death_link_send || death_link_send == BOTH)
        {
            AP_DeathLinkSend();
        }
    }

    void set_object_activity(const std::string& challengeId)
    {
        if (challengeId == "fortmagma") return;
        Track* track = Track::getCurrentTrack();
        if (RaceManager::get()->getTrackName() == "overworld")
        {
            track->getTrackObjectManager()
                 ->getTrackObject("", challenge_to_track_object[challengeId])->setEnabled(false);
        }
    }

    void open_doors()
    {
        Track* track = Track::getCurrentTrack();
        if (RaceManager::get()->getTrackName() == "overworld")
        {
            track->getTrackObjectManager()
                 ->getTrackObject("", "RDoor")->setEnabled(false);
            track->getTrackObjectManager()
                 ->getTrackObject("", "RDoor.001")->setEnabled(true);
            track->getTrackObjectManager()
                 ->getTrackObject("", "LDoor")->setEnabled(false);
            track->getTrackObjectManager()
                 ->getTrackObject("", "LDoor.001")->setEnabled(true);
            track->getTrackObjectManager()
                 ->getTrackObject("", "Empty.043")->setEnabled(false);
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

    void nolok_defeated(const RaceManager::Difficulty difficulty)
    {
        if (goal_type == DEFEAT_NOLOK && difficulty >= goal_difficulty)
        {
            AP_StoryComplete();
        }
    }

    void gp_finished()
    {
        if (goal_type == BEAT_GPS)
        {
            bool completed = true;
            PlayerProfile* player = PlayerManager::getCurrentPlayer();
            const std::vector<const ChallengeStatus*> gp_challenges = {
                player->getChallengeStatus("gp1"),
                player->getChallengeStatus("gp2"),
                player->getChallengeStatus("gp3"),
                player->getChallengeStatus("gp4")
            };
            for (const ChallengeStatus* gp_challenge : gp_challenges)
            {
                if (gp_challenge->highestSolved() < goal_difficulty)
                {
                    completed = false;
                }
            }

            if (completed)
            {
                AP_StoryComplete();
            }
        }
    }

    void reset_unlocks()
    {
        unlocked_challenges = {
            {CORNFIELD_CROSSING, 0},
            {SNOW_PEAK, 0},
            {VOLCANO_ISLAND, 0},
            {HACIENDA, 0},
            {MANSION, 0},
            {ABYSS, 0},
            {SCOTLAND, 0},
            {OLIVER_MATH, 0},
            {GRAND_PARADISO_ISLAND, 0},
            {CANDELA_CITY, 0},
            {LIGHTHOUSE, 0},
            {SNOW_MOUNTAIN, 0},
            {MINIGOLF, 0},
            {GREEN_VALLEY, 0},
            {MINES, 0},
            {SAND_TRACK, 0},
            {ZEN_GARDEN, 0},
            {STK_ENTERPRISE, 0},
            {XR591, 0},
            {COCA_TEMPLE, 0},
            {PENGUIN_PLAYGROUND, 0},
            {OFF_THE_BEATEN_TRACK, 0},
            {TO_THE_MOON_AND_BACK, 0},
            {AT_WORLDS_END, 0},
            {FORT_MAGMA, 0}
        };
    }

    std::vector<std::string> get_recently_unlocked_tracks()
    {
        std::vector<std::string> tracks;
        for (APTrack track : recently_unlocked_tracks)
        {
            if (!is_gp(track))
            {
                tracks.push_back(track_to_id[track]);
            }
        }
        return tracks;
    }

    std::vector<std::string> get_recently_unlocked_gps()
    {
        std::vector<std::string> tracks;
        for (APTrack track : recently_unlocked_tracks)
        {
            if (is_gp(track))
            {
                tracks.push_back(track_to_id[track]);
            }
        }
        return tracks;
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

    bool is_unlocked_by_archipelago(const std::string& challengeId, const int points)
    {
        if (challengeId == "fortmagma")
        {
            return fort_magma_points <= points;
        }
        return unlocked_challenges[id_to_track[challengeId]] > 0;
    }

    bool is_gp(const APTrack track)
    {
        return (track == PENGUIN_PLAYGROUND) || (track == OFF_THE_BEATEN_TRACK)
        || (track == TO_THE_MOON_AND_BACK) || (track == AT_WORLDS_END);
    }

    int get_current_keys()
    {
        return collected_keys;
    }

    int get_required_keys()
    {
        return required_keys;
    }

    bool has_enough_keys()
    {
        if (!keys_initialized) return false;

        return required_keys <= collected_keys;
    }

    RaceManager::Difficulty best_difficulty_unlocked(const std::string& challengeId)
    {
        int unlocked_count = unlocked_challenges[id_to_track[challengeId]];
        unlocked_count--;
        if (unlocked_count < 0) return RaceManager::DIFFICULTY_FIRST;
        return static_cast<RaceManager::Difficulty>(unlocked_count);
    }

    int get_fort_magma_points()
    {
        return fort_magma_points;
    }

    std::string get_key_display()
    {
        return std::to_string(collected_keys) + "/" + std::to_string(required_keys);
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

    bool can_look_back()
    {
        return look_back_ability;
    }
}
