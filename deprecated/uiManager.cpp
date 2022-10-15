#include "common.h"
#include "uiManager.h"

UIManager::UIManager()
{
	auto r = gApp->GetRenderer();
	SDL_Surface* surface = SDL_LoadBMP(BUTTON_IMAGE);
	m_buttonTex = SDL_CreateTextureFromSurface(r, surface);

	int windowWidth, windowHeight;
	SDL_GetWindowSize(gApp->GetWindow(), &windowWidth, &windowHeight);

	m_editWindow = new UIItem_Container({ 0, 0, windowWidth, windowHeight }, 0);

	m_nameTabs = new UIItem_Stack(true, { 0, 0, windowWidth, windowHeight }, 0);
	m_editWindow->Add(m_nameTabs, 0, 0, 1.0f, 20, UIItem_Container::X2_REL);

	m_sourceSplit = new UIItem_Splitter({ 0, 0, 100, 100 }, 0);
	m_editWindow->Add(m_sourceSplit, 0, 20, 1.0f, 1.0f, UIItem_Container::X2_REL | UIItem_Container::Y2_REL);

	m_sourceEdit = new UIItem_SourceEdit(0);
	m_editWindow->Add(m_sourceEdit, 0, 20, 1.0f, 1.0f, UIItem_Container::X2_REL | UIItem_Container::Y2_REL);

	m_sourceMemAddr = new UIItem_SourceMemAddr(0);
	m_editWindow->Add(m_sourceEdit, 0, 20, 1.0f, 1.0f, UIItem_Container::X2_REL | UIItem_Container::Y2_REL);

	m_sourceDecode = new UIItem_SourceDecode(0);
	m_editWindow->Add(m_sourceDecode, 0, 20, 1.0f, 1.0f, UIItem_Container::X2_REL | UIItem_Container::Y2_REL);

	m_sourceContextHelp = new UIItem_SourceContextHelp(0);
	m_editWindow->Add(m_sourceContextHelp, 0, 20, 1.0f, 1.0f, UIItem_Container::X2_REL | UIItem_Container::Y2_REL);
}

UIManager::~UIManager()
{
}

