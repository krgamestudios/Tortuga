/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "base_scene.hpp"

SDL_Renderer* BaseScene::rendererHandle = nullptr;

BaseScene::BaseScene() {
	//EMPTY
}

BaseScene::~BaseScene() {
	//EMPTY
}

void BaseScene::RenderFrame(SDL_Renderer* renderer) {
	//EMPTY
}

void BaseScene::SetRenderer(SDL_Renderer* r) {
	rendererHandle = r;
}

SDL_Renderer* BaseScene::GetRenderer() {
	return rendererHandle;
}

void BaseScene::SetSceneSignal(SceneSignal signal) {
	sceneSignal = signal;
}

SceneSignal BaseScene::GetSceneSignal() {
	return sceneSignal;
}

//-------------------------
//frame phases
//-------------------------

void BaseScene::FrameStart() {
	//EMPTY
}

void BaseScene::Update() {
	//EMPTY
}

void BaseScene::FrameEnd() {
	//EMPTY
}

//-------------------------
//input events
//-------------------------

void BaseScene::QuitEvent() {
	sceneSignal = SceneSignal::QUIT;
}

void BaseScene::MouseMotion(SDL_MouseMotionEvent const& event) {
	//EMPTY
}

void BaseScene::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	//EMPTY
}

void BaseScene::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	//EMPTY
}

void BaseScene::MouseWheel(SDL_MouseWheelEvent const& event) {
	//EMPTY
}

void BaseScene::KeyDown(SDL_KeyboardEvent const& event) {
	//preference as a default
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void BaseScene::KeyUp(SDL_KeyboardEvent const& event) {
	//EMPTY
}

void BaseScene::TextInput(SDL_TextInputEvent const& event) {
	//EMPTY
}
