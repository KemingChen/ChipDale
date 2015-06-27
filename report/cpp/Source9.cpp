class CGameStateInit : public CGameState {
public:
	CGameStateInit(CGame *g);
	void OnInit();  								// 遊戲的初值及圖形設定
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnKeyDown(UINT, UINT, UINT); 				// 處理鍵盤Down的動作
protected:
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	int order;
	CMovingBitmap pic[4];
};
class CGameStateRun : public CGameState {
public:
	CGameStateRun(CGame *g);
	~CGameStateRun();
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();  								// 遊戲的初值及圖形設定
	void OnKeyDown(UINT, UINT, UINT);
	void OnKeyUp(UINT, UINT, UINT);
	static void ToBonus(){TimeToBonus=PassMusicTime;}
	static int multiplayer,Chip_Dale,NowLevel;		// multiplay 1= 1P ,2= 2P nowplayer 0 =Chip, 1= Dale
	static ChipDale		*player[2];
	static bool TimeToGo;
	static bool IfViewer;
	static bool IsBoss;
protected:
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	CMovingBitmap	saved,loading;	// 背景圖
	CMovingBitmap   Health[3],ShowWho[3];
	CFrame			Frame;
	MapManage		*Maps;
	GameMapEdit		EditPointer; //編輯箭頭
	Monster			*AllMonster[ONE_LEVEL_MONSTER_NUM];		// 全部怪物
	Object			*AllThrow[CanTakeNum];
	int				EditMoveMode;
	bool			show_save,show_load;
	static	int		TimeToBonus;
};
class CGameStateBonus : public CGameState {
public:
	CGameStateBonus(CGame *g);
	~CGameStateBonus();
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();
protected:
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
};
class CGameStateOver : public CGameState {
public:
	CGameStateOver(CGame *g);
	void OnBeginState();							// 設定每次重玩所需的變數
	void OnInit();
protected:
	void OnMove();									// 移動遊戲元素
	void OnShow();									// 顯示這個狀態的遊戲畫面
private:
	int counter;	// 倒數之計數器
};
class CAudio {
public:
	//自定義函式
	void	PauseById(int id);				
	void	SetSpeedByID(unsigned int id,int speed);
	void	PlayOnSpe(unsigned,bool repeat=false,int time=0);
	void	Resume_for_CGame();
	//Functions for Game_ChipDale
	void	PlayLevelMusic(int level,bool stop_and_play=false); //限於GameStateRun使用，會自動偵測是否該播放BOSS音樂
	void	PauseLevelMusic(int level);							//限於GameStateRun使用，會自動偵測是否該播放BOSS音樂
	void	StopLevelMusic(int level);							//限於GameStateRun使用，背景音樂與BOSS音樂全部stop
	void	LevelMusicOnit();									//LevelMusic對照Map初始化(只能執行一次，有防呆)
private:
	static map<int,int> LevelMusic;
};
class ToolCDC{
public:
	ToolCDC();
	static void Loading();						//為 ShowProgressBar 的圖片
	static void Fadeout(){NowState=1;}			//setState =1
	static void Fadein(){NowState=4;}			//setState =1
	static void SaveCDC();							//記錄目前螢幕的 Colors
	static int  ReturnStage(){return NowState;}		//傳回目前該做甚麼事情
	static void ToNextState();						//移到下一個state
	static void CDDraw();
	static void ShowProgressBar(int WhichLevel,int percent);
private:
	static COLORREF color[MHEIGHT][MWIDTH];
	static CAnimation MovePointer;
	static CMovingBitmap loading_BG,loading_Bar,loading_Mask,level,letter[LEVELNUM];
	static int NowState;									
	//0 未設定 , 
	//1 下一次要OnShow要save & 鎖人物移動恩螢幕顯示, (淡出)
	//2 saved 跟onMove說可以換人物位置 & 鎖人物移動恩螢幕顯示 , 
	//3 鎖人物移動恩螢幕顯示
	//4 下一次OnShow要save & 鎖人物移動恩螢幕顯示, (淡入)
	//5 鎖人物移動恩螢幕顯示
	static double alpha;									//目前的aplha值
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
	void SetObject(MapManage *map);			//去wx wy 位置放置obj
	int ReturnWX(){return wx;}
	int ReturnWY(){return wy;}
	void FixXY(MapManage *);				// 修正螢幕位置
private:
	int wx,wy;
	int Order;
	int ThisMoveTimesX,ThisMoveTimesY;		// X、Y方向的移動次數
	int ContinueMode;						// TAB切換微調模式 0 1 2
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
