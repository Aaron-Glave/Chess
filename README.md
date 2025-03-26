I am trying to make a command-line Chess program. There's still a ton of stuff to do. I want to:
- Detect whether it is possible to get out of check when I am in check
- Turn the possibility to checkmate on while maintaining the ability for kings to hug. That will probably involve editing Chess.cpp.

The code is written in the Chess_code folder and files to help me edit the program with Visual Studio are in the Visual Studio folder.

In order to make it easier to debug, the main function is written outside the Chess_code folder.
I debug it locally with the [Catch2](https://github.com/catchorg/Catch2) library.
The developers of that library deserve fantastic credit!
