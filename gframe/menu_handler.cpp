#include "config.h"
#include "menu_handler.h"
#include "netserver.h"
#include "duelclient.h"
#include "deck_manager.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "image_manager.h"
#include "sound_manager.h"
#include "game.h"

namespace ygo {

void UpdateDeck() {
	BufferIO::CopyWStr(mainGame->cbCategorySelect->getItem(mainGame->cbCategorySelect->getSelected()),
		mainGame->gameConf.lastcategory, 64);
	BufferIO::CopyWStr(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()),
		mainGame->gameConf.lastdeck, 64);
	char deckbuf[1024];
	char* pdeck = deckbuf;
	BufferIO::WriteInt32(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
	BufferIO::WriteInt32(pdeck, deckManager.current_deck.side.size());
	for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
		BufferIO::WriteInt32(pdeck, deckManager.current_deck.main[i]->first);
	for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
		BufferIO::WriteInt32(pdeck, deckManager.current_deck.extra[i]->first);
	for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
		BufferIO::WriteInt32(pdeck, deckManager.current_deck.side[i]->first);
	DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
}
bool MenuHandler::OnEvent(const irr::SEvent& event) {
	if(mainGame->dField.OnCommonEvent(event))
		return false;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		irr::gui::IGUIElement* caller = event.GUIEvent.Caller;
		s32 id = caller->getID();
		if(mainGame->wQuery->isVisible() && id != BUTTON_YES && id != BUTTON_NO) {
			mainGame->wQuery->getParent()->bringToFront(mainGame->wQuery);
			break;
		}
		if(mainGame->wReplaySave->isVisible() && id != BUTTON_REPLAY_SAVE && id != BUTTON_REPLAY_CANCEL) {
			mainGame->wReplaySave->getParent()->bringToFront(mainGame->wReplaySave);
			break;
		}
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			if(id < 110)
				soundManager.PlaySoundEffect(SOUND_MENU);
			else
				soundManager.PlaySoundEffect(SOUND_BUTTON);
			switch(id) {
			case BUTTON_MODE_EXIT: {
				mainGame->device->closeDevice();
				break;
			}
			case BUTTON_LAN_MODE: {
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_JOIN_SERVER: {
				wchar_t Servidor[40];
				myswprintf(Servidor, L"%ls", mainGame->cbServer->getText());
				
				if ( wcscmp(Servidor,L"Koishi Server MR2020") == 0 ){
					mainGame->ebJoinHost->setText(L"koishi.moecube.com");
					mainGame->ebJoinPort->setText(L"5555");
				} else if ( wcscmp(Servidor,L"Koishi Server TCG") == 0 ){
					mainGame->ebJoinHost->setText(L"koishi.moecube.com");
					mainGame->ebJoinPort->setText(L"1311");
				} else if ( wcscmp(Servidor,L"Koishi Server OCG") == 0 ){
					mainGame->ebJoinHost->setText(L"koishi.moecube.com");
					mainGame->ebJoinPort->setText(L"7210");
				} else if ( wcscmp(Servidor,L"SzefoServer TCG-OCG") == 0 ){
					mainGame->ebJoinHost->setText(L"szefoserver.ddns.net");
					mainGame->ebJoinPort->setText(L"7210");
				} else if ( wcscmp(Servidor,L"Mercury233 Server OCG") == 0 ){
					mainGame->ebJoinHost->setText(L"118.178.111.167");
					mainGame->ebJoinPort->setText(L"233");
				} else if ( wcscmp(Servidor,L"YGOPRO 233 Server") == 0 ){
					mainGame->ebJoinHost->setText(L"s1.ygo233.com");
					mainGame->ebJoinPort->setText(L"233");
				} else if ( wcscmp(Servidor,L"YGOPRO Goat") == 0 ){
					mainGame->ebJoinHost->setText(L"koishi.moecube.com");
					mainGame->ebJoinPort->setText(L"2005");
				} else if ( wcscmp(Servidor,L"YGOPRO Doom Bots of Doom") == 0 ){
					mainGame->ebJoinHost->setText(L"koishi.moecube.com");
					mainGame->ebJoinPort->setText(L"573");
				} else if ( wcscmp(Servidor,L"YGOPRO Speed Duel") == 0 ){
					mainGame->ebJoinHost->setText(L"koishi.moecube.com");
					mainGame->ebJoinPort->setText(L"7373");
				} else if ( wcscmp(Servidor,L"YGOPro 2Pick") == 0 ){
					mainGame->ebJoinHost->setText(L"2pick.mycard.moe");
					mainGame->ebJoinPort->setText(L"765");
				} 
				
				break;
			}
			case BUTTON_JOIN_HOST: {
				bot_mode = false;			
				wchar_t Salas[5];
				myswprintf(Salas, L"%ls", mainGame->ebJoinLista->getText());
				
				if ( wcscmp(Salas,L"1") == 0 ){
					//mainGame->btnJoinHost->setText(L"1");
					int sel = mainGame->lstHostList->getSelected();
					if(sel == -1)
						break;
					int addr = DuelClient::hosts[sel].ipaddr;
					int port = DuelClient::hosts[sel].port;
					wchar_t buf[20];
					myswprintf(buf, L"%d.%d.%d.%d", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
					mainGame->ebJoinHost->setText(buf);
					myswprintf(buf, L"%d", port);
					mainGame->ebJoinPort->setText(buf);
					
				} else if ( wcscmp(Salas,L"2") == 0) {
					//mainGame->btnJoinHost->setText(L"2");
					int sel = mainGame->lstHostList->getSelected();
					if(sel != -1){
						mainGame->ebJoinPass->setText(DuelClient::hosts_srvpro[sel].c_str());
					}
				}
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						if (auto_watch_mode) {
							mainGame->actionSignal.Wait(2000);
							mainGame->device->closeDevice();
						}
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lasthost, 100);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHost->setEnabled(false);
					mainGame->btnJoinHost->setEnabled(false);
					mainGame->btnJoinCancel->setEnabled(false);
				}
				break;
			}
			case BUTTON_JOIN_CANCEL: {
				mainGame->HideElement(mainGame->wLanWindow);
				mainGame->ShowElement(mainGame->wMainMenu);
				if(exit_on_return)
					mainGame->device->closeDevice();
				break;
			}
			case BUTTON_OCG_RANKED: {
				mainGame->ebJoinCondO->setText(L"4");
				
				bot_mode = false;
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						if (auto_watch_mode) {
							mainGame->actionSignal.Wait(2000);
							mainGame->device->closeDevice();
						}
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lasthost, 100);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHostO->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wCreateHostO);
				}	
				break;
			}
			case BUTTON_TCG_RANKED: {
				mainGame->ebJoinCondO->setText(L"5");
				
				bot_mode = false;
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						if (auto_watch_mode) {
							mainGame->actionSignal.Wait(2000);
							mainGame->device->closeDevice();
						}
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lasthost, 100);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHostO->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wCreateHostO);
				}	
				break;
			}
			case BUTTON_LAN_REFRESH: {
				mainGame->ebJoinLista->setText(L"1");
				DuelClient::is_srvpro = false;
				DuelClient::BeginRefreshHost();
				break;
			}
			case BUTTON_IA: {
				mainGame->ebJoinCondO->setText(L"3");
				
				bot_mode = false;
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						if (auto_watch_mode) {
							mainGame->actionSignal.Wait(2000);
							mainGame->device->closeDevice();
						}
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lasthost, 100);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHostO->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wCreateHostO);
				}	
				break;
			}
			case BUTTON_LIST_ROOMS: {
				mainGame->ebJoinCondO->setText(L"2");
				mainGame->ebJoinLista->setText(L"2");
				bot_mode = false;
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						if (auto_watch_mode) {
							mainGame->actionSignal.Wait(2000);
							mainGame->device->closeDevice();
						}
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lasthost, 100);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHostO->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wCreateHostO);
				}	
				break;
			}
			case BUTTON_CREATE_HOST: {
				mainGame->btnHostConfirm->setEnabled(true);
				mainGame->btnHostCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wLanWindow);
				mainGame->ShowElement(mainGame->wCreateHost);
				break;
			}
			case BUTTON_CREATE_HOSTO: {
				mainGame->btnHostConfirmO->setEnabled(true);
				mainGame->btnHostCancelO->setEnabled(true);
				mainGame->HideElement(mainGame->wLanWindow);
				mainGame->ShowElement(mainGame->wCreateHostO);
				break;
			}
			case BUTTON_HOST_CONFIRM: {
				bot_mode = false;
				BufferIO::CopyWStr(mainGame->ebServerName->getText(), mainGame->gameConf.gamename, 20);
				if(!NetServer::StartServer(mainGame->gameConf.serverport))
					break;
				if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
					NetServer::StopServer();
					break;
				}
				mainGame->btnHostConfirm->setEnabled(false);
				mainGame->btnHostCancel->setEnabled(false);
				break;
			}
			case BUTTON_HOST_CONFIRMO: {
				wchar_t texto[20];
				wchar_t Formato[5];
				wchar_t Cartas[5];
				wchar_t Modo[5];
				
				wchar_t ES_FORMATO[5];
				myswprintf(ES_FORMATO, L"%ls", mainGame->cbLFlistO->getText());
				wchar_t ES_CARTAS[10];
				myswprintf(ES_CARTAS, L"%ls", mainGame->cbRuleO->getText());
				wchar_t ES_MODO[10];
				myswprintf(ES_MODO, L"%ls", mainGame->cbMatchModeO->getText());
				
				wchar_t ES_OCG[5];
				wchar_t ES_TCG[5];
				myswprintf(ES_OCG, L"OCG");
				myswprintf(ES_TCG, L"TCG");
				
				wchar_t ES_OCGTCG[10];
				myswprintf(ES_OCGTCG, L"OCG/TCG");
				
				wchar_t ES_MATCH[10];
				wchar_t ES_TAG[10];
				myswprintf(ES_MATCH, L"Match");
				myswprintf(ES_TAG, L"Tag");
				
				if ( wcscmp(ES_FORMATO,ES_OCG) == 0 ){
					myswprintf(Formato, L"OO");
				} else {
					myswprintf(Formato, L"TO");
				}
				
				if ( wcscmp(ES_CARTAS,ES_OCGTCG) == 0 ){
					myswprintf(Cartas, L",OT");
				} else {
					myswprintf(Cartas, L"");
				}
				
				if ( wcscmp(ES_MODO,ES_MATCH) == 0 ){
					myswprintf(Modo, L",M");
				} else if ( wcscmp(ES_MODO,ES_TAG) == 0) {
					myswprintf(Modo, L",T");
				} else {
					myswprintf(Modo, L",S");
				}
				
				myswprintf(texto, L"%ls%ls%ls#%ls", Formato, Cartas, Modo, mainGame->ebServerNameO->getText());
				//mainGame->ebJoinPass->setText(texto);
				mainGame->ebJoinCondO->setText(L"1");
				mainGame->ebJoinPassO->setText(texto);
				
				bot_mode = false;
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						if (auto_watch_mode) {
							mainGame->actionSignal.Wait(2000);
							mainGame->device->closeDevice();
						}
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lasthost, 100);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHostO->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wCreateHostO);
				}	
				
				break;
			}
			case BUTTON_HOST_CANCEL: {
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wCreateHost);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_HOST_CANCELO: {
				mainGame->btnCreateHostO->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wCreateHostO);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_HP_DUELIST: {
				mainGame->cbCategorySelect->setEnabled(true);
				mainGame->cbDeckSelect->setEnabled(true);
				DuelClient::SendPacketToServer(CTOS_HS_TODUELIST);
				break;
			}
			case BUTTON_HP_OBSERVER: {
				DuelClient::SendPacketToServer(CTOS_HS_TOOBSERVER);
				break;
			}
			case BUTTON_HP_KICK: {
				int id = 0;
				while(id < 4) {
					if(mainGame->btnHostPrepKick[id] == caller)
						break;
					id++;
				}
				CTOS_Kick csk;
				csk.pos = id;
				DuelClient::SendPacketToServer(CTOS_HS_KICK, csk);
				break;
			}
			case BUTTON_HP_READY: {
				if(mainGame->cbCategorySelect->getSelected() == -1 || mainGame->cbDeckSelect->getSelected() == -1 ||
					!deckManager.LoadDeck(mainGame->cbCategorySelect, mainGame->cbDeckSelect)) {
					break;
				}
				UpdateDeck();
				DuelClient::SendPacketToServer(CTOS_HS_READY);
				mainGame->cbCategorySelect->setEnabled(false);
				mainGame->cbDeckSelect->setEnabled(false);
				break;
			}
			case BUTTON_HP_NOTREADY: {
				DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
				mainGame->cbCategorySelect->setEnabled(true);
				mainGame->cbDeckSelect->setEnabled(true);
				break;
			}
			case BUTTON_HP_START: {
				DuelClient::SendPacketToServer(CTOS_HS_START);
				mainGame->stRanked->setText(L"");
				break;
			}
			case BUTTON_HP_CANCEL: {
				DuelClient::StopClient();
				mainGame->stRanked->setText(L"");
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->btnStartBot->setEnabled(true);
				mainGame->btnBotCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wHostPrepare);
				if(bot_mode)
					mainGame->ShowElement(mainGame->wSinglePlay);
				else
					mainGame->ShowElement(mainGame->wLanWindow);
				mainGame->wChat->setVisible(false);
				if(exit_on_return)
					mainGame->device->closeDevice();
				break;
			}
			case BUTTON_REPLAY_MODE: {
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wReplay);
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->stReplayInfo->setText(L"");
				mainGame->RefreshReplay();
				break;
			}
			case BUTTON_SINGLE_MODE: {
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wSinglePlay);
				mainGame->RefreshSingleplay();
				mainGame->RefreshBot();
				break;
			}
			case BUTTON_LOAD_REPLAY: {
				if(open_file) {
					ReplayMode::cur_replay.OpenReplay(open_file_name);
					open_file = false;
				} else {
					if(mainGame->lstReplayList->getSelected() == -1)
						break;
					if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected())))
						break;
				}
				mainGame->ClearCardInfo();
				mainGame->wCardImg->setVisible(true);
				mainGame->wInfos->setVisible(false); // Cambiar wInfos true
				//ygopro
				mainGame->tabInfoES->setVisible(true);
				mainGame->tabLogES->setVisible(false);
				mainGame->_tabHelperES->setVisible(false);
				mainGame->_tabSystemES->setVisible(false);
				mainGame->btnInfoES ->setVisible(true);
				mainGame->btnLogES->setVisible(true);
				mainGame->btn_tabHelperES->setVisible(true);
				mainGame->btn_tabSystemES->setVisible(true);
				//end ygoproes
				mainGame->wReplay->setVisible(true);
				mainGame->wReplayControl->setVisible(true);
				mainGame->btnReplayStart->setVisible(false);
				mainGame->btnReplayPause->setVisible(true);
				mainGame->btnReplayStep->setVisible(false);
				mainGame->btnReplayUndo->setVisible(false);
				mainGame->wPhase->setVisible(true);
				mainGame->dField.Clear();
				mainGame->HideElement(mainGame->wReplay);
				mainGame->device->setEventReceiver(&mainGame->dField);
				unsigned int start_turn = _wtoi(mainGame->ebRepStartTurn->getText());
				if(start_turn == 1)
					start_turn = 0;
				ReplayMode::StartReplay(start_turn);
				break;
			}
			case BUTTON_DELETE_REPLAY: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.lock();
				wchar_t textBuffer[256];
				myswprintf(textBuffer, L"%ls\n%ls", mainGame->lstReplayList->getListItem(sel), dataManager.GetSysString(1363));
				mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->guiFont, textBuffer);
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.unlock();
				prev_operation = id;
				prev_sel = sel;
				break;
			}
			case BUTTON_RENAME_REPLAY: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.lock();
				mainGame->wReplaySave->setText(dataManager.GetSysString(1364));
				mainGame->ebRSName->setText(mainGame->lstReplayList->getListItem(sel));
				mainGame->PopupElement(mainGame->wReplaySave);
				mainGame->gMutex.unlock();
				prev_operation = id;
				prev_sel = sel;
				break;
			}
			case BUTTON_CANCEL_REPLAY: {
				mainGame->HideElement(mainGame->wReplay);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_EXPORT_DECK: {
				if(mainGame->lstReplayList->getSelected() == -1)
					break;
				Replay replay;
				wchar_t ex_filename[256];
				wchar_t namebuf[4][20];
				wchar_t filename[256];
				myswprintf(ex_filename, L"%ls", mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected()));
				if(!replay.OpenReplay(ex_filename))
					break;
				const ReplayHeader& rh = replay.pheader;
				if(rh.flag & REPLAY_SINGLE_MODE)
					break;
				int max = (rh.flag & REPLAY_TAG) ? 4 : 2;
				//player name
				for(int i = 0; i < max; ++i)
					replay.ReadName(namebuf[i]);
				//skip pre infos
				for(int i = 0; i < 4; ++i)
					replay.ReadInt32();
				//deck
				for(int i = 0; i < max; ++i) {
					int main = replay.ReadInt32();
					Deck tmp_deck;
					for(int j = 0; j < main; ++j)
						tmp_deck.main.push_back(dataManager.GetCodePointer(replay.ReadInt32()));
					int extra = replay.ReadInt32();
					for(int j = 0; j < extra; ++j)
						tmp_deck.extra.push_back(dataManager.GetCodePointer(replay.ReadInt32()));
					myswprintf(filename, L"./deck/%ls %ls.ydk", ex_filename, namebuf[i]);
					deckManager.SaveDeck(tmp_deck, filename);
				}
				mainGame->stACMessage->setText(dataManager.GetSysString(1335));
				mainGame->PopupElement(mainGame->wACMessage, 20);
				break;
			}
			case BUTTON_BOT_START: {
				int sel = mainGame->lstBotList->getSelected();
				if(sel == -1)
					break;
				bot_mode = true;
#ifdef _WIN32
				if(!NetServer::StartServer(mainGame->gameConf.serverport))
					break;
				if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
					NetServer::StopServer();
					break;
				}
				STARTUPINFOW si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));
				wchar_t cmd[MAX_PATH];
				int flag = 0;
				flag += (mainGame->chkBotHand->isChecked() ? 0x1 : 0);
				myswprintf(cmd, L"Bot.exe \"%ls\" %d %d", mainGame->botInfo[sel].command, flag, mainGame->gameConf.serverport);
				if(!CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					NetServer::StopServer();
					break;
				}
