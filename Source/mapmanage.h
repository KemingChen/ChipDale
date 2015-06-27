namespace game_framework
{
class MapManage
{
    public:
        MapManage(int WhichLevel);
        void OnShow();
        void OnShowObject();
        void LevelLoading(int WhichLevel, bool NotResetAll = false);		//設定關卡  NotResetAll是否重設此關所有變數
        int MoveMap(int direct, int MoveOneX = SPEED, int MoveOneY = SPEED); //move 1,2,4,8 ...
        void SetMapXY(int, int);					//設定 map 顯示位置
        void SetObstacle();						//設定障礙物資料
        void SetMonster(Monster* monster[]);	//設定怪物資料
        void SetObj(int value, int wx, int wy);	//wx,wy	  螢幕座標
        void SetRecord()
        {
            recordx = x;    //設定目前地圖左上角為紀錄點
            recordy = y;
        }
        int ReturnNowX()
        {
            return x;   //傳回螢幕左上角在地圖的x位置
        }
        int ReturnNowY()
        {
            return y;   //傳回螢幕左上角在地圖的y位置
        }
        int ReturnOBJ(int ox, int oy)
        {
            return MapObjXY[oy][ox];   //回傳 Obj 的值
        }
        void SaveObj(int WhichLevel);			//儲存 Obj 資訊
        void ShowObstacle(int i, int j);			//顯示指定的物件  ps: 給edit用的
        void ClearisMoveMap(bool flag);			//清除 isMoveMap 紀錄
        void Set_toRecord();
        int IfCollision(int wx, int wy, int PicWidth, int PicHeight, bool ignore_obj_2 = false, bool ignore_eat = false, bool findNext = false, bool Reverse = false);	//計算是否碰撞
        int  GetRoute()
        {
            return Route[y / MHEIGHT][x / MWIDTH];
        }
        int  GetRoute(int rx, int ry)
        {
            return Route[ry][rx];
        }
        void ClearObstacle(int Value);			//把目標物件值告訴它 讓他直接去刪除碰撞機制
        int FillObstacle(int Value, int setWx, int setWy, bool Visible = true);	//把目標物件值告訴它 讓他直接去增加碰撞機制
        static int isMoveMap;					//地圖移動方向     1,2,4,8  上下左右
        static int Teleport;
        static int LRMargin;
        friend class ChipDale;


    private:
        CMovingBitmap frame_obstacle[OrderSize + MonsterOrderSize + UnVisibleOrderSize];
    protected:
        int Route[MAX_YN][MAX_XN];				//地圖路線移動方向 1,2,4,8 方向 16 出生點 32~  64 ~boss
        int MapObjXY[MAX_OY][MAX_OX];			//物件座標~顯示讀寫檔用
        int Obstacle[MAX_OY][MAX_OX];			//障礙物   判斷碰撞用		//第一部分：種類  第二：左上Y 第三 左上X
        CMovingBitmap MapSP[MAX_YN][MAX_XN];
        int x, y;								//目前螢幕左上角在地圖的座標
        int recordx, recordy;					//死亡起始位置
};
}