namespace game_framework
{

class Object : public AI_Instructions
{
    public:
        Object();
        virtual ~Object()
        {
            ;
        }
        virtual void OnShow(MapManage* map)
        {
            ;
        }
        virtual void OnMove(MapManage* map)
        {
            ;
        }
        virtual void Throw(int setDirect)
        {
            ;
        }
        int ReturnObjValue()
        {
            return ThisObjValue;
        }
        int ReturnNowAct()
        {
            return NowAct;
        }
        void SetNowAct(int setNowAct)
        {
            NowAct = setNowAct;
        }
        void RecoverObj(ChipDale* player);				//重新拾獲時需呼叫
        void CollisionMonster(Monster** monster);
        void CollisionChipDale(ChipDale* player);
    protected:
        int CanAttackMode;								// 攻擊對象 0 任何人 1 除了主人(預設值),2 None 3 只攻擊怪物
        int Direct, LRflag, ThisObjValue, NowAct;			//ThisObjValue用來記錄刪除obstacle的編號
        //NowAct =0 停屍間 1 拿在手上 2運動狀態 3放在地上 4打完怪物
        ChipDale* Owener;
};

class ChipDale_Dead: public Object
{
    public:
        ChipDale_Dead(ChipDale* chip);
        ~ChipDale_Dead();
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);

        friend class ChipDale_Resurrect;
    private:
        int isDale;
        int Shine_Count;
        static CMovingBitmap frame_pic[2][2];
        int Wait_A_Minute;
};

class ChipDale_Resurrect: public Object
{
    public:
        ChipDale_Resurrect(ChipDale_Dead* chip);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
    private:
        int isDale;
        int timeCount;
        static CMovingBitmap frame_pic[2][2];
};

class ChipDale_Taken: public Object
{
    public:
        ChipDale_Taken(ChipDale* chip);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
        void Throw(int setDirect);
    private:
        int isDale;
        static CAnimation animation[2][2];
};

class Star : public Object
{
    public:
        Star(int setWx, int setWy);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
    private:
        int lastUpSpeed;
        static CMovingBitmap frame_pic;
};

class Cheese : public Object
{
    public:
        Cheese(int setWx, int setWy);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
    private:
        int lastUpSpeed;
        static CMovingBitmap frame_pic;
};

class Explosion : public Object
{
    public:
        Explosion(int setOx, int setOy, int setChangeWhat, ChipDale* player);			//ChangeWhat 填代碼 (跟顯示物件代碼一樣)
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
    private:
        int ChangeWhat;
        bool IfNeedReFix;
        static CAnimation frame_pic;
};

class Angel : public Object
{
    public:
        Angel(int setWx, int setWy, ChipDale* player);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
    private:
        int NowAttack;
        bool OutLRflag;
        const int TheBeeMaxSpeed;
        static CAnimation frame_pic[2];
        static Monster** monster;
};

class Apple : public Object
{
    public:
        Apple(ChipDale* player);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
        void Throw(int setDirect);
    private:
        static CMovingBitmap frame_pic[2];
};

class Ball : public Object
{
    public:
        Ball(ChipDale* player);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
        void Throw(int setDirect);
    private:
        int Rebound_times;
        static CMovingBitmap frame_pic[2];
};
class Craft : public Object
{
    public:
        Craft(ChipDale* player);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
        void Throw(int setDirect);
        void CollisionReact(int setDirect, CFrame* which);
        ~Craft()
        {
            TRACE("Craft destuctor run\n");
        }
    private:
        static CMovingBitmap frame_pic[2];
};

class Stone : public Object
{
    public:
        Stone(ChipDale* player);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
        void Throw(int setDirect);
        void CollisionReact(int setDirect, CFrame* which);
        bool MoveRight(MapManage* map);
        bool MoveLeft(MapManage* map);
        void ReBound(MapManage* map);
    private:
        bool HideThrow;
        static CMovingBitmap frame_pic[2];
        int ReboundLR;									// 0 無反彈 1 反彈右邊 -1反彈左邊
        int ReboundSpeed;
};


class Greedy : public Object
{
    public:
        Greedy(int setWx, int setWy, Object* setRemoveObj);
        static void Loading();
        void OnShow(MapManage* map);
        void OnMove(MapManage* map);
    private:
        int PassWx;
        bool showMouse;
        static CAnimation frame_pic[2];
        static CMovingBitmap frame_hole;
        Object* CheeseObj;
};


}