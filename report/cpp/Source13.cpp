class ChipDale : public BasicInstructions{
public:
	ChipDale(int isDale);
	static void Loading();
	void OnMove();
	void GodMove();
	void OnShow();
	ChipDale* Instance(){return this;}
	int ReturnHideComplete(){return (animation[IsDale][1][4][LRflag].GetCurrentBitmapNumber()>=4)&&NowAct==4;}
	int ReturnNowAct(){return NowAct;}
	bool ReturnIsHurting(){return Hurt;}
	int ReturnLastAct(){return LastAct;}
	int ReturnIsTaken(){if(NowTaken>0)return 1;else return 0;}
	int ReturnTakenByPartner(){if(Partner!=NULL&&Partner->NowTaken==ChipDale_taken)return 1 ;else return 0;}
	int ReturnHealth(){return Health;}
	int ReturnLife(){return Life;}
	int ReturnInvincible(){return Invincible;}
	int ReturnIsDale(){return IsDale;}
	int CollisionChipDale(int Direct,int passSpeed,int Squeezemode=1);
	Object* ReturnNowTakeObj(){return NowTakeObj;}
	ChipDale* getPartner(){return Partner;}
	void setFlag(bool value,bool,bool,bool,bool,bool,bool);	//值 上下左右AB
	void SetState();										//設定Act LRdirect
	void SetPartner(ChipDale *setPlayer){Partner = setPlayer;}
	void SetInvincible(int value){Invincible=value;}		//設定Invincible 1無敵 0正常
	void ReleaseNowTakeObj();
	void FixSelf_Onto_ObjectTop();
	void InitialWidthHeight();								//初始化高度
	void Reset(int Wx,int Wy,bool LR=1,bool FullHealth=1);
	void ResetScore(){Score_Flower=Score_Star=0;}
	void Dead();
	void Lock(bool value = true ){LOCK=value;}
	void GetHurt();
	void Faint();
	void GodMode();
	virtual int SetNowTaken(int Value);						//設定NowTaken
	virtual void TriggerObj(int Derict);					//Derict吃花的方向
	int Score_Flower;
	int Score_Star;
	static Object	 **CanThrow;
	static MapManage *Maps;
	static Monster	 **AllMonster;
	friend class BasicInstructions;
	friend class ChipDale_Dead;
	friend class ChipDale_Resurrect;
	friend class ChipDale_Taken;
private:
	static CAnimation animation[2][2][ACTION_NUM][2];		// 動作~方向
	static CAnimation ani_sweat[2],ani_dizzy,ani_god[2];
	int IsDale;
	int ani_sweat_x;
	int ani_sweat_wy;
	int ani_sweat_count;
	int Health;
	int Life;
	int NowAct,LastAct;										// Act 0站 1走 2跳 3丟 4蹲 5受傷 6暈眩
	int NowTaken,LastTaken;									// 0空手 1拿普通東西  6蘋果... (<0 作為再次拿取延遲器)
	int freeze;												// 丟東西的冷卻時間
	bool Hurt;												// 是否受傷
	int IsFaint;											// 是否暈眩 ,counter合一
	bool ani_Hide_freeze_jump;								// 控制HIDE動畫未完成不能下跳，設定在CHIPDALE的動畫切換
	int Last_flag[6];										// 上一次按鍵的旗標，搭配flag做出放開按鍵判斷
	int Invincible;											// 1無敵 0正常 (>1作為ONSHOW的COUNTER)
	Object *NowTakeObj;
	ChipDale *Partner;
	bool IsGod;
	bool LOCK;												// 鎖setFlag
	bool Alive;												// 鎖除了setFlag以外的函式
};