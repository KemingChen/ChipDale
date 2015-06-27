/****************************************************************/
//Monster 實作													*/
//																*/
/****************************************************************/
Monster::Monster(){
	CanTrace = true;
	wait=0;
	MoveSPEED = SPEED;
	JumpSPEED=0;
	NoCollision=false;	
	LR_Space = 0;
	wait = 0;
	UpSpeed = 0;
	LR_flag = 0;
	NowAct = 0;
	IsJump = false;
	Health = 1;
}
void Monster::CollisionChipDale(ChipDale *player){
	if(NowAct>=1&&NowAct<100){
		if(player->ReturnInvincible())return;
		if(player->ReturnHideComplete())return;
		if(player->IfCollision(wx,wy,width,height)){
				player->GetHurt();
		}
	}
}
ChipDale* Monster::Detect(ChipDale **player,int *WLength,int *HLength,int WRange,int HRange,bool IfTraceInvincible){
	ChipDale *result = NULL;
	int twx,twy,twidth,theight,tWLength,tHLength;
	MapManage *Maps = ChipDale::Maps;
	for(int i=0;i<2;i++){
		if(player[i]==NULL || player[i]->ReturnTakenByPartner())	continue;
		if(player[i]->ReturnHealth()<=0)continue;
		if(!IfTraceInvincible && player[i]->ReturnInvincible())continue;
		twidth = 0;
		theight = 0;
		twx = wx + width/2;
		twy = wy;
		if(WRange<0){
			twx -= MWIDTH;
			twidth = 2*MWIDTH + width;
		}
		else{
			twidth = WRange;
			if(!LRflag)
				twx -= WRange;
		}
		if(HRange<0){
			twy = (Maps->ReturnNowY()+twy)/MHEIGHT*MHEIGHT-Maps->ReturnNowY()-WinShowBuffer;
			theight = MHEIGHT+WinShowBuffer;
		}
		else{
			twy -= HRange;
			theight = 2*HRange + height;
		}
		if(player[i]->IfCollision(twx,twy,twidth,theight)){
			if(result!=NULL){
				tWLength = player[i]->ReturnWX() - wx;
				tHLength = player[i]->ReturnWY() - wy;
				if(tWLength*tWLength+tHLength*tHLength < (*WLength)*(*WLength)+(*HLength)*(*HLength)){
					*WLength = tWLength;
					*HLength = tHLength;
					result = player[i];
				}
			}
			else{
				*WLength = player[i]->ReturnWX() - wx;
				*HLength = player[i]->ReturnWY() - wy;
				result = player[i];
			}
		}
	}
	return result;
}
/****************************************************************/
//MachineDog 實作												*/
//																*/
/****************************************************************/
CAnimation MachineDog::frame_monster[2][2];
MachineDog::MachineDog(MapManage *map,int SetOx,int SetOy){
	MoveSPEED = SPEED*3/5;
	JumpSPEED=30;
	NoCollision=false;	
	LR_Space = 15;
	wait = 15;
	width = frame_monster[0][0].Width();
	height = frame_monster[0][0].Height();
	wx = SetOx*ONEOBJX - map->ReturnNowX();
	wy = SetOy*ONEOBJY - map->ReturnNowY();
	lastWy = wy;
	UpSpeed = 0;
	LR_flag = 0;
	NowAct = 0;
	IsJump = false;
	Health = 1;
}
void MachineDog::Loading(){
	frame_monster[0][0].SetDelayCount(5);
	frame_monster[0][0].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_stand_1L.bmp",PURPLE);
	frame_monster[0][0].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_stand_2L.bmp",PURPLE);
	frame_monster[0][1].SetDelayCount(5);
	frame_monster[0][1].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_stand_1R.bmp",PURPLE);
	frame_monster[0][1].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_stand_2R.bmp",PURPLE);
	frame_monster[1][0].SetDelayCount(5);
	frame_monster[1][0].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_run_1L.bmp",PURPLE);
	frame_monster[1][0].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_run_2L.bmp",PURPLE);
	frame_monster[1][1].SetDelayCount(5);
	frame_monster[1][1].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_run_1R.bmp",PURPLE);
	frame_monster[1][1].AddBitmap("Bitmaps/action/","MachineDog/","mechine_dog_run_2R.bmp",PURPLE);
}
void MachineDog::OnShow(MapManage *map){
	if(NowAct>0){
		frame_monster[NowAct>wait][LR_flag].SetBottomLeft(wx,wy,height);
		frame_monster[NowAct>wait][LR_flag].OnShow();
		frame_monster[NowAct>wait][LR_flag].OnMove();
	}
}
bool MachineDog::OnMove(MapManage *map,ChipDale **player){
	int MLength,HLength;
	if(NowAct<0)return false;
	FixXY(map);
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			if(Detect(player,&MLength,&HLength,-1,-1,true)!=NULL){
				LR_flag = MLength >= 0;
				NowAct=1;//啟動 
			}
		}
		else if(NowAct==100){
			MoveLeft(map);
			MoveUp(map);
		}
		else if(NowAct==101){
			MoveRight(map);
			MoveUp(map);
		}
		else if(NowAct>=1){
			if(NowAct <= wait){
				NowAct++;
			}
			else if(NowAct >= wait+1){
				if(!IsJump && NowAct >= wait+4){
					LR_flag = !LR_flag;
					NowAct = wait+1;
				}
				if(!IsJump && wy!=lastWy){
					NowAct=wait+1;
					lastWy = wy;
					LR_Space = 15;
				}
				if((LR_flag==0&&!MoveLeft(map)||LR_flag==1&&!MoveRight(map))&&!IsJump){
					IsJump = true;
					LR_Space = 0;
					Jump();
					NowAct++;
				}
			}
			FallingDown(map);
		}
		return true;
	}
	else if(NowAct>0)
		NowAct=-1;//表示活起來後又離開螢幕死掉
	return true;
}
bool MachineDog::KillMonster(int Direct){
	if(Direct&4)
		NowAct=100;
	else
		NowAct=101;
	IsJump=false;
	MoveSPEED = SPEED*2;
	NoCollision=true;	
	Health = 0;
	return true;
}
/****************************************************************/
//Cactus 實作													*/
//																*/
/****************************************************************/
CMovingBitmap Cactus::frame_monster;
Cactus::Cactus(MapManage *map,int SetOx,int SetOy){
	wx = SetOx*ONEOBJX - map->ReturnNowX();
	wy = SetOy*ONEOBJY - map->ReturnNowY();
	width = frame_monster.Width();
	height = frame_monster.Height();
	NowAct = 0;
	CanTrace=false;
}
void Cactus::Loading(){
	frame_monster.LoadBitmapA("Bitmaps/action/Cactus/Cactus.bmp",PURPLE);
}
bool Cactus::OnMove(MapManage *map,ChipDale **player){
	if(NowAct<0)return false;
	FixXY(map);
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			NowAct=1;//啟動 
		}
	}
	else if(NowAct>0)
		NowAct=-1;//表示活起來後又離開螢幕死掉
	return true;
}
/****************************************************************/
//Electric & Wire 實作											*/
//																*/
/****************************************************************/
///////////////////////////////////////////////////////////
//Electric 屬於 Wire 架框的實際怪物
CMovingBitmap Electric::frame_monster[2];
Electric::Electric(MapManage *map,int SetWx,int SetWy,int SetWireLength):ChangeSpeed(4),MaxMoveSpeed((int)(SPEED*1.5)){
	MoveLength=0;//移動距離 和 顯示時使用
	NoCollision=true;
	width = frame_monster[0].Width();
	height = frame_monster[0].Height();
	wx = SetWx;
	wy = SetWy;
	NowAct = 1;
	WireLength = SetWireLength;
	Health = 1;
	LRflag = 1;//電流的流動方向
	CanTrace=false;
}
void Electric::Loading(){
	frame_monster[0].LoadBitmapA("Bitmaps/action/Electric/Electric1.bmp",PURPLE);
	frame_monster[1].LoadBitmapA("Bitmaps/action/Electric/Electric2.bmp",PURPLE);
}
void Electric::OnShow(MapManage *map){
	if(NowAct>0){
		frame_monster[(MoveLength%(ChangeSpeed*2))>=ChangeSpeed].SetTopLeft(wx,wy);
		frame_monster[(MoveLength%(ChangeSpeed*2))>=ChangeSpeed].ShowBitmap();
	}
}
bool Electric::OnMove(MapManage *map,ChipDale **player){
	if(NowAct<0)return false;
	if(WireLength == MoveLength){
		LRflag = !LRflag;
		MoveLength = 0;
	}
	if(WireLength-MoveLength >= MaxMoveSpeed)	MoveSPEED = MaxMoveSpeed;
	else										MoveSPEED = WireLength-MoveLength;
	MoveLength += MoveSPEED;
	if(!LRflag)	MoveLeft(map);
	else		MoveRight(map);

	return true;
}
///////////////////////////////////////////////////////////
//Wire
Wire::Wire(MapManage *map,int SetOx,int SetOy,int SetWireOLength){
	NoCollision=true;
	WireLength = SetWireOLength*ONEOBJX;
	wx = SetOx*ONEOBJX - map->ReturnNowX();
	wy = SetOy*ONEOBJY - map->ReturnNowY();
	NowAct = 0;
	Health = 1;
	CanTrace=false;
	real_monster = new Electric(map,wx,wy,WireLength);
}
void Wire::Loading(){
	Electric::Loading();
}
void Wire::OnShow(MapManage *map){
	if(NowAct>0){
		real_monster->OnShow(map);
	}
}
bool Wire::OnMove(MapManage *map,ChipDale **player){
	if(NowAct<0)return false;
	FixXY(map);
	real_monster->FixXY(map);
	if(wx > -WinShowBuffer-WireLength && wx < MWIDTH+WireLength && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			NowAct=1;//啟動 
		}
		if(NowAct==1){
			real_monster->OnMove(map,player);
		}
		return true;
	}
	else if(NowAct>0){
		NowAct=-1;//表示活起來後又離開螢幕死掉
	}
	return true;
}
void Wire::CollisionChipDale(ChipDale *player){
	if(NowAct>=1&&NowAct<100){
		if(player->ReturnInvincible())return;
		if(real_monster->ElectricCollision(player)){
				player->GetHurt();
		}
	}
}
void Wire::FixMapMove(int fixX,int fixY)
{
	wx -= fixX;
	wy -= fixY;
	real_monster->FixMapMove(fixX,fixY);
}
/****************************************************************/
//Mouse 實作													*/
//																*/
/****************************************************************/
CAnimation Mouse::frame_monster[3][2];
Mouse::Mouse(MapManage *map,int SetOx,int SetOy){
	MoveSPEED = SPEED*3/5;
	NoCollision=false;	
	LR_Space = 0;
	width = frame_monster[0][0].Width();
	height = frame_monster[0][0].Height();
	wx = SetOx*ONEOBJX - map->ReturnNowX();
	wy = SetOy*ONEOBJY - map->ReturnNowY();
	UpSpeed = 0;
	LR_flag = 0;
	NowAct = 0;
	IsJump = false;
	Health = 1;
}

