#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "gamelib.h"
#include "mygame.h"
#include "mapmanage.h"
#include "instruction.h"
#include "chipdale.h"
#include "object.h"
#include "monster.h"

namespace game_framework
{
/****************************************************************/
//MapManage實作													*/
//																*/
/****************************************************************/
#define SET_INITIAL(Level,LOCATE,INITIAL_Y) limit[(Level-1)*MAX_YN+LOCATE/10][(Level-1)*MAX_YN+LOCATE%MAX_XN-1] += ((INITIAL_Y*DesSize)<<7);
#define DELIVER(Level,LOCATE,DES) limit[(Level-1)*MAX_YN+LOCATE/10][(Level-1)*MAX_YN+LOCATE%MAX_XN-1] += (DES<<7);
int MapManage::isMoveMap = 0;
int MapManage::Teleport = 0;
int MapManage::LRMargin = 0;
MapManage::MapManage(int WhichLevel)
{
    LevelLoading(WhichLevel);
}
void MapManage::LevelLoading(int WhichLevel, bool NotResetAll)
{
    char temp[100];
    int count = 0, OnePercent = (MAX_YN * MAX_XN * 2 + OrderSize + MonsterOrderSize + 1) / 100;
    FILE* file;
    static int limit[MAX_YN * 2][MAX_XN] =	  //第一關
    {
        {15, 0, 15 + 64, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 8, 8, 8, 8, 8, 8, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 9, 8, 8, 8, 8, 8, 8, 0, 0, 0},
        { 1, 0, 0, 0, 0, 0 , 0 , 0, 0, 0},
        { 5, 4, 4, 4, 4, 5, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        { 8 + 16, 8, 8, 8, 8, 9, 0, 0, 0, 0},
        //第二關...(待開發)
    }, IfSetLimit = 0;

    //initial_Y 是要輸入 地板的左上角y位置 不是人物的左上角y位置
    if(!IfSetLimit) //防止跑兩次以上 因為 limit 是 static!!!! (有修正過)
    {
        //第一關
        SET_INITIAL(1, 91, 135);
        SET_INITIAL(1, 21, 326);
        SET_INITIAL(1, 1, 395);
        SET_INITIAL(1, 3, 395);
        DELIVER(1, 47, 21);
        DELIVER(1, 26, 1);
        DELIVER(1, 1, 3);

        //第二關...(待開發)




        //關卡設定結束
        IfSetLimit = 1;
    }


    TRACE("%d\n", (MAX_YN * MAX_XN * 2 + OrderSize + MonsterOrderSize + 1));
    if(!NotResetAll)
    {
        x = 0;
        y = 0;
        ToolCDC::ShowProgressBar(WhichLevel, count / OnePercent);
        for(int i = 0; i < MAX_XN; i++)
        {
            for(int j = 0; j < MAX_YN; j++)
            {
                Route[j][i] = limit[(WhichLevel - 1) * MAX_YN + j][i];
                if(Route[j][i] > 0)
                {
                    sprintf(temp, "Bitmaps/0x%d/%d.bmp", WhichLevel, j * 10 + i + 1);
                    MapSP[j][i].LoadBitmap(temp);
                    if(Route[j][i] & 16)
                    {
                        x = i * MWIDTH;
                        y = j * MHEIGHT;
                        recordx = x;
                        recordy = y;
                        Teleport = Route[j][i];
                    }
                }
                count++;
                ToolCDC::ShowProgressBar(WhichLevel, count / OnePercent);
            }
        }
    }

    /* NotResetAll 有跟無差在底下SET */
    sprintf(temp, "ObjInfo/0x%d.ChipDale", WhichLevel);
    file  = fopen(temp, "rb");
    for(int j = 0; j < MAX_OY; j++)for(int i = 0; i < MAX_OX; i++)MapObjXY[j][i] = 0;
    if(file)
    {
        for(int j = 0; j < MAX_YN; j++)
        {
            for(int i = 0; i < MAX_XN; i++)
            {
                if(Route[j][i])
                {
                    /*for(int k=0;k<MAX_OY/MAX_YN;k++){
                    	for(int l=0;l<MAX_OX/MAX_XN;l++){
                    		fscanf(file,"%d",&MapObjXY[j*MAX_OY/MAX_YN+k][i*MAX_OX/MAX_XN+l]);
                    	}
                    }*/
                    for(int k = 0; k < MAX_OY / MAX_YN; k++)
                    {
                        fread(&MapObjXY[j * MAX_OY / MAX_YN + k][i * MAX_OX / MAX_XN], MAX_OX / MAX_XN, sizeof(int), file);
                    }
                }
                if(!NotResetAll)
                {
                    count++;
                    ToolCDC::ShowProgressBar(WhichLevel, count / OnePercent);
                }
            }
        }
        fclose(file);
    }
    else
    {
        SaveObj(WhichLevel);
        if(!NotResetAll)
        {
            count += MAX_YN * MAX_XN;
            ToolCDC::ShowProgressBar(WhichLevel, count / OnePercent);
        }
    }


    if(!NotResetAll)
    {
        for(int i = 0; i < OrderSize; i++)
        {
            sprintf(temp, "Bitmaps/object/%d.bmp", i);
            frame_obstacle[i].LoadBitmapA(temp, PURPLE);
            count++;
            ToolCDC::ShowProgressBar(WhichLevel, count / OnePercent);
        }
        for(int i = -1; i >= -MonsterOrderSize; i--)
        {
            sprintf(temp, "Bitmaps/monster/%d.bmp", -i);
            frame_obstacle[-i + OrderSize - 1].LoadBitmapA(temp, PURPLE);
            count++;
            ToolCDC::ShowProgressBar(WhichLevel, count / OnePercent);
        }
        frame_obstacle[OrderSize + MonsterOrderSize + 0].LoadBitmapA("Bitmaps/object/StoneUnVisible.bmp", PURPLE);
        count++;
        ToolCDC::ShowProgressBar(WhichLevel, 100);
    }
}
void MapManage::OnShow()
{
    int gx = x / MWIDTH, gy = y / MHEIGHT; //地圖左上角轉換成螢幕格座標
    int ox = x / ONEOBJX, oy = y / ONEOBJY;
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            if(gx + i >= 0 && gx + i < MAX_XN && gy + j >= 0 && gy + j < MAX_YN && Route[gy + j][gx + i] > 0)
            {
                MapSP[gy + j][gx + i].SetTopLeft((gx + i)*MWIDTH - x, (gy + j)*MHEIGHT - y);
                MapSP[gy + j][gx + i].ShowBitmap();
            }
        }
    }
}
void MapManage::OnShowObject()
{
    int gx = x / MWIDTH, gy = y / MHEIGHT; //地圖左上角轉換成螢幕格座標
    int ox = x / ONEOBJX, oy = y / ONEOBJY;
    for(int j = -(WinShowBuffer / ONEOBJY); j < MHEIGHT / ONEOBJY; j++)
    {
        if(oy + j < 0)j = -oy;
        for(int i = -(WinShowBuffer / ONEOBJX); i < MWIDTH / ONEOBJX; i++)
        {
            if(ox + i < 0)i = -ox;
            if(MapObjXY[oy + j][ox + i] >= OrderSize || MapObjXY[oy + j][ox + i] <= 2)
                continue;
            frame_obstacle[MapObjXY[oy + j][ox + i]].SetTopLeft( (ox + i)*ONEOBJX - x , (oy + j)*ONEOBJY - y );
            frame_obstacle[MapObjXY[oy + j][ox + i]].ShowBitmap();
        }
    }
}
void MapManage::ShowObstacle(int i, int j)
{

    int ox = (x - WinShowBuffer) / ONEOBJX, oy = (y - WinShowBuffer) / ONEOBJY, temp = 0;

    if(ox + i >= 0 && ox + i < MAX_OX && oy + j >= 0 && oy + j < MAX_OY && MapObjXY[oy + j][ox + i] != 0)
    {
        if(MapObjXY[oy + j][ox + i] >= OrderSize || MapObjXY[oy + j][ox + i] < -MonsterOrderSize)
            return;
        temp = (MapObjXY[oy + j][ox + i] < 0) * (-MapObjXY[oy + j][ox + i] + OrderSize - 1) + (MapObjXY[oy + j][ox + i] >= 0) * (MapObjXY[oy + j][ox + i]);
        //TRACE("ShowObstacle %d\n",temp);
        frame_obstacle[ temp ].SetTopLeft((ox + i)*ONEOBJX - x, (oy + j)*ONEOBJY - y);
        frame_obstacle[ temp ].ShowBitmap();
    }
}
int MapManage::MoveMap(int move, int MoveOneX, int MoveOneY)
{
    if((move & 4) || (move & 8))
    {
        LRMargin = MoveOneX;
    }
    if((move & 1) || (move & 2))
    {
        CFrame::Jump_Fix = MoveOneY;
    }
    if( (move & 1) && !(isMoveMap & 1))
    {
        if(x % MWIDTH == 0 && ( (Route[y / MHEIGHT][x / MWIDTH] & 1) || ((Route[y / MHEIGHT][x / MWIDTH] & 2) && (isMoveMap & 16))))
        {
            y -= MoveOneY;
            isMoveMap |= 1;
            if(y < 0 || Route[y / MHEIGHT][x / MWIDTH] == 0)
            {
                y += MoveOneY;
                isMoveMap &= 30;
            }
        }
        else if(x % MWIDTH <= SPEED && (Route[y / MHEIGHT][x / MWIDTH] & 1))
        {
            y -= MoveOneY;
            x -= x % MWIDTH;
            isMoveMap |= (1 + 4);
            LRMargin = x % MWIDTH;
            if(y < 0 || Route[y / MHEIGHT][x / MWIDTH] == 0)
            {
                y += MoveOneY;
                isMoveMap &= 30;
            }
        }
        else if((x + SPEED) % MWIDTH <= SPEED && (Route[y / MHEIGHT][(x + SPEED) / MWIDTH] & 1))
        {
            y -= MoveOneY;
            x += (x + SPEED) / MWIDTH * MWIDTH - x;
            isMoveMap |= (1 + 8);
            LRMargin = (x + SPEED) / MWIDTH * MWIDTH - x;
            if(y < 0 || Route[y / MHEIGHT][x / MWIDTH] == 0)
            {
                y += MoveOneY;
                isMoveMap &= 30;
            }
        }
    }

    if( (move & 2) && !(isMoveMap & 2))
    {
        if(x % MWIDTH == 0 && ( (Route[y / MHEIGHT][x / MWIDTH] & 2) || ((Route[y / MHEIGHT][x / MWIDTH] & 1) && (isMoveMap & 16))))
        {
            y += MoveOneY;
            isMoveMap |= 2;
            if(y + MHEIGHT - 1 > MHEIGHT * MAX_YN || Route[(y + MHEIGHT - 1) / MHEIGHT][x / MWIDTH] == 0)
            {
                y -= MoveOneY;
                isMoveMap &= 29;
            }
        }
    }


    if( (move & 4) && !(isMoveMap & 4))
    {
        if(y % MHEIGHT == 0 && ( (Route[y / MHEIGHT][x / MWIDTH] & 4) || ((Route[y / MHEIGHT][x / MWIDTH] & 8) && (isMoveMap & 16))))
        {
            x -= MoveOneX;
            isMoveMap |= 4;
            if(x < 0 || Route[y / MHEIGHT][x / MWIDTH] == 0)
            {
                x += MoveOneX;
                isMoveMap &= 27;
            }
        }
    }

    if( (move & 8) && !(isMoveMap & 8))
    {
        if(y % MHEIGHT == 0 && ( (Route[y / MHEIGHT][x / MWIDTH] & 8) || ((Route[y / MHEIGHT][x / MWIDTH] & 4) && (isMoveMap & 16))))
        {
            x += MoveOneX;
            isMoveMap |= 8;
            if(x + MWIDTH - 1 > MWIDTH * MAX_XN || Route[y / MHEIGHT][(x + MWIDTH - 1) / MWIDTH] == 0)
            {
                x -= MoveOneX;
                isMoveMap &= 23;
            }
        }
    }
    //TRACE("x: %d , y: %d\n",x,y);
    return isMoveMap;
}
void MapManage::SetMapXY(int setX, int setY)
{
    x = setX;
    y = setY;
}
void MapManage::Set_toRecord()
{
    Object** AllThrow = ChipDale::CanThrow;
    LevelLoading(CGameStateRun::NowLevel, true);
    SetMapXY(recordx, recordy);
    SetObstacle();
    SetMonster(ChipDale::AllMonster);
    for(int i = 0; i < CanTakeNum; i++)
    {
        if(AllThrow[i] != NULL)
            AllThrow[i]->SetNowAct(0);
    }
}
void MapManage::SetObj(int value, int wx, int wy)
{
    MapObjXY[(wy + y) / ONEOBJY][(wx + x) / ONEOBJX] = value;
}
void MapManage::SaveObj(int WhichLevel)
{
    char temp[100];
    FILE* file;
    sprintf(temp, "ObjInfo/0x%d.ChipDale", WhichLevel);
    file  = fopen(temp, "wb");
    for(int j = 0; j < MAX_YN; j++)
    {
        for(int i = 0; i < MAX_XN; i++)
        {
            if(Route[j][i])
            {
                /*for(int k=0;k<MAX_OY/MAX_YN;k++){
                	for(int l=0;l<MAX_OX/MAX_XN;l++){
                		//if(MapObjXY[j*MAX_OY/MAX_YN+k][i*MAX_OX/MAX_XN+l]<0)
                			//continue;
                		fprintf(file,"%d ",MapObjXY[j*MAX_OY/MAX_YN+k][i*MAX_OX/MAX_XN+l]);
                	}
                }*/
                for(int k = 0; k < MAX_OY / MAX_YN; k++)
                {
                    fwrite(&MapObjXY[j * MAX_OY / MAX_YN + k][i * MAX_OX / MAX_XN], MAX_OX / MAX_XN, sizeof(int), file);
                }
            }
        }
    }
    fclose(file);
}
void MapManage::ClearisMoveMap(bool WatchMode)
{
    if(WatchMode)isMoveMap = 16;
    else isMoveMap = 0;
}
int MapManage::IfCollision(int wx, int wy, int PicWidth, int PicHeight, bool ignore_obj_2, bool ignore_eat, bool findNext, bool Reverse)
{
    int start_ox, start_oy, end_ox, end_oy, tj;
    if(wy < 0)
    {
        PicHeight += wy;
        wy = 0;
    }
    start_ox = (x + wx) / ONEOBJX;
    start_oy = (y + wy) / ONEOBJY;
    if(start_ox < 0)start_ox = 0;
    if(start_oy < 0)start_oy = 0;
    end_ox = (x + wx + PicWidth) / ONEOBJX;
    end_oy = (y + wy + PicHeight) / ONEOBJY;
    if(end_ox >= MAX_OX)end_ox = MAX_OX - 1;
    if(end_oy >= MAX_OY)end_oy = MAX_OY - 1;

    for(int j = end_oy - 1; j >= start_oy; j--)
    {
        if(Reverse)	tj = (start_oy + end_oy - 1) - j;
        else		tj = j;
        for(int i = start_ox; i < end_ox; i++)
        {
            //要不要忽略吃的東西 (要true 不要false)
            if(!ignore_eat && ( Obstacle[tj][i] / E8 == 4 || Obstacle[tj][i] / E8 >= 7 && Obstacle[tj][i] / E8 <= 13 || Obstacle[tj][i] / E8 == 17) ) //花
                continue;
            //要不要忽略2地板 (要true 不要false)
            if(ignore_obj_2 && Obstacle[tj][i] == 2)
                continue;
            if(Obstacle[tj][i] > 0)
            {
                if(Obstacle[tj][i] != 1 && Obstacle[tj][i] != 2)
                    if(findNext == true)
                    {
                        j = (Obstacle[tj][i] % E8) / E4 - 1;
                        findNext = false;
                        break;
                    }
                return Obstacle[tj][i];
            }
        }
    }

    return 0;
}
void MapManage::SetObstacle()
{
    for(int i = 0; i < MAX_OX; i++)
        for(int j = 0; j < MAX_OY; j++)
            Obstacle[j][i] = 0;

    int ox_end, oy_end, ox_start, oy_start;

    for(int i = 0; i < MAX_OX; i++)
    {
        for(int j = 0; j < MAX_OY; j++)
        {
            if(MapObjXY[j][i] < OrderSize && MapObjXY[j][i] > 0)
            {
                ox_start = i;
                oy_start = j;
                switch(MapObjXY[j][i])
                {
                    case 1:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    case 17:
                    case 18:
                    case BALL:
                        ox_end = ox_start + frame_obstacle[MapObjXY[j][i]].Width() / ONEOBJX;
                        oy_end = oy_start + frame_obstacle[MapObjXY[j][i]].Height() / ONEOBJY;
                        break;
                    case 9:
                    case 10:
                    case 11:
                        ox_end = ox_start + frame_obstacle[MapObjXY[j][i]].Width() / ONEOBJX;
                        oy_end = oy_start + frame_obstacle[MapObjXY[j][i]].Height() / ONEOBJY;
                        oy_start += 5;
                        ox_start += 2;
                        ox_end -= 2;
                        break;
                    case 2:
                        ox_end = ox_start + frame_obstacle[MapObjXY[j][i]].Width() / ONEOBJX;
                        oy_end = oy_start + 1;
                        break;
                    default:
                        ox_end = ox_start;
                        oy_end = oy_start;
                        break;
                }
                for(int l = oy_start; l < oy_end; l++)
                {
                    for(int k = ox_start; k < ox_end; k++)
                    {
                        if(MapObjXY[j][i] == 1 || MapObjXY[j][i] == 2)
                            Obstacle[l][k] = MapObjXY[j][i];
                        else
                            Obstacle[l][k] = MapObjXY[j][i] * E8 + (j + 1) * E4 + (i + 1);
                    }
                }
                switch(MapObjXY[j][i])
                {
                    case 9:
                    case 10:
                    case 11:
                        MapObjXY[j][i] = 18;
                        break;
                    case 14:
                    case 15:
                    case 16:
                        MapObjXY[j][i] = 3;
                        break;
                    case 13:
                    case 17:
                        MapObjXY[j][i] = 0;
                        break;
                }
            }
        }
    }
}
void MapManage::SetMonster(Monster* monster[])
{
    int lengthO, k = 0;
    for(int i = k; i < ONE_LEVEL_MONSTER_NUM; i++)
    {
        if(monster[i] != NULL)
        {
            delete(monster[i]);
        }
    }

    for(int j = 0; j < MAX_OY; j++)
    {
        for(int i = 0; i < MAX_OX; i++)
        {
            switch(MapObjXY[j][i])
            {
                case -1:
                    monster[k] = new MachineDog(this, i, j);
                    k++;
                    break;
                case -2:
                    monster[k] = new Cactus(this, i, j);
                    k++;
                    break;
                case -3:
                    for(lengthO = i + 1; lengthO < i + 50; lengthO++)
                    {
                        if(MapObjXY[j][lengthO] == -3)
                        {
                            MapObjXY[j][lengthO] = 0;
                            break;
                        }
                    }
                    monster[k] = new Wire(this, i, j, lengthO - i);
                    k++;
                    break;
                case -4:
                    monster[k] = new Mouse(this, i, j);
                    k++;
                    break;
                case -5:
                    monster[k] = new Wasp(this, i, j);
                    k++;
                    break;
                case -6:
                    monster[k] = new Centipede(this, i, j);
                    k++;
                    break;
            }
        }
    }
    if(k >= ONE_LEVEL_MONSTER_NUM)
    {
        TRACE("SetMonster Overflow ( %d )!!!\n", k);
        Sleep(10000);
    }
    else
    {
        for(int i = k; i < ONE_LEVEL_MONSTER_NUM; i++)
            monster[i] = NULL;
    }
}
void MapManage::ClearObstacle(int Value)
{
    int ox_start, oy_start;
    int ox_end, oy_end;
    int ObjetID;
    ox_start = Value % E4 - 1;
    Value /= E4;
    oy_start = Value % E4 - 1;
    Value /= E4;
    ObjetID = Value;
    if(Value != 0)
    {
        ox_end = ox_start + frame_obstacle[ObjetID].Width() / ONEOBJX;
        oy_end = oy_start + frame_obstacle[ObjetID].Height() / ONEOBJY;
        for(int k = ox_start; k < ox_end; k++)
            for(int l = oy_start; l < oy_end; l++)
                Obstacle[l][k] = 0;
        MapObjXY[oy_start][ox_start] = 0;
        if(Value == 5)
            MapObjXY[oy_start + 1][ox_start] = 0;
    }
}
int MapManage::FillObstacle(int Value, int setWx, int setWy, bool Visible)
{
    int minusX = 0, minusY = 0;
    int ox_start, oy_start;
    int ox_end, oy_end;
    int ObjetID;
    ox_start = (setWx + x) / ONEOBJX;
    oy_start = (setWy + y) / ONEOBJY;
    //TRACE("Fill ox %d , oy %d\n",ox_start,oy_start);
    ObjetID = Value;
    if(Value != 0)
    {
        ox_end = ox_start + frame_obstacle[ObjetID].Width() / ONEOBJX;
        oy_end = oy_start + frame_obstacle[ObjetID].Height() / ONEOBJY;
        switch(ObjetID)
        {
            case 4:
            case 7:
            case 8:
                minusX = 1;
                minusY = 1;
                break;
        }
        for(int l = oy_start + minusY; l < oy_end - minusY; l++)
            for(int k = ox_start + minusX; k < ox_end - minusX; k++)
                Obstacle[l][k] = Value * E8 + (oy_start + 1) * E4 + (ox_start + 1);
        if(Visible)
            MapObjXY[oy_start][ox_start] = Value;
    }
    return Value * E8 + (oy_start + 1) * E4 + (ox_start + 1);
}
}