#else
				if(fork() == 0) {
					usleep(100000);
					char arg1[512];
					BufferIO::EncodeUTF8(mainGame->botInfo[sel].command, arg1);
					int flag = 0;
					flag += (mainGame->chkBotHand->isChecked() ? 0x1 : 0);
					char arg2[8];
					sprintf(arg2, "%d", flag);
					char arg3[8];
					sprintf(arg3, "%d", mainGame->gameConf.serverport);
					execl("./bot", "bot", arg1, arg2, arg3, NULL);
					exit(0);
				} else {
					if(!NetServer::StartServer(mainGame->gameConf.serverport))
						break;
					if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
						NetServer::StopServer();
						break;
					}
				}
#endif
				mainGame->btnStartBot->setEnabled(false);
				mainGame->btnBotCancel->setEnabled(false);
				break;
			}
			case BUTTON_LOAD_SINGLEPLAY: {
				if(!open_file && mainGame->lstSinglePlayList->getSelected() == -1)
					break;
				mainGame->singleSignal.SetNoWait(false);
				SingleMode::StartPlay();
				break;
			}
			case BUTTON_CANCEL_SINGLEPLAY: {
				mainGame->HideElement(mainGame->wSinglePlay);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_DECK_EDIT: {
				mainGame->RefreshCategoryDeck(mainGame->cbDBCategory, mainGame->cbDBDecks);
				if(open_file && deckManager.LoadDeck(open_file_name)) {
#ifdef WIN32
					wchar_t *dash = wcsrchr(open_file_name, L'\\');
#else
					wchar_t *dash = wcsrchr(open_file_name, L'/');
#endif
					wchar_t *dot = wcsrchr(open_file_name, L'.');
					if(dash && dot) {
						wchar_t deck_name[256];
						wcsncpy(deck_name, dash + 1, dot - dash - 1);
						deck_name[dot - dash - 1] = L'\0';
						mainGame->ebDeckname->setText(deck_name);
						mainGame->cbDBDecks->setSelected(-1);
					} else {
						for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
							if(!wcscmp(mainGame->cbDBDecks->getItem(i), open_file_name)) {
								mainGame->cbDBDecks->setSelected(i);
								break;
							}
						}
					}
					open_file = false;
				} else if(mainGame->cbDBCategory->getSelected() != -1 && mainGame->cbDBDecks->getSelected() != -1) {
					deckManager.LoadDeck(mainGame->cbDBCategory, mainGame->cbDBDecks);
					mainGame->ebDeckname->setText(L"");
				}
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->deckBuilder.Initialize();
				break;
			}
			case BUTTON_YES: {
				mainGame->HideElement(mainGame->wQuery);
				if(prev_operation == BUTTON_DELETE_REPLAY) {
					if(Replay::DeleteReplay(mainGame->lstReplayList->getListItem(prev_sel))) {
						mainGame->stReplayInfo->setText(L"");
						mainGame->lstReplayList->removeItem(prev_sel);
					}
				}
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			case BUTTON_NO: {
				mainGame->HideElement(mainGame->wQuery);
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			case BUTTON_REPLAY_SAVE: {
				mainGame->HideElement(mainGame->wReplaySave);
				if(prev_operation == BUTTON_RENAME_REPLAY) {
					wchar_t newname[256];
					BufferIO::CopyWStr(mainGame->ebRSName->getText(), newname, 256);
					if(mywcsncasecmp(newname + wcslen(newname) - 4, L".yrp", 4)) {
						myswprintf(newname, L"%ls.yrp", mainGame->ebRSName->getText());
					}
					if(Replay::RenameReplay(mainGame->lstReplayList->getListItem(prev_sel), newname)) {
						mainGame->lstReplayList->setItem(prev_sel, newname, -1);
					} else {
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1365));
					}
				}
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			case BUTTON_REPLAY_CANCEL: {
				mainGame->HideElement(mainGame->wReplaySave);
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_LISTBOX_CHANGED: {
			switch(id) {
			case LISTBOX_LAN_HOST: {
				// int sel = mainGame->lstHostList->getSelected();
				// if(sel == -1)
					// break;
				// if(DuelClient::is_srvpro) {
					////mainGame->ebJoinPass->setText(DuelClient::hosts_srvpro[sel].c_str());
					// break;
				// }
				// int addr = DuelClient::hosts[sel].ipaddr;
				// int port = DuelClient::hosts[sel].port;
				// wchar_t buf[20];
				// myswprintf(buf, L"%d.%d.%d.%d", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
				// mainGame->ebJoinHost->setText(buf);
				// myswprintf(buf, L"%d", port);
				// mainGame->ebJoinPort->setText(buf);
				break;
			}
			case LISTBOX_REPLAY_LIST: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1)
					break;
				if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(sel)))
					break;
				wchar_t infobuf[256];
				std::wstring repinfo;
				time_t curtime = ReplayMode::cur_replay.pheader.seed;
				tm* st = localtime(&curtime);
				wcsftime(infobuf, 256, L"%Y/%m/%d %H:%M:%S\n", st);
				repinfo.append(infobuf);
				wchar_t namebuf[4][20];
				ReplayMode::cur_replay.ReadName(namebuf[0]);
				ReplayMode::cur_replay.ReadName(namebuf[1]);
				if(ReplayMode::cur_replay.pheader.flag & REPLAY_TAG) {
					ReplayMode::cur_replay.ReadName(namebuf[2]);
					ReplayMode::cur_replay.ReadName(namebuf[3]);
				}
				if(ReplayMode::cur_replay.pheader.flag & REPLAY_TAG)
					myswprintf(infobuf, L"%ls\n%ls\n===VS===\n%ls\n%ls\n", namebuf[0], namebuf[1], namebuf[2], namebuf[3]);
				else
					myswprintf(infobuf, L"%ls\n===VS===\n%ls\n", namebuf[0], namebuf[1]);
				repinfo.append(infobuf);
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->SetStaticText(mainGame->stReplayInfo, 180, mainGame->guiFont, repinfo.c_str());
				break;
			}
			case LISTBOX_SINGLEPLAY_LIST: {
				int sel = mainGame->lstSinglePlayList->getSelected();
				if(sel == -1)
					break;
				const wchar_t* name = mainGame->lstSinglePlayList->getListItem(sel);
				wchar_t fname[256];
				myswprintf(fname, L"./single/%ls", name);
				FILE *fp;
#ifdef _WIN32
				fp = _wfopen(fname, L"rb");
#else
				char filename[256];
				BufferIO::EncodeUTF8(fname, filename);
				fp = fopen(filename, "rb");
#endif
				if(!fp) {
					mainGame->stSinglePlayInfo->setText(L"");
					break;
				}
				char linebuf[1024];
				wchar_t wlinebuf[1024];
				std::wstring message = L"";
				bool in_message = false;
				while(fgets(linebuf, 1024, fp)) {
					if(!strncmp(linebuf, "--[[message", 11)) {
						size_t len = strlen(linebuf);
						char* msgend = strrchr(linebuf, ']');
						if(len <= 13) {
							in_message = true;
							continue;
						} else if(len > 15 && msgend) {
							*(msgend - 1) = '\0';
							BufferIO::DecodeUTF8(linebuf + 12, wlinebuf);
							message.append(wlinebuf);
							break;
						}
					}
					if(!strncmp(linebuf, "]]", 2)) {
						in_message = false;
						break;
					}
					if(in_message) {
						BufferIO::DecodeUTF8(linebuf, wlinebuf);
						message.append(wlinebuf);
					}
				}
				fclose(fp);
				mainGame->SetStaticText(mainGame->stSinglePlayInfo, 200, mainGame->guiFont, message.c_str());
				break;
			}
			case LISTBOX_BOT_LIST: {
				int sel = mainGame->lstBotList->getSelected();
				if(sel == -1)
					break;
				mainGame->SetStaticText(mainGame->stBotInfo, 200, mainGame->guiFont, mainGame->botInfo[sel].desc);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch(id) {
			case CHECKBOX_HP_READY: {
				if(!caller->isEnabled())
					break;
				mainGame->env->setFocus(mainGame->wHostPrepare);
				if(static_cast<irr::gui::IGUICheckBox*>(caller)->isChecked()) {
					if(mainGame->cbCategorySelect->getSelected() == -1 || mainGame->cbDeckSelect->getSelected() == -1 ||
						!deckManager.LoadDeck(mainGame->cbCategorySelect, mainGame->cbDeckSelect)) {
						static_cast<irr::gui::IGUICheckBox*>(caller)->setChecked(false);
						break;
					}
					UpdateDeck();
					DuelClient::SendPacketToServer(CTOS_HS_READY);
					mainGame->cbCategorySelect->setEnabled(false);
					mainGame->cbDeckSelect->setEnabled(false);
				} else {
					DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
					mainGame->cbCategorySelect->setEnabled(true);
					mainGame->cbDeckSelect->setEnabled(true);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_BOT_RULE: {
				mainGame->RefreshBot();
				break;
			}
			case COMBOBOX_HP_CATEGORY: {
				int catesel = mainGame->cbCategorySelect->getSelected();
				if(catesel == 3) {
					catesel = 2;
					mainGame->cbCategorySelect->setSelected(2);
				}
				if(catesel >= 0) {
					mainGame->RefreshDeck(mainGame->cbCategorySelect, mainGame->cbDeckSelect);
					mainGame->cbDeckSelect->setSelected(0);
				}
				break;
			}
			}
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
	return false;
}

}
