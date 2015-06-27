/*
* mygame.cpp: ���ɮ��x�C��������class��implementation
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
* History:
*   2002-03-04 V3.1
*          Add codes to demostrate the use of CMovingBitmap::ShowBitmap(CMovingBitmap &).
*	 2004-03-02 V4.0
*      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
*         demonstrate the use of states.
*      2. Demo the use of CInteger in CGameStateRun.
*   2005-09-13
*      Rewrite the codes for CBall and CEraser.
*   2005-09-20 V4.2Beta1.
*   2005-09-29 V4.2Beta2.
*      1. Add codes to display IDC_GAMECURSOR in GameStateRun.
*   2006-02-08 V4.2
*      1. Revise sample screens to display in English only.
*      2. Add code in CGameStateInit to demo the use of PostQuitMessage().
*      3. Rename OnInitialUpdate() -> OnInit().
*      4. Fix the bug that OnBeginState() of GameStateInit is not called.
*      5. Replace AUDIO_CANYON as AUDIO_NTUT.
*      6. Add help bitmap to CGameStateRun.
*   2006-09-09 V4.3
*      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
*         event driven.
*   2006-12-30
*      1. Bug fix: fix a memory leak problem by replacing PostQuitMessage(0) as
*         PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE,0,0).
*   2008-02-15 V4.4
*      1. Add namespace game_framework.
*      2. Replace the demonstration of animation as a new bouncing ball.
*      3. Use ShowInitProgress(percent) to display loading progress.
*   2010-03-23 V4.6
*      1. Demo MP3 support: use lake.mp3 to replace lake.wav.
*/
#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "audio.h"
#include "gamelib.h"
#include "mygame.h"
#include "instruction.h"
#include "chipdale.h"
#include "monster.h"
#include "object.h"
#include "mapmanage.h"


