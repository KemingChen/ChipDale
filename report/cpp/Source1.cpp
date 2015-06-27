CGameStateInit::CGameStateInit(CGame *g): CGameState(g){}
void CGameStateInit::OnInit()
{
	// 當圖很多時，OnInit載入所有的圖要花很多時間。為避免玩遊戲的人
	//     等的不耐煩，遊戲會出現「Loading ...」，顯示Loading的進度。
	ShowInitProgress(0);	// 一開始的loading進度為0%
	// 開始載入資料
	GameScore::Instance()->Loading();
	//logo.LoadBitmap(IDB_BACKGROUND);
	Sleep(300);				// 放慢，以便看清楚進度，實際遊戲請刪除此Sleep
	// 此OnInit動作會接到CGameStaterRun::OnInit()，所以進度還沒到100%
	CAudio::Instance()->Load(AUDIO_BEGIN,  "sounds\\begin.mp3");
	CAudio::Instance()->Load(AUDIO_BUTTON,  "sounds\\button.mp3");
	pic[0].LoadBitmapA("Bitmaps\\BeginState\\1.bmp");
	pic[1].LoadBitmapA("Bitmaps\\BeginState\\2.bmp");
	pic[2].LoadBitmapA("Bitmaps\\BeginState\\3.bmp");
	pic[3].LoadBitmapA("Bitmaps\\BeginState\\4.bmp");
}
void CGameStateInit::OnBeginState()
{
	order=0;
}
void CGameStateInit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	const char KEY_ESC = 27;
	const char KEY_ENTER = 13;
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	if(nChar == KEY_RIGHT){
		if(order==0){
			CAudio::Instance()->Play(AUDIO_BUTTON);
			order=1;
		}
		if(order==2){
			CAudio::Instance()->Play(AUDIO_BUTTON);
			order=3;
		}
	}
	if(nChar == KEY_LEFT){
		if(order==1){
			CAudio::Instance()->Play(AUDIO_BUTTON);
			order=0;
		}
		if(order==3){
			CAudio::Instance()->Play(AUDIO_BUTTON);
			order=2;
		}
	}
	if(nChar == KEY_ENTER){
		CAudio::Instance()->Play(AUDIO_BUTTON);
		if(order==0)order=2;
		else {
			CAudio::Instance()->Stop(AUDIO_BEGIN);
			if(order==1){
				CGameStateRun::multiplayer = 2;
				GotoGameState(GAME_STATE_RUN);
			}
			else{
				CGameStateRun::multiplayer = 1;
				if(order==2)CGameStateRun::Chip_Dale=0;
				else CGameStateRun::Chip_Dale=1;
				GotoGameState(GAME_STATE_RUN);
			}
		}
	}
}
void CGameStateInit::OnShow()
{
	pic[order].ShowBitmap();
}								
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
/////////////////////////////////////////////////////////////////////////////
int CGameStateRun::multiplayer=1;
int CGameStateRun::NowLevel=1;
int CGameStateRun::Chip_Dale=0;
int CGameStateRun::TimeToBonus=0;
bool CGameStateRun::TimeToGo;
bool CGameStateRun::IfViewer=false;
bool CGameStateRun::IsBoss=false;
ChipDale* CGameStateRun::player[2];
CGameStateRun::CGameStateRun(CGame *g)
: CGameState(g)
{
	IfViewer = false;
	NowLevel=1;
	show_save = false;
	show_load = false;
	EditMoveMode = 0;
	Maps = NULL;
	player[0] = NULL;
	player[1] = NULL;
	for(int i=0;i<CanTakeNum;i++) //這只需寫一次 初始化陣列
		AllThrow[i]=NULL;
	for(int i=0;i<ONE_LEVEL_MONSTER_NUM;i++) //這只需寫一次 初始化陣列
		AllMonster[i]=NULL;
	ChipDale::CanThrow = AllThrow;
	ChipDale::AllMonster = AllMonster;
}
CGameStateRun::~CGameStateRun()
{
	if(Maps!=NULL)	delete(Maps);
	for(int i=0;i<ONE_LEVEL_MONSTER_NUM && AllMonster[i]!=NULL;i++){
		delete(AllMonster[i]);
	}
	for(int i=0;i<CanTakeNum;i++){
		if(AllThrow[i]==NULL)continue;
		delete(AllThrow[i]);
	}
	for(int i=0;i<2 && player[i]!=NULL;i++)
		delete(player[i]);
}
void CGameStateRun::OnBeginState()
{
	int initial_Y;
	TimeToGo=false;
	EditPointer.SetWxWy(0,0);
	Maps = new MapManage(NowLevel);
	CAudio::Instance()->PlayLevelMusic(0,true);
	CAudio::Instance()->PauseLevelMusic(0);
	CAudio::Instance()->Play(AUDIO_1_1A);
	ChipDale::Maps = Maps;
	Maps->SetObstacle();
	Maps->SetMonster(AllMonster);
	for(int i=0;i<CanTakeNum;i++){
		if(AllThrow[i]!=NULL){
			delete AllThrow[i];
			AllThrow[i]=NULL;
		}
	}
	if(player[0]!=NULL)delete player[0];
	if(player[1]!=NULL)delete player[1];
	player[0] = new ChipDale(Chip_Dale);
	if(multiplayer==2){
		player[1] = new ChipDale(1);
	}
	else
		player[1]=NULL;
	player[0]->SetPartner(player[1]);
	if(multiplayer==2)	player[1]->SetPartner(player[0]);
	initial_Y = (MapManage::Teleport>>7)/DesSize;//得到出生的高 , DesSize 表示 Des 最大的值(128)
	TRACE("initial_Y: %d\n",initial_Y);
	player[0]->InitialWidthHeight();
	player[0]->SetWxWy(80,initial_Y-player[0]->ReturnHeight());
	if(multiplayer==2){
		player[1]->InitialWidthHeight();
		player[1]->SetWxWy(80+player[1]->ReturnWidth(),initial_Y-player[1]->ReturnHeight());
	}
}

