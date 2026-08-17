# SuperTuxKart Archipelago

A SuperTuxKart Archipelago implementation. SuperTuxKart is a free racing game 

## Hardware Requirements
To run SuperTuxKart, make sure that your computer's specifications are equal or higher than the following specifications:

* A graphics card capable of 3D rendering - NVIDIA GeForce 470 GTX, AMD Radeon 6870 HD series card or Intel HD Graphics 4000 and newer. OpenGL >= 3.3
* You should have a dual-core CPU that's running at 1 GHz or faster.
* You'll need at least 512 MB of free VRAM (video memory).
* System memory: 1 GB
* Minimum disk space: 700 MB
* Ideally, you'll want a joystick with at least 6 buttons.

## Set Up
The setup is pretty simple:
* Download the AP-World and send it to the host
* Create an options file (usually using the options creator in the archipelago launcher)
* Download the game client from the releases
* Run the game and click story. This opens a new menu where you can connect to the server
* Wait for the host to finish and enter server address, your slot name and password (if the game has one) of your Archipelago game

## How is this game randomized
This implementation supports the story mode only.

# Locations
Completing challenges sends checks into the multiword for **every Difficulty** you complete the challenge in.
You can see if the challenge was sent to the multiworld in the challenge GUI:
* If the location can be completed, an archipelago symbol will be displayed
* If it was completed already, you can see a green check symbol on the bottom left
* Unlocking tracks by points sends additional checks to the multiworld

# Items
Every normally locked challenge can't be unlocked by points. You have to receive the Item named the same way as the track to unlock it.

## License
This Project keeps the original license wich can be found [here](/COPYING)

---

## Building from source

Building from source also works the same as in original SuperTuxKart. Just follow the guide in [`INSTALL.md`](/INSTALL.md)
