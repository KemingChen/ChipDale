class CGameStateInit : public CGameState {
public:
	CGameStateInit(CGame *g);
	void OnInit();  								// �C������Ȥιϧγ]�w
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnKeyDown(UINT, UINT, UINT); 				// �B�z��LDown���ʧ@
protected:
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	int order;
	CMovingBitmap pic[4];
};
class CGameStateRun : public CGameState {
public:
	CGameStateRun(CGame *g);
	~CGameStateRun();
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();  								// �C������Ȥιϧγ]�w
	void OnKeyDown(UINT, UINT, UINT);
	void OnKeyUp(UINT, UINT, UINT);
	static void ToBonus(){TimeToBonus=PassMusicTime;}
	static int multiplayer,Chip_Dale,NowLevel;		// multiplay 1= 1P ,2= 2P nowplayer 0 =Chip, 1= Dale
	static ChipDale		*player[2];
	static bool TimeToGo;
	static bool IfViewer;
	static bool IsBoss;
protected:
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	CMovingBitmap	saved,loading;	// �I����
	CMovingBitmap   Health[3],ShowWho[3];
	CFrame			Frame;
	MapManage		*Maps;
	GameMapEdit		EditPointer; //�s��b�Y
	Monster			*AllMonster[ONE_LEVEL_MONSTER_NUM];		// �����Ǫ�
	Object			*AllThrow[CanTakeNum];
	int				EditMoveMode;
	bool			show_save,show_load;
	static	int		TimeToBonus;
};
class CGameStateBonus : public CGameState {
public:
	CGameStateBonus(CGame *g);
	~CGameStateBonus();
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();
protected:
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
};
class CGameStateOver : public CGameState {
public:
	CGameStateOver(CGame *g);
	void OnBeginState();							// �]�w�C�������һݪ��ܼ�
	void OnInit();
protected:
	void OnMove();									// ���ʹC������
	void OnShow();									// ��ܳo�Ӫ��A���C���e��
private:
	int counter;	// �˼Ƥ��p�ƾ�
};
class CAudio {
public:
	//�۩w�q�禡
	void	PauseById(int id);				
	void	SetSpeedByID(unsigned int id,int speed);
	void	PlayOnSpe(unsigned,bool repeat=false,int time=0);
	void	Resume_for_CGame();
	//Functions for Game_ChipDale
	void	PlayLevelMusic(int level,bool stop_and_play=false); //����GameStateRun�ϥΡA�|�۰ʰ����O�_�Ӽ���BOSS����
	void	PauseLevelMusic(int level);							//����GameStateRun�ϥΡA�|�۰ʰ����O�_�Ӽ���BOSS����
	void	StopLevelMusic(int level);							//����GameStateRun�ϥΡA�I�����ֻPBOSS���֥���stop
	void	LevelMusicOnit();									//LevelMusic���Map��l��(�u�����@���A�����b)
private:
	static map<int,int> LevelMusic;
};
class ToolCDC{
public:
	ToolCDC();
	static void Loading();						//�� ShowProgressBar ���Ϥ�
	static void Fadeout(){NowState=1;}			//setState =1
	static void Fadein(){NowState=4;}			//setState =1
	static void SaveCDC();							//�O���ثe�ù��� Colors
	static int  ReturnStage(){return NowState;}		//�Ǧ^�ثe�Ӱ��ƻ�Ʊ�
	static void ToNextState();						//����U�@��state
	static void CDDraw();
	static void ShowProgressBar(int WhichLevel,int percent);
private:
	static COLORREF color[MHEIGHT][MWIDTH];
	static CAnimation MovePointer;
	static CMovingBitmap loading_BG,loading_Bar,loading_Mask,level,letter[LEVELNUM];
	static int NowState;									
	//0 ���]�w , 
	//1 �U�@���nOnShow�nsave & ��H�����ʮ��ù����, (�H�X)
	//2 saved ��onMove���i�H���H����m & ��H�����ʮ��ù���� , 
	//3 ��H�����ʮ��ù����
	//4 �U�@��OnShow�nsave & ��H�����ʮ��ù����, (�H�J)
	//5 ��H�����ʮ��ù����
	static double alpha;									//�ثe��aplha��
};
class GameMapEdit{
public:
	GameMapEdit();
	void Loading();
	void OnShow(MapManage *);
	void SetOrder(int);
	int  nowOrder(){return Order;};
	void OnMove(MapManage *map);
	void SetWxWy(int setWx,int setWy);
	void SetMove(bool,bool,bool,bool,bool,int mode=0);
	void SetObject(MapManage *map);			//�hwx wy ��m��mobj
	int ReturnWX(){return wx;}
	int ReturnWY(){return wy;}
	void FixXY(MapManage *);				// �ץ��ù���m
private:
	int wx,wy;
	int Order;
	int ThisMoveTimesX,ThisMoveTimesY;		// X�BY��V�����ʦ���
	int ContinueMode;						// TAB�����L�ռҦ� 0 1 2
	bool move[4];
	CMovingBitmap frame_pic;
	CMovingBitmap frame_background[OrderSize+MonsterOrderSize];
};
class GameScore{
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
	CMovingBitmap F,S;
	CMovingBitmap L_C,L_D;
	CInteger* score_p;
};