namespace game_framework
{


/****************************************************************/
//CFrame��@													*/
//																*/
/****************************************************************/
int CFrame::Jump_Fix = 0;
CFrame::CFrame()
{
    wx = wy = 0;
    for (int i = 0; i < 6; i++)
        flag[i] = false;
    LRflag = 1;

}

void CFrame::SetWxWy(int setX, int setY, bool shiftmode)
{
    if (shiftmode)
    {
        if ((wx + width + setX) <= MWIDTH && (wx + setX) >= 0)	wx += setX;
        if ((wy + setY) < MHEIGHT)				wy += setY;
    }
    else
    {
        if (setX >= 0 && (width + setX) < MWIDTH)			wx = setX;
        if (setY < MHEIGHT)							wy = setY;
    }
}
void CFrame::SetWidthHeight(int tempW, int tempH)
{
    width = tempW;
    height = tempH;
}
void CFrame::FixXY(MapManage* map)
{
    int temp = map->MoveMap(0);
    if (temp & 1)
        wy += Jump_Fix;
    if (temp & 2)
        wy -= Jump_Fix;
    if (temp & 4)
        wx += MapManage::LRMargin;
    if (temp & 8)
        wx -= MapManage::LRMargin;
}
void CFrame::ReFixXY(MapManage* map)
{
    int temp = map->MoveMap(0);
    if (temp & 1)
        wy -= Jump_Fix;
    if (temp & 2)
        wy += SPEED;
    if (temp & 4)
        wx -= MapManage::LRMargin;
    if (temp & 8)
        wx += MapManage::LRMargin;
}
void CFrame::FixMapMove(int fixX, int fixY)
{
    wx -= fixX;
    wy -= fixY;
}
int CFrame::IfCollision(int twx, int twy, int twidth, int theight)
{
    //���W 1
    int cx, cy;
    for (int i = 1; i <= 4; i++)
    {
        switch (i)
        {
            case 1:
                cx = twx;
                cy = twy;
                break;
            case 2:
                cx = twx + twidth;
                cy = twy;
                break;
            case 3:
                cx = twx + twidth;
                cy = twy + theight;
                break;
            case 4:
                cx = twx;
                cy = twy + theight;
                break;
        }
        if (wx <= cx			&& wx + width >= cx			&& wy <= cy			&& wy + height >= cy)		return i;
    }
    if (twx <= wx			&& twx + twidth >= wx			&& twy <= wy			&& twy + theight >= wy)		return 5;
    //�S���I�� 0
    //TRACE("Obj_Throw: %d %d %d %d\n",twx,twy,twidth,theight);
    //TRACE("Monster: %d %d %d %d\n",wx,wy,width,height);
    return 0;
}
int CFrame::IfCollision(int Direct, int passSpeed)
{
    MapManage* map = ChipDale::Maps;
    passSpeed = abs(passSpeed);
    switch (Direct)
    {
        case 1:
            return map->IfCollision(wx, wy - passSpeed, width, passSpeed, true);
        case 4:
            if (wx - passSpeed < 0) return 1;
            return map->IfCollision(wx - passSpeed, wy, passSpeed, height, true);
        case 8:
            if (wx + width + passSpeed > MWIDTH) return 1;
            return map->IfCollision(wx + width, wy, passSpeed, height, true);
    }
    return 0;
}
/****************************************************************/
//GameScore��@													*/
//																*/
/****************************************************************/
GameScore GameScore::Game_Score;

GameScore::GameScore()
{
    Switch = false;
    score_p = new CInteger(2);
}
GameScore::~GameScore()
{
    delete(score_p);
    TRACE("~GameScore()\n");
}
GameScore* GameScore::Instance()
{
    return &Game_Score;
}
void GameScore::Loading()
{
    Head[0].LoadBitmapA("Bitmaps/score/chiphead.bmp");
    Head[1].LoadBitmapA("Bitmaps/score/dalehead.bmp");
    F.LoadBitmapA("Bitmaps/score/F.bmp", PURPLE);
    S.LoadBitmapA("Bitmaps/score/S.bmp", PURPLE);
    L_C.LoadBitmapA("Bitmaps/score/L_C.bmp", PURPLE);
    L_D.LoadBitmapA("Bitmaps/score/L_D.bmp", PURPLE);
    score_p->LoadBitmap();
}
void GameScore::ShowScroe()
{
    if (Switch == true)Switch = false;
    else			Switch = true;
}
void GameScore::onShow()
{
    if (Switch == false)return;

    if (CGameStateRun::player[0]->ReturnIsDale() == 0)
    {
        Head[0].SetTopLeft(MWIDTH / 2 - 100 - Head[0].Width(), 100);
        Head[0].ShowBitmap();

        F.SetTopLeft(MWIDTH / 2 - 100 - Head[0].Width(), 240);
        S.SetTopLeft(MWIDTH / 2 - 100 - Head[0].Width(), 300);
        L_C.SetTopLeft(MWIDTH / 2 - 100 - Head[0].Width(), 360);
        F.ShowBitmap();
        S.ShowBitmap();
        L_C.ShowBitmap();

        score_p->SetInteger(CGameStateRun::player[0]->Score_Flower);
        score_p->SetTopLeft(MWIDTH / 2 - 50 - Head[0].Width(), 250);
        score_p->ShowBitmap();

        score_p->SetInteger(CGameStateRun::player[0]->Score_Star);
        score_p->SetTopLeft(MWIDTH / 2 - 50 - Head[0].Width(), 310);
        score_p->ShowBitmap();

        score_p->SetInteger(CGameStateRun::player[0]->ReturnLife());
        score_p->SetTopLeft(MWIDTH / 2 - 50 - Head[0].Width(), 370);
        score_p->ShowBitmap();


    }
    if (CGameStateRun::multiplayer == 2 || CGameStateRun::player[0]->ReturnIsDale())
    {
        ChipDale* p = CGameStateRun::player[0];
        if (CGameStateRun::multiplayer == 2)p = CGameStateRun::player[1];

        Head[1].SetTopLeft(MWIDTH / 2 + 100, 100);
        Head[1].ShowBitmap();

        F.SetTopLeft(MWIDTH / 2 + 100, 240);
        S.SetTopLeft(MWIDTH / 2 + 100, 300);
        L_D.SetTopLeft(MWIDTH / 2 + 100, 360);
        F.ShowBitmap();
        S.ShowBitmap();
        L_D.ShowBitmap();

        score_p->SetInteger(p->Score_Flower);
        score_p->SetTopLeft(MWIDTH / 2 + 150, 250);
        score_p->ShowBitmap();

        score_p->SetInteger(p->Score_Star);
        score_p->SetTopLeft(MWIDTH / 2 + 150, 310);
        score_p->ShowBitmap();

        score_p->SetInteger(p->ReturnLife());
        score_p->SetTopLeft(MWIDTH / 2 + 150, 370);
        score_p->ShowBitmap();
    }


}
/****************************************************************/
//GameMapEdit��@												*/
//																*/
/****************************************************************/
GameMapEdit::GameMapEdit()
{
    Order = 0;
    wx = wy = 0;
    for (int i = 0; i < 4; i++)
        move[i] = false;
    ContinueMode = 0;
}
void GameMapEdit::Loading()
{
    char temp[100];
    frame_pic.LoadBitmapA("Bitmaps/object/Pointer.bmp");

    for (int i = 0; i < OrderSize; i++)
    {
        sprintf(temp, "Bitmaps/object/%d.bmp", i);
        frame_background[i].LoadBitmapA(temp, PURPLE);
    }
    for (int i = -1; i >= -MonsterOrderSize; i--)
    {
        sprintf(temp, "Bitmaps/monster/%d.bmp", -i);
        //TRACE("%s\n",temp);
        frame_background[-i + OrderSize - 1].LoadBitmapA(temp, PURPLE);
    }
}
void GameMapEdit::OnShow(MapManage* map)
{
    int temp;
    //��� �ثe�ù��W��OBJ
    for (int i = 0; i <= (MWIDTH + 2 * WinShowBuffer) / ONEOBJX; i++)
        for (int j = 0; j <= (MHEIGHT + 2 * WinShowBuffer) / ONEOBJY; j++)
            map->ShowObstacle(i, j);


    //��� �ثe��m
    //TRACE("x: %d , y: %d \n",wx,wy);
    //FixXY(map);
    //TRACE("FIXED -> x: %d , y: %d \n",wx,wy);
    temp = (Order < 0) * (-Order + OrderSize - 1) + (Order >= 0) * (Order);
    if (Order == 0)
        frame_background[0].SetTopLeft(wx - 16, wy - 16);
    else
        frame_background[temp].SetTopLeft(wx, wy);
    frame_background[temp].ShowBitmap();

    frame_pic.SetTopLeft(wx, wy);
    frame_pic.ShowBitmap();
}
void GameMapEdit::SetOrder(int value)
{

    if (value < OrderSize && value >= -MonsterOrderSize)
        Order = value;
    if (value < -MonsterOrderSize)
        Order = OrderSize - 1;
    if (value >= OrderSize)
        Order = -MonsterOrderSize;

}
void GameMapEdit::OnMove(MapManage* map)
{
    int width, height, temp;				//�ثe�Ϥ����שM�e��
    int timesX, timesY;
    if (ContinueMode == 0)
    {
        timesX = 1;
        timesY = 3;
    }
    else if (ContinueMode == 1)
    {
        timesX = 1;
        timesY = 1;
    }
    else if (ContinueMode == 2)
    {
        timesX = -1;
        timesY = -1;
    }

    temp = (Order < 0) * (-Order + OrderSize - 1) + (Order >= 0) * (Order);
    width = frame_background[temp].Width();
    height = frame_background[temp].Height();

    if (timesX == -1 || timesY == -1)
    {
        timesX = width / ONEOBJX;
        timesY = height / ONEOBJY;
    }
    static int limitX = (MWIDTH + width) / 2;
    static int limitY = (MHEIGHT + height) / 2;

    if (move[0])
    {
        wy -= ONEOBJY * timesY;
        if (wy < 0)
        {
            wy += ONEOBJY * timesY;
        }
        if (wy < limitY)
        {
            if ((map->GetRoute() & 14) && ((map->ReturnNowY()) % MHEIGHT) < ONEOBJY * timesY && ((map->ReturnNowY()) % MHEIGHT) > 0)
                timesY = ((map->ReturnNowY()) % MHEIGHT) / ONEOBJY;
            map->MoveMap(1, ONEOBJX * timesX, ONEOBJY * timesY);
        }
        ThisMoveTimesX = timesX;
        ThisMoveTimesY = timesY;
    }
    if (move[1])
    {
        wy += ONEOBJY * timesY;
        if (wy > MHEIGHT - height)
        {
            wy -= ONEOBJY * timesY;
        }
        if (wy > limitY)
        {
            if ((map->GetRoute() & 13) && MHEIGHT - ((map->ReturnNowY()) % MHEIGHT) < ONEOBJY * timesY && MHEIGHT - ((map->ReturnNowY()) % MHEIGHT) > 0)
                timesY = (MHEIGHT - ((map->ReturnNowY()) % MHEIGHT)) / ONEOBJY;
            map->MoveMap(2, ONEOBJX * timesX, ONEOBJY * timesY);
        }
        ThisMoveTimesX = timesX;
        ThisMoveTimesY = timesY;
    }
    if (move[2])
    {
        wx -= ONEOBJX * timesX;
        if (wx < 0)
        {
            wx += ONEOBJX * timesX;
        }
        if (wx < limitX)
            map->MoveMap(4, ONEOBJX * timesX, ONEOBJY * timesY);

        ThisMoveTimesX = timesX;
        ThisMoveTimesY = timesY;
    }
    if (move[3])
    {
        wx += ONEOBJX * timesX;
        if (wx > MWIDTH - width)
        {
            wx -= ONEOBJX * timesX;
        }
        if (wx > limitX)
            map->MoveMap(8, ONEOBJX * timesX, ONEOBJY * timesY);

        ThisMoveTimesX = timesX;
        ThisMoveTimesY = timesY;
    }
    if (ContinueMode > 0)
    {
        move[0] = move[1] = move[2] = move[3] = false;
        ContinueMode = 0;
    }
}
void GameMapEdit::SetWxWy(int setWx, int setWy)
{
    wx = setWx;
    wy = setWy;
}
void GameMapEdit::SetMove(bool flag, bool IsUp, bool IsDown, bool IsLeft, bool IsRight, int mode)
{
    if (IsUp)	move[0] = flag;
    if (IsDown)	move[1] = flag;
    if (IsLeft)	move[2] = flag;
    if (IsRight)	move[3] = flag;
    if (mode)	ContinueMode = mode;
}
void GameMapEdit::FixXY(MapManage* map)
{
    int temp = map->MoveMap(0);
    if (temp & 1)
        wy += ONEOBJY * ThisMoveTimesY;
    if (temp & 2)
        wy -= ONEOBJY * ThisMoveTimesY;
    if (temp & 4)
        wx += ONEOBJX * ThisMoveTimesX;
    if (temp & 8)
        wx -= ONEOBJX * ThisMoveTimesX;

    ThisMoveTimesX = 0;
    ThisMoveTimesY = 0;
}
void GameMapEdit::SetObject(MapManage* map)
{
    map->SetObj(Order, wx, wy);
}



/////////////////////////////////////////////////////////////////////////////
// ToolCDC
/////////////////////////////////////////////////////////////////////////////
COLORREF ToolCDC::color[MHEIGHT][MWIDTH];
CAnimation ToolCDC::MovePointer;
CMovingBitmap ToolCDC::loading_BG, ToolCDC::loading_Bar, ToolCDC::loading_Mask;
CMovingBitmap ToolCDC::letter[LEVELNUM], ToolCDC::level;
int ToolCDC::NowState = 0;
double ToolCDC::alpha = 0.0;
ToolCDC::ToolCDC()
{

}
void ToolCDC::Loading()
{
    char name[100];
    MovePointer.SetDelayCount(20);
    MovePointer.AddBitmap("Bitmaps/action/Chip/run_1R.bmp", PURPLE);
    MovePointer.AddBitmap("Bitmaps/action/Chip/run_2R.bmp", PURPLE);
    MovePointer.AddBitmap("Bitmaps/action/Chip/run_3R.bmp", PURPLE);
    MovePointer.AddBitmap("Bitmaps/action/Chip/run_4R.bmp", PURPLE);
    loading_BG.LoadBitmapA("Bitmaps/BeginState/loadingBG.bmp", PURPLE);
    loading_Bar.LoadBitmapA("Bitmaps/BeginState/loadingBar.bmp", PURPLE);
    loading_Mask.LoadBitmapA("Bitmaps/BeginState/loadingMask.bmp", PURPLE);
    level.LoadBitmapA("Bitmaps/BeginState/Level.bmp", PURPLE);
    for (int i = 0; i < LEVELNUM; i++)
    {
        sprintf(name, "Bitmaps/BeginState/Letter%d.bmp", i + 1);
        letter[i].LoadBitmapA(name, PURPLE);
    }
}
void ToolCDC::CDDraw()
{
    CDC* pDC = CDDraw::GetBackCDC();			// ���o Back Plain �� CDC
    for (int i = 0; i < MWIDTH; i += 1)
    {
        for (int j = 0; j < MHEIGHT; j += 1)
        {
            pDC->SetPixel(i, j, ALPHA(alpha, i, j));
        }
    }
    CDDraw::ReleaseBackCDC();					// �� Back Plain �� CDC
    //TRACE("state: %d %f\n",NowState,alpha);
}
void ToolCDC::SaveCDC()
{
    CDC* pDC = CDDraw::GetBackCDC();			// ���o Back Plain �� CDC
    for (int i = 0; i < MWIDTH; i += 1)
    {
        for (int j = 0; j < MHEIGHT; j += 1)
        {
            if (i % 2 == 0 && j % 2 == 0)
                color[j][i] = pDC->GetPixel(i, j);
            else
                color[j][i] = color[j / 2 * 2][i / 2 * 2];
        }
    }
    CDDraw::ReleaseBackCDC();					// �� Back Plain �� CDC
    if (NowState <= 2)	alpha = 0.75;
    if (NowState >= 3)	alpha = 0.25;
}
void ToolCDC::ToNextState()
{
    switch (NowState)
    {
        case 1:
        case 2:
        case 4:
            NowState += 1;
            break;
        case 3:
            if (alpha != 0.0)
            {
                alpha -= 0.25;
                if (alpha < 0.0)alpha = 0.0;
            }
            else
            {
                NowState += 1;
            }
            break;
        case 5:
            if (alpha != 1.0)
            {
                alpha += 0.25;
                if (alpha > 1.0)alpha = 1.0;
            }
            else
            {
                NowState = 0;
            }
            break;
    }

}
void ToolCDC::ShowProgressBar(int WhichLevel, int percent)
{
    int width = MovePointer.Width(), height = MovePointer.Height();
    if (percent > 100)percent = 100;
    CDDraw::BltBackColor(DEFAULT_BG_COLOR);		// �N Back Plain ��W�w�]���C��
    loading_Mask.SetTopLeft(95, 275);
    loading_Mask.ShowBitmap();
    loading_Bar.SetTopLeft(95 - 450 * (100 - percent) / 100, 275);
    loading_Bar.ShowBitmap();
    loading_BG.SetTopLeft(0, 0);
    loading_BG.ShowBitmap();
    MovePointer.SetBottomLeft(95 + 450 * percent / 100 - (width) / 2, 275 - (height - 30) / 2, height);
    MovePointer.OnShow();
    MovePointer.OnMove();
    level.SetTopLeft(160, 140);
    level.ShowBitmap();
    letter[WhichLevel - 1].SetTopLeft(415, 140);
    letter[WhichLevel - 1].ShowBitmap();
    CDDraw::BltBackToPrimary();					// �N Back Plain �K��ù�
}
/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C���}�Y�e������
/////////////////////////////////////////////////////////////////////////////
CGameStateInit::CGameStateInit(CGame* g)
    : CGameState(g)
{
}

