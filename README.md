# Tetris_with_Automatic_mode

Here is a tetris game I made using a free C++ library named SFML, with which you can create a window and display anything you want on it

The commands are very simple:
- Q to go left
- D to go right
- S for soft drop (ie tetromino descends progressively)
- SPACE for hard drop (ie tetromino descends all the field in one time)
- E for clockwise rotation
- Z for anti-clockwise rotation
- C for exchanging the current tetromino with the "held" one
- P for pause

I also made an automatic mode, in which the computer can play the game endlessly. The automatic mode also has an instant mode, that places the tetrominoes at the right position directly. This mode is useful for testing the automatic mode's performances since it is much faster than the classic automatic mode.

The SFML files are not provided with the github. However, you can install them and integrate them to your solution. If you use Visual Studio, you can simply follow this tutorial: https://www.youtube.com/watch?v=axIgxBQVBg0&list=PL21OsoBLPpMOO6zyVlxZ4S4hwkY_SLRW9&ab_channel=HilzeVonck.

The textures I used come from
- https://tetris.com/play-tetris for the background.
- https://fr.m.wikipedia.org/wiki/Fichier:Tetrominoes_IJLO_STZ_Worlds.svg for the tetrominoes.
I also used a few gamerules from the real tetris games, which are:
- The Super Rotation System (SRS), for the rotation and the wall kicks. See https://tetris.fandom.com/wiki/SRS.
- The "Frames per gridcell" (ie tetrominoes' speed) depending on the level. See https://tetris.fandom.com/wiki/Tetris_(NES,_Nintendo).

If you see any bug, please don't hesitate to tell me :)



## Video captures

https://user-images.githubusercontent.com/100591116/160396623-196d0432-ee22-4a06-ad58-40e0a517dca6.MOV



https://user-images.githubusercontent.com/100591116/160396678-2b497538-866a-4452-bbff-0ae05e98869b.MOV

