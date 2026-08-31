# SuperTuxKart Archipelago

A SuperTuxKart Archipelago implementation. 
SuperTuxKart is a free open-source racing game featuring linux characters like tux

## Set Up

For the setup see [setup up guide](/SET_UP_GUIDE.md)

## How is this game randomized

This implementation supports the story mode only.

# Locations

Completing challenges sends checks into the multiword for **every Difficulty** you complete the challenge in. 
You can see if the challenge was sent to the multiworld in the challenge GUI:

* If the location can be completed, an archipelago symbol will be displayed
* If it was completed already, you can see a green check symbol on the bottom left
* Unlocking tracks by points sends additional checks to the multiworld

# Items

- Every normally locked challenge can't be unlocked by points. 
  You have to receive the Item named the same way as the track to unlock it. 
  This also includes unlocking new difficulties
- Some abilities can be randomized
  - Skid
  - Nitro
  - Look back
- Filler Items
  - Random item
  - Nitro canister

## License

This Project keeps the original license wich can be found [here](/COPYING)

## Building from source

Building from source also works the same as in original SuperTuxKart. Just follow the guide in [`INSTALL.md`](/INSTALL.md)
