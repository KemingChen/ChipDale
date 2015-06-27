class Monster : public AI_Instructions{
public:
	Monster();
	virtual ~Monster(){;}
	void SetNowAct(int Value){NowAct = Value;}
	int ReturnNowAct(){return NowAct;}
	bool ReturnCanTrace(){return CanTrace;}
	virtual void CollisionChipDale(ChipDale *player);
	virtual void OnShow(MapManage *map){;}
	virtual bool OnMove(MapManage *map,ChipDale **player){return false;}
	virtual void SetMonster(MapManage *map,int SetOx,int SetOy){;}
	virtual bool KillMonster(int Direct){return false;}
	ChipDale* Detect(ChipDale **player,int *WLength,int *HLength,int WRange,int HRange,bool IfTraceInvincible=false);
	friend class ChipDale;
protected:
	int LR_flag,NowAct;					//NowAct 正常介於1-100  100UP作為彈飛動畫
	int lastWy,wait;
	int Health;
	bool CanTrace;				//是否可以被跟蹤 給其他OBJ用 (ex: Object::Angel)
};	
class bullet : public Monster{
public:
	bullet();
	virtual void OnShow(MapManage *map){;}
	bool OnMove(MapManage *map);
	int Collision(ChipDale *player){return player->IfCollision(wx,wy,width,height);}
protected:
	int MoveLR,MoveUD;
};
class MachineDog : public Monster{
public:
	MachineDog(MapManage *map,int SetOx,int SetOy);
	static void Loading();
	void OnShow(MapManage *map);
	bool OnMove(MapManage *map,ChipDale **player);
	bool KillMonster(int Direct);
private:
	static CAnimation frame_monster[2][2];	//NowAct LRflag
};
class Cactus : public Monster{
public:
	Cactus(MapManage *map,int SetOx,int SetOy);
	static void Loading();
	bool OnMove(MapManage *map,ChipDale **player);
private:
	static CMovingBitmap frame_monster;
};
class Electric : public Monster{
public:
	Electric(MapManage *map,int SetWx,int SetWy,int SetWireOLength);
	static void Loading();
	void OnShow(MapManage *map);
	bool OnMove(MapManage *map,ChipDale **player);
	int  ElectricCollision(ChipDale *player){return player->IfCollision(wx,wy,width,height);}
private:
	int MoveLength;
	int WireLength;
	const int ChangeSpeed,MaxMoveSpeed;
	static CMovingBitmap frame_monster[2];
};
class Wire : public Monster{
public:
	Wire(MapManage *map,int SetOx,int SetOy,int SetWireLength);
	~Wire(){delete(real_monster);}
	static void Loading();
	void OnShow(MapManage *map);
	bool OnMove(MapManage *map,ChipDale **player);
	void CollisionChipDale(ChipDale *player);
	void FixMapMove(int fixX,int fixY);
private:
	int WireLength;
	Electric *real_monster;
};
class Mouse : public Monster{
public:
	Mouse(MapManage *map,int SetOx,int SetOy);
	static void Loading();
	void OnShow(MapManage *map);
	bool OnMove(MapManage *map,ChipDale **player);
	bool KillMonster(int Direct);
private:
	ChipDale *tracePlayer;
	static CAnimation frame_monster[3][2];	//NowAct LRflag
};
class Wasp : public Monster{
public:
	Wasp(MapManage *map,int SetOx,int SetOy);
	static void Loading();
	void OnShow(MapManage *map);
	bool OnMove(MapManage *map,ChipDale **player);
	bool KillMonster(int Direct);
private:
	int tLRMoveSPEED,tDownMoveSPEED;
	static CAnimation frame_monster[2];
};
class Lighting : public bullet{
public:
	Lighting(MapManage *map,int SetWx,int SetWy,int LR,int UD);
	static void Loading();
	void OnShow(MapManage *map);
private:
	static CMovingBitmap frame_monster;
};
class Centipedelimbs : public Monster{
public:
	Centipedelimbs(MapManage *map,int SetWx,int SetWy,int SetSelect);
	~Centipedelimbs();
	static void Loading();
	void OnShow(MapManage *map,int countFlicker);
	bool OnMove(MapManage *map,ChipDale **player);
	int CentipedelimbsCollision(ChipDale *player);
private:
	static CAnimation frame_monster[3];
	bullet *bullets[CentipedeLightingNum];
	int Select;
	const int TotalBullTime,ReleaseBullTime,BulletMaxSpeed,WRandBullet;
};
class Centipede : public Monster{
public:
	Centipede(MapManage *map,int SetOx,int SetOy);
	~Centipede();
	static void Loading();
	void OnShow(MapManage *map);
	bool OnMove(MapManage *map,ChipDale **player);
	void CollisionChipDale(ChipDale *player);
	void FixMapMove(int fixX,int fixY);
	int IfCollision(int twx,int twy,int twidth,int theight){return real_monster[0]->IfCollision(twx,twy,twidth,theight);}
	bool KillMonster(int Direct);
private:
	Centipedelimbs *real_monster[3];
	int countFlicker;
	static CAnimation frame_monster;
};