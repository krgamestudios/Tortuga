/* Copyright: (c) Kayne Ruse 2013
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
* /
#include "server_application.hpp"

#include "SDL/SDL.h"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	cout << "Beginning server" << endl;
	try {
		ServerApplication app;
		app.Init(argc, argv);
		app.Loop();
		app.Quit();
	}
	catch(exception& e) {
		cerr << "Fatal error: " << e.what() << endl;
		return 1;
	}
	cout << "Clean exit" << endl;
	return 0;
}*/

#include "network_packet.hpp"
#include "serial.hpp"
#include "region.hpp"
#include "map_generator.hpp"

#include <iostream>

using namespace std;

int main(int, char**) {
	char buffer[PACKET_BUFFER_SIZE];
	Region region(20, 20, 3, 0, 0);
	NetworkPacket packet;

	packet.meta.type = NetworkPacket::Type::REGION_CONTENT;
	packet.regionInfo.region = &region;

	for (int i = 0; i < packet.regionInfo.region->GetWidth(); i++) {
		for (int j = 0; j < packet.regionInfo.region->GetHeight(); j++) {
			for (int k = 0; k < packet.regionInfo.region->GetDepth(); k++) {
				packet.regionInfo.region->SetTile(i, j, k, i*20*3 + j*3 +k);
			}
		}
	}
	serialize(&packet, buffer);

	for (int i = 0; i < PACKET_BUFFER_SIZE; i++) {
		cout << ((Region::type_t*)(buffer))[i] << ",";
	}
	cout << endl << endl;

	deserialize(&packet, buffer);

	for (int i = 0; i < packet.regionInfo.region->GetWidth(); i++) {
		for (int j = 0; j < packet.regionInfo.region->GetHeight(); j++) {
			for (int k = 0; k < packet.regionInfo.region->GetDepth(); k++) {
				cout << packet.regionInfo.region->GetTile(i, j, k) << ",";
			}
		}
	}
	cout << endl;

	BlankGenerator().Unload(packet.regionInfo.region);
	return 0;
}