void CGameStateInit::OnInit()
{
    //
    // ��ϫܦh�ɡAOnInit���J�Ҧ����ϭn��ܦh�ɶ��C���קK���C�����H
    //     �������@�СA�C���|�X�{�uLoading ...�v�A���Loading���i�סC
    //
    ShowInitProgress(0);	// �@�}�l��loading�i�׬�0%
    //
    // �}�l���J���
    GameScore::Instance()->Loading();

    //
    //logo.LoadBitmap(IDB_BACKGROUND);
    Sleep(300);				// ��C�A�H�K�ݲM���i�סA��ڹC���ЧR����Sleep
    //
    // ��OnInit�ʧ@�|����CGameStaterRun::OnInit()�A�ҥH�i���٨S��100%
    //
    CAudio::Instance()->Load(AUDIO_BEGIN, "sounds\\begin.mp3");
    CAudio::Instance()->Load(AUDIO_BUTTON, "sounds\\button.mp3");

    pic[0].LoadBitmapA("Bitmaps\\BeginState\\1.bmp");
    pic[1].LoadBitmapA("Bitmaps\\BeginState\\2.bmp");
    pic[2].LoadBitmapA("Bitmaps\\BeginState\\3.bmp");
    pic[3].LoadBitmapA("Bitmaps\\BeginState\\4.bmp");
    pic[4].LoadBitmapA("Bitmaps\\BeginState\\info.bmp");
}