void CGameStateRun::OnMove()							// 移動遊戲元素
{
	// 如果希望修改cursor的樣式，則將下面程式的commment取消即可
	// SetCursor(AfxGetApp()->LoadCursor(IDC_GAMECURSOR));
	//TRACE("1\n");
	int des,initial_Y,fixX,fixY;
	int toolCDC_State = ToolCDC::ReturnStage();
	static int music_count=0;
	if(music_count<6*33-5)music_count++;
	if(music_count==6*33-5){
		CAudio::Instance()->PlayLevelMusic(NowLevel,true);
		music_count++;
	}
	if(GameScore::Instance()->Switch)return;
	if(TimeToGo||TimeToBonus==1){
		CAudio::Instance()->StopLevelMusic(NowLevel);
		CAudio::Instance()->Stop(AUDIO_EDIT);
		if(TimeToGo){
			player[0]->ResetScore();
			if(player[1]!=NULL)player[1]->ResetScore();
			TimeToGo=false;
			GotoGameState(GAME_STATE_OVER);
		}else{
			TimeToBonus=false;
			GotoGameState(GAME_STATE_BONUS);
		}
	}
	if(TimeToBonus>0){
		if(TimeToBonus==PassMusicTime)
		{
			CAudio::Instance()->StopLevelMusic(NowLevel);
			CAudio::Instance()->Play(AUDIO_1_PASS);
		}
		TimeToBonus--;
	}
	if(toolCDC_State==0){
		if(!IfViewer){
			//人物移動
			if(!TimeToBonus){
				player[0]->OnMove();
				if(multiplayer==2)
					player[1]->OnMove();
			}
			//取消注解此行可得人物Y軸
			//static int lastNowY = 0;int tempNowY = player[0]->ReturnWY()+player[0]->ReturnHeight();if(lastNowY!=tempNowY){TRACE("now_Y: %d\n",tempNowY);lastNowY=tempNowY;}
			//人物修正
			player[0]->FixXY(Maps);
			if(multiplayer==2)
				player[1]->FixXY(Maps);
			for(int i=0;i<CanTakeNum;i++){
				if(AllThrow[i]==NULL)continue;
				if(AllThrow[i]->ReturnNowAct()==0){
					delete(AllThrow[i]);
					AllThrow[i]=NULL;
					continue;
				}
				if(AllThrow[i]->ReturnNowAct()>=0)//有時候不想FIX修正位置 可以讓 NowAct 為負的!!!
					AllThrow[i]->FixXY(Maps);
				AllThrow[i]->OnMove(Maps);
			}
			//怪物移動+修正+碰撞測試
			for(int i=0;i<ONE_LEVEL_MONSTER_NUM && AllMonster[i]!=NULL;i++){
				if(AllMonster[i]->OnMove(Maps,player)){
					AllMonster[i]->CollisionChipDale(player[0]);
					if(multiplayer==2)
						AllMonster[i]->CollisionChipDale(player[1]);
				}
			}
			//碰撞 物件 怪物 人物 測試
			for(int i=0;i<CanTakeNum;i++){
				if(AllThrow[i]==NULL)continue;
				AllThrow[i]->CollisionMonster(AllMonster);
				AllThrow[i]->CollisionChipDale(player[0]);
				if(multiplayer==2)
					AllThrow[i]->CollisionChipDale(player[1]);
			}
		}
		else{
			EditPointer.OnMove(Maps);
			EditPointer.FixXY(Maps);
		}
	}
	else if(toolCDC_State==2){
		des = (Maps->Teleport)%DesSize;//得到出生的高 , DesSize 表示 Des 最大的值(128)
		initial_Y = (Maps->GetRoute((des-1)%10,(des-1)/10)>>7)/DesSize;
		TRACE("initial_Y: %d\n",initial_Y);
		fixX = -Maps->ReturnNowX()+(des-1)%MAX_XN*MWIDTH;
		fixY = -Maps->ReturnNowY()+(des-1)/MAX_YN*MHEIGHT;
		Maps->SetMapXY((des-1)%MAX_XN*MWIDTH ,(des-1)/MAX_YN*MHEIGHT);
		Maps->SetRecord();
		
		for(int i=0;i<ONE_LEVEL_MONSTER_NUM && AllMonster[i]!=NULL;i++){
			AllMonster[i]->FixMapMove(fixX,fixY);
		}
		for(int i=0;i<CanTakeNum;i++){
			if(AllThrow[i]==NULL)continue;
			if(AllThrow[i]->ReturnNowAct()!=1){
				delete(AllThrow[i]);
				AllThrow[i]=NULL;
			}
		}
		player[0]->Reset(80,initial_Y-player[0]->ReturnHeight(),true,false);
		if(multiplayer==2)
			player[1]->Reset(125,initial_Y-player[1]->ReturnHeight(),true,false);
		ToolCDC::ToNextState();
	}else if(toolCDC_State==4){
		if(Maps->GetRoute()&64){
			IsBoss=true;
			CAudio::Instance()->PlayLevelMusic(NowLevel,true);
		}else{
			IsBoss=false;
		}
	}
}
void CGameStateRun::OnInit()// 遊戲的初值及圖形設定
{
	// 當圖很多時，OnInit載入所有的圖要花很多時間。為避免玩遊戲的人
	//     等的不耐煩，遊戲會出現「Loading ...」，顯示Loading的進度。
	ShowInitProgress(33);	// 接個前一個狀態的進度，此處進度視為33%
	//
	// 開始載入資料
	//
	Health[0].LoadBitmapA("Bitmaps/StateRun/health_1.bmp",PURPLE);
	Health[1].LoadBitmapA("Bitmaps/StateRun/health_2.bmp",PURPLE);
	Health[2].LoadBitmapA("Bitmaps/StateRun/health_3.bmp",PURPLE);
	ShowWho[0].LoadBitmapA("Bitmaps/StateRun/chip_L.bmp",PURPLE);
	ShowWho[1].LoadBitmapA("Bitmaps/StateRun/dale_L.bmp",PURPLE);
	ShowWho[2].LoadBitmapA("Bitmaps/StateRun/dale_R.bmp",PURPLE);
	ShowInitProgress(40);
	ChipDale::Loading();
	EditPointer.Loading();
	ToolCDC::Loading();
	ShowInitProgress(45);
	//怪物圖片loading
	MachineDog::Loading();
	Cactus::Loading();
	Wire::Loading();
	Mouse::Loading();
	Wasp::Loading();
	Centipede::Loading();
	//End Loading
	ShowInitProgress(50);
	//物件圖片loading
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
	saved.LoadBitmapA("Bitmaps/object/save.bmp",0xffffff);
	loading.LoadBitmapA("Bitmaps/object/loading.bmp",0xffffff);
	// 完成部分Loading動作，提高進度
	ShowInitProgress(80);
	// 繼續載入其他資料
	CAudio::Instance()->LevelMusicOnit();
	CAudio::Instance()->Load(AUDIO_JUMP,  "sounds/jump.mp3");
	CAudio::Instance()->Load(AUDIO_EDIT,  "sounds/edit.mp3");
	CAudio::Instance()->Load(AUDIO_DEAD,  "sounds/dead.mp3");
	CAudio::Instance()->Load(AUDIO_1_1,   "sounds/state/1/level_1B.mp3");
	CAudio::Instance()->Load(AUDIO_1_BOSS,"sounds/state/1/boss_1B.mp3");
	CAudio::Instance()->Load(AUDIO_1_1A,  "sounds/state/1/level_1A.mp3");
	CAudio::Instance()->Load(AUDIO_1_PASS,"sounds/state/1/level_1pass.mp3");
	// 此OnInit動作會接到CGameStaterOver::OnInit()，所以進度還沒到100%
}
void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//密技區 宣告
	static char GodMode[]="GOD";static int  GodModeI = 0;
	static char EditMode[]="EDIT";static int EditModeI = 0;
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	//密技區 實做
	if(nChar==GodMode[GodModeI]){
		GodModeI++;
		if(GodMode[GodModeI]==0){
			player[0]->GodMode();
			GodModeI=0;
		}
	}
	else	GodModeI=0;
	if(nChar==EditMode[EditModeI]){// CapsLock 切換人物與造物者模式
		EditModeI++;
		if(EditMode[EditModeI]==0){
			if(player[0]->ReturnHealth()<=0||multiplayer==2){EditModeI=0;return;}
			IfViewer = !IfViewer;
			if(IfViewer){
				CAudio::Instance()->PlayLevelMusic(0);
			}else{
				CAudio::Instance()->PlayLevelMusic(NowLevel);
			}
			player[0]->Reset(EditPointer.ReturnWX(),EditPointer.ReturnWY(),player[0]->ReturnLR(),false);
			if(multiplayer==2){
				player[1]->Reset(EditPointer.ReturnWX()+player[1]->ReturnWidth(),EditPointer.ReturnWY(),player[1]->ReturnLR(),false);
			}
			EditPointer.SetMove(false,true,true,true,true,EditMoveMode);
			if(IfViewer){
				Maps->LevelLoading(NowLevel,true);
				Maps->SetMapXY(Maps->ReturnNowX()/ONEOBJX*ONEOBJX,Maps->ReturnNowY()/ONEOBJY*ONEOBJY);
			}
			else{
				Maps->SetMapXY(Maps->ReturnNowX()/SPEED*SPEED,Maps->ReturnNowY()/SPEED*SPEED);
				Maps->SetObstacle();
				Maps->SetMonster(AllMonster);
				player[0]->ReleaseNowTakeObj();
				if(multiplayer==2)
					player[1]->ReleaseNowTakeObj();
				for(int i=0;i<CanTakeNum;i++){
					if(AllThrow[i]==NULL)continue;
					if(AllThrow[i]->ReturnNowAct()>=0){
						delete(AllThrow[i]);
						AllThrow[i]=NULL;
					}
				}
			}
			EditModeI = 0;
		}
	}else	EditModeI = 0;
	//一般按鍵觸發
	if(!IfViewer){
		if(multiplayer==1)
			player[0]->setFlag(true,(nChar==KEY_UP),(nChar == KEY_DOWN),(nChar == KEY_LEFT),(nChar == KEY_RIGHT),(nChar == 'Z'),(nChar == 'X'));
		if(multiplayer==2){
			player[0]->setFlag(true,(nChar==KEY_UP),(nChar == KEY_DOWN),(nChar == KEY_LEFT),(nChar == KEY_RIGHT),(nChar == 'N'),(nChar == 'M'));
			player[1]->setFlag(true,(nChar==87),(nChar == 83),(nChar == 65),(nChar == 68),(nChar == 90),(nChar == 88));
		}
		if(nChar==9)GameScore::Instance()->ShowScroe();
	}
	else{
		show_save = false;
		EditPointer.SetMove(true,(nChar == KEY_UP),(nChar == KEY_DOWN),(nChar == KEY_LEFT),(nChar == KEY_RIGHT),EditMoveMode);
		if(nChar=='Z')	EditPointer.SetOrder(EditPointer.nowOrder()-1);
		if(nChar=='X')	EditPointer.SetOrder(EditPointer.nowOrder()+1);
		if(nChar==13)	EditPointer.SetObject(Maps);		// Enter 放置
		if(nChar==9)	EditMoveMode = (EditMoveMode+1)%3;	// TAB 切換模式
		if(nChar==27)  {// ESC 儲存
			Maps->SaveObj(NowLevel);
			show_save=true;
		}
	}
	
}
void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//TRACE("3\n");
	const char KEY_LEFT  = 0x25; // keyboard左箭頭
	const char KEY_UP    = 0x26; // keyboard上箭頭
	const char KEY_RIGHT = 0x27; // keyboard右箭頭
	const char KEY_DOWN  = 0x28; // keyboard下箭頭
	if(!IfViewer){
		if(multiplayer==1)
			player[0]->setFlag(false,(nChar==KEY_UP),(nChar == KEY_DOWN),(nChar == KEY_LEFT),(nChar == KEY_RIGHT),(nChar == 'Z'),(nChar == 'X'));
		if(multiplayer==2){
			player[0]->setFlag(false,(nChar==KEY_UP),(nChar == KEY_DOWN),(nChar == KEY_LEFT),(nChar == KEY_RIGHT),(nChar == 'N'),(nChar == 'M'));
			player[1]->setFlag(false,(nChar==87),(nChar == 83),(nChar == 65),(nChar == 68),(nChar == 90),(nChar == 88));
		}
		if(nChar==9)GameScore::Instance()->ShowScroe();
	}
	else{
		EditPointer.SetMove(false,(nChar == KEY_UP),(nChar == KEY_DOWN),(nChar == KEY_LEFT),(nChar == KEY_RIGHT));
	}
}
void CGameStateRun::OnShow()
{
	//  注意：Show裡面千萬不要移動任何物件的座標，移動座標的工作應由Move做才對，
	//        否則當視窗重新繪圖時(OnDraw)，物件就會移動，看起來會很怪。換個術語
	//        說，Move負責MVC中的Model，Show負責View，而View不應更動Model。
	//  貼上背景圖、撞擊數、球、擦子、彈跳的球
	int toolCDC_State = ToolCDC::ReturnStage();
	if(GameScore::Instance()->Switch){
		GameScore::Instance()->onShow();
		return;
	}
	if(toolCDC_State==0 || toolCDC_State==1 || toolCDC_State==4){
		Maps->OnShow();

		if(!IfViewer){
			for(int i=0;i<CanTakeNum;i++){//物件顯示靜止 NowAct=3 顯是在怪物之後
				if(AllThrow[i]==NULL)continue;
				if(AllThrow[i]->ReturnNowAct()!=3)continue;
				AllThrow[i]->OnShow(Maps);
			}

			for(int i=0;i<ONE_LEVEL_MONSTER_NUM && AllMonster[i]!=NULL;i++){//怪物顯示
				AllMonster[i]->OnShow(Maps);
			}

			for(int i=0;i<CanTakeNum;i++){//物件顯示靜止 NowAct=5 靜止在地上 顯是在怪物之前
				if(AllThrow[i]==NULL)continue;
				if(AllThrow[i]->ReturnNowAct()!=5)continue;
				AllThrow[i]->OnShow(Maps);
			}
			Maps->OnShowObject();
			//物件顯示在手中 NowAct=1
			if(player[0]->ReturnNowTakeObj()!=NULL)
				player[0]->ReturnNowTakeObj()->OnShow(Maps);
			if(multiplayer==2 && player[1]->ReturnNowTakeObj()!=NULL){
				player[1]->ReturnNowTakeObj()->OnShow(Maps);
			}
			player[0]->OnShow();//人物顯示
			if(multiplayer==2)
				player[1]->OnShow();
			for(int i=0;i<CanTakeNum;i++){//物件顯示運動中 NowAct = 2 4
				if(AllThrow[i]==NULL)continue;
				if(AllThrow[i]->ReturnNowAct()!=2 && AllThrow[i]->ReturnNowAct()!=4)continue;
				AllThrow[i]->OnShow(Maps);
			}
			ShowWho[Chip_Dale].SetTopLeft(0,0);
			ShowWho[Chip_Dale].ShowBitmap();
			switch(player[0]->ReturnHealth()){//血量顯示
				case 1 : Health[0].SetTopLeft(0,0);Health[0].ShowBitmap();break;
				case 2 : Health[1].SetTopLeft(0,0);Health[1].ShowBitmap();break;
				case 3 : Health[2].SetTopLeft(0,0);Health[2].ShowBitmap();break;
			}
			if(multiplayer==2){
				ShowWho[2].SetTopLeft(MWIDTH-ShowWho[2].Width(),0);
				ShowWho[2].ShowBitmap();
				switch(player[1]->ReturnHealth()){//血量顯示
					case 1 : Health[0].SetTopLeft(MWIDTH-ShowWho[2].Width(),0);Health[0].ShowBitmap();break;
					case 2 : Health[1].SetTopLeft(MWIDTH-ShowWho[2].Width(),0);Health[1].ShowBitmap();break;
					case 3 : Health[2].SetTopLeft(MWIDTH-ShowWho[2].Width(),0);Health[2].ShowBitmap();break;
				}
			}
		}
		else
			EditPointer.OnShow(Maps);
		//ClearisMoveMap 一定要放在所有要 show 物件的最後面
		Maps->ClearisMoveMap(IfViewer);
	
		if(show_save)
			saved.ShowBitmap();
		
		if(toolCDC_State==1||toolCDC_State==4){
			ToolCDC::SaveCDC();
			ToolCDC::ToNextState();
			if(toolCDC_State==4)	ToolCDC::CDDraw();
		}
	}
	else{
		ToolCDC::CDDraw();
		ToolCDC::ToNextState();
	}
}
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的結束狀態(Game Bonus)
/////////////////////////////////////////////////////////////////////////////
CGameStateBonus::CGameStateBonus(CGame* g)
: CGameState(g)
{
	TRACE("BonusConstruct\n");
}
CGameStateBonus::~CGameStateBonus(){
	TRACE("~CGameStateBonus()\n");
}
void CGameStateBonus::OnInit(){
	TRACE("BonusInit\n");
}
void CGameStateBonus::OnBeginState(){
	TRACE("BonusOnBeginState\n");
}
void CGameStateBonus::OnMove(){}
void CGameStateBonus::OnShow(){}
/////////////////////////////////////////////////////////////////////////////
// 這個class為遊戲的結束狀態(Game Over)
/////////////////////////////////////////////////////////////////////////////
CGameStateOver::CGameStateOver(CGame *g): CGameState(g){}
void CGameStateOver::OnMove()
{
	counter--;
	if (counter < 0){
		CAudio::Instance()->Play(AUDIO_BEGIN,true);
		GotoGameState(GAME_STATE_INIT);
	}
}
void CGameStateOver::OnBeginState()
{
	counter = 30 * 5; // 5 seconds
	CAudio::Instance()->Play(AUDIO_GAMEOVER);
}
void CGameStateOver::OnInit()
{
	// 當圖很多時，OnInit載入所有的圖要花很多時間。為避免玩遊戲的人
	//     等的不耐煩，遊戲會出現「Loading ...」，顯示Loading的進度。
	ShowInitProgress(90);	// 接個前一個狀態的進度，此處進度視為66%
	// 開始載入資料
	//Sleep(300);				// 放慢，以便看清楚進度，實際遊戲請刪除此Sleep
	// 最終進度為100%
	ShowInitProgress(100);
	CAudio::Instance()->Load(AUDIO_GAMEOVER,"sounds\\gameover.mp3");
	CAudio::Instance()->Play(AUDIO_BEGIN,true);
}
void CGameStateOver::OnShow()
{
	CDC *pDC = CDDraw::GetBackCDC();			// 取得 Back Plain 的 CDC 
	CFont f,*fp;
	f.CreatePointFont(160,"Times New Roman");	// 產生 font f; 160表示16 point的字
	fp=pDC->SelectObject(&f);					// 選用 font f
	pDC->SetBkColor(RGB(0,0,0));
	pDC->SetTextColor(RGB(255,255,0));
	char str[80];								// Demo 數字對字串的轉換
	sprintf(str, "Game Over ! (%d)", counter / 30);
	pDC->TextOut(240,210,str);
	pDC->SelectObject(fp);						// 放掉 font f (千萬不要漏了放掉)
	CDDraw::ReleaseBackCDC();					// 放掉 Back Plain 的 CDC
}
void CAudio::Resume_for_CGame()
{
	int state=CGame::Instance()->WhichState();
	if(state==GAME_STATE_RUN){
		if(CGameStateRun::IfViewer)
			PlayLevelMusic(0);
		else
		{
			PlayLevelMusic(CGameStateRun::NowLevel);
		}
	}else
		Resume();
}
void CAudio::PlayLevelMusic(int level,bool stop_and_play){
	if (!Instance()->isOpened)
		return;
	int Notboss;
	if(CGameStateRun::IsBoss)Notboss=0;
	else Notboss=1;
	if(level==0)Notboss=0;
	char command[MAX_MCI_COMMAND_SIZE];
	if(stop_and_play)
		sprintf(command, "play device%d from 0 repeat", LevelMusic.at(level*2-Notboss));
	else
		sprintf(command, "resume device%d", LevelMusic.at(level*2-Notboss));
	Instance()->SendMciCommand(command);
	if(level!=0){
		Instance()->PauseById(LevelMusic[0]);
		if(Notboss==0)Instance()->PauseById(LevelMusic[level*2-1]);
	}
	else{
		Instance()->PauseById(LevelMusic[CGameStateRun::NowLevel*2-1]);
		Instance()->PauseById(LevelMusic[CGameStateRun::NowLevel*2]);
	}
}
void CAudio::PauseLevelMusic(int level){
	if (!Instance()->isOpened)
		return;
	int Notboss;
	if(CGameStateRun::IsBoss)Notboss=0;
	else Notboss=1;
	if(level==0)Notboss=0;
	char command[MAX_MCI_COMMAND_SIZE];
	sprintf(command, "pause device%d wait", LevelMusic.at(level*2-Notboss));
	Instance()->SendMciCommand(command);
}
void CAudio::StopLevelMusic(int level){
	if (!Instance()->isOpened)
		return;
	char command[MAX_MCI_COMMAND_SIZE];
	sprintf(command, "stop device%d", LevelMusic.at(level*2-1));
	Instance()->SendMciCommand(command);
	sprintf(command, "stop device%d", LevelMusic.at(level*2));
	Instance()->SendMciCommand(command);
}
void CAudio::LevelMusicOnit(){
	static bool IsExecuted=false;
	if(IsExecuted)return;
	LevelMusic.insert(pair<int,int>(0,AUDIO_EDIT));
	LevelMusic.insert(pair<int,int>(1,AUDIO_1_1));
	LevelMusic.insert(pair<int,int>(2,AUDIO_1_BOSS));
	IsExecuted=true;
}
void CAudio::PauseById(int id)
{
	int j=0;
	if (!isOpened)
		return;
	map<int, Info>::iterator i;
	i=info.find(id);
	if (i->second.isGood) {
		char command[MAX_MCI_COMMAND_SIZE];
		sprintf(command, "pause device%d wait", i->first);
		SendMciCommand(command);
	}
}
void CAudio::SetSpeedByID(unsigned int id,int speed){
	if (!isOpened)
		return;
	GAME_ASSERT(info.find(id) != info.end(), "Can not play back audio: incorrect Audio ID!");
	if (!info[id].isGood)		
		return;
	char command[400];
	sprintf(command, "set device%d speed %d", id,speed);
	SendMciCommand(command);
}
/////////////////////////////////////////////////////////////////////////////
// ToolCDC
/////////////////////////////////////////////////////////////////////////////
COLORREF ToolCDC::color[MHEIGHT][MWIDTH];
CAnimation ToolCDC::MovePointer;
CMovingBitmap ToolCDC::loading_BG,ToolCDC::loading_Bar,ToolCDC::loading_Mask;
CMovingBitmap ToolCDC::letter[LEVELNUM],ToolCDC::level;
int ToolCDC::NowState=0;
double ToolCDC::alpha=0.0;
ToolCDC::ToolCDC(){}
void ToolCDC::Loading(){
	char name[100];
	MovePointer.SetDelayCount(20);
	MovePointer.AddBitmap("Bitmaps/action/","Chip/","run_1R.bmp",PURPLE);
	MovePointer.AddBitmap("Bitmaps/action/","Chip/","run_2R.bmp",PURPLE);
	MovePointer.AddBitmap("Bitmaps/action/","Chip/","run_3R.bmp",PURPLE);
	MovePointer.AddBitmap("Bitmaps/action/","Chip/","run_4R.bmp",PURPLE);
	loading_BG.LoadBitmapA("Bitmaps/BeginState/loadingBG.bmp",PURPLE);
	loading_Bar.LoadBitmapA("Bitmaps/BeginState/loadingBar.bmp",PURPLE);
	loading_Mask.LoadBitmapA("Bitmaps/BeginState/loadingMask.bmp",PURPLE);
	level.LoadBitmapA("Bitmaps/BeginState/Level.bmp",PURPLE);
	for(int i=0;i<LEVELNUM;i++){
		sprintf(name,"Bitmaps/BeginState/Letter%d.bmp",i+1);
		letter[i].LoadBitmapA(name,PURPLE);
	}
}
void ToolCDC::CDDraw()
{
	CDC *pDC = CDDraw::GetBackCDC();			// 取得 Back Plain 的 CDC 
	for(int i=0;i<MWIDTH;i+=1){
		for(int j=0;j<MHEIGHT;j+=1){
			pDC->SetPixel(i,j,ALPHA(alpha,i,j));
		}
	}
	CDDraw::ReleaseBackCDC();					// 放掉 Back Plain 的 CDC
}
void ToolCDC::SaveCDC()
{
	CDC *pDC = CDDraw::GetBackCDC();			// 取得 Back Plain 的 CDC 
	for(int i=0;i<MWIDTH;i+=1){
		for(int j=0;j<MHEIGHT;j+=1){
			if(i%2==0&&j%2==0)
				color[j][i] =pDC->GetPixel(i,j);
			else
				color[j][i] = color[j/2*2][i/2*2];
		}
	}
	CDDraw::ReleaseBackCDC();					// 放掉 Back Plain 的 CDC
	if(NowState<=2)	alpha = 0.75;
	if(NowState>=3)	alpha = 0.25;
}
void ToolCDC::ToNextState()
{
	switch(NowState){
		case 1:case 2:case 4:
			NowState += 1; 
			break;
		case 3:
			if(alpha!=0.0){
				alpha -= 0.25;
				if(alpha<0.0)alpha=0.0;
			}
			else{
				NowState += 1;
			}
			break;
		case 5:
			if(alpha!=1.0){
				alpha += 0.25;
				if(alpha>1.0)alpha=1.0;
			}
			else{
				NowState = 0;
			}
			break;
	}
}
void ToolCDC::ShowProgressBar(int WhichLevel,int percent){
	int width = MovePointer.Width(),height = MovePointer.Height();
	if(percent>100)percent = 100;
	CDDraw::BltBackColor(DEFAULT_BG_COLOR);		// 將 Back Plain 塗上預設的顏色
	loading_Mask.SetTopLeft(95,275);
	loading_Mask.ShowBitmap();
	loading_Bar.SetTopLeft(95-450*(100-percent)/100,275);
	loading_Bar.ShowBitmap();
	loading_BG.SetTopLeft(0,0);
	loading_BG.ShowBitmap();
	MovePointer.SetBottomLeft(95+450*percent/100-(width)/2,275-(height-30)/2,height);
	MovePointer.OnShow();
	MovePointer.OnMove();
	level.SetTopLeft(160,140);
	level.ShowBitmap();
	letter[WhichLevel-1].SetTopLeft(415,140);
	letter[WhichLevel-1].ShowBitmap();
	CDDraw::BltBackToPrimary();					// 將 Back Plain 貼到螢幕
}
/****************************************************************/
//GameMapEdit實作												*/
//																*/
/****************************************************************/
GameMapEdit::GameMapEdit(){
	Order = 0;
	wx = wy = 0;
	for(int i=0;i<4;i++)
		move[i]=false;
	ContinueMode = 0;
}
void GameMapEdit::Loading(){
	char temp[100];
	frame_pic.LoadBitmapA("Bitmaps/object/Pointer.bmp");
	for(int i=0;i<OrderSize;i++){
		sprintf(temp,"Bitmaps/object/%d.bmp",i);
		frame_background[i].LoadBitmapA(temp,PURPLE);
	}
	for(int i=-1;i>=-MonsterOrderSize;i--){
		sprintf(temp,"Bitmaps/monster/%d.bmp",-i);
		frame_background[-i+OrderSize-1].LoadBitmapA(temp,PURPLE);
	}
}
void GameMapEdit::OnShow(MapManage *map){
	int temp;
	//顯示 目前螢幕上的OBJ
	for(int i=0;i<=(MWIDTH+2*WinShowBuffer)/ONEOBJX;i++)
		for(int j=0;j<=(MHEIGHT+2*WinShowBuffer)/ONEOBJY;j++)
			map->ShowObstacle(i,j);
	//顯示 目前位置
	temp = (Order<0)*(-Order+OrderSize-1) + (Order>=0)*(Order); 
	if(Order==0)
		frame_background[0].SetTopLeft(wx-16,wy-16);
	else
		frame_background[temp].SetTopLeft(wx,wy);
	frame_background[temp].ShowBitmap();
	frame_pic.SetTopLeft(wx,wy);
	frame_pic.ShowBitmap();
}
void GameMapEdit::SetOrder(int value){
	if (value < OrderSize && value >= -MonsterOrderSize)
		Order = value;
	if (value < -MonsterOrderSize)
		Order = OrderSize-1;
	if (value >=OrderSize)
		Order = -MonsterOrderSize;
}
void GameMapEdit::OnMove(MapManage *map)
{
	int width,height,temp;				//目前圖片高度和寬度
	int timesX,timesY;
	if(ContinueMode==0)		{timesX=1; timesY=3; }
	else if(ContinueMode==1){timesX=1; timesY=1; }
	else if(ContinueMode==2){timesX=-1;timesY=-1;}
	temp = (Order<0)*(-Order+OrderSize-1) + (Order>=0)*(Order); 
	width = frame_background[temp].Width();
	height = frame_background[temp].Height();
	if(timesX==-1||timesY==-1){
		timesX = width/ONEOBJX;
		timesY = height/ONEOBJY;
	}
	static int limitX = (MWIDTH + width)/2;
	static int limitY = (MHEIGHT + height)/2;
	if(move[0]){
		wy -= ONEOBJY*timesY;
		if(wy < 0){
			wy += ONEOBJY*timesY;
		}
		if(wy < limitY){
			if((map->GetRoute()&14) && ((map->ReturnNowY())%MHEIGHT)<ONEOBJY*timesY && ((map->ReturnNowY())%MHEIGHT)>0)
				timesY =((map->ReturnNowY())%MHEIGHT)/ONEOBJY;
			map->MoveMap(1,ONEOBJX*timesX,ONEOBJY*timesY);
		}
		ThisMoveTimesX=timesX;
		ThisMoveTimesY=timesY;
	}
	if(move[1]){
		wy += ONEOBJY*timesY;
		if(wy > MHEIGHT-height){
			wy -= ONEOBJY*timesY;
		}
		if(wy > limitY){
			if((map->GetRoute()&13) && MHEIGHT-((map->ReturnNowY())%MHEIGHT)<ONEOBJY*timesY && MHEIGHT-((map->ReturnNowY())%MHEIGHT)>0)
				timesY =(MHEIGHT-((map->ReturnNowY())%MHEIGHT))/ONEOBJY;
			map->MoveMap(2,ONEOBJX*timesX,ONEOBJY*timesY);
		}
		ThisMoveTimesX=timesX;
		ThisMoveTimesY=timesY;
	}
	if(move[2]){
		wx -= ONEOBJX*timesX;
		if(wx < 0){
			wx += ONEOBJX*timesX;
		}
		if(wx < limitX)
			map->MoveMap(4,ONEOBJX*timesX,ONEOBJY*timesY);
		
		ThisMoveTimesX=timesX;
		ThisMoveTimesY=timesY;
	}
	if(move[3]){
		wx += ONEOBJX*timesX;
		if(wx > MWIDTH-width){
			wx -= ONEOBJX*timesX;
		}
		if(wx > limitX)
			map->MoveMap(8,ONEOBJX*timesX,ONEOBJY*timesY);
		
		ThisMoveTimesX=timesX;
		ThisMoveTimesY=timesY;
	}
	if(ContinueMode>0){
		move[0] = move[1] = move[2] = move[3] = false;
		ContinueMode =0;
	}
}
void GameMapEdit::SetWxWy(int setWx,int setWy)
{
	wx = setWx;
	wy = setWy;
}
void GameMapEdit::SetMove(bool flag,bool IsUp,bool IsDown,bool IsLeft,bool IsRight,int mode)
{
	if(IsUp)	move[0]=flag;
	if(IsDown)	move[1]=flag;
	if(IsLeft)	move[2]=flag;
	if(IsRight)	move[3]=flag;
	if(mode)	ContinueMode = mode;
}
void GameMapEdit::FixXY(MapManage *map)
{
	int temp = map->MoveMap(0);
	if(temp&1)
		wy += ONEOBJY*ThisMoveTimesY;
	if(temp&2)
		wy -= ONEOBJY*ThisMoveTimesY;
	if(temp&4)
		wx += ONEOBJX*ThisMoveTimesX;
	if(temp&8)
		wx -= ONEOBJX*ThisMoveTimesX;
	
	ThisMoveTimesX=0;
	ThisMoveTimesY=0;
}
void GameMapEdit::SetObject(MapManage *map)
{
	map->SetObj(Order,wx,wy);
}
/****************************************************************/
//GameScore實作													*/
//																*/
/****************************************************************/
GameScore GameScore::Game_Score;
GameScore::GameScore(){
	Switch=false;
	score_p=new CInteger(2);
}
GameScore::~GameScore(){
	delete(score_p);
	TRACE("~GameScore()\n");
}
GameScore* GameScore::Instance()
{
	return &Game_Score;
}
void GameScore::Loading(){
	Head[0].LoadBitmapA("Bitmaps/score/chiphead.bmp");
	Head[1].LoadBitmapA("Bitmaps/score/dalehead.bmp");
	F.LoadBitmapA("Bitmaps/score/F.bmp",PURPLE);
	S.LoadBitmapA("Bitmaps/score/S.bmp",PURPLE);
	L_C.LoadBitmapA("Bitmaps/score/L_C.bmp",PURPLE);
	L_D.LoadBitmapA("Bitmaps/score/L_D.bmp",PURPLE);
	score_p->LoadBitmap();
}
void GameScore::ShowScroe(){
	if(Switch==true)Switch=false;
	else			Switch=true;
}
void GameScore::onShow(){
	if(Switch==false)return;
	if(CGameStateRun::player[0]->ReturnIsDale()==0)
	{
		Head[0].SetTopLeft(MWIDTH/2-100-Head[0].Width(),100);
		Head[0].ShowBitmap();
		F.SetTopLeft(MWIDTH/2-100-Head[0].Width(),240);
		S.SetTopLeft(MWIDTH/2-100-Head[0].Width(),300);
		L_C.SetTopLeft(MWIDTH/2-100-Head[0].Width(),360);
		F.ShowBitmap();
		S.ShowBitmap();
		L_C.ShowBitmap();
		score_p->SetInteger(CGameStateRun::player[0]->Score_Flower);
		score_p->SetTopLeft(MWIDTH/2-50-Head[0].Width(),250);
		score_p->ShowBitmap();
		score_p->SetInteger(CGameStateRun::player[0]->Score_Star);
		score_p->SetTopLeft(MWIDTH/2-50-Head[0].Width(),310);
		score_p->ShowBitmap();
		score_p->SetInteger(CGameStateRun::player[0]->ReturnLife());
		score_p->SetTopLeft(MWIDTH/2-50-Head[0].Width(),370);
		score_p->ShowBitmap();
	}
	if(CGameStateRun::multiplayer==2||CGameStateRun::player[0]->ReturnIsDale())
	{
		ChipDale* p=CGameStateRun::player[0];
		if(CGameStateRun::multiplayer==2)p=CGameStateRun::player[1];
		Head[1].SetTopLeft(MWIDTH/2+100,100);
		Head[1].ShowBitmap();
		F.SetTopLeft(MWIDTH/2+100,240);
		S.SetTopLeft(MWIDTH/2+100,300);
		L_D.SetTopLeft(MWIDTH/2+100,360);
		F.ShowBitmap();
		S.ShowBitmap();
		L_D.ShowBitmap();
		score_p->SetInteger(p->Score_Flower);
		score_p->SetTopLeft(MWIDTH/2+150,250);
		score_p->ShowBitmap();
		score_p->SetInteger(p->Score_Star);
		score_p->SetTopLeft(MWIDTH/2+150,310);
		score_p->ShowBitmap();
		score_p->SetInteger(p->ReturnLife());
		score_p->SetTopLeft(MWIDTH/2+150,370);
		score_p->ShowBitmap();
	}
}