class AI_Instructions : public CFrame{
public:
	AI_Instructions();
	bool MoveUp(MapManage* map);
	bool MoveDown(MapManage* map);
	bool MoveRight(MapManage* map);
	bool MoveLeft(MapManage* map);
	void Jump();
	int FallingDown(MapManage* map);
protected:
	int UpSpeed,MoveSPEED,JumpSPEED;
	int LR_Space;
	bool IsJump;
	bool NoCollision;								// NoCollision =	true ���n�P�_�I��
	bool NoIgnore_2,NoIgnore_eat;
};
class BasicInstructions : public CFrame{
public:
	BasicInstructions();
	bool MoveRight(MapManage* map);
	bool MoveLeft(MapManage* map);
	bool Jump();
	void Squat();
	int FallingDown(MapManage* map);
	virtual ChipDale* getPartner(){return NULL;}
	virtual int CollisionChipDale(int Direct,int passSpeed,int Squeezemode=1){return false;}
	virtual void TriggerObj(int Derict){TRACE("TriggerObj error\n");}	// Derict�Y�᪺��V
	virtual int SetNowTaken(int Value){return -1;}	//�]�wNowTaken
	virtual ChipDale* Instance(){return NULL;}
protected:
	int UpSpeed,time_jump;					// time_jump �קK�G�q��
	bool IsJump,IsRun,IsLessCollision;		// isLessCollison = true ���n�P�_�a�O2���I��(�U��Ĳ�o)
	int ani_jump_count;						// animation �����۪��p�ƾ�
	int Reduce_UP_VELOCITY;
	static bool canMoveMapX,canMoveMapY;
};