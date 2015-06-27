/*
* mygame.h: 本檔案儲遊戲本身的class的interface
* Copyright (C) 2002-2008 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
*
* This file is part of game, a free game development framework for windows.
*
* game is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* game is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*	 2004-03-02 V4.0
*      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
*         demonstrate the use of states.
*   2005-09-13
*      Rewrite the codes for CBall and CEraser.
*   2005-09-20 V4.2Beta1.
*   2005-09-29 V4.2Beta2.
*   2006-02-08 V4.2
*      1. Rename OnInitialUpdate() -> OnInit().
*      2. Replace AUDIO_CANYON as AUDIO_NTUT.
*      3. Add help bitmap to CGameStateRun.
*   2006-09-09 V4.3
*      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
*         event driven.
*   2008-02-15 V4.4
*      1. Add namespace game_framework.
*      2. Replace the demonstration of animation as a new bouncing ball.
*      3. Use ShowInitProgress(percent) to display loading progress.
*/

/////////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////////

enum AUDIO_ID  				// 定義各種音效的編號
{
    AUDIO_BUTTON,			// 0
    AUDIO_BEGIN,			// 1
    AUDIO_1_1,				// 2
    AUDIO_JUMP,				// 3
    AUDIO_EDIT,				// 4
    AUDIO_DEAD,				// 5
    AUDIO_GAMEOVER,			// 6
    AUDIO_1_BOSS,			// 7
    AUDIO_1_1A,				// 8
    AUDIO_1_PASS,			// 9
    AUDIO_WORD				//10
};

namespace game_framework
{
#include <time.h>
//Global
#define MWIDTH 640
#define MHEIGHT 480
#define MAX_XN 10
#define MAX_YN 10
#define SPEED 10
#define MAX_OX 800
#define MAX_OY 1600
#define ONEOBJX 8
#define ONEOBJY 3
#define OrderSize 20
#define UnVisibleOrderSize 1
#define MonsterOrderSize 6
#define WinShowBuffer 90
#define GRAVITY 5
#define UP_VELOCITY 36
#define TIMEFORJUMP 8
#define TIMEFOR_ANI_JUMP 3
#define ACTION_NUM 10
#define ONE_LEVEL_MONSTER_NUM 101
#define TAKEN_FLAG (NowTaken>0)
#define E8 4194304
#define E4 2048
#define DesSize 128 //Des 最大的值(128)
#define ALPHA(alpha,i,j) (((unsigned long)(((color[j][i]>>16)&0xFF)*alpha)<<16) | ((unsigned long)(((color[j][i]>>8)&0xFF)*alpha)<<8) | ((unsigned long)(((color[j][i])&0xFF)*alpha)))
#define PURPLE RGB(163,73,164)
#define CanTakeNum 30
#define LEVELNUM 2
#define HOLD(times,count) if(count<times){count++;return;}count=0;



//各種物件編號定義
enum OBJ_ID
{
    EMPTY,			//0  => X
    RED_FLOOR,		//1  => 紅色地板(不可穿透)
    BLUE_FLOOR,		//2  => 藍色地板(可穿透)
    CRAFT,			//3  => 小箱子(可丟的那種)
    FLOWER,			//4  => 花
    STONE,			//5  => 石頭(未拿起)
    APPLE,			//6  => 蘋果
    STAR,			//7  => 星星
    FRUIT,			//8 => 果實
    BOX_ANGLE,		//9 => 大箱子(小天使)
    BOX_CHEESE,		//10 => 大箱子(起司)
    BOX_FRUIT,		//11 => 大箱子(果實)
    TOOL,			//12 => 工具(後面關卡有 十字稿)
    DOOR,			//13 => 過關門
    CRAFT_STAR,		//14 => 箱子(星星)
    CRAFT_FRUIT,	//15 => 箱子(果實)
    CRAFT_FLOWER,	//16 => 箱子(花)
    STAR_HIDE,		//17 => 隱藏星星
    BOX_EMPTY,		//18 => 大箱子(空的顯示用)
    BALL,			//19 => 紅色球球
    CHEESE,			//20 => 起司
    ChipDale_taken	//21 => 拿起來的ChipDale
};
//陸續增加中!!!

//宣告class ..........................................//
class CFrame;
class BasicInstructions;
class AI_Instructions;
class MapManage;
class Object;
class ChipDale;
class Monster;
class GameScore;
/////////////////////////////////////////////////////////////////////////////
// CFrame
//
/////////////////////////////////////////////////////////////////////////////
class CFrame
{
    public:
        CFrame();
        void SetWxWy(int wx, int wy, bool shiftmode = false);
        virtual void FixXY(MapManage*);				// 修正螢幕位置
        virtual void ReFixXY(MapManage*);				// 反修正螢幕位置
        virtual void FixMapMove(int fixX, int fixY);
        void SetWidthHeight(int tempW, int tempH);
        virtual int IfCollision(int twx, int twy, int twidth, int theight);
        int IfCollision(int Direct, int passSpeed);		//與地圖做運算碰撞
        virtual void CollisionReact(int setDirect, CFrame* which)
        {
            ;
        }
        int ReturnWX()
        {
            return wx;    // 回傳螢幕x座標
        }
        int ReturnWY()
        {
            return wy;    // 回傳螢幕y座標
        }
        int ReturnWidth()
        {
            return width;
        }
        int ReturnHeight()
        {
            return height;
        }
        bool ReturnLR()
        {
            return LRflag;
        }
        static int Jump_Fix;

