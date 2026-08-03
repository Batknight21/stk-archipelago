//
// Created by tbgamer on 02.08.26.
//

#ifndef SUPERTUXKART_AP_SCREEN_HPP
#define SUPERTUXKART_AP_SCREEN_HPP
#include "guiengine/screen.hpp"

using namespace GUIEngine;

namespace GUIEngine { class Widget; class LabelWidget; class TextBoxWidget; class ButtonWidget; }

class KartSelectionScreen;
class CutSceneGeneral;

class APConnectScreen : public GUIEngine::Screen,
                        public GUIEngine::ScreenSingleton<APConnectScreen>
{
private:
    friend class GUIEngine::ScreenSingleton<APConnectScreen>;
    APConnectScreen();

    TextBoxWidget *m_slot_name_box;
    TextBoxWidget *m_server_address;

public:

    void init() OVERRIDE;
    void loadedFromFile() OVERRIDE;
    void eventCallback(GUIEngine::Widget* widget, const std::string& name, const int playerID) OVERRIDE;
};

#endif //SUPERTUXKART_AP_SCREEN_HPP