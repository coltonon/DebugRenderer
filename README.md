# DebugRenderer 

## Dead simple DebugRenderer class for FrostBite engine

![](https://i.imgur.com/hYAR1qP.jpg)

![](https://i.imgur.com/YJ9xsZj.jpg)

As a part of rewriting OpenGameCamera, I decided to switch from dx11 and imgui, to a custom UI system based off the game's internal DebugRenderer system.

Usage requires a **single header**, and a setup function to get things rolling.  Works with SWBF2 and BFV out of the box, offsets will obviously need to be updated.

It should be pretty self explanatory how to use, look at `Source.cpp` for a simple integration.

No fancy bells or whistles, but if you need some quick and dirty code to get started with DebugRenderer, this fits the bill.

Credits to [Speedi13](https://github.com/Speedi13/)