void CGameStateInit::OnBeginState()
{
    IsInfo = false;
    order = 0;
}
void CGameStateInit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    const char KEY_SPACE = 32;
    if (nChar == KEY_SPACE)
    {
        order = lastOrder;
        IsInfo = false;
    }
}
void CGameStateInit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    const char KEY_ESC = 27;
    const char KEY_SPACE = 32;
    const char KEY_ENTER = 13;
    const char KEY_LEFT = 0x25; // keyboard���b�Y
    const char KEY_RIGHT = 0x27; // keyboard�k�b�Y

    if (nChar == KEY_SPACE)
    {
        if (order != 4)
            lastOrder = order;
        order = 4;
        IsInfo = true;
    }

    if (!IsInfo && nChar == KEY_RIGHT)
    {

        if (order == 0)
        {
            CAudio::Instance()->Play(AUDIO_BUTTON);
            order = 1;
        }
        if (order == 2)
        {
            CAudio::Instance()->Play(AUDIO_BUTTON);
            order = 3;
        }
    }
    if (!IsInfo && nChar == KEY_LEFT)
    {
        if (order == 1)
        {
            CAudio::Instance()->Play(AUDIO_BUTTON);
            order = 0;
        }
        if (order == 3)
        {
            CAudio::Instance()->Play(AUDIO_BUTTON);
            order = 2;
        }
    }

    if (!IsInfo && nChar == KEY_ENTER)
    {
        CAudio::Instance()->Play(AUDIO_BUTTON);
        if (order == 0)order = 2;
        else
        {
            CAudio::Instance()->Stop(AUDIO_BEGIN);
            if (order == 1)
            {
                CGameStateRun::multiplayer = 2;
                GotoGameState(GAME_STATE_RUN);
            }
            else
            {
                CGameStateRun::multiplayer = 1;
                if (order == 2)CGameStateRun::Chip_Dale = 0;
                else CGameStateRun::Chip_Dale = 1;
                GotoGameState(GAME_STATE_RUN);
            }
        }
    }
}

void CGameStateInit::OnShow()
{
    //

    // �K�Wlogo
    //
    //
    // Demo�ù��r�����ϥΡA���L�}�o�ɽкɶq�קK�����ϥΦr���A���CMovingBitmap����n
    //
    pic[order].ShowBitmap();
    /*CDC *pDC = CDDraw::GetBackCDC();			// ���o Back Plain �� CDC
    CFont f,*fp;
    f.CreatePointFont(160,"Times New Roman");	// ���� font f; 160���16 point���r
    fp=pDC->SelectObject(&f);					// ��� font f
    pDC->SetBkColor(RGB(0,0,0));
    pDC->SetTextColor(RGB(255,255,0));
    pDC->TextOut(120,220,"Please click mouse or press SPACE to begin.");
    pDC->TextOut(5,395,"Press Ctrl-F to switch in between window mode and full screen mode.");
    if (ENABLE_GAME_PAUSE)
    pDC->TextOut(5,425,"Press Ctrl-Q to pause the Game.");
    pDC->TextOut(5,455,"Press Alt-F4 or ESC to Quit.");
    pDC->SelectObject(fp);						// �� font f (�d�U���n�|�F��)
    CDDraw::ReleaseBackCDC();					// �� Back Plain �� CDC*/
}
/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����������A(Game Bonus)
/////////////////////////////////////////////////////////////////////////////

CGameStateBonus::CGameStateBonus(CGame* g)
    : CGameState(g)
{
    count = 0;
    delay = 13;
}
CGameStateBonus::~CGameStateBonus()
{
    TRACE("~CGameStateBonus()\n");
}
void CGameStateBonus::OnInit()
{
    int i = 0;
    char temp[50] = "0";

    for (i = 0; i < 20; i++)
    {
        sprintf(temp, "Bitmaps\\bonus\\%d.bmp", i);
        To_Be_Continue.AddBitmap(temp);
    }
    To_Be_Continue.SetDelayCount(delay);

    //CAudio::Instance()->Load(AUDIO_WORD,"sounds/word.mp3");
    CAudio::Instance()->Load(AUDIO_WORD, "sounds/knocking.mp3");
}
void CGameStateBonus::OnBeginState()
{
    TRACE("BonusOnBeginState\n");
}
void CGameStateBonus::OnMove()
{

    count++;
    /*if(count%delay==0)
    CAudio::Instance()->Play(AUDIO_1_PASS);*/
    if (count == delay * 20)
    {
        CAudio::Instance()->Play(AUDIO_BEGIN, true);
        GotoGameState(GAME_STATE_INIT);
    }
}
void CGameStateBonus::OnShow()
{
    To_Be_Continue.SetBottomLeft(0, 0, 480);
    if (!To_Be_Continue.IsFinalBitmap())
        To_Be_Continue.OnMove();
    To_Be_Continue.OnShow();

}

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����������A(Game Over)
/////////////////////////////////////////////////////////////////////////////
CGameStateWorlds::CGameStateWorlds(CGame* g)
    : CGameState(g)
{
    TRACE("GameStateWorlds()\n");
}
CGameStateWorlds::~CGameStateWorlds()
{
    TRACE("~GameStateWorlds()\n");
}
void CGameStateWorlds::OnInit()
{
    TRACE("CGameStateWorldsOnInit\n");
}
void CGameStateWorlds::OnBeginState()
{
    TRACE("CGameStateWorldsOnBegin\n");
}
void CGameStateWorlds::OnMove() {}
void CGameStateWorlds::OnShow() {}
/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����������A(Game Over)
/////////////////////////////////////////////////////////////////////////////

CGameStateOver::CGameStateOver(CGame* g)
    : CGameState(g)
{
}

void CGameStateOver::OnMove()
{
    counter--;
    if (counter < 0)
    {
        GameScore::Instance()->ShowScroe();
        CAudio::Instance()->Play(AUDIO_BEGIN, true);
        GotoGameState(GAME_STATE_INIT);
    }
}

