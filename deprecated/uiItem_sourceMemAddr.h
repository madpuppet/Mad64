#pragma once

class UIItem_SourceMemAddr : public UIItem
{
public:
	UIItem_SourceMemAddr(void *userData) : UIItem(userData) {}

	virtual void OnDraw();
};
