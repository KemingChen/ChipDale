#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "audio.h"
#include "gamelib.h"
#include "mygame.h"
#include "instruction.h"
#include "chipdale.h"
#include "object.h"
#include "monster.h"
#include "mapmanage.h"

namespace game_framework
{
/****************************************************************/
//ChipDale實作													*/
//																*/
/****************************************************************/
Object**  	ChipDale::CanThrow;
MapManage* ChipDale::Maps;
Monster**  	ChipDale::AllMonster;
CAnimation ChipDale::animation[2][2][ACTION_NUM][2];					// 動作~方向
CAnimation ChipDale::ani_sweat[2];
CAnimation ChipDale::ani_dizzy;
CAnimation ChipDale::ani_god[2];

ChipDale::ChipDale(int isDale)
{
    IsDale = isDale;
    Life = 3;
    Reset(10, 10);//必須先有Life才能reset
    IsGod = false;
    ResetScore();
}
void ChipDale::Reset(int Wx, int Wy, bool LR, bool FullHealth)
{
    if (Life <= 0)return;
    wx = Wx;
    wy = Wy;
    LRflag = LR;
    if (FullHealth)Health = 3;
    LOCK = false;
    time_jump = 0;
    NowAct = LastAct = 0;
    NowTaken = 0;
    freeze = 0;
    Reduce_UP_VELOCITY = 0;
    Invincible = IsFaint = 0;
    Hurt = false;
    NowTakeObj = NULL;
    ani_sweat_count = 0;
    ani_Hide_freeze_jump = false;
    setFlag(false, true, true, true, true, true, true);
    Alive = true;

    //
    UpSpeed = 0;
    time_jump = 0;
    IsJump = false;
    IsRun = false;
    IsLessCollision = false;
    ani_jump_count = TIMEFOR_ANI_JUMP + 1;
    Reduce_UP_VELOCITY = 0;
    //


    TRACE("Life %d wy %d\n", Life, wy);
};
void ChipDale::Loading()
{
    char name[6] = "chip/";
    for (int i = 0; i < 2; i++)
    {

        animation[i][0][0][0].AddBitmap("Bitmaps/action/", name, "stand_L.bmp", PURPLE);
        animation[i][0][0][1].AddBitmap("Bitmaps/action/", name, "stand_R.bmp", PURPLE);
        animation[i][0][1][0].AddBitmap("Bitmaps/action/", name, "run_1L.bmp", PURPLE);
        animation[i][0][1][0].AddBitmap("Bitmaps/action/", name, "run_2L.bmp", PURPLE);
        animation[i][0][1][0].AddBitmap("Bitmaps/action/", name, "run_3L.bmp", PURPLE);
        animation[i][0][1][0].AddBitmap("Bitmaps/action/", name, "run_4L.bmp", PURPLE);
        animation[i][0][1][1].AddBitmap("Bitmaps/action/", name, "run_1R.bmp", PURPLE);
        animation[i][0][1][1].AddBitmap("Bitmaps/action/", name, "run_2R.bmp", PURPLE);
        animation[i][0][1][1].AddBitmap("Bitmaps/action/", name, "run_3R.bmp", PURPLE);
        animation[i][0][1][1].AddBitmap("Bitmaps/action/", name, "run_4R.bmp", PURPLE);

        animation[i][0][2][0].AddBitmap("Bitmaps/action/", name, "jump_1L.bmp", PURPLE);
        animation[i][0][2][1].AddBitmap("Bitmaps/action/", name, "jump_1R.bmp", PURPLE);

        animation[i][0][3][0].AddBitmap("Bitmaps/action/", name, "throw_L.bmp", PURPLE);
        animation[i][0][3][1].AddBitmap("Bitmaps/action/", name, "throw_R.bmp", PURPLE);
        animation[i][0][4][0].AddBitmap("Bitmaps/action/", name, "Squat_1L.bmp", PURPLE);
        animation[i][0][4][1].AddBitmap("Bitmaps/action/", name, "Squat_1R.bmp", PURPLE);
        animation[i][0][5][0].AddBitmap("Bitmaps/action/", name, "Hurt_L.bmp", PURPLE);
        animation[i][0][5][1].AddBitmap("Bitmaps/action/", name, "Hurt_R.bmp", PURPLE);
        animation[i][0][6][0].AddBitmap("Bitmaps/action/", name, "Faint_1L.bmp", PURPLE);
        animation[i][0][6][0].AddBitmap("Bitmaps/action/", name, "Faint_2L.bmp", PURPLE);
        animation[i][0][6][0].AddBitmap("Bitmaps/action/", name, "Faint_3L.bmp", PURPLE);
        animation[i][0][6][0].AddBitmap("Bitmaps/action/", name, "Faint_4L.bmp", PURPLE);
        animation[i][0][6][1].AddBitmap("Bitmaps/action/", name, "Faint_1R.bmp", PURPLE);
        animation[i][0][6][1].AddBitmap("Bitmaps/action/", name, "Faint_2R.bmp", PURPLE);
        animation[i][0][6][1].AddBitmap("Bitmaps/action/", name, "Faint_3R.bmp", PURPLE);
        animation[i][0][6][1].AddBitmap("Bitmaps/action/", name, "Faint_4R.bmp", PURPLE);
        ////
        animation[i][1][0][0].AddBitmap("Bitmaps/action/", name, "Take_L.bmp", PURPLE);
        animation[i][1][0][1].AddBitmap("Bitmaps/action/", name, "Take_R.bmp", PURPLE);
        animation[i][1][1][0].AddBitmap("Bitmaps/action/", name, "Take_Run_1L.bmp", PURPLE);
        animation[i][1][1][0].AddBitmap("Bitmaps/action/", name, "Take_Run_2L.bmp", PURPLE);
        animation[i][1][1][1].AddBitmap("Bitmaps/action/", name, "Take_Run_1R.bmp", PURPLE);
        animation[i][1][1][1].AddBitmap("Bitmaps/action/", name, "Take_Run_2R.bmp", PURPLE);
        animation[i][1][2][0].AddBitmap("Bitmaps/action/", name, "Take_Jump_L.bmp", PURPLE);
        animation[i][1][2][1].AddBitmap("Bitmaps/action/", name, "Take_Jump_R.bmp", PURPLE);
        ////
        animation[i][1][3][0].AddBitmap("Bitmaps/action/", name, "throw_L.bmp", PURPLE);
        animation[i][1][3][1].AddBitmap("Bitmaps/action/", name, "throw_R.bmp", PURPLE);
        animation[i][1][4][0].SetDelayCount(2);
        animation[i][1][4][0].AddBitmap("Bitmaps/action/", name, "Hide_crate_1L.bmp", PURPLE);
        animation[i][1][4][0].AddBitmap("Bitmaps/action/", name, "Hide_crate_2L.bmp", PURPLE);
        animation[i][1][4][0].AddBitmap("Bitmaps/action/", name, "Hide_crate_3L.bmp", PURPLE);
        animation[i][1][4][0].AddBitmap("Bitmaps/action/", name, "Hide_crate_4L.bmp", PURPLE);
        animation[i][1][4][0].AddBitmap("Bitmaps/action/", name, "Hide_crate_5L.bmp", PURPLE);
        animation[i][1][4][0].AddBitmap("Bitmaps/action/", name, "Hide_crate_6L.bmp", PURPLE);
        animation[i][1][4][1].SetDelayCount(2);
        animation[i][1][4][1].AddBitmap("Bitmaps/action/", name, "Hide_crate_1R.bmp", PURPLE);
        animation[i][1][4][1].AddBitmap("Bitmaps/action/", name, "Hide_crate_2R.bmp", PURPLE);
        animation[i][1][4][1].AddBitmap("Bitmaps/action/", name, "Hide_crate_3R.bmp", PURPLE);
        animation[i][1][4][1].AddBitmap("Bitmaps/action/", name, "Hide_crate_4R.bmp", PURPLE);
        animation[i][1][4][1].AddBitmap("Bitmaps/action/", name, "Hide_crate_5R.bmp", PURPLE);
        animation[i][1][4][1].AddBitmap("Bitmaps/action/", name, "Hide_crate_6R.bmp", PURPLE);
        animation[i][1][5][0].AddBitmap("Bitmaps/action/", name, "Hurt_L.bmp", PURPLE);
        animation[i][1][5][1].AddBitmap("Bitmaps/action/", name, "Hurt_R.bmp", PURPLE);
        animation[i][1][6][0].AddBitmap("Bitmaps/action/", name, "Faint_1L.bmp", PURPLE);
        animation[i][1][6][0].AddBitmap("Bitmaps/action/", name, "Faint_2L.bmp", PURPLE);
        animation[i][1][6][0].AddBitmap("Bitmaps/action/", name, "Faint_3L.bmp", PURPLE);
        animation[i][1][6][0].AddBitmap("Bitmaps/action/", name, "Faint_4L.bmp", PURPLE);
        animation[i][1][6][1].AddBitmap("Bitmaps/action/", name, "Faint_1R.bmp", PURPLE);
        animation[i][1][6][1].AddBitmap("Bitmaps/action/", name, "Faint_2R.bmp", PURPLE);
        animation[i][1][6][1].AddBitmap("Bitmaps/action/", name, "Faint_3R.bmp", PURPLE);
        animation[i][1][6][1].AddBitmap("Bitmaps/action/", name, "Faint_4R.bmp", PURPLE);

        sprintf(name, "%s", "dale/");
    }
    ani_sweat[0].AddBitmap("Bitmaps/action/ANI_ChipDale/sweat_L.bmp", PURPLE);
    ani_sweat[1].AddBitmap("Bitmaps/action/ANI_ChipDale/sweat_R.bmp", PURPLE);

    ani_dizzy.SetDelayCount(7);
    ani_dizzy.AddBitmap("Bitmaps/action/ANI_ChipDale/Dizzy1.bmp", PURPLE);
    ani_dizzy.AddBitmap("Bitmaps/action/ANI_ChipDale/Dizzy2.bmp", PURPLE);
    ani_dizzy.AddBitmap("Bitmaps/action/ANI_ChipDale/Dizzy3.bmp", PURPLE);
    ani_dizzy.AddBitmap("Bitmaps/action/ANI_ChipDale/Dizzy4.bmp", PURPLE);

    ani_god[0].AddBitmap("Bitmaps/action/ANI_ChipDale/god_L.bmp", PURPLE);
    ani_god[1].AddBitmap("Bitmaps/action/ANI_ChipDale/god_R.bmp", PURPLE);

    ///
}
void ChipDale::OnShow()
{
    //TRACE("%d\n",wy);
    int Last_ani_height, fix_hieght;
    if (!Alive && !IsGod)return;

    //特定動畫重設循環
    if (NowAct != 4)
    {
        if (animation[IsDale][1][4][0].GetCurrentBitmapNumber() >= 4)
        {
            animation[IsDale][1][4][0].SetDelayCount(2);/*SET完記得RESET，不然第一張圖會用到舊的DELAY_COUNTER*/
            animation[IsDale][1][4][0].Reset();

        }
        if (animation[IsDale][1][4][1].GetCurrentBitmapNumber() >= 4)
        {
            animation[IsDale][1][4][1].SetDelayCount(2);
            animation[IsDale][1][4][1].Reset();
        }


        //if(animation[1][4][0].IsFinalBitmap())animation[1][4][0].Reset();
        //if(animation[1][4][1].IsFinalBitmap())animation[1][4][1].Reset();
    }

    if (LastAct == 2)
    {
        animation[IsDale][0][2][0].Reset();
        animation[IsDale][0][2][1].Reset();
    }

    if (ani_Hide_freeze_jump == 0)ani_Hide_freeze_jump = 1;



    //動畫切9=換 及 重設height wxwy
    if (!ReturnHideComplete()
            /*!(animation[1][4][1].GetCurrentBitmapNumber()>=4)&&
            !(animation[1][4][0].GetCurrentBitmapNumber()>=4)*/
            /*!animation[1][4][1].IsFinalBitmap()
            &&!animation[1][4][0].IsFinalBitmap()*/)
    {
        //TRACE("%d\n",animation[IsDale][1][4][1].GetCurrentBitmapNumber());
        Last_ani_height = height;

        if (NowAct != 2)
            animation[IsDale][TAKEN_FLAG][NowAct][LRflag].OnMove();

        if (Last_ani_height != animation[IsDale][TAKEN_FLAG][NowAct][LRflag].Height())
        {
            height = animation[IsDale][TAKEN_FLAG][NowAct][LRflag].Height();
            fix_hieght = Last_ani_height - height;
            SetWxWy(wx, wy + fix_hieght);
        }
    }
    else
    {
        ani_Hide_freeze_jump = 0;
        if (TAKEN_FLAG && NowAct == 4)
        {
            if (!animation[IsDale][1][4][LRflag].IsFinalBitmap())
            {

                animation[IsDale][1][4][LRflag].SetDelayCount(5);
                animation[IsDale][1][4][LRflag].OnMove();
            }
            else
            {
                animation[IsDale][1][4][LRflag].SetDelayCount(100);
                animation[IsDale][1][4][LRflag].OnMoveToNum(4);
            }
        }
    }

    //顯示人物
    //人物無敵效果

    if ((Invincible && (Invincible % 5 == 2 || Invincible % 5 == 3)) || !Invincible || (NowAct == 4 && TAKEN_FLAG))
    {
        if (IsGod)
        {
            IsJump = false;
            if (LRflag)
                ani_god[LRflag].SetBottomLeft(wx + (-ani_god[LRflag].Width() + 15), wy, ani_god[LRflag].Height());
            else
                ani_god[LRflag].SetBottomLeft(wx + width - 20, wy, ani_god[LRflag].Height());
            ani_god[LRflag].OnShow();
        }
        if (ani_jump_count < TIMEFOR_ANI_JUMP)
        {
            //跳轉蹲動畫
            /*animation[0][2][LRflag].SetBottomLeft(wx,wy,height);
            animation[0][2][LRflag].OnMove();
            animation[0][2][LRflag].OnShow();*/
            Last_ani_height = height;
            height = animation[IsDale][0][4][LRflag].Height();
            fix_hieght = Last_ani_height - height;
            SetWxWy(wx, wy + fix_hieght);
            //TRACE("跳轉蹲\n");
            animation[IsDale][0][4][LRflag].SetBottomLeft(wx, wy, height);
            animation[IsDale][0][4][LRflag].OnShow();
        }
        else
        {
            //一般狀態動畫顯示
            animation[IsDale][TAKEN_FLAG][NowAct][LRflag].SetBottomLeft(wx, wy, height);
            animation[IsDale][TAKEN_FLAG][NowAct][LRflag].OnShow();
        }
        //流汗動畫顯示
        if (ani_sweat_count > 0)
        {
            int tempx = ani_sweat_x + 10 - Maps->ReturnNowX();/*-((Maps->MoveMap(0)&8)>0)*SPEED*ani_sweat_count*/;
            if (!LRflag)tempx = ani_sweat_x + width - 20 - Maps->ReturnNowX();/*+((Maps->MoveMap(0)&4)>0)*SPEED*ani_sweat_count*/;

            ani_sweat[LRflag].SetBottomLeft(tempx, ani_sweat_wy + ani_sweat_count * 7/*汗的位移*/, ani_sweat[LRflag].Height());
            ani_sweat[LRflag].OnShow();
            //TRACE("count %d\n",ani_sweat_count);
        }
    }

    //暈眩顯示
    if (NowAct == 6)
    {
        ani_dizzy.SetBottomLeft(wx, wy - ani_dizzy.Height(), ani_dizzy.Height());
        ani_dizzy.OnShow();
        ani_dizzy.OnMove();
    }
}
void ChipDale::Dead()
{
    Object** CanThrow = ChipDale::CanThrow;
    Life--;
    for (int i = 0; i < CanTakeNum; i++)
    {
        if (CanThrow[i] != NULL)continue;
        CanThrow[i] = new ChipDale_Dead(this);
        break;
    }
    ReleaseNowTakeObj();
    Alive = false;
    Health = 0;
}
void ChipDale::GetHurt()
{
    if (!Alive || IsGod)return;
    Hurt = true;
    if (UpSpeed >= 0)UpSpeed = 40;
    else UpSpeed += 40;

    SetState();
    Health--;
    if (Health <= 0)
    {
        //if((Partner==NULL||Partner->Life==0)&&Life>0)
        Dead();
    }
}
void ChipDale::Faint()
{
    if (!Alive)return;
    IsFaint = 1;
    SetState();
}

int ChipDale::SetNowTaken(int Value)
{
    if (!Alive)return 0;
    if (Value == -1)
        return NowTaken != 0;
    else
    {
        NowTaken = Value / E8;
        if (NowTaken == OrderSize + MonsterOrderSize + 0)
        {
            for (int i = 0; i < CanTakeNum; i++)
            {
                if (CanThrow[i] == NULL)continue;
                if (CanThrow[i]->ReturnObjValue() == Value)
                {
                    NowTakeObj = CanThrow[i];
                    NowTakeObj->RecoverObj(this);
                    break;
                }
                if (i + 1 == CanTakeNum)
                {
                    TRACE("SetNowTaken: No Find In CanThrow\n");
                    Sleep(100000);
                }
            }
        }
        else
        {
            switch (NowTaken)
            {
                case 14:
                    NowTaken = 3;
                    Maps->FillObstacle(7, (Value % E4 - 1)*ONEOBJX - Maps->ReturnNowX(), (Value % E8 / E4 - 1)*ONEOBJY - Maps->ReturnNowY());
                    break;
                case 15:
                    NowTaken = 3;
                    Maps->FillObstacle(8, (Value % E4 - 1)*ONEOBJX - Maps->ReturnNowX(), (Value % E8 / E4 - 1)*ONEOBJY - Maps->ReturnNowY());
                    break;
                case 16:
                    NowTaken = 3;
                    Maps->FillObstacle(4, (Value % E4 - 1)*ONEOBJX - Maps->ReturnNowX(), (Value % E8 / E4 - 1)*ONEOBJY - Maps->ReturnNowY());
                    break;
            }
            for (int i = 0; i < CanTakeNum; i++)
            {
                if (CanThrow[i] == NULL)
                {
                    switch (NowTaken)
                    {
                        case 3:
                            CanThrow[i] = new Craft(this);
                            break;
                        case 5:
                            CanThrow[i] = new Stone(this);
                            break;
                        case 6:
                            CanThrow[i] = new Apple(this);
                            break;
                        case BALL:
                            CanThrow[i] = new Ball(this);
                            break;
                    }
                    NowTakeObj = CanThrow[i];
                    break;
                }
                if (i + 1 == CanTakeNum)
                {
                    TRACE("SetNowTaken: No Find In CanThrow\n");
                    Sleep(100000);
                }
            }
        }
        return Value;
    }
}

void ChipDale::FixSelf_Onto_ObjectTop()
{
    int i = 0;
    if (!Alive)return;
    while (Maps->IfCollision(wx, wy + i, width, height, true))i--;
    wy += i;
    if (i < 0)TRACE("fix\n");
}

void ChipDale::SetState()
{
    //TRACE("1` %d\n",wy);
    //TRACE("1 Wy=  %d TAKEN %d ACT %d LR %d height=%d\n",wy,TAKEN_FLAG,NowAct,LRflag,height);
    int LastHeight = height;
    LastAct = NowAct;
    //if(!Alive)return;



    //設定LRflag
    if (!(LastAct == 4) || (Last_flag[1] && !DOWN_FLAG))
    {
        //蹲下不可以變換方向
        if (LEFT_FLAG && !RIGHT_FLAG) LRflag = 0;
        if (!LEFT_FLAG && RIGHT_FLAG) LRflag = 1;
    }
    //state切換

    if (LEFT_FLAG == RIGHT_FLAG)
    {
        NowAct = 0;    //左右同時按 或都不按 Act=站
        IsRun = false;
    }
    else
    {
        NowAct = 1;
        if (!DOWN_FLAG || (NowTaken == 6))IsRun = true;
    }
    if (IsJump)	NowAct = 2;
    if (DOWN_FLAG && NowAct != 2)
    {
        if (NowTaken != APPLE && NowTaken != ChipDale_taken)
        {
            NowAct = 4;
            IsRun = 0;
        }
    }
    if (A_FLAG && (TAKEN_FLAG))	NowAct = 3;
    if (Hurt)NowAct = 5;
    if (IsFaint)NowAct = 6;

    //重設wx wy
    if ((NowAct == 4/*||LastAct==4*/) && TAKEN_FLAG)
    {
        SetWxWy(wx, wy - 34 + height);
    }
    else
        SetWxWy(wx, wy - animation[IsDale][TAKEN_FLAG][NowAct][LRflag].Height() + height);
    //重設height

    //可以改了
    if (NowAct == 4 && TAKEN_FLAG && animation[IsDale][0][4][LRflag].GetCurrentBitmapNumber() == 0)height = 34;
    else SetWidthHeight(45, animation[IsDale][TAKEN_FLAG][NowAct][LRflag].Height());
    //TRACE("2 Wy=  %d TAKEN %d ACT %d LR %d height=%d\n",wy,TAKEN_FLAG,NowAct,LRflag,height);

    //蹲下放開需要調整2P位置
    if (LastAct == 4 && Partner != NULL)
    {
        if (CollisionChipDale(0, 0, 0))
            Partner->SetWxWy(0, LastHeight - height, true);
    }
}
void ChipDale::setFlag(bool value, bool up, bool down, bool left, bool right, bool A, bool B)
{
    if (LOCK)			return; //!Alive必須能控制 ChipDale_
    if (IsFaint && value)	return;
    if (up)				UP_FLAG = value;
    if (down)			DOWN_FLAG = value;
    if (left)			LEFT_FLAG = value;
    if (right)			RIGHT_FLAG = value;
    if (A)				A_FLAG = value;
    if (B)
    {
        if (!B_FLAG && time_jump == 0 && value && !DOWN_FLAG)
        {
            B_FLAG = true;
            IsJump = true;
        }
        if (!value)
            B_FLAG = false;
        if (DOWN_FLAG && value && !IsJump)
        {
            if ((TAKEN_FLAG && (!ani_Hide_freeze_jump || NowTaken == APPLE || NowTaken == ChipDale_taken)) || !TAKEN_FLAG)//HIDE動畫冷卻時間
                IsLessCollision = true;
        }
    }
    SetState();
    //設定LAST_FLAG
    for (int i = 0; i < 6; i++)
    {
        Last_flag[i] = flag[i];
    }
};
void ChipDale::GodMode()
{
    //GodMode不支援2P
    if (CGameStateRun::multiplayer != 2)
        IsGod = !IsGod;
}
void ChipDale::GodMove()
{
    int limitX = (MWIDTH + width) / 2;
    int limitY = (MHEIGHT + height) / 2;
    if (LEFT_FLAG && wx - SPEED * 2 >= 0)
    {

        SetWxWy(-SPEED * 2, 0, true);
        if (wx < limitX)Maps->MoveMap(4, 20);

    }
    else if (RIGHT_FLAG && wx + width + SPEED * 2 <= MWIDTH)
    {

        SetWxWy(SPEED * 2, 0, true);
        if (wx > limitX)Maps->MoveMap(8, 20);
    }
    if (UP_FLAG && wy - SPEED * 2 >= 0)
    {

        SetWxWy(0, -SPEED * 2, true);
        if (wy < limitY)
        {
            Maps->MoveMap(1, 10, 20);
        }

    }
    else if (DOWN_FLAG && wy + height + SPEED * 2 <= MHEIGHT)
    {

        SetWxWy(0, SPEED * 2, true);
        if (wy > limitY)
        {
            Maps->MoveMap(2, 10, 20);
        }
    }
};
void ChipDale::OnMove()
{
    int temp, tDerict, LRMargin = MapManage::LRMargin;
    if (!Alive)return;
    if (wy > MHEIGHT + 50)
    {
        Dead();
        return;
    }
    if (freeze)
    {
        //if(freeze_count<10){freeze_count++;}else {freeze_count=0;
        HOLD(3, freeze)
        {
            freeze = 0;
        };   //HOLD值   2~3為佳   (1為沒效果)
        //freeze_counter 不可以太大
    }
    TriggerObj(0);
    //運動
    if (!IsGod)
    {
        if ((!freeze && IsRun && !LRflag && !Hurt || Hurt && LRflag))MoveLeft(Maps);
        if (!freeze && IsRun && LRflag && !Hurt || Hurt && !LRflag)MoveRight(Maps);
        if (IsJump)Jump();

        if (FallingDown(Maps))
        {
            SetState();
        }
    }
    else
    {
        GodMove();
    }
    if (NowTaken == 6) Reduce_UP_VELOCITY = 10;//增加重力加速度
    //TRACE("%d wx=%d width=%d\n ",IsDale,wx,width);

    //丟的冷卻時間
    if (NowTaken < 0)NowTaken++;
    //丟實作
    if (NowAct == 3 && NowTakeObj != NULL)
    {
        tDerict = 1 * (UP_FLAG) + 4 * (!LRflag) + 8 * (LRflag);
        if (UP_FLAG && !IsJump)	UpSpeed += 20;//上丟會稍微往上跳
        NowTakeObj->Throw(tDerict);
        NowTakeObj = NULL;
        if (NowTaken == 6)Reduce_UP_VELOCITY = 0;//改成正常重力加速度
        NowTaken = -6;
    }
    //丟得僵直時間
    if (NowTaken == -3)
    {
        setFlag(0, 0, 0, 0, 0, 1, 0);    //決定丟動畫的延遲時間 NowTaken判斷可為-1 ~-5
        freeze = 1;
    }

    //跳轉蹲動畫切換變數設定
    if (ani_jump_count < TIMEFOR_ANI_JUMP)
    {
        if (ani_jump_count < 1)
        {
            //clock_t t=clock();
            CAudio::Instance()->Play(AUDIO_JUMP);
            //TRACE("CAudio %d\n",clock()-t);
        }
        if (ani_jump_count == 2)SetState();
        //TRACE("%d wy=%d height=%d \n",ani_jump_count,wy,height);
        ani_jump_count++;
    }

    //受傷及無敵的變數設定

    const int Hurt_time = 10;
    const int Hurt_Shine_time = 50;
    const int invincibility_time = 240;
    const int Hurt_start = invincibility_time - Hurt_Shine_time;
    const int Hurt_end = Hurt_start + Hurt_time;

    if (Invincible)Invincible++;
    if (Hurt && Invincible < Hurt_start)Invincible = Hurt_start;
    if (Invincible > invincibility_time)
    {
        SetInvincible(0);
        if (Partner == NULL || !Partner->ReturnInvincible())
            CAudio::Instance()->SetSpeedByID(AUDIO_1_1, 1000);
    }
    //受傷回復設定
    if (Invincible == Hurt_end)
    {
        Hurt = 0;
        SetState();
    };
    //暈眩變數設定
    if (IsFaint)
    {
        if (NowTakeObj != NULL)ReleaseNowTakeObj();
        //石頭暈眩修正
        if (IsFaint < 7) //暈眩圖片寬度不一，此條件防止連續修正
        {
            int i = 0, j = 0;
            while (Maps->IfCollision(wx, wy + i, width, height, true))i--;//先修正WY  如修正超過一個物件高則修正WX
            if (-i > 34)
            {
                i = 0;
                while (Maps->IfCollision(wx - LRflag * j + !LRflag * j, wy, width, height, true))j++; //根據人物面對方向往反方向修正
                if (j > 45)
                {
                    j = 0;
                    //反方向修正超過一個物件寬則朝相同方向修正
                    while (Maps->IfCollision(wx - LRflag * j + !LRflag * j, wy, width, height, true))j--;
                    if (-j > 45)j = 0; //三種方向修正都超過一個物件長寬則不修正(卡在石頭裡)
                }
                wx -= (LRflag * j - !LRflag * j);
            }
            wy += i;
        }
        IsFaint++;
        if (IsFaint > 50)
        {
            IsFaint = 0;
            SetState();
        }
        setFlag(0, 1, 1, 1, 1, 1, 1);
    }
    //流汗動畫顯示變數設定
    if (NowTaken == APPLE)ani_sweat_count++;
    else ani_sweat_count = 0;
    if (ani_sweat_count > 5/*週期*/)ani_sweat_count = 1;	 //更改週期亦需改變每顆汗流下的位移(onshow)
    if (ani_sweat_count == 1)
    {
        ani_sweat_x = wx + Maps->ReturnNowX();
        ani_sweat_wy = wy;
    }
    //TRACE("count %d\n",ani_sweat_count);

    //TRACE("Act=%d %d %d %d\n",NowAct,wx,wy,Invincible);
    //2P移動鎖螢幕
    canMoveMapX = true;
    canMoveMapY = true;
    temp = Maps->GetRoute();
    if ((temp & 8) && wx <= LRMargin || (temp & 4) && (wx >= MWIDTH - width - LRMargin))
        canMoveMapX = false;
    if ((temp & 2) && wy <= Jump_Fix || (temp & 1) && wy >= (MHEIGHT - 2 * height - Jump_Fix))
        canMoveMapY = false;
}
void ChipDale::InitialWidthHeight()
{
    width = animation[IsDale][0][0][0].Width();
    height = animation[IsDale][0][0][0].Height();
}
int ChipDale::CollisionChipDale(int Direct, int passSpeed, int mode)
{
    int temp = 0, ObjFix = 0;
    const int LRspace = 5;
    if (Partner == NULL || !Partner->Alive)return 0;
    passSpeed = abs(passSpeed);

    if (Partner->ReturnNowTakeObj() != NULL && !Partner->ReturnHideComplete())
    {
        ObjFix = Partner->ReturnNowTakeObj()->ReturnHeight();
        wy += ObjFix;
    }
    if (Direct == 0)		temp = Partner->IfCollision(wx + LRspace, wy, width - 2 * LRspace, height);
    else if (Direct == 1)	temp = Partner->IfCollision(wx + LRspace, wy - passSpeed, width - 2 * LRspace, passSpeed);
    else if (Direct == 2)	temp = Partner->IfCollision(wx + LRspace, wy + height, width - 2 * LRspace, passSpeed);
    else if (Direct == 4)	temp = Partner->IfCollision(wx + LRspace - passSpeed, wy, passSpeed, height);
    else if (Direct == 8)	temp = Partner->IfCollision(wx - LRspace + width, wy, passSpeed, height);

    if (ObjFix != 0)
        wy -= ObjFix;

    if (temp > 0)
    {
        if (!Partner->IfCollision(Direct, passSpeed))
        {
            if (mode)
            {
                if (Direct == 1)	Partner->SetWxWy(0, -passSpeed, true);
                else if (Direct == 4)	Partner->SetWxWy(-passSpeed, 0, true);
                else if (Direct == 8)	Partner->SetWxWy(passSpeed, 0, true);
            }
            //return 0;
            return 1;//發生碰撞，推擠2P
        }
        return 2;//發生碰撞，不能推擠2P
    }
    return 0;//無發生碰撞
}
void ChipDale::TriggerObj(int Derict)
{
    int temp = 0, fixX = 0, fixY = 0, ox, oy;
    if (!Alive)return;
    if (Maps->MoveMap(0) & 1)fixY = Jump_Fix;
    if (Maps->MoveMap(0) & 4)fixX = SPEED;
    if (Maps->MoveMap(0) & 8)fixX = -SPEED;
    while (true)
    {
        if (Derict == 0)	temp = Maps->IfCollision(wx + fixX, wy + fixY, width, height, true, true);
        else if (Derict == 1)	temp = Maps->IfCollision(wx + fixX, wy + fixY + height, width, abs(UpSpeed), false, true, true);
        else if (Derict == 2)	temp = Maps->IfCollision(wx + fixX, wy + fixY - UpSpeed, width, abs(UpSpeed), true, true);
        else if (Derict == 4)	temp = Maps->IfCollision(wx + width + fixX, wy + fixY, ONEOBJX, height, true, true);
        else if (Derict == 3)	temp = Maps->IfCollision(wx - ONEOBJX + fixX, wy + fixY, ONEOBJX, height, true, true);
        switch (temp / E8)
        {
            case FLOWER:
                Score_Flower++;
                Maps->ClearObstacle(temp);
                break;
            case STAR:
                Score_Star++;
                Maps->ClearObstacle(temp);
                break;
            case 8:
                Health = 3;
                Maps->ClearObstacle(temp);
                break;
            case 9:
                CAudio::Instance()->SetSpeedByID(AUDIO_1_1, 1500);
                Maps->ClearObstacle(temp);
                for (int i = 0; i < CanTakeNum; i++)
                {
                    if (CanThrow[i] == NULL)
                    {
                        CanThrow[i] = new Explosion(temp % E4 - 1, temp % E8 / E4 - 1, 9, this);
                        //CanThrow[i]->ReFixXY(Maps);
                        TRACE("trigger(%d): %d %d\n", i, temp % E8 / E4 - 1, temp % E4 - 1);
                        break;
                    }
                    if (i + 1 == CanTakeNum)
                    {
                        TRACE("SetNowTaken: No Find In CanThrow\n");
                        Sleep(100000);
                    }
                }
                break;
            case 10://Cheese Box
                Maps->ClearObstacle(temp);
                for (int i = 0; i < CanTakeNum; i++)
                {
                    if (CanThrow[i] == NULL)
                    {
                        CanThrow[i] = new Explosion(temp % E4 - 1, temp % E8 / E4 - 1, CHEESE, this);
                        //CanThrow[i]->ReFixXY(Maps);
                        TRACE("trigger(%d): %d %d\n", i, temp % E8 / E4 - 1, temp % E4 - 1);
                        break;
                    }
                    if (i + 1 == CanTakeNum)
                    {
                        TRACE("SetNowTaken: No Find In CanThrow\n");
                        Sleep(100000);
                    }
                }
                break;
            case 11:
                Maps->ClearObstacle(temp);
                break;
            case 12:
                Maps->ClearObstacle(temp);
                break;
            case DOOR:
                Lock();
                if (Partner != NULL)Partner->Lock();
                ToolCDC::Fadeout();
                ox = temp % E4 - 1;
                oy = temp % E8 / E4 - 1;
                Maps->Teleport = Maps->GetRoute(ox * ONEOBJX / MWIDTH, oy * ONEOBJY / MHEIGHT) >> 7;
                Lock(0);
                if (Partner != NULL)Partner->Lock(0);
                return;
            case 17:
                Maps->ClearObstacle(temp);
                for (int i = 0; i < CanTakeNum; i++)
                {
                    if (CanThrow[i] == NULL)
                    {
                        CanThrow[i] = new Explosion(temp % E4 - 1, temp % E8 / E4 - 1, 7, this);
                        CanThrow[i]->ReFixXY(Maps);
                        TRACE("trigger(%d): %d %d\n", i, temp % E8 / E4 - 1, temp % E4 - 1);
                        break;
                    }
                    if (i + 1 == CanTakeNum)
                    {
                        TRACE("SetNowTaken: No Find In CanThrow\n");
                        Sleep(100000);
                    }
                }
                return;
            default:
                return;
        }
    }
}
void ChipDale::ReleaseNowTakeObj()
{
    //TRACE("NowTaken: %d\n",NowTaken);
    if (NowTakeObj == NULL || !Alive) return;
    if (NowTaken == STONE || NowTaken == MonsterOrderSize + OrderSize + 0)
        NowTakeObj->Throw(0);
    else
        NowTakeObj->Throw(1);
    NowTakeObj = NULL;
    NowTaken = -6;
}

}