void CGameStateOver::OnBeginState()
{
    counter = 30 * 5; // 5 seconds

    CAudio::Instance()->Play(AUDIO_GAMEOVER);
    GameScore::Instance()->ShowScroe();
}

void CGameStateOver::OnInit()
{
    //
    // ��ϫܦh�ɡAOnInit���J�Ҧ����ϭn��ܦh�ɶ��C���קK���C�����H
    //     �������@�СA�C���|�X�{�uLoading ...�v�A���Loading���i�סC
    //
    ShowInitProgress(90);	// ���ӫe�@�Ӫ��A���i�סA���B�i�׵���66%
    //
    // �}�l���J���
    //
    //Sleep(300);				// ��C�A�H�K�ݲM���i�סA��ڹC���ЧR����Sleep
    //
    // �̲׶i�׬�100%
    //
    ShowInitProgress(100);
    CAudio::Instance()->Load(AUDIO_GAMEOVER, "sounds\\gameover.mp3");
    CAudio::Instance()->Play(AUDIO_BEGIN, true);
}

void CGameStateOver::OnShow()
{
    GameScore::Instance()->onShow();
    CDC* pDC = CDDraw::GetBackCDC();			// ���o Back Plain �� CDC

    CFont f, *fp;
    f.CreatePointFont(160, "Times New Roman");	// ���� font f; 160���16 point���r
    fp = pDC->SelectObject(&f);					// ��� font f
    pDC->SetBkColor(RGB(0, 0, 0));
    pDC->SetTextColor(RGB(255, 255, 0));
    char str[80];								// Demo �Ʀr��r�ꪺ�ഫ
    sprintf(str, "Game Over ! (%d)", counter / 30);
    pDC->TextOut(240, 210, str);
    pDC->SelectObject(fp);						// �� font f (�d�U���n�|�F��)
    CDDraw::ReleaseBackCDC();					// �� Back Plain �� CDC
}

/////////////////////////////////////////////////////////////////////////////
// �o��class���C�����C�����檫��A�D�n���C���{�����b�o��
/////////////////////////////////////////////////////////////////////////////
int CGameStateRun::multiplayer = 1;
int CGameStateRun::NowLevel = 1;
int CGameStateRun::Chip_Dale = 0;
int CGameStateRun::TimeToBonus = 0;
bool CGameStateRun::TimeToGo;
bool CGameStateRun::IfViewer = false;
bool CGameStateRun::IsBoss = false;
bool CGameStateRun::IsNTUT = false;
ChipDale* CGameStateRun::player[2];
CGameStateRun::CGameStateRun(CGame* g)
    : CGameState(g)
{
    IfViewer = false;
    NowLevel = 1;
    show_save = false;
    show_load = false;
    EditMoveMode = 0;
    Maps = NULL;
    player[0] = NULL;
    player[1] = NULL;
    for (int i = 0; i < CanTakeNum; i++) //�o�u�ݼg�@�� ��l�ư}�C
        AllThrow[i] = NULL;
    for (int i = 0; i < ONE_LEVEL_MONSTER_NUM; i++) //�o�u�ݼg�@�� ��l�ư}�C
        AllMonster[i] = NULL;
    ChipDale::CanThrow = AllThrow;
    ChipDale::AllMonster = AllMonster;


}

CGameStateRun::~CGameStateRun()
{
    if (Maps != NULL)	delete(Maps);
    for (int i = 0; i < ONE_LEVEL_MONSTER_NUM && AllMonster[i] != NULL; i++)
    {
        delete(AllMonster[i]);
    }
    for (int i = 0; i < CanTakeNum; i++)
    {
        if (AllThrow[i] == NULL)continue;
        delete(AllThrow[i]);
    }
    for (int i = 0; i < 2 && player[i] != NULL; i++)
        delete(player[i]);
}

void CGameStateRun::OnBeginState()
{
    int initial_Y;


    TimeToGo = false;

    EditPointer.SetWxWy(0, 0);
    Maps = new MapManage(NowLevel);

    CAudio::Instance()->PlayLevelMusic(0, true);
    CAudio::Instance()->PauseLevelMusic(0);
    CAudio::Instance()->Play(AUDIO_1_1A);
    //CAudio::Instance()->PlayLevelMusic(NowLevel,true);

    ChipDale::Maps = Maps;
    Maps->SetObstacle();
    Maps->SetMonster(AllMonster);
    for (int i = 0; i < CanTakeNum; i++)
    {
        if (AllThrow[i] != NULL)
        {
            delete AllThrow[i];
            AllThrow[i] = NULL;
        }
    }


    if (player[0] != NULL)delete player[0];
    if (player[1] != NULL)delete player[1];

    player[0] = new ChipDale(Chip_Dale);
    if (multiplayer == 2)
    {
        player[1] = new ChipDale(1);
    }
    else
        player[1] = NULL;

    player[0]->SetPartner(player[1]);
    if (multiplayer == 2)	player[1]->SetPartner(player[0]);

    initial_Y = (MapManage::Teleport >> 7) / DesSize;//�o��X�ͪ��� , DesSize ��� Des �̤j����(128)
    TRACE("initial_Y: %d\n", initial_Y);
    player[0]->InitialWidthHeight();
    player[0]->SetWxWy(80, initial_Y - player[0]->ReturnHeight());
    if (multiplayer == 2)
    {
        player[1]->InitialWidthHeight();
        player[1]->SetWxWy(80 + player[1]->ReturnWidth(), initial_Y - player[1]->ReturnHeight());
    }
}

