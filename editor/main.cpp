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
#include "editor_application.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int, char**) {
	cout << "Beginning editor" << endl;
	try {
		EditorApplication::GetInstance()->Init();
		EditorApplication::GetInstance()->Proc();
		EditorApplication::GetInstance()->Quit();
	}
	catch(exception& e) {
		cerr << "Fatal exception thrown: " << e.what() << endl;
		return 1;
	}
	cout << "Clean exit" << endl;
	return 0;
}
*/
#include "region_pager.hpp"
#include "map_file_format.hpp"
#include "map_generator.hpp"

#include <iostream>

int main() {
	RegionPager<MapGenerator, MapFileFormat> pager(40, 40, 3);
	pager.CreateRegion(0, 0);
	pager.SaveRegion(0, 0);
	pager.LoadRegion(0, 0);
	pager.UnloadRegion(0, 0);
	std::cout << "Finishing program" << std::endl;
	return 0;
}