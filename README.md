_This project is currently in it's early prototype stage. You can see other versions in various branches, tagged as prototype-X._

Tortuga is a 3/4 top down multiplayer RPG set in a large archipelago occupied by warring pirate clans. The emphasis of this game is on multiplayer competition, exploration in an open world, and solo adventuring. The game runs on a Minecraft like server system with a goal of about 50-100 players on a single server. The player characters are tied to the server where they are created, and are susceptible to permadeath: deletion of a character upon death. The servers are designed to allow a large amount of modification by the hosts.

## Libraries

* [SDL](http://www.libsdl.org/) - Simple DirectMedia Layer API
* [SDL_net](http://www.libsdl.org/projects/SDL_net/) - SDL's networking extension, modified; libs/SDL_net
* [Codebase](https://github.com/Ratstail91/Codebase) - generic reusable code modules; libs/Codebase

## Instructions

* This project uses C++11, which is available via GNU (or MinGW) 4.7, or Visual Studio 2012. Personally, I'm using MinGW 4.7.2.
* I'm trying to keep this as IDE agnostic as possible, so if you use an IDE, please add it's files to .gitignore.
* You can read more details on the GitHub wiki [here](https://github.com/Ratstail91/Tortuga/wiki).

## Copyright

Tortuga is released under the [zlib license](http://en.wikipedia.org/wiki/Zlib_License).  

Copyright (c) 2013 Kayne Ruse

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.