void CGameStateRun::OnMove()							// ���ʹC������
{
    //
    // �p�G�Ʊ�ק�cursor���˦��A�h�N�U���{����commment�����Y�i
    //
    // SetCursor(AfxGetApp()->LoadCursor(IDC_GAMECURSOR));
    //
    //
    //TRACE("1\n");
    int des, initial_Y, fixX, fixY;
    int toolCDC_State = ToolCDC::ReturnStage();

    static int music_count = 0;
    if (music_count < 6 * 33 - 5)music_count++;
    if (music_count == 6 * 33 - 5)
    {
        CAudio::Instance()->PlayLevelMusic(NowLevel, true);
        music_count++;
    }

    if (GameScore::Instance()->Switch)return;
    //����Ȱ���_���n�P�ɼ���Ҧ�����
    /*if(IfViewer){
    CAudio::Instance()->PauseById(AUDIO_1_1);
    }else{
    CAudio::Instance()->PauseById(AUDIO_EDIT);
    }*/
    //CAudio::Instance()->StopLevelMusic(NowLevel);
    //CAudio::Instance()->Stop(AUDIO_EDIT);
    //GotoGameState(GAME_STATE_BONUS);
    if (TimeToGo || TimeToBonus == 1)
    {
        //CAudio::Instance()->Stop(AUDIO_1_1);
        CAudio::Instance()->StopLevelMusic(NowLevel);
        CAudio::Instance()->Stop(AUDIO_EDIT);
        if (TimeToGo)
        {
            player[0]->ResetScore();
            if (player[1] != NULL)player[1]->ResetScore();
            TimeToGo = false;
            GotoGameState(GAME_STATE_OVER);

        }
        else
        {
            TimeToBonus = false;
            GotoGameState(GAME_STATE_BONUS);

        }
    }
    if (TimeToBonus > 0)
    {
        if (TimeToBonus == PassMusicTime)
        {
            CAudio::Instance()->StopLevelMusic(NowLevel);
            CAudio::Instance()->Play(AUDIO_1_PASS);
        }
        TimeToBonus--;
    }
    if (toolCDC_State == 0)
    {
        if (!IfViewer)
        {

            //�H������
            if (!TimeToBonus)
            {
                player[0]->OnMove();
                if (multiplayer == 2)
                    player[1]->OnMove();
            }
            //�����`�Ѧ���i�o�H��Y�b
            //static int lastNowY = 0;int tempNowY = player[0]->ReturnWY()+player[0]->ReturnHeight();if(lastNowY!=tempNowY){TRACE("now_Y: %d\n",tempNowY);lastNowY=tempNowY;}

            //�H���ץ�
            player[0]->FixXY(Maps);
            if (multiplayer == 2)
                player[1]->FixXY(Maps);

            for (int i = 0; i < CanTakeNum; i++)
            {
                if (AllThrow[i] == NULL)continue;
                if (AllThrow[i]->ReturnNowAct() == 0)
                {
                    delete(AllThrow[i]);
                    AllThrow[i] = NULL;
                    continue;
                }
                if (AllThrow[i]->ReturnNowAct() >= 0)//���ɭԤ��QFIX�ץ���m �i�H�� NowAct ���t��!!!
                    AllThrow[i]->FixXY(Maps);
                AllThrow[i]->OnMove(Maps);
            }

            //�Ǫ�����+�ץ�+�I������
            for (int i = 0; i < ONE_LEVEL_MONSTER_NUM && AllMonster[i] != NULL; i++)
            {
                if (AllMonster[i]->OnMove(Maps, player))
                {
                    AllMonster[i]->CollisionChipDale(player[0]);
                    if (multiplayer == 2)
                        AllMonster[i]->CollisionChipDale(player[1]);
                }
            }


            //�I�� ���� �Ǫ� �H�� ����
            for (int i = 0; i < CanTakeNum; i++)
            {
                if (AllThrow[i] == NULL)continue;
                AllThrow[i]->CollisionMonster(AllMonster);
                AllThrow[i]->CollisionChipDale(player[0]);
                if (multiplayer == 2)
                    AllThrow[i]->CollisionChipDale(player[1]);
            }
        }
        else
        {
            EditPointer.OnMove(Maps);
            EditPointer.FixXY(Maps);
        }
    }
    else if (toolCDC_State == 2)
    {
        des = (Maps->Teleport) % DesSize;//�o��X�ͪ��� , DesSize ��� Des �̤j����(128)
        initial_Y = (Maps->GetRoute((des - 1) % 10, (des - 1) / 10) >> 7) / DesSize;
        TRACE("initial_Y: %d\n", initial_Y);
        fixX = -Maps->ReturnNowX() + (des - 1) % MAX_XN * MWIDTH;
        fixY = -Maps->ReturnNowY() + (des - 1) / MAX_YN * MHEIGHT;
        Maps->SetMapXY((des - 1) % MAX_XN * MWIDTH, (des - 1) / MAX_YN * MHEIGHT);
        Maps->SetRecord();

        for (int i = 0; i < ONE_LEVEL_MONSTER_NUM && AllMonster[i] != NULL; i++)
        {
            AllMonster[i]->FixMapMove(fixX, fixY);
        }
        for (int i = 0; i < CanTakeNum; i++)
        {
            if (AllThrow[i] == NULL)continue;
            if (AllThrow[i]->ReturnNowAct() != 1)
            {
                delete(AllThrow[i]);
                AllThrow[i] = NULL;
            }
        }
        //player[0]->Reset(80,player[0]->ReturnWY());
        //if(multiplayer==2)
        //player[1]->Reset(125,player[1]->ReturnWY());
        player[0]->Reset(80, initial_Y - player[0]->ReturnHeight(), true, false);
        if (multiplayer == 2)
            player[1]->Reset(125, initial_Y - player[1]->ReturnHeight(), true, false);
        //player[0]->SetWxWy(80,player[0]->ReturnWY());//-1 ��O�����
        //if(multiplayer==2)
        //	player[1]->SetWxWy(125,player[1]->ReturnWY());//-1 ��O�����
        ToolCDC::ToNextState();
    }
    else if (toolCDC_State == 4)
    {
        if (Maps->GetRoute() & 64)
        {
            IsBoss = true;
            CAudio::Instance()->PlayLevelMusic(NowLevel, true);
        }
        else
        {
            IsBoss = false;
        }
    }
}

