Made by Aaron Glave.

I am trying to make a command-line Chess program. 

**Note that a bug exists: reloading a save file right after upgrading a pawn deletes it, but you can get it back my quitting the game and then restarting it.**

Bat files associated with the game are in the [Chess2/PlayChess2](https://github.com/Aaron-Glave/Chess/tree/main/Chess2/PlayChess2) folder.

The code is written in the Chess_code folder and files to help me edit the program with Visual Studio are in the Chess2 folder.

In order to make it easier to debug, the main function is written outside the Chess_code folder.
I debug this game locally with the [Catch2](https://github.com/catchorg/Catch2) library.
I also grant credit to the great people who wrote the [VisualStudio.gitignore](https://github.com/github/gitignore/blob/main/VisualStudio.gitignore) file, which I copied and edited for my own `.gitignore` file.
The developers of that library deserve fantastic credit!