void Mouse::Loading(){
	frame_monster[0][0].SetDelayCount(5);
	frame_monster[0][0].AddBitmap("Bitmaps/action/","Mouse/","mouse_stand_L.bmp",PURPLE);
	frame_monster[0][1].SetDelayCount(5);
	frame_monster[0][1].AddBitmap("Bitmaps/action/","Mouse/","mouse_stand_R.bmp",PURPLE);
	frame_monster[1][0].SetDelayCount(5);
	frame_monster[1][0].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_1L.bmp",PURPLE);
	frame_monster[1][0].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_2L.bmp",PURPLE);
	frame_monster[1][0].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_3L.bmp",PURPLE);
	frame_monster[1][0].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_4L.bmp",PURPLE);
	frame_monster[1][1].SetDelayCount(5);
	frame_monster[1][1].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_1R.bmp",PURPLE);
	frame_monster[1][1].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_2R.bmp",PURPLE);
	frame_monster[1][1].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_3R.bmp",PURPLE);
	frame_monster[1][1].AddBitmap("Bitmaps/action/","Mouse/","mouse_Run_4R.bmp",PURPLE);
	frame_monster[2][0].SetDelayCount(5);
	frame_monster[2][0].AddBitmap("Bitmaps/action/","Mouse/","mouse_Jump_L.bmp",PURPLE);
	frame_monster[2][1].SetDelayCount(5);
	frame_monster[2][1].AddBitmap("Bitmaps/action/","Mouse/","mouse_Jump_R.bmp",PURPLE);
}
void Mouse::OnShow(MapManage *map){
	int showAct;
	if(NowAct>0){
		if(!IsJump)		showAct=0;
		else			showAct=2;
		if(NowAct>=50)	showAct=1;
		frame_monster[showAct][LR_flag].SetBottomLeft(wx,wy,height);
		frame_monster[showAct][LR_flag].OnShow();
		frame_monster[showAct][LR_flag].OnMove();
	}
}
bool Mouse::OnMove(MapManage *map,ChipDale **player){
	int MLength,HLength;
	if(NowAct<0)return false;
	FixXY(map);
	if(wx > -WinShowBuffer && wx < MWIDTH+WinShowBuffer && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0 && wx > 0 && wx < MWIDTH && wy > 0 && wy < MHEIGHT){
			if(Detect(player,&MLength,&HLength,-1,-1,true)!=NULL){
				LR_flag = MLength >= 0;
				NowAct=1;//啟動 
			}
			NowAct=1;//啟動 
		}
		else if(NowAct==100){
			MoveLeft(map);
			MoveUp(map);
		}
		else if(NowAct==101){
			MoveRight(map);
			MoveUp(map);
		}
		else if(NowAct>=1){
			tracePlayer = Detect(player,&MLength,&HLength,-1,0);
			if(tracePlayer!=NULL){
				LR_flag = MLength >= 0;
				NowAct = 50;
			}
			if(NowAct <= wait){
				NowAct++;
			}
			else if(NowAct >= wait+1 && NowAct < 50){
				if(!IsJump){
					if(NowAct >= wait+5*4){
						LR_flag = !LR_flag;
						NowAct = wait+1;
					}
					if((NowAct-wait)%4==1){
						IsJump = true;
						JumpSPEED=30;
						Jump();
					}
					NowAct++;

				}
				else{
					if(LR_flag==0){
						if(!MoveLeft(map)){
							LR_flag = !LR_flag;
							NowAct = wait+2;
						}
					}
					else if(LR_flag==1){
						if(!MoveRight(map)){
							LR_flag = !LR_flag;
							NowAct = wait+2;
						}
					}
				}
			}
			else if(NowAct >= 50){
				if((LR_flag==0&&!MoveLeft(map)||LR_flag==1&&!MoveRight(map)) && !IsJump){
					IsJump = true;
					JumpSPEED=30;
					Jump();
					if(NowAct==52){
						LR_flag = !LR_flag;
						NowAct = 50;
					}
					NowAct++;
				}
				if(Detect(player,&MLength,&HLength,-1,height)==NULL){
					NowAct = wait+1;
					//LR_flag = !LR_flag;
				}
			}
			FallingDown(map);
		}
		return true;
	}
	else if(NowAct>0)
		NowAct=-1;//表示活起來後又離開螢幕死掉
	return true;
}
bool Mouse::KillMonster(int Direct){
	if(Direct&4)
		NowAct=100;
	else
		NowAct=101;
	IsJump=false;
	MoveSPEED = SPEED*2;
	NoCollision=true;	
	Health = 0;
	return true;
}
/****************************************************************/
//Wasp 實作														*/
//																*/
/****************************************************************/
CAnimation Wasp::frame_monster[2];
Wasp::Wasp(MapManage *map,int SetOx,int SetOy){
	NoCollision=true;	
	width = frame_monster[0].Width();
	height = frame_monster[0].Height();
	wx = SetOx*ONEOBJX - map->ReturnNowX();
	wy = SetOy*ONEOBJY - map->ReturnNowY();
	NowAct = 0;
	Health = 1;
	MoveSPEED = 3;
}
void Wasp::Loading(){
	frame_monster[0].SetDelayCount(5);
	frame_monster[0].AddBitmap("Bitmaps/action/","Wasp/","Wasp_1L.bmp",PURPLE);
	frame_monster[0].AddBitmap("Bitmaps/action/","Wasp/","Wasp_2L.bmp",PURPLE);
	frame_monster[1].SetDelayCount(5);
	frame_monster[1].AddBitmap("Bitmaps/action/","Wasp/","Wasp_1R.bmp",PURPLE);
	frame_monster[1].AddBitmap("Bitmaps/action/","Wasp/","Wasp_2R.bmp",PURPLE);
}
void Wasp::OnShow(MapManage *map){
	if(NowAct>0){
		frame_monster[LR_flag].SetBottomLeft(wx,wy,height);
		frame_monster[LR_flag].OnShow();
		frame_monster[LR_flag].OnMove();
	}
}
bool Wasp::OnMove(MapManage *map,ChipDale **player){
	int MLength,HLength;
	if(NowAct<0)return false;
	FixXY(map);
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			if(Detect(player,&MLength,&HLength,-1,-1,true)!=NULL){
				LR_flag = MLength >= 0;
				NowAct=1;//啟動 
			}
		}
		else if(NowAct==100){
			MoveLeft(map);
			MoveUp(map);
		}
		else if(NowAct==101){
			MoveRight(map);
			MoveUp(map);
		}
		else if(NowAct>=1){
			if(NowAct <= wait){
				NowAct++;
			}
			else if(NowAct >= wait+1){
				if(NowAct == wait+1){
					MoveDown(map);
					if(Detect(player,&MLength,&HLength,-1,height/2)){
						NowAct++;
					}
				}
				else{
					MoveSPEED = 1;
					MoveDown(map);
					MoveSPEED = SPEED;
					if(LR_flag==0)	MoveLeft(map);
					if(LR_flag==1)	MoveRight(map);
				}
			}
		}
		return true;
	}
	else if(NowAct>0)
		NowAct=-1;//表示活起來後又離開螢幕死掉
	return true;
}
bool Wasp::KillMonster(int Direct){
	if(Direct&4)
		NowAct=100;
	else
		NowAct=101;
	IsJump=false;
	MoveSPEED = SPEED*2;
	NoCollision=true;	
	Health = 0;
	return true;
}
/****************************************************************/
//bullet 實作	怪物所有用的子彈								*/
//				需要另外做onshow								*/
/****************************************************************/
bullet::bullet(){
	NoCollision=true;
	NowAct = 1;
	Health = 1;
	LRflag = 1;
	CanTrace=false;
}
bool bullet::OnMove(MapManage *map){
	if(NowAct<0)return false;
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			NowAct=1;//啟動 
		}
		if(NowAct==1){
			MoveSPEED = abs(MoveLR);
			if(MoveLR<0)	MoveLeft(map);
			if(MoveLR>0)	MoveRight(map);
			MoveSPEED = abs(MoveUD);
			if(MoveUD<0)	MoveUp(map);
			if(MoveUD>0)	MoveDown(map);
		}
		return true;
	}
	else if(NowAct>0){
		NowAct=-1;//表示活起來後又離開螢幕死掉
	}
	return true;
}
/****************************************************************/
//Centipede &  Centipedelimbs 實作								*/
//																*/
/****************************************************************/
///////////////////////////////////////////////////////////
//Lighting
CMovingBitmap Lighting::frame_monster;
Lighting::Lighting(MapManage *map,int SetWx,int SetWy,int LR,int UD){
	wx = SetWx;
	wy = SetWy;
	MoveLR=LR;
	MoveUD=UD;
}
void Lighting::Loading(){
	frame_monster.LoadBitmapA("Bitmaps/action/Centipede/Lightning.bmp",PURPLE);
}
void Lighting::OnShow(MapManage *map){
	if(NowAct>0){
		frame_monster.SetTopLeft(wx,wy);
		frame_monster.ShowBitmap();
	}
}
///////////////////////////////////////////////////////////
//Centipedelimbs
CAnimation Centipedelimbs::frame_monster[3];
Centipedelimbs::Centipedelimbs(MapManage *map,int SetWx,int SetWy,int SetSelect):ReleaseBullTime(15),TotalBullTime(50),BulletMaxSpeed(10),WRandBullet(100){
	NoCollision=true;
	wx = SetWx;
	wy = SetWy;
	NowAct = 0;
	Health = 1;
	CanTrace=false;
	Select = SetSelect;
	width = frame_monster[Select].Width();
	height = frame_monster[Select].Height();
	for(int i=0;i<CentipedeLightingNum;i++)
		bullets[i] = NULL;
}
Centipedelimbs::~Centipedelimbs(){
	for(int i=0;i<CentipedeLightingNum;i++)
		if(bullets[i]!=NULL)
			delete(bullets[i]);
}
void Centipedelimbs::Loading(){
	frame_monster[0].SetDelayCount(2);
	frame_monster[0].AddBitmap("Bitmaps/action/","Centipede/","head.bmp",PURPLE);
	frame_monster[0].AddBitmap("Bitmaps/action/","Centipede/","head_die.bmp",PURPLE);
	frame_monster[1].SetDelayCount(1);
	frame_monster[1].AddBitmap("Bitmaps/action/","Centipede/","L_Fist.bmp",PURPLE);
	frame_monster[1].AddBitmap("Bitmaps/action/","Centipede/","L_Cloth.bmp",PURPLE);
	frame_monster[2].SetDelayCount(1);
	frame_monster[2].AddBitmap("Bitmaps/action/","Centipede/","R_Fist.bmp",PURPLE);
	frame_monster[2].AddBitmap("Bitmaps/action/","Centipede/","R_Cloth.bmp",PURPLE);
}
void Centipedelimbs::OnShow(MapManage *map,int countFlicker){
	if(NowAct<0)return;
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		switch(Select){
		case 0:
			if(countFlicker==0)
				frame_monster[0].Reset();
			frame_monster[0].SetBottomLeft(wx,wy,height);
			frame_monster[0].OnShow();
			frame_monster[0].OnMove();
			break;
		case 1:case 2:
			if(TotalBullTime-NowAct<=ReleaseBullTime)	frame_monster[Select].OnMoveToNum(1);
			else										frame_monster[Select].OnMoveToNum(0);
			frame_monster[Select].SetBottomLeft(wx,wy,height);
			frame_monster[Select].OnShow();
			for(int i=0;i<CentipedeLightingNum;i++){
				if(bullets[i]!=NULL)
					bullets[i]->OnShow(map);
			}
			break;
		}
	}
}
bool Centipedelimbs::OnMove(MapManage *map,ChipDale **player){
	int WLength,HLength,UD,LR;
	if(NowAct<0)return false;
	for(int i=0;i<CentipedeLightingNum;i++){
		if(bullets[i]!=NULL)
			bullets[i]->FixXY(map);
	}
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			NowAct=1;//啟動 
		}
		if(NowAct>=1){
			if(Select>=1){//Select 1 & 2 左手跟右手   ,  0 是頭
				if(TotalBullTime-NowAct==ReleaseBullTime){
					for(int i=0;i<CentipedeLightingNum;i++){
						if(bullets[i]!=NULL){
							if(bullets[i]->ReturnNowAct()<0){
								delete(bullets[i]);
								bullets[i]=NULL;
							}
							continue;
						}
						else{
							Detect(player,&WLength,&HLength,-1,-1,true);
							WLength += rand()%(WRandBullet*2)-WRandBullet;
							if(WLength==0&&HLength==0)	continue;
							if(abs(WLength)>abs(HLength)){
								LR = WLength>0 ? BulletMaxSpeed:-BulletMaxSpeed;
								UD = HLength/(WLength/LR);
							}
							else{
								UD = HLength>0 ? BulletMaxSpeed:-BulletMaxSpeed;
								LR = WLength/(HLength/UD);
							}
						}
						if(Select==1){
							bullets[i] = new Lighting(map,wx+36,wy+32,LR,UD);
							break;
						}
						else if(Select==2){
							bullets[i] = new Lighting(map,wx+10,wy+23,LR,UD);
							break;
						}
					}
				}
				NowAct++;
				if(NowAct>50)NowAct=1;
			}
			for(int i=0;i<CentipedeLightingNum;i++){
				if(bullets[i]!=NULL)
					bullets[i]->OnMove(map);
			}
		}
		return true;
	}
	else if(NowAct>0){
		NowAct=-1;//表示活起來後又離開螢幕死掉
	}
	return true;
}
int Centipedelimbs::CentipedelimbsCollision(ChipDale *player){
	if(player->IfCollision(wx,wy,width,height))
		return true;
	for(int i=0;i<CentipedeLightingNum;i++){
		if(bullets[i]!=NULL && bullets[i]->Collision(player))
			return true;
	}
	return false;
}
///////////////////////////////////////////////////////////
//Centipede
CAnimation Centipede::frame_monster;
Centipede::Centipede(MapManage *map,int SetOx,int SetOy){
	NoCollision=true;
	wx = SetOx*ONEOBJX - map->ReturnNowX();
	wy = SetOy*ONEOBJY - map->ReturnNowY();
	width = frame_monster.Width();
	height = frame_monster.Height();
	TRACE("Centipede!!!  wx %d  , wy %d\n",wx,wy);
	NowAct = 0;
	Health = 5;
	CanTrace=false;
	real_monster[0] = new Centipedelimbs(map,wx+180,wy-32,0);
	real_monster[1] = new Centipedelimbs(map,wx-22,wy+52,1);
	real_monster[2] = new Centipedelimbs(map,wx+321,wy-24,2);
	countFlicker=0;
}
Centipede::~Centipede(){
	for(int i=0;i<3;i++)
		delete(real_monster[i]);
}
void Centipede::Loading(){
	frame_monster.SetDelayCount(2);
	frame_monster.AddBitmap("Bitmaps/action/","Centipede/","Centipede_body_normal.bmp",PURPLE);
	frame_monster.AddBitmap("Bitmaps/action/","Centipede/","Centipede_body_die.bmp",PURPLE);
	Lighting::Loading();
	Centipedelimbs::Loading();
}
void Centipede::OnShow(MapManage *map){
	if(NowAct<0)return;
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(countFlicker==0)
			frame_monster.Reset();
		else
			countFlicker--;
		frame_monster.SetBottomLeft(wx,wy,height);
		frame_monster.OnShow();
		frame_monster.OnMove();
		for(int i=0;i<3;i++)
			real_monster[i]->OnShow(map,countFlicker);
	}
}
bool Centipede::OnMove(MapManage *map,ChipDale **player){
	if(NowAct<0)return false;
	FixXY(map);
	real_monster[0]->FixXY(map);
	real_monster[1]->FixXY(map);
	real_monster[2]->FixXY(map);
	if(wx > -WinShowBuffer && wx < MWIDTH && wy > -WinShowBuffer && wy < MHEIGHT){
		if(NowAct==0){
			NowAct=1;//啟動 
		}
		if(NowAct==1){
			for(int i=1;i<3;i++)//real_monster[0] 基本上不會有OnMove 
				real_monster[i]->OnMove(map,player);
		}
		return true;
	}
	else if(NowAct>0){
		NowAct=-1;//表示活起來後又離開螢幕死掉
	}
	return true;
}
void Centipede::CollisionChipDale(ChipDale *player){
	if(NowAct>=1&&NowAct<100){
		if(player->ReturnInvincible())return;
		if(real_monster[1]->CentipedelimbsCollision(player)||real_monster[2]->CentipedelimbsCollision(player)){
			player->GetHurt();
		}
	}
}
void Centipede::FixMapMove(int fixX,int fixY)
{
	wx -= fixX;
	wy -= fixY;
	real_monster[0]->FixMapMove(fixX,fixY);
	real_monster[1]->FixMapMove(fixX,fixY);
	real_monster[2]->FixMapMove(fixX,fixY);
}
bool Centipede::KillMonster(int Direct){
	if(countFlicker!=0)	return false;
	Health--;
	if(Health>0){
		countFlicker = 50;
		return false;
	}
	else{
		CGameStateRun::ToBonus();
		NowAct=-1;
		return true;
	}
}