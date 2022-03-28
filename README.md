# Tetris_with_Automatic_mode

Here is a Tetris game I made using a free C++ library named SFML, which allows us to draw things into the screen. You can find more informations about SFML here : https://www.sfml-dev.org/.

The commands are very simple:
- Q to go left
- D to go right
- S for soft drop (ie Tetromino descends progressively)
- SPACE for hard drop (ie Tetromino descends all the field in one time)
- Z for anti-clockwise rotation
- E for clockwise rotation
- C for exchanging the current tetromino with the "held" Tetromino
- P for pause

I also made an automatic mode, in which the computer plays the game automatically and endlessly. You can find a little demo below.

The SFML files are not provided with the project. However, once you download the project, you can install them and integrate them to your solution. If you use Visual Studio, you can simply follow this tutorial: https://www.youtube.com/watch?v=axIgxBQVBg0&list=PL21OsoBLPpMOO6zyVlxZ4S4hwkY_SLRW9&ab_channel=HilzeVonck.

The textures I used come from
- https://tetris.com/play-tetris for the background.
- https://fr.m.wikipedia.org/wiki/Fichier:Tetrominoes_IJLO_STZ_Worlds.svg for the tetrominoes.
I also used a few gamerules from the real tetris games, which are:
- The Super Rotation System (SRS), for the rotation and the wall kicks. See https://tetris.fandom.com/wiki/SRS.
- The "Frames per gridcell" (ie tetrominoes' speed) depending on the level. See https://tetris.fandom.com/wiki/Tetris_(NES,_Nintendo).

If you see any bug, please don't hesitate to tell me :)



## Video captures



https://user-images.githubusercontent.com/100591116/160397555-9c8ac1fd-680f-49ae-9874-d353fad0104c.MOV



https://user-images.githubusercontent.com/100591116/160397583-31ae7e90-de25-40f7-9f54-d46607c3d817.MOV

