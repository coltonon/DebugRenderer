#define TARGET_SWBF2
//#define TARGET_BFV
#include "Renderer.hpp"

void drawLoop(Renderer* pRenderer) {
	pRenderer->drawLine(0, 0, 100, 100, { 0xff, 0, 0, 0xff });
	pRenderer->drawRect(100, 100, 100, 100, { 0, 0xff, 0, 0xff });
	pRenderer->drawText(100, 210, { 128, 0, 255, 255 }, "Minimal Renderer Test", 2.f);
}

DWORD mainThread(HMODULE hMod)
{
	Renderer::setup(drawLoop);
	for (;;) { 
		Sleep(100);
		if (GetAsyncKeyState(VK_END)) {
			Renderer::shutdown();
			FreeLibraryAndExitThread(hMod, 0);
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, hModule, NULL, NULL);
	return true;
}