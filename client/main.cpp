/* Copyright: (c) Kayne Ruse 2013, 2014
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
#include "client_application.hpp"

//singletons
#include "config_utility.hpp"
#include "udp_network_utility.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	try {
		//create the singletons
		ConfigUtility::CreateSingleton();
		UDPNetworkUtility::CreateSingleton();

		//call the client's routines
		ClientApplication::CreateSingleton();
		ClientApplication& app = ClientApplication::GetSingleton();

		app.Init(argc, argv);
		app.Proc();
		app.Quit();

		ClientApplication::DeleteSingleton();

		//delete the singletons
		ConfigUtility::DeleteSingleton();
		UDPNetworkUtility::DeleteSingleton();
	}
	catch(exception& e) {
		cerr << "Fatal exception thrown: " << e.what() << endl;
		return 1;
	}
	return 0;
}