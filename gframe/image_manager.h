#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "config.h"
#include "data_manager.h"
#include <unordered_map>

namespace ygo {

class ImageManager {
public:
	std::vector<std::wstring> ImageList[7];
	int saved_image_id[7];
	bool Initial();
	//random image
	irr::video::ITexture* GetRandomImage(int image_type);
	irr::video::ITexture* GetRandomImage(int image_type, s32 width, s32 height);
	void RefreshRandomImageList();
	void RefreshImageDir(std::wstring path, int image_type);
	void SetDevice(irr::IrrlichtDevice* dev);
	void ClearTexture();
	void RemoveTexture(int code);
	void ResizeTexture();
	irr::video::ITexture* GetTextureFromFile(const char* file, s32 width, s32 height);
	irr::video::ITexture* GetTexture(int code, bool fit = false);
	irr::video::ITexture* GetTextureThumb(int code);
	irr::video::ITexture* GetTextureField(int code);

	std::unordered_map<int, irr::video::ITexture*> tMap[2];
	std::unordered_map<int, irr::video::ITexture*> tThumb;
	std::unordered_map<int, irr::video::ITexture*> tFields;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::video::ITexture* tCover[4];
	irr::video::ITexture* tUnknown;
	irr::video::ITexture* tUnknownFit;
	irr::video::ITexture* tUnknownThumb;
	irr::video::ITexture* tAct;
	irr::video::ITexture* tAttack;
	irr::video::ITexture* tNegated;
	irr::video::ITexture* tChain;
	irr::video::ITexture* tNumber;
	irr::video::ITexture* tLPFrame;
	irr::video::ITexture* tLPBar;
	irr::video::ITexture* tMask;
	irr::video::ITexture* tEquip;
	irr::video::ITexture* tTarget;
	irr::video::ITexture* tChainTarget;
	irr::video::ITexture* tLim;
	irr::video::ITexture* tOT;
	irr::video::ITexture* tHand[3];
	irr::video::ITexture* tBackGround;
	irr::video::ITexture* tBackGround_menu;
	irr::video::ITexture* tBackGround_deck;
	irr::video::ITexture* tCardType;
	irr::video::ITexture* tField[2];
	irr::video::ITexture* tFieldTransparent[2];
	irr::video::ITexture* tRScale[14];
	irr::video::ITexture* tLScale[14];
	irr::video::ITexture* tClock;
	irr::video::ITexture* tAvatar[2];
	irr::video::ITexture* tLPBarFrame;
	irr::video::ITexture* tTurn;
	irr::video::ITexture* tBtn1;
	irr::video::ITexture* tBtn2;
	irr::video::ITexture* tBtn3;
	irr::video::ITexture* tBtn4;
	irr::video::ITexture* tBtn5;
	irr::video::ITexture* tBtn6;
	irr::video::ITexture* tBtnr1;
	irr::video::ITexture* tBtnr2;
	irr::video::ITexture* tBtnr3;
	irr::video::ITexture* tBtnr4;
	irr::video::ITexture* tBtnr5;
	irr::video::ITexture* tBtnr6;
	
	//YGOPROES
	irr::video::ITexture* btnInfo;
	irr::video::ITexture* btnLog;
	irr::video::ITexture* btnHelper;
	irr::video::ITexture* btnSystem;
	
	std::list<std::string> support_types;
};

extern ImageManager imageManager;

}

#endif // IMAGEMANAGER_H