    private:
    protected:
        int wx, wy;								// 框框左上角的螢幕座標	window x,y
        int width, height;
        bool flag[6];							// 上下左右AB	 按鍵旗標
        bool LRflag;							// 0 左 1 右     標示方向
};
/////////////////////////////////////////////////////////////////////////////
// GameScore
//
/////////////////////////////////////////////////////////////////////////////
class GameScore
{
    public:
        static GameScore* Instance();
        ~GameScore();
        void Loading();
        void ShowScroe();
        void onShow();
        bool Switch;
    private:
        GameScore();
        static GameScore Game_Score;
        CMovingBitmap Head[2];
        CMovingBitmap F, S;
        CMovingBitmap L_C, L_D;
        CInteger* score_p;
};
/////////////////////////////////////////////////////////////////////////////
// GameMapEdit
//
/////////////////////////////////////////////////////////////////////////////
class GameMapEdit
{
    public:
        GameMapEdit();
        void Loading();
        void OnShow(MapManage*);
        void SetOrder(int);
        int  nowOrder()
        {
            return Order;
        };
        void OnMove(MapManage* map);
        void SetWxWy(int setWx, int setWy);
        void SetMove(bool, bool, bool, bool, bool, int mode = 0);
        void SetObject(MapManage* map);			//去wx wy 位置放置obj
        int ReturnWX()
        {
            return wx;
        }
        int ReturnWY()
        {
            return wy;
        }
        void FixXY(MapManage*);				// 修正螢幕位置

