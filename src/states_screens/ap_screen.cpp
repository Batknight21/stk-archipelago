//
// Created by tbgamer on 02.08.26.
//

#include "ap_screen.hpp"

#include <iostream>

#include "archipelago/stk_archipelago.hpp"
#include "challenges/story_mode_timer.hpp"
#include "config/player_manager.hpp"
#include "config/user_config.hpp"
#include "guiengine/widgets/bubble_widget.hpp"
#include "guiengine/widgets/button_widget.hpp"
#include "modes/cutscene_world.hpp"
#include "modes/overworld.hpp"
#include "network/network_config.hpp"
#include "online/profile_manager.hpp"
#include "states_screens/cutscene_general.hpp"
#include "states_screens/offline_kart_selection.hpp"

using namespace GUIEngine;

APConnectScreen::APConnectScreen() : Screen("ap_screen.stkgui")
{
}

void APConnectScreen::loadedFromFile()
{
}

void APConnectScreen::init()
{
    Screen::init();
    m_server_address_box = getWidget<TextBoxWidget>("address");
    m_server_address_box->setText("archipelago.gg:");
    m_slot_name_box = getWidget<TextBoxWidget>("slot_name");
    m_password_box = getWidget<TextBoxWidget>("password");
}

void APConnectScreen::eventCallback(Widget* widget, const std::string& name, const int playerID)
{
    if (name == "continue")
    {
        start_ap(m_server_address_box->getText(), m_slot_name_box->getText(), m_password_box->getText());
        NetworkConfig::get()->unsetNetworking();
        PlayerProfile *other_player = PlayerManager::get()->getPlayer(m_slot_name_box->getText());
        PlayerProfile *player = PlayerManager::get()->addNewPlayer(m_slot_name_box->getText());
        PlayerManager::get()->setCurrentPlayer(player);
        if (other_player != nullptr)
        {
            player->setFirstTime(other_player->isFirstTime());
            PlayerManager::get()->deletePlayer(other_player);
        }
        PlayerManager::get()->save();

        // Start the story mode (and speedrun) timer
        story_mode_timer->startTimer();

        if (player->isFirstTime())
        {
            CutsceneWorld::setUseDuration(true);
            StateManager::get()->enterGameState();
            RaceManager::get()->setMinorMode(RaceManager::MINOR_MODE_CUTSCENE);
            RaceManager::get()->setNumKarts( 0 );
            RaceManager::get()->setNumPlayers(0);
            RaceManager::get()->startSingleRace("introcutscene", 999, false);

            std::vector<std::string> parts;
            parts.push_back("introcutscene");
            parts.push_back("introcutscene2");
            ((CutsceneWorld*)World::getWorld())->setParts(parts);
            //RaceManager::get()->startSingleRace("introcutscene2", 999, false);

            CutSceneGeneral* scene = CutSceneGeneral::getInstance();
            scene->push();
            return;
        }
        else
        {
            // Unpause the story mode timer when entering back the story mode
            story_mode_timer->unpauseTimer(false);

            const std::string default_kart = UserConfigParams::m_default_kart;
            if (player->isLocked(default_kart))
            {
                KartSelectionScreen *next = OfflineKartSelectionScreen::getInstance();
                next->setGoToOverworldNext();
                next->setMultiplayer(false);
                next->push();
                return;
            }
            OverWorld::enterOverWorld();
        }
    }
    else if (name == "back")
    {
        StateManager::get()->escapePressed();
    }
}
