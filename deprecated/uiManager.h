#pragma once

#include "uiItem.h"
#include "uiItem_container.h"
#include "uiItem_stack.h"
#include "uiItem_splitter.h"
#include "uiItem_sourceEdit.h"
#include "uiItem_sourceMemAddr.h"
#include "uiItem_sourceDecode.h"
#include "uiItem_sourceContextHelp.h"

#define BUTTON_IMAGE "data/button_frame"



class UIManager
{
public:
	UIManager();
	~UIManager();

	void Draw();

protected:
	vector<UIItem*> m_items;
	SDL_Texture* m_buttonTex;

	// events
	void OnLoadPressed(UIItem* item) {}

	// ui containers
	UIItem_Container* m_editWindow;					// full edit window ui tree
	UIItem_Stack* m_nameTabs;						// name tabs along the top
	UIItem_Splitter* m_sourceSplit;					// splits memAddr/decode/sourceEdit/contextHelp horizontally
	UIItem_SourceEdit* m_sourceEdit;				// where you actually edit the source
	UIItem_SourceMemAddr* m_sourceMemAddr;			// shows memory address lines
	UIItem_SourceDecode* m_sourceDecode;			// shows assembly decode bytes
	UIItem_SourceContextHelp* m_sourceContextHelp;	// shows help about current instruction you are editting
};

