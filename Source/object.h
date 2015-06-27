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
        void RecoverObj(ChipDale* player);				//���s�B��ɻݩI�s
        void CollisionMonster(Monster** monster);
        void CollisionChipDale(ChipDale* player);
    protected:
        int CanAttackMode;								// ������H 0 ����H 1 ���F�D�H(�w�]��),2 None 3 �u�����Ǫ�
        int Direct, LRflag, ThisObjValue, NowAct;			//ThisObjValue�ΨӰO���R��obstacle���s��
        //NowAct =0 ���Ͷ� 1 ���b��W 2�B�ʪ��A 3��b�a�W 4�����Ǫ�
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
        Explosion(int setOx, int setOy, int setChangeWhat, ChipDale* player);			//ChangeWhat ��N�X (����ܪ���N�X�@��)
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
        int ReboundLR;									// 0 �L�ϼu 1 �ϼu�k�� -1�ϼu����
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