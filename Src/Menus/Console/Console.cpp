//library includes
#include <bbgraphics.h>
#include <bbinput.h>

//project includes
#include "../../AssetMgmt/Assets.h"
#include "Console.h"
#include "../../GameMain.h"
#include "../Menu.h"
#include "ConsoleCommands.h"
#include "../../Config/Options.h"

namespace CBN {

// Structs.
Console* console;
Console::Console() {
    int x = 0;
    int y = 1024 - 300;
    int width = 1024;
    int height = 270;
    cmdInput = GUIInput(x, y + height, width, 30, "", 0, 100);
    cmdInput.widthRelativeToScreenWidth = true;
    scroll = 0.f;
    scrollDragging = false;
    mouseMem = 0;
    reissue = 0;
    msgR = 255;
    msgG = 255;
    msgB = 255;

    ConsoleCmd::generateCommands();
}

Console::~Console() {
    msgList.clear();
    ConsoleCmd::clearCommands();
}

ConsoleMsg::ConsoleMsg() {
    txt = "";
    isCommand = false;
    r = -1;
    g = -1;
    b = -1;
}
ConsoleMsg::~ConsoleMsg() {
    for (int i = 0; i < (int)console->msgList.size(); i++) {
        if (console->msgList[i] == this) {
            console->msgList.erase(console->msgList.begin() + i);
            break;
        }
    }
}

// Functions.
void ConsoleMsg::create(const String& txt, int r, int g, int b, bool isCommand) {
    ConsoleMsg* c = new ConsoleMsg();
    console->msgList.insert(console->msgList.begin(), c);

    c->txt = txt;
    c->isCommand = isCommand;

    c->r = r;
    c->g = g;
    c->b = b;

    if (c->r<0) {
        c->r = console->msgR;
    }
    if (c->g<0) {
        c->g = console->msgG;
    }
    if (c->b<0) {
        c->b = console->msgB;
    }
}

void Console::update() {
    cmdInput.selected = CurrGameState == GAMESTATE_CONSOLE;
    if (CurrGameState==GAMESTATE_CONSOLE) {
        console->msgR = 255;
        console->msgG = 255;
        console->msgB = 255;

        int x = 0;
        int y = userOptions->screenHeight-(int)(300.f*MenuScale);
        int width = userOptions->screenWidth;
        int height = (int)(300.f*MenuScale-30.f*MenuScale);

        int consoleHeight = 0;
        int scrollbarHeight = 0;
        for (int i = 0; i < (int)console->msgList.size(); i++) {
            ConsoleMsg* cm = console->msgList[i];

            consoleHeight = consoleHeight + (int)(15.f*MenuScale);
        }
        scrollbarHeight = (int)(((float)(height)/(float)(consoleHeight))*height);
        if (scrollbarHeight>height) {
            scrollbarHeight = height;
        }
        if (consoleHeight<height) {
            consoleHeight = height;
        }

        bool inBar = MouseOn(x+width-(int)(26.f*MenuScale),y,(int)(26.f*MenuScale),height);

        bool inBox = MouseOn(x+width-(int)(23.f*MenuScale),y+height-scrollbarHeight+(int)(console->scroll*scrollbarHeight/height),(int)(20.f*MenuScale),scrollbarHeight);

        if (!bbMouseDown(1)) {
            console->scrollDragging = false;
        } else if (console->scrollDragging) {
            console->scroll = console->scroll+((bbMouseY()-console->mouseMem)*height/scrollbarHeight);
            console->mouseMem = bbMouseY();
        }

        if (!console->scrollDragging) {
            if (MouseHit1) {
                if (inBox) {
                    console->scrollDragging = true;
                    console->mouseMem = bbMouseY();
                } else if (inBar) {
                    console->scroll = console->scroll+((bbMouseY()-(y+height))*consoleHeight/height+(height/2));
                    console->scroll = console->scroll/2;
                }
            }
        }

        int mouseScroll = bbMouseZSpeed();
        if (mouseScroll==1) {
            console->scroll = console->scroll - 15*MenuScale;
        } else if (mouseScroll==-1) {
            console->scroll = console->scroll + 15*MenuScale;
        }

        if (bbKeyHit(200) && console->msgList.size() > 0) {
            int initIndex = console->reissue;
            if (initIndex<0 || initIndex >= (int)console->msgList.size()) {
                initIndex = 0;
            }
            int index = console->reissue + 1;

            while (index != console->reissue) {
                index %= console->msgList.size();
                if (console->msgList[index]->isCommand) {
                    break;
                }
                index++;
            }
            if (console->msgList[index]->isCommand) {
                console->reissue = index;
                console->cmdInput.setInput(console->msgList[index]->txt);
                console->scroll = -console->reissue * (15.0f*MenuScale) + (height / 2);
            }
        }

        if (bbKeyHit(208) && console->msgList.size() > 0) {
            int initIndex = console->reissue;
            if (initIndex<0 || initIndex >= (int)console->msgList.size()) {
                initIndex = 0;
            }
            int index = console->reissue - 1;

            while (index != initIndex) {
                if (index<0) { index += console->msgList.size(); }
                if (console->msgList[index]->isCommand) {
                    break;
                }
                index--;
            }
            if (console->msgList[index]->isCommand) {
                console->reissue = index;
                console->cmdInput.setInput(console->msgList[index]->txt);
                console->scroll = -console->reissue * (15.0f*MenuScale) + (height / 2);
            }
        }


        if (console->scroll<-consoleHeight+height) {
            console->scroll = (float)(-consoleHeight+height);
        }
        if (console->scroll>0) {
            console->scroll = 0;
        }

        String oldConsoleInput = console->cmdInput.getInput();
        bbSetFont(uiAssets->consoleFont);
        console->cmdInput.update();
        bbSetFont(uiAssets->font[0]);
        if (!oldConsoleInput.equals(console->cmdInput.getInput())) {
            console->reissue = -1;
        }
        console->cmdInput.toLower();

        if (bbKeyHit(28) && !console->cmdInput.getInput().isEmpty()) {
            String consoleCommand = console->cmdInput.getInput().trim();
            console->reissue = -1;
            console->scroll = 0;
            ConsoleMsg::create(consoleCommand,255,255,0,true);

			String commandName;
			std::vector<String> args;
            if (consoleCommand.findFirst(" ") > 0) {
                commandName = consoleCommand.substr(0, consoleCommand.findFirst(" "));
                args = consoleCommand.substr(commandName.size()).split(" ", true);
            } else {
                commandName = console->cmdInput.getInput();
            }

            ConsoleCmd::executeCommand(commandName, args);
            console->cmdInput.clear();
        }
    }
}

void Console::draw() {
    if (CurrGameState==GAMESTATE_CONSOLE) {
        bbSetFont(uiAssets->consoleFont);
        int x = 0;
        int y = userOptions->screenHeight-(int)(300.f*MenuScale);
        int width = userOptions->screenWidth;
        int height = (int)(300.f*MenuScale-30.f*MenuScale);

        DrawFrame(x,y,width,height+(int)(30.f*MenuScale));

        int consoleHeight = 0;
        int scrollbarHeight = 0;

        for (int i = 0; i < (int)console->msgList.size(); i++) {
            ConsoleMsg* cm = console->msgList[i];

            consoleHeight = consoleHeight + (int)(15.f*MenuScale);
        }
        if (consoleHeight == 0) {
            scrollbarHeight = 0;
        } else {
            scrollbarHeight = (int)(((float)(height) / (float)(consoleHeight))*height);
        }
        if (scrollbarHeight>height) {
            scrollbarHeight = height;
        }
        if (consoleHeight<height) {
            consoleHeight = height;
        }

        bbColor(50,50,50);
        int inBar = MouseOn(x+width-(int)(26.f*MenuScale),y,(int)(26.f*MenuScale),height);
        if (inBar) {
            bbColor(70,70,70);
        }
        bbRect(x+width-(int)(26.f*MenuScale),y,(int)(26.f*MenuScale),height,true);


        bbColor(120,120,120);
        bool inBox = MouseOn(x+width-(int)(23.f*MenuScale),y+height-scrollbarHeight+(int)(console->scroll*scrollbarHeight/height),(int)(20.f*MenuScale),scrollbarHeight);
        if (inBox) {
            bbColor(200,200,200);
        }
        if (console->scrollDragging) {
            bbColor(255,255,255);
        }
        bbRect(x+width-(int)(23.f*MenuScale),y+height-scrollbarHeight+(int)(console->scroll*scrollbarHeight/height),(int)(20.f*MenuScale),scrollbarHeight,true);

        bbColor(255, 255, 255);

        int tempY = y + height - (int)(25.f*MenuScale) - (int)(console->scroll);
        int count = 0;
        for (int i = 0; i < (int)console->msgList.size(); i++) {
			ConsoleMsg* cm = console->msgList[i];

            count++;
            if (count>1000) {
                delete cm;
            } else {
                if (tempY >= y && tempY < y + height - (int)(20.f*MenuScale)) {
                    if (i==console->reissue) {
                        bbColor(cm->r/4,cm->g/4,cm->b/4);
                        bbRect(x, tempY -(int)(2.f*MenuScale),width-(int)(30.f*MenuScale),(int)(24.f*MenuScale),true);
                    }
                    bbColor(cm->r,cm->g,cm->b);
                    if (cm->isCommand) {
                        bbText(x + (int)(20.f*MenuScale), tempY, "> "+cm->txt);
                    } else {
                        bbText(x + (int)(20.f*MenuScale), tempY, cm->txt);
                    }
                }
				tempY -= (int)(15.f*MenuScale);
            }

        }

        bbColor(255,255,255);

        console->cmdInput.draw();
        bbSetFont(uiAssets->font[0]);
    }
}

}
