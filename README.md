# CHIP8 Emulator

# Features
- Handles all 35 opcodes as mentioned in the CHIP8 archives.
- Renders most of the ROM's. (Excluding Super CHIP8)
- Currently running at 800 instructions per frame.
- Emulator configured to 60 FPS.

# Future Works
- Plan to add sound timer for ROM's utilising the sound register.
- Making the Emulator more cocnfigurable from the terminal using args. (In terms of display size, refresh rate etc).
- Creating my very own test ROM.

# Compiling and Running
- Make sure build-essentials are installed on the system ```sudo apt-get install build-essentials```.
- ```make``` Compiles the release code provided raylib package is located on the system.
- ```make debug``` Compiles the Debug binary which has required debug output for different opcodes.
- ```make clean``` Removes the binary which is compiled and cleans up the folder.
- After compiling to run ```./chip8 <Rom file name>```

# Screenshots
- Verifying most opcodes are working
  ![Screenshot from 2024-05-09 19-18-29](https://github.com/Cioraz/CHIP8/assets/76161837/30b039d7-662b-4563-bfff-c10cf87f9364)

# References
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
- https://github.com/cj1128/chip8-emulator/tree/master/rom (For BC Test aswell as testopcode ROM's)
- https://github.com/kripod/chip8-roms (Various other ROM's for testing the Emulator)
