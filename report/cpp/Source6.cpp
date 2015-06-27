/****************************************************************/
//Object��@													*/
//																*/
/****************************************************************/
Object::Object()
{
	NowAct = 1;			//���Y���_
	ThisObjValue = 0;
	wx = wy = 0;
	CanAttackMode = 1;
	Owener = NULL;
	Direct = 0;
}
void Object::RecoverObj(ChipDale *player){
	Owener = player;//�O��ۤv���D�H
	NowAct = 1;
	ThisObjValue = 0;
}
void Object::CollisionMonster(Monster **monster)
{
	int temp;
	if(NowAct==0 || Owener==NULL || CanAttackMode==2)	return; //NowAct==0 �ǳƧ�L , �S���D�H���� , CanAttackMode==2 ����������H
	if(NowAct==1 && Owener->ReturnHideComplete()){//NowAct==1 �B�D�H�۵� ���n���ץ�wx wy (�ثe�P�D�H����) 
		wx = Owener->ReturnWX();
		wy = Owener->ReturnWY()+Owener->ReturnHeight()-height;
	}
	else if(NowAct!=2)return; //NowAct==2 ���B�ʪ��A�~������H , ���p�GNowAct==1 ���ҥ~ �D�H�۵� �n�P�_�I��
	for(int i=0;i<ONE_LEVEL_MONSTER_NUM && monster[i]!=NULL;i++){
		if(monster[i]->ReturnNowAct()<=0)	continue; //�Ǫ��B�ʪ��A�� 0 ��ܩ|���J�� , -1 �w���`
		temp = monster[i]->IfCollision(wx,wy,width,height);//���ոI��
		switch(temp){
			case 1:case 4://�c�l�����W���U�I��Ǫ�
				if(monster[i]->KillMonster(4)){
					CollisionReact(temp,monster[i]);
					if(NowAct==1 && Owener->ReturnHideComplete())	Owener->ReleaseNowTakeObj();//�����Ǫ���X�����~
					NowAct=4;//��ܤw�g��X�h����Ǫ�
					i=ONE_LEVEL_MONSTER_NUM;//���X���j�� �H��double kill
				}
				break;
			case 2:case 3://�c�l���k�W�k�U�I��Ǫ�
				if(monster[i]->KillMonster(8)){
					CollisionReact(temp,monster[i]);
					if(NowAct==1 && Owener->ReturnHideComplete())Owener->ReleaseNowTakeObj();//�����Ǫ���X�����~
					NowAct=4;//��ܤw�g��X�h����Ǫ�
					i=ONE_LEVEL_MONSTER_NUM;//���X���j�� �H��double kill
				}
				break;
			case 5://�c�l��Ӧb�Ǫ��̭� �X�G���i��o��
				if(monster[i]->KillMonster(1)){
					CollisionReact(temp,monster[i]);
					if(NowAct==1 && Owener->ReturnHideComplete())Owener->ReleaseNowTakeObj();//�����Ǫ���X�����~
					NowAct=4;//��ܤw�g��X�h����Ǫ�
					i=ONE_LEVEL_MONSTER_NUM;//���X���j�� �H��double kill
				}
				break;
		}
	}
}
void Object::CollisionChipDale(ChipDale *player){
	if(Owener==NULL)return;//�S���D�H�����󤣧����H
	if(CanAttackMode==2 || CanAttackMode==3)return;//CanAttackMode = 2 ��ܦ����󤣨������)  ,  3 ��ܦ�����u�����Ǫ�
	if(player->ReturnInvincible())return;//�����a���L�� ������
	if(NowAct==2||NowAct==4){//NowAct==2 ��ܦ�����b�B�ʪ��L�{ �B�u���B�ʹL�{�~�|���ˤH
		if(Owener==player){
			//TRACE("Self\n");
			if(CanAttackMode==1)return;//CanAttackMode == 1 ��ܦ����󤣷|�����ۤv���D�H
			if(CanAttackMode==0 && Owener->ReturnNowAct()>3 && Owener->ReturnNowAct()!=4 
				&& !Owener->ReturnHideComplete()&&!(Direct&1))return;
			if((Direct&32)&&Owener->ReturnNowAct()==4)return;
				//��CanAttackMode==0�� �B�c�l�ۤv���D�H��NowAct>=3 �h�������D�H
		}
		int ThrowFix=(Owener==player)*(4*(LRflag)-4*(!LRflag));
		if(player->IfCollision(wx+ThrowFix,wy,width,height)){
			player->Faint();
		}
	}
}
/****************************************************************/
//ChipDale_Dead��@												*/
//																*/
/****************************************************************/
CMovingBitmap ChipDale_Dead::frame_pic[2][2];
ChipDale_Dead::ChipDale_Dead(ChipDale* chip){
	isDale	=chip->IsDale;
	LRflag	=chip->LRflag;
	wx		=chip->wx;
	wy		=chip->wy;
	Owener  =chip;
	CanAttackMode=2;
	height=frame_pic[0][0].Height();
	NowAct=2;					//�B��
	NoCollision = true;
	UpSpeed = 0;
	JumpSPEED = 30;
	MoveSPEED = SPEED/2;
	Wait_A_Minute=-1;
	CAudio::Instance()->PauseLevelMusic(CGameStateRun::NowLevel);
	CAudio::Instance()->Play(AUDIO_DEAD);
}
ChipDale_Dead::~ChipDale_Dead(){
	CAudio::Instance()->PlayLevelMusic(CGameStateRun::NowLevel);
}
void ChipDale_Dead::Loading(){
	frame_pic[0][0].LoadBitmapA("Bitmaps/action/chip/Hurt_L.bmp",PURPLE);
	frame_pic[0][1].LoadBitmapA("Bitmaps/action/chip/Hurt_R.bmp",PURPLE);
	frame_pic[1][0].LoadBitmapA("Bitmaps/action/dale/Hurt_L.bmp",PURPLE);
	frame_pic[1][1].LoadBitmapA("Bitmaps/action/dale/Hurt_R.bmp",PURPLE);
}
void ChipDale_Dead::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	//���`�����ݮɶ�
	if(Wait_A_Minute>=0){
		if(Wait_A_Minute>0)Wait_A_Minute--;
		else {
			NowAct=0;
			CAudio::Instance()->PlayLevelMusic(CGameStateRun::NowLevel);
			if((Owener->Partner==NULL||Owener->Partner->Life<=0)&&Owener->Life>0){
				//�^�����I
				map->Set_toRecord();
				Owener->Reset(10,10);
			}else if(Owener->Life<=0&&(Owener->Partner==NULL||Owener->Partner->Life<=0)){
				//GameOver
				CGameStateRun::TimeToGo=true;
			}
			
		}
		return;
	}
	//Dead����ͩR�g�������]�w
	if(wy+frame_pic[isDale][LRflag].Height()>MHEIGHT+100 ){
		Wait_A_Minute=33;
		if(Owener->Life>0&&Owener->Partner!=NULL&&Owener->Partner->Life>0){
			//�_��
			for(int i=0;i<CanTakeNum;i++){
				if(ChipDale::CanThrow[i]!=NULL)continue;
				ChipDale::CanThrow[i] = new ChipDale_Resurrect(this);
				break;
			}
		}else if((Owener->Partner==NULL||Owener->Partner->Life<=0)&&Owener->Life>0){
			//�^�����I
		}else if(Owener->Life<=0&&(Owener->Partner==NULL||Owener->Partner->Life<=0))
		{
			//GameOver
		}
	}
	if(!IsJump && wy+height<MHEIGHT){
		IsJump = true;
		Jump();
	}
	if(!LRflag&&wx+width+MoveSPEED<MWIDTH)	MoveRight(map);
	else if(LRflag&&wx-MoveSPEED>0)		MoveLeft(map);
	FallingDown(map);
}
void ChipDale_Dead::OnShow(MapManage* map){
	
	frame_pic[isDale][LRflag].SetTopLeft(wx,wy);
	frame_pic[isDale][LRflag].ShowBitmap();
}
/****************************************************************/
//ChipDale_Resurrect��@										*/
//																*/
/****************************************************************/
CMovingBitmap ChipDale_Resurrect::frame_pic[2][2];
ChipDale_Resurrect::ChipDale_Resurrect(ChipDale_Dead* chip){
	isDale	=chip->isDale;
	LRflag	=chip->LRflag;
	wx		=chip->wx;
	wy		=chip->wy;
	Owener	=chip->Owener;
	timeCount=0;
	CanAttackMode=2;
	height=frame_pic[isDale][LRflag].Height();
	width=frame_pic[isDale][LRflag].Width();
	Direct  =1;//�W��
	NowAct=2;					//�B��
	NoCollision = true;
	MoveSPEED = SPEED;
	if(wx<0)wx=0;
	if(wx+width>MWIDTH)wx=MWIDTH-width;
}
void ChipDale_Resurrect::Loading(){
	frame_pic[0][0].LoadBitmapA("Bitmaps/action/chip/Resurrect_L.bmp",PURPLE);
	frame_pic[0][1].LoadBitmapA("Bitmaps/action/chip/Resurrect_R.bmp",PURPLE);
	frame_pic[1][0].LoadBitmapA("Bitmaps/action/dale/Resurrect_L.bmp",PURPLE);
	frame_pic[1][1].LoadBitmapA("Bitmaps/action/dale/Resurrect_R.bmp",PURPLE);
}
void ChipDale_Resurrect::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(Owener->RIGHT_FLAG){
		LRflag=1;
		if(wx+width+MoveSPEED<MWIDTH)MoveRight(map);
	}
	else if(Owener->LEFT_FLAG){
		LRflag=0;
		if(wx-MoveSPEED>0 )MoveLeft(map);
	}
	//�۰ʤW�U
	if(Direct==1)MoveUp(map);
	else if(Direct==2)MoveDown(map);
	if(wy<=0)Direct=2;
	else if(wy+height>MHEIGHT) Direct=1;
	//�l��(1����A��ʥl��or10��L��۰ʥl��)
	if(((Owener->A_FLAG&&timeCount>33*1)||timeCount>33*10)&&(wy+height<MHEIGHT)){
		Owener->Reset(wx,wy,LRflag==1);
		//Owener->SetWxWy(wx,wy);
		NowAct=0;
	}else timeCount++;
	//����Q���X��
	if(wx<0)wx=0;
	if(wx+width>MWIDTH)wx=MWIDTH-width;
}
void ChipDale_Resurrect::OnShow(MapManage* map){
	frame_pic[isDale][LRflag].SetTopLeft(wx,wy);
	//1��e���{�{�ĪG
	if(timeCount>33||(timeCount%5==2||timeCount%5==3))
		frame_pic[isDale][LRflag].ShowBitmap();
}
/****************************************************************/
//ChipDale_Taken��@											*/
//Note::������O�Hchip��NowTakeObj�гy							*/
/****************************************************************/
CAnimation ChipDale_Taken::animation[2][2];
ChipDale_Taken::ChipDale_Taken(ChipDale* chip){

	isDale	=chip->Partner->IsDale;
	LRflag	=chip->LRflag;
	height=animation[0][0].Height();
	width=animation[0][0].Width();
	wx		=chip->wx;
	wy		=chip->wy-height;
	Owener  =chip;
	Owener->Partner->Alive=false;
	CanAttackMode=2;
	NowAct=1;					//���b��W
	NoCollision = true;
	UpSpeed = 0;
	JumpSPEED = 30;
	MoveSPEED = SPEED;
}
void ChipDale_Taken::Loading(){
	animation[0][0].SetDelayCount(5);
	animation[0][1].SetDelayCount(5);
	animation[1][0].SetDelayCount(5);
	animation[1][1].SetDelayCount(5);
	animation[0][0].AddBitmap("Bitmaps/action/chip/taken_1L.bmp",PURPLE);
	animation[0][0].AddBitmap("Bitmaps/action/chip/taken_2L.bmp",PURPLE);
	animation[0][0].AddBitmap("Bitmaps/action/chip/taken_3L.bmp",PURPLE);
	animation[0][1].AddBitmap("Bitmaps/action/chip/taken_1R.bmp",PURPLE);
	animation[0][1].AddBitmap("Bitmaps/action/chip/taken_2R.bmp",PURPLE);
	animation[0][1].AddBitmap("Bitmaps/action/chip/taken_3R.bmp",PURPLE);
	animation[1][0].AddBitmap("Bitmaps/action/dale/taken_1L.bmp",PURPLE);
	animation[1][0].AddBitmap("Bitmaps/action/dale/taken_2L.bmp",PURPLE);
	animation[1][0].AddBitmap("Bitmaps/action/dale/taken_3L.bmp",PURPLE);
	animation[1][1].AddBitmap("Bitmaps/action/dale/taken_1R.bmp",PURPLE);
	animation[1][1].AddBitmap("Bitmaps/action/dale/taken_2R.bmp",PURPLE);
	animation[1][1].AddBitmap("Bitmaps/action/dale/taken_3R.bmp",PURPLE);
}
void ChipDale_Taken::Throw(int setDirect){
	LRflag=Owener->LRflag;
	NowAct=2;
	Jump();
}
void ChipDale_Taken::OnMove(MapManage* map){
	if(NowAct==1){
		wx=Owener->wx;
		wy=Owener->wy-height;
		LRflag=Owener->LRflag;
	}
	else if(NowAct==2){
		if(LRflag&&wx+width<MWIDTH)
			MoveRight(map);
		else if(!LRflag&&wx>0)
			MoveLeft(map);
		IsJump = true;
		FallingDown(map);
		if(UpSpeed<=0){
			NowAct=0;
			Owener->Partner->Reset(wx,wy,LRflag==1,false);
		}
	}
}
void ChipDale_Taken::OnShow(MapManage* map){
	animation[isDale][LRflag].SetBottomLeft(wx,wy,height);
	animation[isDale][LRflag].OnMove();
	animation[isDale][LRflag].OnShow();
}
/****************************************************************/
//Apple��@														*/
//																*/
/****************************************************************/
CMovingBitmap Apple::frame_pic[2];
Apple::Apple(ChipDale *player){
	width = frame_pic[0].Width();
	height = frame_pic[0].Height();
	Owener = player;//�O��ۤv���D�H
}
void Apple::Loading(){
	frame_pic[0].LoadBitmapA("Bitmaps/object/Apple_L.bmp",PURPLE);
	frame_pic[1].LoadBitmapA("Bitmaps/object/Apple_R.bmp",PURPLE);
}
void Apple::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	else if(NowAct==1){//���b��W
		frame_pic[Owener->ReturnLR()].SetTopLeft(Owener->ReturnWX()-(width-Owener->ReturnWidth())/2,Owener->ReturnWY()-height*(!Owener->ReturnHideComplete()));
		frame_pic[Owener->ReturnLR()].ShowBitmap();
	}
	else{//��X�P���a
		frame_pic[LRflag].SetTopLeft(wx,wy);
		frame_pic[LRflag].ShowBitmap();
	}
}
void Apple::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(NowAct==1)	return;//�٦b�H����W
	if(wx < -WinShowBuffer || wx > MWIDTH || wy < -WinShowBuffer){
		NowAct = 0;
		return;
	}
	if(Direct&1)	MoveUp(map);
	if(Direct&4)	MoveLeft(map);
	if(Direct&8)	MoveRight(map);
}
void Apple::Throw(int setDirect){
	NowAct=2;
	LRflag = Owener->ReturnLR();
	Direct = setDirect;
	JumpSPEED = 0;
	MoveSPEED = SPEED *2;
	NoCollision = true;
	IsJump = false;
	if(Direct&1)Direct=1;
	else		Direct = Direct&12;
	wx = Owener->ReturnWX()-(width-Owener->ReturnWidth())/2;
	wy = Owener->ReturnWY()+Owener->ReturnHeight()-height*(!Owener->ReturnHideComplete())-40;//���a�� 40 ����
}
/****************************************************************/
//Angel��@														*/
//																*/
/****************************************************************/
CAnimation Angel::frame_pic[2];
Monster	   **Angel::monster;
Angel::Angel(int setWx,int setWy,ChipDale *player):TheBeeMaxSpeed(SPEED*2){//Angel X Y ��V�����ʳt�׳̰��� SPEED*2
	width = frame_pic[0].Width();
	height = frame_pic[0].Height();
	Owener = player;//�O��ۤv���D�H
	wx = setWx;
	wy = setWy;
	NowAttack=-1;
	CanAttackMode = 3;
	LRflag=0;
	NoCollision = true;
}
void Angel::Loading(){
	frame_pic[0].AddBitmap("Bitmaps/action/Angel/bee_1R.bmp",PURPLE);
	frame_pic[1].AddBitmap("Bitmaps/action/Angel/bee_1L.bmp",PURPLE);
	monster = ChipDale::AllMonster;
}
void Angel::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	frame_pic[LRflag].SetBottomLeft(wx,wy,height);
	frame_pic[LRflag].OnShow();
}
void Angel::OnMove(MapManage* map){
	int tempX,tempY,tempT;
	if(NowAct==0){
		return;//���`�ǳƧ�L
	}
	NowAct=2;
	if(!Owener->ReturnInvincible()){
		if(wx < -WinShowBuffer || wx > MWIDTH || wy < -WinShowBuffer){
			NowAct = 0;
			return;
		}
		MoveSPEED = TheBeeMaxSpeed;
		if(((ChipDale::Maps->GetRoute())&8)==8)	MoveLeft(map);
		else									MoveRight(map);
		MoveUp(map);
		NowAct=4;
		return;
	}
	if(NowAttack!=-1 && (monster[NowAttack]->ReturnNowAct()<=0 || monster[NowAttack]->ReturnNowAct()>=100))	NowAttack=-1;//Look if Monster is Alive?
	if(NowAttack==-1){//�����D�n�h�������@�өǪ� �j�M�ؼ�
		for(int i=0;i<ONE_LEVEL_MONSTER_NUM && monster[i]!=NULL;i++){
			if(monster[i]->ReturnNowAct()>=1 && monster[i]->ReturnNowAct()<100 && monster[i]->ReturnCanTrace()){
				NowAttack = i;
				break;
			}
		}
	}
	if(NowAttack==-1){
		LRflag = !LRflag;
		if(LRflag)	tempX = (Owener->ReturnWX()+(Owener->ReturnWidth()-width)/2-5)-wx;//���H�����߰���5px
		else		tempX = (Owener->ReturnWX()+(Owener->ReturnWidth()-width)/2+5)-wx;//���H�����߰���5px
		tempY = (Owener->ReturnWY()-height)-wy;
	}
	else{
		tempX = monster[NowAttack]->ReturnWX()-wx;
		tempY = monster[NowAttack]->ReturnWY()-wy;
		if(tempX<0)	LRflag = 1;
		else		LRflag = 0;
	}
	//���R����(�s��)
	if(tempX>tempY){
		tempT = tempX/TheBeeMaxSpeed;
		if(abs(tempX)>=TheBeeMaxSpeed) MoveSPEED=TheBeeMaxSpeed;
		else						   MoveSPEED=abs(tempX);
		if(tempX<0)					   MoveLeft(map);
		else if(tempX>0)			   MoveRight(map);

		if(abs(tempT)==0)tempT=1;
		if(abs(tempY)>=TheBeeMaxSpeed) MoveSPEED=TheBeeMaxSpeed;
		else						   MoveSPEED=abs(tempY/tempT);
		if(tempY<0)					   MoveUp(map);
		else if(tempY>0)			   MoveDown(map);
	}
	else{
		tempT = tempY/TheBeeMaxSpeed;
		if(abs(tempY)>=TheBeeMaxSpeed) MoveSPEED=TheBeeMaxSpeed;
		else						   MoveSPEED=abs(tempY);
		if(tempY<0)					   MoveUp(map);
		else if(tempY>0)			   MoveDown(map);

		if(abs(tempT)==0)tempT=1;
		if(abs(tempX)>=TheBeeMaxSpeed) MoveSPEED=TheBeeMaxSpeed;
		else						   MoveSPEED=abs(tempX/tempT);
		if(tempX<0)					   MoveLeft(map);
		else if(tempX>0)			   MoveRight(map);
	}
}
/****************************************************************/
//Ball��@														*/
//																*/
/****************************************************************/
CMovingBitmap Ball::frame_pic[2];
Ball::Ball(ChipDale *player){
	width = frame_pic[0].Width();
	height = frame_pic[0].Height();
	Owener = player;//�O��ۤv���D�H
	CanAttackMode = 1;
	LRflag=0;
	NowAct=1;
	Rebound_times=0;
	IsJump=false;
	NoCollision=false;
	MoveSPEED=SPEED*2;
	UpSpeed=0;
	JumpSPEED=40;
}
void Ball::Loading()
{
	frame_pic[0].LoadBitmapA("Bitmaps/object/Ball_L.bmp",PURPLE);
	frame_pic[1].LoadBitmapA("Bitmaps/object/Ball_R.bmp",PURPLE);
}
void Ball::OnMove(MapManage* map)
{
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(NowAct==1)	return;//�٦b�H����W
	if(NowAct==3)	{
		CanAttackMode=1;
		return;	//�b�a�W
	}
	if(wx < -WinShowBuffer || wx > MWIDTH || wy < -WinShowBuffer){
		NowAct = 0;
		return;
	}
	if(NowAct==2||NowAct==4)		   //�B�ʪ��A
	{	
		
		if(Direct&4){
			MoveLeft(map);
			LRflag=0;
			if(wx<=0){
				wx=0;
				Direct=8;
				Rebound_times--;
			}
		}
		else if(Direct&8){
			MoveRight(map);
			LRflag=1;
			if(wx+width>=MWIDTH){
				wx=MWIDTH-width;
				Direct=4;
				Rebound_times--;
			}
		}else if(Direct&1){
			MoveUp(map);
			if(wy<0){
				Direct=2;
				Rebound_times--;
			}
		}else if(!MoveDown(map)){
			Rebound_times--;
		}
		if(Rebound_times<=0){
			//Direct=0;
			IsJump=true;
			FallingDown(map);
			
			CanAttackMode=0;
			if(IsJump==false)
			{
				ThisObjValue = map->FillObstacle(OrderSize+MonsterOrderSize + 0,wx,wy); 
				wx = (ThisObjValue%E4-1)*ONEOBJX-map->ReturnNowX();
				wy = (ThisObjValue%E8/E4-1)*ONEOBJY-map->ReturnNowY();
				NowAct=5;
			}
		}
		else if(Rebound_times==1)CanAttackMode=0;
	}
}
void Ball::OnShow(MapManage* map)
{
	if(NowAct==0)	return;//���`�ǳƧ�L
	else if(NowAct==1){//���b��W
		frame_pic[Owener->ReturnLR()].SetTopLeft(Owener->ReturnWX()-(width-Owener->ReturnWidth())/2,Owener->ReturnWY()-height*(!Owener->ReturnHideComplete()));
		frame_pic[Owener->ReturnLR()].ShowBitmap();
	}
	else{//��X�P���a
		frame_pic[LRflag].SetTopLeft(wx,wy);
		frame_pic[LRflag].ShowBitmap();
	}
}
void Ball::Throw(int setdirect)
{
	NowAct=2;
	CanAttackMode=1;
	Rebound_times=2;
	wx = Owener->ReturnWX()+(Owener->ReturnWidth()-width)/2;
	if(Owener->ReturnLastAct()==4)
		wy = Owener->ReturnWY()+Owener->ReturnHeight()-height;
	else
		wy = Owener->ReturnWY();
	if(setdirect&1)
		Direct=1;
	else if(setdirect&4)
		Direct=4;
	else if(setdirect&8)
		Direct=8;
}
/****************************************************************/
//Craft��@														*/
//																*/
/****************************************************************/
CMovingBitmap Craft::frame_pic[2];
Craft::Craft(ChipDale *player){
	width = frame_pic[0].Width();
	height = frame_pic[0].Height();
	Owener = player;//�O��ۤv���D�H
}
void Craft::Loading(){
	frame_pic[0].LoadBitmapA("Bitmaps/object/Craft_L.bmp",PURPLE);
	frame_pic[1].LoadBitmapA("Bitmaps/object/Craft_R.bmp",PURPLE);
}
void Craft::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	else if(NowAct==1){//���b��W
		frame_pic[Owener->ReturnLR()].SetTopLeft(Owener->ReturnWX()-(width-Owener->ReturnWidth())/2,Owener->ReturnWY()-height*(!Owener->ReturnHideComplete()));
		frame_pic[Owener->ReturnLR()].ShowBitmap();
	}
	else{//��X�P���a
		frame_pic[LRflag].SetTopLeft(wx,wy);
		frame_pic[LRflag].ShowBitmap();
	}
}
void Craft::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(NowAct==1)	return;//�٦b�H����W
	if(wx < -WinShowBuffer || wx > MWIDTH || wy < -WinShowBuffer){
		NowAct = 0;
		return;
	}
	if(Direct&1)	MoveUp(map);
	if(Direct&4)	MoveLeft(map);
	if(Direct&8)	MoveRight(map);
}
void Craft::Throw(int setDirect){
	NowAct=2;
	LRflag = Owener->ReturnLR();
	Direct = setDirect;
	JumpSPEED = 0;
	MoveSPEED = SPEED *2;
	NoCollision = true;
	IsJump = false;
	if(Direct&1)Direct=1;
	else		Direct = Direct&12;
	wx = Owener->ReturnWX()-(width-Owener->ReturnWidth())/2;
	if(Owener->ReturnLastAct()==4)
		wy = Owener->ReturnWY()+Owener->ReturnHeight()-height;
	else
		wy = Owener->ReturnWY()+Owener->ReturnHeight()-height*(!Owener->ReturnHideComplete())-40;//���a�� 40 ����
}
void Craft::CollisionReact(int setDirect,CFrame *which){
	switch(setDirect){
	case 1:case 4:
		Direct = 9;
		break;
	case 2:case 3:
		Direct = 5;
		break;
	case 5:
		Direct = 1;
		break;
	}
}
/****************************************************************/
//Explosion��@													*/
//																*/
/****************************************************************/
CAnimation Explosion::frame_pic;
Explosion::Explosion(int setOx,int setOy,int setChangeWhat,ChipDale *player){
	MapManage *map = ChipDale::Maps;
	width = frame_pic.Width();
	height = frame_pic.Height();
	ChangeWhat = setChangeWhat;
	wx = setOx*ONEOBJX-map->ReturnNowX();
	wy = setOy*ONEOBJY-map->ReturnNowY();
	frame_pic.Reset();
	CanAttackMode = 2;
	NowAct=2;
	Owener = player;
	IfNeedReFix = false;
}
void Explosion::Loading(){
	frame_pic.SetDelayCount(2);
	frame_pic.AddBitmap("Bitmaps/action/Explosion/Explosion_1.bmp",PURPLE);
	frame_pic.AddBitmap("Bitmaps/action/Explosion/Explosion_2.bmp",PURPLE);
	frame_pic.AddBitmap("Bitmaps/action/Explosion/Explosion_3.bmp",PURPLE);
	frame_pic.AddBitmap("Bitmaps/action/Explosion/Explosion_4.bmp",PURPLE);
}
void Explosion::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	frame_pic.SetBottomLeft(wx,wy,height);
	frame_pic.OnShow();
}
void Explosion::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	Object **CanThrow = ChipDale::CanThrow;
	bool IfNeedReFix=false;
	if(frame_pic.IsFinalBitmap()){
		switch(ChangeWhat){ //���ܦ��ƻ�n�g�b�o��
			case STAR:
				for(int i=0,IfNeedReFix=false;i<CanTakeNum;i++){
					if(CanThrow[i]==this)	IfNeedReFix=true;
					if(CanThrow[i]==NULL){
						CanThrow[i] = new Star(wx,wy);
						if(IfNeedReFix)	CanThrow[i]->ReFixXY(map);
						break;
					}
					if(i+1==CanTakeNum){
						TRACE("CanThrow OverFlow!!!\n");
						Sleep(10000);
					}
				}
				break;
			case BOX_ANGLE:
				for(int i=0,IfNeedReFix=false;i<CanTakeNum;i++){
					if(CanThrow[i]==this)	IfNeedReFix=true;
					if(CanThrow[i]==NULL){
						CanThrow[i] = new Angel(wx,wy,Owener);
						break;
					}
					if(i+1==CanTakeNum){
						TRACE("CanThrow OverFlow!!!\n");
						Sleep(10000);
					}
				}
				Owener->SetInvincible(1);
				break;
			case CHEESE:
				for(int i=0,IfNeedReFix=false;i<CanTakeNum;i++){
					if(CanThrow[i]==this)	IfNeedReFix=true;
					if(CanThrow[i]==NULL){
						CanThrow[i] = new Cheese(wx,wy);
						if(IfNeedReFix)	CanThrow[i]->ReFixXY(map);
						break;
					}
					if(i+1==CanTakeNum){
						TRACE("CanThrow OverFlow!!!\n");
						Sleep(10000);
					}
				}
		}
		NowAct=0;
	}
	frame_pic.OnMove();
}
/****************************************************************/
//Star��@														*/
//																*/
/****************************************************************/
CMovingBitmap Star::frame_pic;
Star::Star(int setWx,int setWy){
	width = frame_pic.Width();
	height = frame_pic.Height();
	wx = setWx;
	wy = setWy;
	CanAttackMode = 2;
	NowAct=2;
	UpSpeed=30;
	IsJump = false;
}
void Star::Loading(){
	frame_pic.LoadBitmapA("Bitmaps/object/7.bmp",PURPLE);
}
void Star::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	frame_pic.SetTopLeft(wx,wy);
	frame_pic.ShowBitmap();
}
void Star::OnMove(MapManage* map){
	if(NowAct==-1){
		NowAct=2;
		return;
	}
	if(NowAct==0)	return;//���`�ǳƧ�L
	lastUpSpeed=UpSpeed;
	FallingDown(map);
	if(!IsJump && lastUpSpeed==0){
		map->FillObstacle(7,wx,wy);
		NowAct=0;
	}
	else if(!IsJump){
		UpSpeed = abs(lastUpSpeed)*2/3;//���a�u�� 2/3���t��
		IsJump=true;
	}
}
/****************************************************************/
//Cheese��@													*/
//																*/
/****************************************************************/
CMovingBitmap Cheese::frame_pic;
Cheese::Cheese(int setWx,int setWy){
	width = frame_pic.Width();
	height = frame_pic.Height();
	wx = setWx;
	wy = setWy;
	CanAttackMode = 2;
	NowAct=2;
	UpSpeed=30;
	IsJump = false;
}
void Cheese::Loading(){
	frame_pic.LoadBitmapA("Bitmaps/object/Cheese.bmp",PURPLE);
}
void Cheese::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	frame_pic.SetTopLeft(wx,wy);
	frame_pic.ShowBitmap();
}
void Cheese::OnMove(MapManage* map){
	Object **CanThrow = ChipDale::CanThrow;
	if(NowAct==-1){
		NowAct=2;
		return;
	}
	if(NowAct==0)	return;//���`�ǳƧ�L
	lastUpSpeed=UpSpeed;
	FallingDown(map);
	if(!IsJump && lastUpSpeed==0&&NowAct!=3){
		//map->FillObstacle(7,wx,wy);
		//NowAct=0;
		for(int i=0;i<CanTakeNum;i++){
			if(CanThrow[i]==NULL){
				CanThrow[i] = new Greedy(0,wy+height,this);
				TRACE("Greedy: %d %d\n",wx,wy);
				break;
			}
			if(i+1==CanTakeNum){
				TRACE("CanThrow OverFlow!!!\n");
				Sleep(10000);
			}
		}
		NowAct=3;
	}
	else if(!IsJump){
		UpSpeed = abs(lastUpSpeed)*2/3;//���a�u�� 2/3���t��
		IsJump=true;
	}
}
/****************************************************************/
//Stone��@														*/
//��Class����Direct�s�X => 1�W , 2�U , 4�� , 8�k ,				*/
//						   16�]�@�� , 32ReFix					*/
/****************************************************************/
CMovingBitmap Stone::frame_pic[2];
Stone::Stone(ChipDale *player){
	width = frame_pic[0].Width();
	height = frame_pic[0].Height();
	Owener = player;//�O��ۤv���D�H
	CanAttackMode = 0;
	HideThrow=false;
	ReboundLR=0;
	ReboundSpeed=0;
	NoIgnore_eat = true;
}
void Stone::Loading(){
	frame_pic[0].LoadBitmapA("Bitmaps/object/Stone_L.bmp",PURPLE);
	frame_pic[1].LoadBitmapA("Bitmaps/object/Stone_R.bmp",PURPLE);
}
void Stone::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	else if(NowAct==1){//���b��W
		frame_pic[Owener->ReturnLR()].SetTopLeft(Owener->ReturnWX()-(width-Owener->ReturnWidth())/2,Owener->ReturnWY()-height*(!Owener->ReturnHideComplete()));
		frame_pic[Owener->ReturnLR()].ShowBitmap();
	}
	else{//��X�P���a
		frame_pic[LRflag].SetTopLeft(wx,wy);
		frame_pic[LRflag].ShowBitmap();
		//TRACE("show!!\n");
	}
}
bool Stone::MoveLeft(MapManage* map){
	if(!map->IfCollision(wx-MoveSPEED,wy,MoveSPEED,height,true)){
		wx -= MoveSPEED;
		return true;
	}else{
		int i=0;
		while(!map->IfCollision(wx-i,wy,MoveSPEED,height,true))i++;
		wx-=i;
		Jump();
		ReboundLR=1;
		ReboundSpeed=(MoveSPEED-i)/4;
	}
	return false;
}
bool Stone::MoveRight(MapManage* map){
	if(!map->IfCollision(wx+width,wy,MoveSPEED,height,true)){
		wx += MoveSPEED;
		return true;
	}else{
		int i=0;
		while(!map->IfCollision(wx+width,wy,i,height,true))i++;
		wx+=i;
		Jump();
		ReboundLR=-1;
		ReboundSpeed=(MoveSPEED-i)/4;
	}
	return false;
}
void Stone::ReBound(MapManage* map){
	if(!IsJump){
		ReboundLR=0;
		ReboundSpeed=0;
	}
	if(ReboundLR==-1){
		if(!map->IfCollision(wx-MoveSPEED,wy,MoveSPEED,height,true))
			wx-=ReboundSpeed;
	}
	else if(ReboundLR==1){
		if(!map->IfCollision(wx+width,wy,MoveSPEED,height,true))
			wx+=ReboundSpeed;
	}
}
void Stone::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(NowAct==1)	return;//�٦b�H����W
	if(wx < -WinShowBuffer || wx > MWIDTH || wy < -WinShowBuffer){
		if(NowAct==3){
			map->ClearObstacle(ThisObjValue);
			TRACE("clear!!!\n");
		}
		NowAct = 0;
		return;
	}
	if(NowAct==3)	return;
	if(!IsJump){
		IsJump = false;
		ThisObjValue = map->FillObstacle(OrderSize+MonsterOrderSize + 0,wx/*+(Direct&4)/4*(5)*(!HideThrow)+(Direct&8)/8*(-5)*(!HideThrow)*/,wy); //(Direct&4)/4*(5)+(Direct&8)/8*(-5) ��X���a�᩹�^�� ���M�@�}�l���ỷ �|����ۤv
		wx = (ThisObjValue%E4-1)*ONEOBJX-map->ReturnNowX();
		wy = (ThisObjValue%E8/E4-1)*ONEOBJY-map->ReturnNowY();
		if(Direct&32){//���ۤU�{��Ǫ� �|�ץ��H����m �� Direct 32 ��s�X
			Owener->FixSelf_Onto_ObjectTop();
			Direct &= 31;
		}
		NowAct = 3;
		return;
	}
	if(Direct&4 && !(Direct&16)) {MoveLeft(map);Direct|=16;}
	if(Direct&8 && !(Direct&16)) {MoveRight(map);Direct|=16;}
	ReBound(map);

	if(FallingDown(map))ReboundLR=0;;
}
void Stone::Throw(int setDirect){
	NowAct=2;
	LRflag = Owener->ReturnLR();
	Direct = setDirect | (Direct&32);
	if(Direct&1)Direct=1;
	UpSpeed = 0;
	MoveSPEED = 45-5;
	JumpSPEED = 30;
	NoCollision = false;
	IsJump = true;
	if(Direct&1) Jump();
	//TRACE("Direct: %d\n",Direct);
	wx = Owener->ReturnWX()+(Owener->ReturnWidth()-width)/2;
	if(Owener->ReturnLastAct()==4)
		wy = Owener->ReturnWY()+Owener->ReturnHeight()-height;
	else
		wy = Owener->ReturnWY()-height*(!Owener->ReturnHideComplete());
}
void Stone::CollisionReact(int setDirect,CFrame *which){
	if(NowAct==1) Direct |= 32;//���ۤU�{��Ǫ� �|�ץ��H����m �� Direct 32 ��s�X
}
/****************************************************************/
//Greedy��@													*/
//																*/
/****************************************************************/
CAnimation Greedy::frame_pic[2];
CMovingBitmap Greedy::frame_hole;
Greedy::Greedy(int setWx,int setBottonWy,Object *setCheeseObj){
	width = frame_pic[1].Width();
	height = frame_pic[1].Height();
	wx = setWx;
	wy = setBottonWy-height;
	Owener = NULL;//�O��ۤv���D�H
	CanAttackMode = 2;
	if(setWx<=MWIDTH/2){
		LRflag=1;
		wx -= width;
	}
	else
		LRflag=0;
	NowAct = 2;
	MoveSPEED = SPEED*4/5;
	CheeseObj = setCheeseObj;
	NoCollision = false;
	showMouse=true;
}
void Greedy::Loading(){
	frame_pic[0].SetDelayCount(5);
	frame_pic[0].AddBitmap("Bitmaps/action/Greedy/Greedy_1L.bmp",PURPLE);
	frame_pic[0].AddBitmap("Bitmaps/action/Greedy/Greedy_2L.bmp",PURPLE);
	frame_pic[1].SetDelayCount(5);
	frame_pic[1].AddBitmap("Bitmaps/action/Greedy/Greedy_1R.bmp",PURPLE);
	frame_pic[1].AddBitmap("Bitmaps/action/Greedy/Greedy_2R.bmp",PURPLE);
	frame_hole.LoadBitmapA("Bitmaps/object/hole.bmp",PURPLE);
}
void Greedy::OnShow(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(showMouse && NowAct>0){//���b��W
		frame_pic[LRflag].SetBottomLeft(wx,wy,height);
		frame_pic[LRflag].OnShow();
		frame_pic[LRflag].OnMove();
	}
	else if(!showMouse){
		frame_hole.SetTopLeft(wx,wy);
		frame_hole.ShowBitmap();
	}
}
void Greedy::OnMove(MapManage* map){
	if(NowAct==0)	return;//���`�ǳƧ�L
	if(NowAct==1)	return;//�٦b�H����W
	if(!showMouse)	return;//�ܦ��}�F
	if(wx>=-width && wx<=width+MWIDTH){
		if(!(!LRflag&&MoveLeft(map)) && !(LRflag&&MoveRight(map))){
			if(!NoCollision){
				NoCollision = true;
				if(!LRflag){
					PassWx = wx;
					MoveLeft(map);
				}
				else{
					PassWx = wx+width;
					MoveRight(map);
				}
			}
		}
		else if(CheeseObj!=NULL && CheeseObj->IfCollision(wx+(width/2)*(!LRflag),wy,width/2,height)){//�u�B��ѹ����@�b�e�� �o�ˬݰ_�Ӥ�������Y��
			CheeseObj->SetNowAct(0);
			CheeseObj=NULL;
		}
		else if(NoCollision && (!LRflag&&wx<=PassWx || LRflag&&wx>=PassWx)){
			showMouse = false;
			map->FillObstacle(13,PassWx,wy-11,false);//11���g������ ��ӹϤ�(Greedy & hole)�����t��
			map->FillObstacle(13,PassWx,wy+height-34,false);//34���g���� �L����������
			wx = PassWx;
			wy -= 11;
		}
	}	
	else //�۰ʮ��� (���}��ܿù�)
		NowAct=0;
}