void CGameStateRun::OnInit()  								// �C������Ȥιϧγ]�w
{
    //
    // ��ϫܦh�ɡAOnInit���J�Ҧ����ϭn��ܦh�ɶ��C���קK���C�����H
    //     �������@�СA�C���|�X�{�uLoading ...�v�A���Loading���i�סC
    //
    ShowInitProgress(33);	// ���ӫe�@�Ӫ��A���i�סA���B�i�׵���33%
    //
    // �}�l���J���
    //

    Health[0].LoadBitmapA("Bitmaps/StateRun/health_1.bmp", PURPLE);
    Health[1].LoadBitmapA("Bitmaps/StateRun/health_2.bmp", PURPLE);
    Health[2].LoadBitmapA("Bitmaps/StateRun/health_3.bmp", PURPLE);
    ShowWho[0].LoadBitmapA("Bitmaps/StateRun/chip_L.bmp", PURPLE);
    ShowWho[1].LoadBitmapA("Bitmaps/StateRun/dale_L.bmp", PURPLE);
    ShowWho[2].LoadBitmapA("Bitmaps/StateRun/dale_R.bmp", PURPLE);

    ShowInitProgress(40);
    ChipDale::Loading();
    EditPointer.Loading();
    ToolCDC::Loading();

    //Maps->LevelLoading(NowLevel);
    //�p��i�J�ĤG��(���O�g�b�o!!!! �o�O�ܽd)
    //delete(Maps);
    //Maps = new MapManage();
    //Maps->LevelLoading(NowLevel);

    ShowInitProgress(45);

    //�Ǫ��Ϥ�loading
    MachineDog::Loading();
    Cactus::Loading();
    Wire::Loading();
    Mouse::Loading();
    Wasp::Loading();
    Centipede::Loading();
    //End Loading

    ShowInitProgress(50);

    //����Ϥ�loading
    Apple::Loading();
    Stone::Loading();
    Craft::Loading();
    Star::Loading();
    Explosion::Loading();
    Angel::Loading();
    ChipDale_Dead::Loading();
    ChipDale_Resurrect::Loading();
    ChipDale_Taken::Loading();
    Greedy::Loading();
    Cheese::Loading();
    Ball::Loading();
    //End Loading

    ShowInitProgress(75);

    saved.LoadBitmapA("Bitmaps/object/save.bmp", 0xffffff);
    loading.LoadBitmapA("Bitmaps/object/loading.bmp", 0xffffff);
    //
    // ��������Loading�ʧ@�A�����i��
    //
    ShowInitProgress(80);
    //Sleep(300); // ��C�A�H�K�ݲM���i�סA��ڹC���ЧR����Sleep
    //
    // �~����J��L���
    //

    CAudio::Instance()->LevelMusicOnit();

    CAudio::Instance()->Load(AUDIO_JUMP, "sounds/jump.mp3");
    CAudio::Instance()->Load(AUDIO_EDIT, "sounds/edit.mp3");
    CAudio::Instance()->Load(AUDIO_DEAD, "sounds/dead.mp3");
    CAudio::Instance()->Load(AUDIO_1_1, "sounds/state/1/level_1B.mp3");
    CAudio::Instance()->Load(AUDIO_1_BOSS, "sounds/state/1/boss_1B.mp3");
    CAudio::Instance()->Load(AUDIO_1_1A, "sounds/state/1/level_1A.mp3");
    CAudio::Instance()->Load(AUDIO_1_PASS, "sounds/state/1/level_1pass.mp3");



    //
    // ��OnInit�ʧ@�|����CGameStaterOver::OnInit()�A�ҥH�i���٨S��100%
    //
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    //�K�ް� �ŧi
    static char GodMode[] = "GOD";
    static int  GodModeI = 0;
    static char EditMode[] = "EDIT";
    static int EditModeI = 0;
    static char NTUTMode[] = "NTUT";
    static int NTUTModeI = 0;
    static char Full[] = "9999";
    static int FullI = 0;


    //TRACE("2\n");
    const char KEY_LEFT = 0x25; // keyboard���b�Y
    const char KEY_UP = 0x26; // keyboard�W�b�Y
    const char KEY_RIGHT = 0x27; // keyboard�k�b�Y
    const char KEY_DOWN = 0x28; // keyboard�U�b�Y

    //�K�ް� �갵
    //God
    if (nChar == GodMode[GodModeI])
    {
        GodModeI++;
        if (GodMode[GodModeI] == 0)
        {
            player[0]->GodMode();
            GodModeI = 0;
        }
    }
    else	GodModeI = 0;
    //Full
    if (nChar == Full[FullI])
    {
        FullI++;
        if (Full[FullI] == 0)
        {
            player[0]->SetHealth(9999);
            GodModeI = 0;
        }
    }
    else	FullI = 0;
    //EDIT
    if (nChar == NTUTMode[NTUTModeI])
    {
        NTUTModeI++;
        if (NTUTMode[NTUTModeI] == 0)
        {
            IsNTUT = !IsNTUT;
            NTUTModeI = 0;
        }
    }
    else	NTUTModeI = 0;

    if (nChar == EditMode[EditModeI]) // CapsLock �����H���P�y���̼Ҧ�
    {
        EditModeI++;
        if (EditMode[EditModeI] == 0)
        {
            if (player[0]->ReturnHealth() <= 0 || multiplayer == 2)
            {
                EditModeI = 0;
                return;
            }
            IfViewer = !IfViewer;
            if (IfViewer)
            {
                CAudio::Instance()->PlayLevelMusic(0);
                //CAudio::Instance()->Resume();
                //CAudio::Instance()->PauseById(AUDIO_1_1);
            }
            else
            {
                CAudio::Instance()->PlayLevelMusic(NowLevel);
                //CAudio::Instance()->Resume();
                //CAudio::Instance()->PauseById(AUDIO_EDIT);
            }
            //player[0].SetWxWy(player[0].ReturnWX(),0);
            //player[0]->SetWxWy(EditPointer.ReturnWX(),EditPointer.ReturnWY());
            player[0]->Reset(EditPointer.ReturnWX(), EditPointer.ReturnWY(), player[0]->ReturnLR(), false);

            if (multiplayer == 2)
            {
                //player[1]->SetWxWy(EditPointer.ReturnWX()+player[1]->ReturnWidth(),EditPointer.ReturnWY());
                player[1]->Reset(EditPointer.ReturnWX() + player[1]->ReturnWidth(), EditPointer.ReturnWY(), player[1]->ReturnLR(), false);
            }

            EditPointer.SetMove(false, true, true, true, true, EditMoveMode);

            if (IfViewer)
            {
                Maps->LevelLoading(NowLevel, true);
                Maps->SetMapXY(Maps->ReturnNowX() / ONEOBJX * ONEOBJX, Maps->ReturnNowY() / ONEOBJY * ONEOBJY);
            }
            else
            {
                Maps->SetMapXY(Maps->ReturnNowX() / SPEED * SPEED, Maps->ReturnNowY() / SPEED * SPEED);
                Maps->SetObstacle();
                Maps->SetMonster(AllMonster);
                player[0]->ReleaseNowTakeObj();
                if (multiplayer == 2)
                    player[1]->ReleaseNowTakeObj();
                for (int i = 0; i < CanTakeNum; i++)
                {
                    if (AllThrow[i] == NULL)continue;
                    if (AllThrow[i]->ReturnNowAct() >= 0)
                    {
                        delete(AllThrow[i]);
                        AllThrow[i] = NULL;
                    }
                }
            }
            EditModeI = 0;
        }
    }
    else	EditModeI = 0;

    //�@�����Ĳ�o
    if (!IfViewer)
    {
        if (multiplayer == 1)
            player[0]->setFlag(true, (nChar == KEY_UP), (nChar == KEY_DOWN), (nChar == KEY_LEFT), (nChar == KEY_RIGHT), (nChar == 'Z'), (nChar == 'X'));
        if (multiplayer == 2)
        {
            player[0]->setFlag(true, (nChar == KEY_UP), (nChar == KEY_DOWN), (nChar == KEY_LEFT), (nChar == KEY_RIGHT), (nChar == 'N'), (nChar == 'M'));
            player[1]->setFlag(true, (nChar == 87), (nChar == 83), (nChar == 65), (nChar == 68), (nChar == 90), (nChar == 88));
        }
        if (nChar == 9)GameScore::Instance()->ShowScroe();
    }
    else
    {
        show_save = false;
        EditPointer.SetMove(true, (nChar == KEY_UP), (nChar == KEY_DOWN), (nChar == KEY_LEFT), (nChar == KEY_RIGHT), EditMoveMode);
        if (nChar == 'Z')	EditPointer.SetOrder(EditPointer.nowOrder() - 1);
        if (nChar == 'X')	EditPointer.SetOrder(EditPointer.nowOrder() + 1);
        if (nChar == 13)	EditPointer.SetObject(Maps);		// Enter ��m
        if (nChar == 9)	EditMoveMode = (EditMoveMode + 1) % 3;	// TAB �����Ҧ�
        if (nChar == 27)   // ESC �x�s
        {
            Maps->SaveObj(NowLevel);
            show_save = true;
        }
    }

}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    //TRACE("3\n");
    const char KEY_LEFT = 0x25; // keyboard���b�Y
    const char KEY_UP = 0x26; // keyboard�W�b�Y
    const char KEY_RIGHT = 0x27; // keyboard�k�b�Y
    const char KEY_DOWN = 0x28; // keyboard�U�b�Y
    if (!IfViewer)
    {
        if (multiplayer == 1)
            player[0]->setFlag(false, (nChar == KEY_UP), (nChar == KEY_DOWN), (nChar == KEY_LEFT), (nChar == KEY_RIGHT), (nChar == 'Z'), (nChar == 'X'));
        if (multiplayer == 2)
        {
            player[0]->setFlag(false, (nChar == KEY_UP), (nChar == KEY_DOWN), (nChar == KEY_LEFT), (nChar == KEY_RIGHT), (nChar == 'N'), (nChar == 'M'));
            player[1]->setFlag(false, (nChar == 87), (nChar == 83), (nChar == 65), (nChar == 68), (nChar == 90), (nChar == 88));
        }
        if (nChar == 9)GameScore::Instance()->ShowScroe();
    }
    else
    {
        EditPointer.SetMove(false, (nChar == KEY_UP), (nChar == KEY_DOWN), (nChar == KEY_LEFT), (nChar == KEY_RIGHT));
    }
}

