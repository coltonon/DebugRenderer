#pragma once
#include <Windows.h>
#include <string>
#ifdef TARGET_SWBF2
#define OFFSET_DEBUGRENDERER 0x14020e4f0 // E8 ?? ?? ?? ?? 4C 8B F0 48 85 C0 0F ?? ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? F3
#define OFFSET_DRAWLINE 0x140211360 // e8 ?? ?? ?? ?? ff c3 8b 0d ?? ?? ?? ?? ff c9
#define OFFSET_DRAWRECT2D 0x140211560 // e8 ?? ?? ?? ?? c7 87 ?? ?? 00 00 37
#define OFFSET_DRAWTEXT 0x1402117c0 // e8 ?? ?? ?? ?? 83 c7 14 48 8d 76 04 4d 8d 76 08 49 83 ed 01
#define OFFSET_DXRENDERER 0x1446bfa38 // 48 8b 0d ?? ?? ?? ?? 48 8d 55 c0 4c 8b 75 c0
#define OFFSET_DXRENDERER_VTABLE_ENDFRAME 13
#endif
#ifdef TARGET_BFV
#define OFFSET_DEBUGRENDERER 0x1405AF5F0
#define OFFSET_DRAWLINE 0x1405B1090 
#define OFFSET_DRAWRECT2D 0x1459805D0 
#define OFFSET_DRAWTEXT 0x1405B15B0 
#define OFFSET_DXRENDERER 0x1446C96A8 
#define OFFSET_DXRENDERER_VTABLE_ENDFRAME 9
#endif

struct Color32 {
	byte r;
	byte g;
	byte b;
	byte a;
};
struct Vec2 {
	float x;
	float y;
};

template <typename T>
void CreateVTableHook(void* pVTable, void* pHook, T& t, uint32_t iIndex) {
	DWORD64** ppVTable = (DWORD64**)pVTable;
	DWORD dwOld = 0;
	VirtualProtect((void*)((*ppVTable) + iIndex), sizeof(PDWORD64), PAGE_EXECUTE_READWRITE, &dwOld);
	PBYTE pOrig = ((PBYTE)(*ppVTable)[iIndex]);
	(*ppVTable)[iIndex] = (DWORD64)pHook;
	VirtualProtect((void*)((*ppVTable) + iIndex), sizeof(PDWORD64), dwOld, &dwOld);
	t = (T)pOrig;
}

typedef DWORD64(__fastcall* tEndFrame)(void*, DWORD64 a2, bool presentEnable);
tEndFrame oEndFrame = nullptr;

class Renderer {
public:
	typedef void(__fastcall* tRENDERERCALLBACK)(Renderer*);
private:
	static tRENDERERCALLBACK callbackFunction;
	static DWORD64 __fastcall hkEndFrame(void* _this, DWORD64 a2, bool presentEnable);
	void* getDebugRenderer(void);
	static Renderer* pRenderer;
public:
	static void setup(tRENDERERCALLBACK callback);
	static void shutdown(void);
	void drawRect(float x, float y, float width, float height, Color32 color);
	void drawText(int x, int y, Color32 color, const std::string& text, float scale);
	void drawText(int x, int y, Color32 color, char* text, float scale);
	void drawLine(float x1, float y1, float x2, float y2, Color32 color);
};

DWORD64 __fastcall Renderer::hkEndFrame(void* _this, DWORD64 a2, bool presentEnable)
{
	Renderer::callbackFunction(Renderer::pRenderer);
	return oEndFrame(_this, a2, presentEnable);
};

void* Renderer::getDebugRenderer(void)
{
	static void* dbgRenderer = nullptr;
	if (dbgRenderer == nullptr) {
		typedef void* (__thiscall* fb__DebugRenderManager_getThreadContext_t)(void);
		fb__DebugRenderManager_getThreadContext_t fb__DebugRenderManager_getThreadContext = (fb__DebugRenderManager_getThreadContext_t)OFFSET_DEBUGRENDERER;
		dbgRenderer = fb__DebugRenderManager_getThreadContext();
	}
	return dbgRenderer;
}

void Renderer::shutdown(void) {
	if (oEndFrame != nullptr) {
		tEndFrame ohEndFrame;
		CreateVTableHook<tEndFrame>(*(void**)OFFSET_DXRENDERER, oEndFrame, ohEndFrame, OFFSET_DXRENDERER_VTABLE_ENDFRAME);
	}
}

void Renderer::setup(tRENDERERCALLBACK callback) {
	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
		Renderer::callbackFunction = callback;
		Renderer::pRenderer = new Renderer();
		CreateVTableHook<tEndFrame>(*(void**)OFFSET_DXRENDERER, Renderer::hkEndFrame, oEndFrame, OFFSET_DXRENDERER_VTABLE_ENDFRAME);
	}
}

void Renderer::drawRect(float x, float y, float width, float height, Color32 color)
{
	Vec2 f = { x, y }, t = { x + width, y + height };
	((void(__fastcall*)(void*, Vec2*, Vec2*, Color32))OFFSET_DRAWRECT2D)(this->getDebugRenderer(), &t, &f, color);
}

void Renderer::drawText(int x, int y, Color32 color, char* text, float scale)
{
	((void(__fastcall*)(void*, int, int, char*, Color32, float))OFFSET_DRAWTEXT)(this->getDebugRenderer(), x, y, text, color, scale);
}

void Renderer::drawText(int x, int y, Color32 color, const std::string& text, float scale) {
	this->drawText(x, y, color, (char*)text.c_str(), scale);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, Color32 color)
{
	Vec2 f = { x1, y1 }, t = { x2, y2 };
	((void(__fastcall*)(void*, Vec2*, Vec2*, Color32))OFFSET_DRAWLINE)(this->getDebugRenderer(), &f, &t, color);
}

Renderer* Renderer::pRenderer = nullptr;
Renderer::tRENDERERCALLBACK Renderer::callbackFunction = nullptr;
