/*
* mygame.h: ���ɮ��x�C��������class��interface
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

enum AUDIO_ID  				// �w�q�U�ح��Ī��s��
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
#define DesSize 128 //Des �̤j����(128)
#define ALPHA(alpha,i,j) (((unsigned long)(((color[j][i]>>16)&0xFF)*alpha)<<16) | ((unsigned long)(((color[j][i]>>8)&0xFF)*alpha)<<8) | ((unsigned long)(((color[j][i])&0xFF)*alpha)))
#define PURPLE RGB(163,73,164)
#define CanTakeNum 30
#define LEVELNUM 2
#define HOLD(times,count) if(count<times){count++;return;}count=0;



//�U�ت���s���w�q
enum OBJ_ID
{
    EMPTY,			//0  => X
    RED_FLOOR,		//1  => ����a�O(���i��z)
    BLUE_FLOOR,		//2  => �Ŧ�a�O(�i��z)
    CRAFT,			//3  => �p�c�l(�i�᪺����)
    FLOWER,			//4  => ��
    STONE,			//5  => ���Y(�����_)
    APPLE,			//6  => ī�G
    STAR,			//7  => �P�P
    FRUIT,			//8 => �G��
    BOX_ANGLE,		//9 => �j�c�l(�p�Ѩ�)
    BOX_CHEESE,		//10 => �j�c�l(�_�q)
    BOX_FRUIT,		//11 => �j�c�l(�G��)
    TOOL,			//12 => �u��(�᭱���d�� �Q�r�Z)
    DOOR,			//13 => �L����
    CRAFT_STAR,		//14 => �c�l(�P�P)
    CRAFT_FRUIT,	//15 => �c�l(�G��)
    CRAFT_FLOWER,	//16 => �c�l(��)
    STAR_HIDE,		//17 => ���ìP�P
    BOX_EMPTY,		//18 => �j�c�l(�Ū���ܥ�)
    BALL,			//19 => ����y�y
    CHEESE,			//20 => �_�q
    ChipDale_taken	//21 => ���_�Ӫ�ChipDale
};
//����W�[��!!!

//�ŧiclass ..........................................//
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
        virtual void FixXY(MapManage*);				// �ץ��ù���m
        virtual void ReFixXY(MapManage*);				// �ϭץ��ù���m
        virtual void FixMapMove(int fixX, int fixY);
        void SetWidthHeight(int tempW, int tempH);
        virtual int IfCollision(int twx, int twy, int twidth, int theight);
        int IfCollision(int Direct, int passSpeed);		//�P�a�ϰ��B��I��
        virtual void CollisionReact(int setDirect, CFrame* which)
        {
            ;
        }
        int ReturnWX()
        {
            return wx;    // �^�ǿù�x�y��
        }
        int ReturnWY()
        {
            return wy;    // �^�ǿù�y�y��
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
        int wx, wy;								// �خإ��W�����ù��y��	window x,y
        int width, height;
        bool flag[6];							// �W�U���kAB	 ����X��
        bool LRflag;							// 0 �� 1 �k     �Хܤ�V
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
        void SetObject(MapManage* map);			//�hwx wy ��m��mobj
        int ReturnWX()
        {
            return wx;
        }
        int ReturnWY()
        {
            return wy;
        }
        void FixXY(MapManage*);				// �ץ��ù���m

    private:
        int wx, wy;
        int Order;
        int ThisMoveTimesX, ThisMoveTimesY;		// X�BY��V�����ʦ���
        int ContinueMode;						// TAB�����L�ռҦ� 0 1 2
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
        static void Loading();						//�� ShowProgressBar ���Ϥ�
        static void Fadeout()
        {
            NowState = 1;    //setState =1
        }
        static void Fadein()
        {
            NowState = 4;    //setState =1
        }
        static void SaveCDC();							//�O���ثe�ù��� Colors
        static int  ReturnStage()
        {
            return NowState;    //�Ǧ^�ثe�Ӱ��ƻ�Ʊ�
        }
        static void ToNextState();						//����U�@��state
        static void CDDraw();
        static void ShowProgressBar(int WhichLevel, int percent);
    private:
        static COLORREF color[MHEIGHT][MWIDTH];
        static CAnimation MovePointer;
        static CMovingBitmap loading_BG, loading_Bar, loading_Mask, level, letter[LEVELNUM];
        static int NowState;									//0 ���]�w ,
        //1 �U�@���nOnShow�nsave & ��H�����ʮ��ù����, (�H�X)
        //2 saved ��onMove���i�H���H����m & ��H�����ʮ��ù���� ,
        //3 ��H�����ʮ��ù����
        //4 �U�@��OnShow�nsave & ��H�����ʮ��ù����, (�H�J)
        //5 ��H�����ʮ��ù����
        static double alpha;									//�ثe��aplha��
};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C���}�Y�e������
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateInit : public CGameState
{
    public:
        CGameStateInit(CGame* g);
        void OnInit();  								// �C������Ȥιϧγ]�w
        void OnBeginState();							// �]�w�C�������һݪ��ܼ�
        void OnKeyDown(UINT, UINT, UINT); 				// �B�z��LDown���ʧ@
        void OnKeyUp(UINT, UINT, UINT); 				// �B�z��LUp���ʧ@
    protected:
        void OnShow();									// ��ܳo�Ӫ��A���C���e��
    private:
        int order, lastOrder;
        bool IsInfo;
        CMovingBitmap pic[5];
};

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C�����檫��A�D�n���C���{�����b�o��
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////
#define PassMusicTime 33*5
class CGameStateRun : public CGameState
{
    public:
        CGameStateRun(CGame* g);
        ~CGameStateRun();
        void OnBeginState();							// �]�w�C�������һݪ��ܼ�
        void OnInit();  								// �C������Ȥιϧγ]�w
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
        void OnMove();									// ���ʹC������
        void OnShow();									// ��ܳo�Ӫ��A���C���e��
    private:
        CMovingBitmap	saved, loading;	// �I����
        CMovingBitmap   Health[3], ShowWho[3];
        CFrame			Frame;
        MapManage*		Maps;

        GameMapEdit		EditPointer; //�s��b�Y
        Monster*			AllMonster[ONE_LEVEL_MONSTER_NUM];		// �����Ǫ�
        Object*			AllThrow[CanTakeNum];

        int				EditMoveMode;
        bool			show_save, show_load;
        static	int		TimeToBonus;

};
/////////////////////////////////////////////////////////////////////////////
// �o��class���C�������y���A(Game Bonus)
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateBonus : public CGameState
{
    public:
        CGameStateBonus(CGame* g);
        ~CGameStateBonus();
        void OnBeginState();							// �]�w�C�������һݪ��ܼ�
        void OnInit();
    protected:
        void OnMove();									// ���ʹC������
        void OnShow();									// ��ܳo�Ӫ��A���C���e��
    private:
        CAnimation To_Be_Continue;
        int count;
        int delay;
};
/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����@�ɦa��(Game Worlds)
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateWorlds : public CGameState
{
    public:
        CGameStateWorlds(CGame* g);
        ~CGameStateWorlds();
        void OnBeginState();							// �]�w�C�������һݪ��ܼ�
        void OnInit();
    protected:
        void OnMove();									// ���ʹC������
        void OnShow();									// ��ܳo�Ӫ��A���C���e��

};
/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����������A(Game Over)
// �C��Member function��Implementation���n����
/////////////////////////////////////////////////////////////////////////////

class CGameStateOver : public CGameState
{
    public:
        CGameStateOver(CGame* g);
        void OnBeginState();							// �]�w�C�������һݪ��ܼ�
        void OnInit();
    protected:
        void OnMove();									// ���ʹC������
        void OnShow();									// ��ܳo�Ӫ��A���C���e��
    private:
        int counter;	// �˼Ƥ��p�ƾ�
};

}