void CGameStateRun::OnShow()
{
    //
    //  �`�N�GShow�̭��d�U���n���ʥ��󪫥󪺮y�СA���ʮy�Ъ��u�@����Move���~��A
    //        �_�h��������sø�Ϯ�(OnDraw)�A����N�|���ʡA�ݰ_�ӷ|�ܩǡC���ӳN�y
    //        ���AMove�t�dMVC����Model�AShow�t�dView�A��View�������Model�C
    //
    //
    //  �K�W�I���ϡB�����ơB�y�B���l�B�u�����y
    //
    //
    //TRACE("4\n");
    int toolCDC_State = ToolCDC::ReturnStage();
    if (GameScore::Instance()->Switch)
    {
        GameScore::Instance()->onShow();
        return;
    }
    if (toolCDC_State == 0 || toolCDC_State == 1 || toolCDC_State == 4)
    {
        Maps->OnShow();

        if (!IfViewer)
        {
            for (int i = 0; i < CanTakeNum; i++) //��������R�� NowAct=3 ��O�b�Ǫ�����
            {
                if (AllThrow[i] == NULL)continue;
                if (AllThrow[i]->ReturnNowAct() != 3)continue;
                //TRACE("AllThrow %d\n",AllThrow[i]->ReturnNowAct());
                AllThrow[i]->OnShow(Maps);
            }

            for (int i = 0; i < ONE_LEVEL_MONSTER_NUM && AllMonster[i] != NULL; i++) //�Ǫ����
            {
                AllMonster[i]->OnShow(Maps);
                //TRACE("game show %d\n",i);
            }

            for (int i = 0; i < CanTakeNum; i++) //��������R�� NowAct=5 �R��b�a�W ��O�b�Ǫ����e
            {
                if (AllThrow[i] == NULL)continue;
                if (AllThrow[i]->ReturnNowAct() != 5)continue;
                //TRACE("AllThrow %d\n",AllThrow[i]->ReturnNowAct());
                AllThrow[i]->OnShow(Maps);
            }
            Maps->OnShowObject();

            //������ܦb�⤤ NowAct=1
            if (player[0]->ReturnNowTakeObj() != NULL)
                player[0]->ReturnNowTakeObj()->OnShow(Maps);
            if (multiplayer == 2 && player[1]->ReturnNowTakeObj() != NULL)
            {
                player[1]->ReturnNowTakeObj()->OnShow(Maps);
            }

            player[0]->OnShow();//�H�����
            if (multiplayer == 2)
                player[1]->OnShow();

            for (int i = 0; i < CanTakeNum; i++) //������ܹB�ʤ� NowAct = 2 4
            {
                if (AllThrow[i] == NULL)continue;
                if (AllThrow[i]->ReturnNowAct() != 2 && AllThrow[i]->ReturnNowAct() != 4)continue;
                AllThrow[i]->OnShow(Maps);
            }


            ShowWho[Chip_Dale].SetTopLeft(0, 0);
            ShowWho[Chip_Dale].ShowBitmap();
            switch (player[0]->ReturnHealth()) //��q���
            {
                case 1:
                    Health[0].SetTopLeft(0, 0);
                    Health[0].ShowBitmap();
                    break;
                case 2:
                    Health[1].SetTopLeft(0, 0);
                    Health[1].ShowBitmap();
                    break;
                default:
                    Health[2].SetTopLeft(0, 0);
                    Health[2].ShowBitmap();
                    break;
            }
            if (multiplayer == 2)
            {
                ShowWho[2].SetTopLeft(MWIDTH - ShowWho[2].Width(), 0);
                ShowWho[2].ShowBitmap();
                switch (player[1]->ReturnHealth()) //��q���
                {
                    case 1:
                        Health[0].SetTopLeft(MWIDTH - ShowWho[2].Width(), 0);
                        Health[0].ShowBitmap();
                        break;
                    case 2:
                        Health[1].SetTopLeft(MWIDTH - ShowWho[2].Width(), 0);
                        Health[1].ShowBitmap();
                        break;
                    default:
                        Health[2].SetTopLeft(MWIDTH - ShowWho[2].Width(), 0);
                        Health[2].ShowBitmap();
                        break;
                }
            }
        }
        else
            EditPointer.OnShow(Maps);

        //ClearisMoveMap �@�w�n��b�Ҧ��n show ���󪺳̫᭱
        Maps->ClearisMoveMap(IfViewer);

        if (show_save)
            saved.ShowBitmap();

        if (toolCDC_State == 1 || toolCDC_State == 4)
        {
            ToolCDC::SaveCDC();
            ToolCDC::ToNextState();
            if (toolCDC_State == 4)	ToolCDC::CDDraw();
        }
    }
    else
    {
        ToolCDC::CDDraw();
        ToolCDC::ToNextState();
    }

    //TRACE("(wx,wy)player1: %d , %d  , player2: %d , %d \n",player[0].ReturnWX(),player[0].ReturnWY(),player[1].ReturnWX(),player[1].ReturnWY());
    //TRACE("(x,y)Maps: %d , %d\n",Maps.ReturnNowX(),Maps.ReturnNowY());
#if defined (DELAY)
    Sleep(DELAY);
#endif
    //
    //  �K�W���W�Υk�U��������
    //
}

}