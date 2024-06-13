# Multi-Threaded_Pacman
A project based off Pacman's Rules made Multi-Threaded to improve speed of game 
and make the entities move more independently and fluidly.

Done as a Course Project for Operating Systems, it uses SFML and the POSIX pthreads library.

To build this project, you will need a POSIX compliant Operating System.
eg: Linux, WSL.

You will need to install sfml on your system
sudo apt-get install libsfml-dev

Also ensure the necessary libraries are present in your application, run this command to build the project
g++ main.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -pthread

To run:
./sfml-app

I have also provided a soundsystem.h so if you wish to make changes and add sound effects, those are usable.

For the general game logic, it is different from the traditional Pacman, and some elements are still incomplete.

Only 2 ghosts can be out at any given time due to the semaphores of permit and key, both must be acquired for a ghost to be able
to move, and they return them after a player eats them and sends them back.

Players and Ghosts both have pellets they can use for powerups, ghosts get a speed boost for a certain time upon eating a pellet,
and utilize the A* shortest path algorithm for pathfinding.

Art was mostly done from scratch, and this project was done by myself and Hadiya Tanveer (Github: hadiyatanveer).