    private:
        int wx, wy;
        int Order;
        int ThisMoveTimesX, ThisMoveTimesY;		// X、Y方向的移動次數
        int ContinueMode;						// TAB切換微調模式 0 1 2
        bool move[4];
        CMovingBitmap frame_pic;
        CMovingBitmap frame_background[OrderSize + MonsterOrderSize];
};
/////////////////////////////////////////////////////////////////////////////
// ToolCDC
//
/////////////////////////////////////////////////////////////////////////////
class ToolCDC
{
    public:
        ToolCDC();
        static void Loading();						//為 ShowProgressBar 的圖片
        static void Fadeout()
        {
            NowState = 1;    //setState =1
        }
        static void Fadein()
        {
            NowState = 4;    //setState =1
        }
        static void SaveCDC();							//記錄目前螢幕的 Colors
        static int  ReturnStage()
        {
            return NowState;    //傳回目前該做甚麼事情
        }
        static void ToNextState();						//移到下一個state
        static void CDDraw();
        static void ShowProgressBar(int WhichLevel, int percent);
    private:
        static COLORREF color[MHEIGHT][MWIDTH];
        static CAnimation MovePointer;
        static CMovingBitmap loading_BG, loading_Bar, loading_Mask, level, letter[LEVELNUM];
        static int NowState;									//0 未設定 ,
        //1 下一次要OnShow要save & 鎖人物移動恩螢幕顯示, (淡出)
        //2 saved 跟onMove說可以換人物位置 & 鎖人物移動恩螢幕顯示 ,
        //3 鎖人物移動恩螢幕顯示
        //4 下一次OnShow要save & 鎖人物移動恩螢幕顯示, (淡入)
        //5 鎖人物移動恩螢幕顯示
        static double alpha;									//目前的aplha值
};

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲開頭畫面物件
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateInit : public CGameState
{
    public:
        CGameStateInit(CGame* g);
        void OnInit();  								// 遊戲的初值及圖形設定
        void OnBeginState();							// 設定每次重玩所需的變數
        void OnKeyDown(UINT, UINT, UINT); 				// 處理鍵盤Down的動作
        void OnKeyUp(UINT, UINT, UINT); 				// 處理鍵盤Up的動作
    protected:
        void OnShow();									// 顯示這個狀態的遊戲畫面
    private:
        int order, lastOrder;
        bool IsInfo;
        CMovingBitmap pic[5];
};

/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////
#define PassMusicTime 33*5
class CGameStateRun : public CGameState
{
    public:
        CGameStateRun(CGame* g);
        ~CGameStateRun();
        void OnBeginState();							// 設定每次重玩所需的變數
        void OnInit();  								// 遊戲的初值及圖形設定
        void OnKeyDown(UINT, UINT, UINT);
        void OnKeyUp(UINT, UINT, UINT);
        static void ToBonus()
        {
            TimeToBonus = PassMusicTime;
        }
        static int multiplayer, Chip_Dale, NowLevel;		// multiplay 1= 1P ,2= 2P nowplayer 0 =Chip, 1= Dale
        static ChipDale*		player[2];
        static bool TimeToGo;
        static bool IfViewer;
        static bool IsBoss, IsNTUT;
    protected:
        void OnMove();									// 移動遊戲元素
        void OnShow();									// 顯示這個狀態的遊戲畫面
    private:
        CMovingBitmap	saved, loading;	// 背景圖
        CMovingBitmap   Health[3], ShowWho[3];
        CFrame			Frame;
        MapManage*		Maps;

        GameMapEdit		EditPointer; //編輯箭頭
        Monster*			AllMonster[ONE_LEVEL_MONSTER_NUM];		// 全部怪物
        Object*			AllThrow[CanTakeNum];

        int				EditMoveMode;
        bool			show_save, show_load;
        static	int		TimeToBonus;

};
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的獎勵狀態(Game Bonus)
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateBonus : public CGameState
{
    public:
        CGameStateBonus(CGame* g);
        ~CGameStateBonus();
        void OnBeginState();							// 設定每次重玩所需的變數
        void OnInit();
    protected:
        void OnMove();									// 移動遊戲元素
        void OnShow();									// 顯示這個狀態的遊戲畫面
    private:
        CAnimation To_Be_Continue;
        int count;
        int delay;
};
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的世界地圖(Game Worlds)
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateWorlds : public CGameState
{
    public:
        CGameStateWorlds(CGame* g);
        ~CGameStateWorlds();
        void OnBeginState();							// 設定每次重玩所需的變數
        void OnInit();
    protected:
        void OnMove();									// 移動遊戲元素
        void OnShow();									// 顯示這個狀態的遊戲畫面

};
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的結束狀態(Game Over)
// 每個Member function的Implementation都要弄懂
/////////////////////////////////////////////////////////////////////////////

class CGameStateOver : public CGameState
{
    public:
        CGameStateOver(CGame* g);
        void OnBeginState();							// 設定每次重玩所需的變數
        void OnInit();
    protected:
        void OnMove();									// 移動遊戲元素
        void OnShow();									// 顯示這個狀態的遊戲畫面
    private:
        int counter;	// 倒數之計數器
};

}