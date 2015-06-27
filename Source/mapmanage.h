namespace game_framework
{
class MapManage
{
    public:
        MapManage(int WhichLevel);
        void OnShow();
        void OnShowObject();
        void LevelLoading(int WhichLevel, bool NotResetAll = false);		//�]�w���d  NotResetAll�O�_���]�����Ҧ��ܼ�
        int MoveMap(int direct, int MoveOneX = SPEED, int MoveOneY = SPEED); //move 1,2,4,8 ...
        void SetMapXY(int, int);					//�]�w map ��ܦ�m
        void SetObstacle();						//�]�w��ê�����
        void SetMonster(Monster* monster[]);	//�]�w�Ǫ����
        void SetObj(int value, int wx, int wy);	//wx,wy	  �ù��y��
        void SetRecord()
        {
            recordx = x;    //�]�w�ثe�a�ϥ��W���������I
            recordy = y;
        }
        int ReturnNowX()
        {
            return x;   //�Ǧ^�ù����W���b�a�Ϫ�x��m
        }
        int ReturnNowY()
        {
            return y;   //�Ǧ^�ù����W���b�a�Ϫ�y��m
        }
        int ReturnOBJ(int ox, int oy)
        {
            return MapObjXY[oy][ox];   //�^�� Obj ����
        }
        void SaveObj(int WhichLevel);			//�x�s Obj ��T
        void ShowObstacle(int i, int j);			//��ܫ��w������  ps: ��edit�Ϊ�
        void ClearisMoveMap(bool flag);			//�M�� isMoveMap ����
        void Set_toRecord();
        int IfCollision(int wx, int wy, int PicWidth, int PicHeight, bool ignore_obj_2 = false, bool ignore_eat = false, bool findNext = false, bool Reverse = false);	//�p��O�_�I��
        int  GetRoute()
        {
            return Route[y / MHEIGHT][x / MWIDTH];
        }
        int  GetRoute(int rx, int ry)
        {
            return Route[ry][rx];
        }
        void ClearObstacle(int Value);			//��ؼЪ���ȧi�D�� ���L�����h�R���I������
        int FillObstacle(int Value, int setWx, int setWy, bool Visible = true);	//��ؼЪ���ȧi�D�� ���L�����h�W�[�I������
        static int isMoveMap;					//�a�ϲ��ʤ�V     1,2,4,8  �W�U���k
        static int Teleport;
        static int LRMargin;
        friend class ChipDale;


    private:
        CMovingBitmap frame_obstacle[OrderSize + MonsterOrderSize + UnVisibleOrderSize];
    protected:
        int Route[MAX_YN][MAX_XN];				//�a�ϸ��u���ʤ�V 1,2,4,8 ��V 16 �X���I 32~  64 ~boss
        int MapObjXY[MAX_OY][MAX_OX];			//����y��~���Ū�g�ɥ�
        int Obstacle[MAX_OY][MAX_OX];			//��ê��   �P�_�I����		//�Ĥ@�����G����  �ĤG�G���WY �ĤT ���WX
        CMovingBitmap MapSP[MAX_YN][MAX_XN];
        int x, y;								//�ثe�ù����W���b�a�Ϫ��y��
        int recordx, recordy;					//���`�_�l��m
};
}