/* Copyright: (c) Kayne Ruse 2013-2015
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#include "server_application.hpp"

//singletons
#include "account_manager.hpp"
#include "character_manager.hpp"
#include "client_manager.hpp"
#include "config_utility.hpp"
#include "room_manager.hpp"
#include "udp_network_utility.hpp"
#include "waypoint_manager.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	try {
		//create the singletons
		AccountManager::CreateSingleton();
		CharacterManager::CreateSingleton();
		ClientManager::CreateSingleton();
		ConfigUtility::CreateSingleton();
		RoomManager::CreateSingleton();
		UDPNetworkUtility::CreateSingleton();

		//call the server's routines
		ServerApplication::CreateSingleton();
		ServerApplication& app = ServerApplication::GetSingleton();

		app.Init(argc, argv);
		app.Proc();
		app.Quit();

		ServerApplication::DeleteSingleton();

		//delete the singletons
		AccountManager::DeleteSingleton();
		CharacterManager::DeleteSingleton();
		ClientManager::DeleteSingleton();
		ConfigUtility::DeleteSingleton();
		RoomManager::DeleteSingleton();
		UDPNetworkUtility::DeleteSingleton();
	}
	catch(exception& e) {
		cerr << "Fatal exception thrown: " << e.what() << endl;
		return 1;
	}
	return 0;
}