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
	void setFlag(bool value,bool,bool,bool,bool,bool,bool);	//�� �W�U���kAB
	void SetState();										//�]�wAct LRdirect
	void SetPartner(ChipDale *setPlayer){Partner = setPlayer;}
	void SetInvincible(int value){Invincible=value;}		//�]�wInvincible 1�L�� 0���`
	void ReleaseNowTakeObj();
	void FixSelf_Onto_ObjectTop();
	void InitialWidthHeight();								//��l�ư���
	void Reset(int Wx,int Wy,bool LR=1,bool FullHealth=1);
	void ResetScore(){Score_Flower=Score_Star=0;}
	void Dead();
	void Lock(bool value = true ){LOCK=value;}
	void GetHurt();
	void Faint();
	void GodMode();
	virtual int SetNowTaken(int Value);						//�]�wNowTaken
	virtual void TriggerObj(int Derict);					//Derict�Y�᪺��V
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
	static CAnimation animation[2][2][ACTION_NUM][2];		// �ʧ@~��V
	static CAnimation ani_sweat[2],ani_dizzy,ani_god[2];
	int IsDale;
	int ani_sweat_x;
	int ani_sweat_wy;
	int ani_sweat_count;
	int Health;
	int Life;
	int NowAct,LastAct;										// Act 0�� 1�� 2�� 3�� 4�� 5���� 6�w�t
	int NowTaken,LastTaken;									// 0�Ť� 1�����q�F��  6ī�G... (<0 �@���A����������)
	int freeze;												// ��F�誺�N�o�ɶ�
	bool Hurt;												// �O�_����
	int IsFaint;											// �O�_�w�t ,counter�X�@
	bool ani_Hide_freeze_jump;								// ����HIDE�ʵe����������U���A�]�w�bCHIPDALE���ʵe����
	int Last_flag[6];										// �W�@�����䪺�X�СA�f�tflag���X��}����P�_
	int Invincible;											// 1�L�� 0���` (>1�@��ONSHOW��COUNTER)
	Object *NowTakeObj;
	ChipDale *Partner;
	bool IsGod;
	bool LOCK;												// ��setFlag
	bool Alive;												// �갣�FsetFlag�H�~���禡
};