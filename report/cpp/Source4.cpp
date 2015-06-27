/****************************************************************/
//AI_Instructions實作											*/
//																*/
/****************************************************************/
AI_Instructions::AI_Instructions()
{
	UpSpeed=0;
	IsJump = false;
	LR_Space = 0;
	NoIgnore_2 = true;
	NoIgnore_eat = false;
}
bool AI_Instructions::MoveUp(MapManage* map){
	if(NoCollision || !map->IfCollision(wx,wy-MoveSPEED,width,height,true)){
		wy -= MoveSPEED;
		return true;
	}
	return false;
}
bool AI_Instructions::MoveDown(MapManage* map){
	if(NoCollision || !map->IfCollision(wx,wy+MoveSPEED,width,height,true)){
		wy += MoveSPEED;
		return true;
	}
	return false;
}
bool AI_Instructions::MoveLeft(MapManage* map){
	if(NoCollision || !map->IfCollision(wx-MoveSPEED,wy,MoveSPEED,height,true)){
		wx -= MoveSPEED;
		return true;
	}
	return false;
}
bool AI_Instructions::MoveRight(MapManage* map){
	if(NoCollision || !map->IfCollision(wx+width,wy,MoveSPEED,height,true)){
		wx += MoveSPEED;
		return true;
	}
	return false;
}
void AI_Instructions::Jump()
{
	UpSpeed = JumpSPEED;
}
int AI_Instructions::FallingDown(MapManage* map)
{
	int i=0;
	int fixY=0,fixX=0;
	int IsLand=false;
	//修正落下時左右兩邊應該空多少不判斷
	wx += LR_Space;
	width -= 2*LR_Space;
	//最後面記得要回復原本設定
	//落下過程的FLAG初始化
	if(IsJump || !map->IfCollision(wx+fixX,wy+height+fixY,width,1,false,NoIgnore_eat/*ONEOBJY*/)){
		UpSpeed -= GRAVITY;
		IsJump = true;
	}
	///////////////////////////////
	//以下部分為操作速度的實作過程
	///////////////////////////////
	if(UpSpeed<0 && map->IfCollision(wx+fixX,wy+height+fixY,width,abs(UpSpeed),false,NoIgnore_eat)){
		//落地動作
		while(!map->IfCollision(wx+fixX,wy+height+i+fixY,width,1,false,NoIgnore_eat))i++;/* 碰撞的修正 */
		wy += i;
		UpSpeed = 0;
		IsJump = false;
		IsLand=true;
	}
	else if(UpSpeed>0 && map->IfCollision(wx+fixX,wy+fixY-abs(UpSpeed),width,abs(UpSpeed),NoIgnore_2)){
		/*向上跳過程遇到障礙物*/
		i=0;while(!map->IfCollision(wx+fixX,wy-i+fixY-1,width,1,NoIgnore_2))i++;// 碰撞的修正 
		wy -= i;
		UpSpeed = 0;
	}
	else{
		//跳躍過程
		wy -= UpSpeed;
	}
	if(map->IfCollision(wx+fixX,wy+height+fixY,width,1,false,NoIgnore_eat/*ONEOBJY*/)){
		IsLand=true;
		IsJump = false;
	}
	//修正落下時左右兩邊應該空多少不判斷 回復原本設定
	wx -= LR_Space;
	width += 2*LR_Space;
	return IsLand;
}
/****************************************************************/
//BasicInstructions實作											*/
//																*/
/****************************************************************/
bool BasicInstructions::canMoveMapX = true;
bool BasicInstructions::canMoveMapY = true;
BasicInstructions::BasicInstructions()
{
	UpSpeed=0;
	time_jump=0;
	IsJump = false;
	IsRun  = false;
	IsLessCollision = false;
	ani_jump_count=TIMEFOR_ANI_JUMP+1;
	Reduce_UP_VELOCITY = 0;
}
bool BasicInstructions::MoveLeft(MapManage* map){
	int i,limitX = (MWIDTH + width)/2,temp,testTemp;
	int UnVisibleStone = OrderSize+MonsterOrderSize +0;
	TriggerObj(3);
	if(ToolCDC::ReturnStage()!=0)return false;	//防止已經存圖又移動
	//抓人
	if(Instance()->Partner!=NULL&&CollisionChipDale(4,2,0)&&!Instance()->ReturnIsTaken()&&Instance()->A_FLAG
		&&!Instance()->Partner->ReturnIsTaken()&&!Instance()->Partner->ReturnInvincible()){
		Object** CanThrow=ChipDale::CanThrow;
		for(int i=0;i<CanTakeNum;i++){
			if(CanThrow[i]==NULL){
				//if(Partner->Alive)break; //註解掉此行會有殘影效果
				CanThrow[i]=new ChipDale_Taken(Instance());
				Instance()->NowTakeObj=CanThrow[i];
				Instance()->NowTaken=ChipDale_taken;
				Instance()->A_FLAG = false;
				break;
			}
		}
	}
	//拿東西
	if(A_FLAG && !SetNowTaken(-1)){
		temp = map->IfCollision(wx-SPEED-ONEOBJX,wy,ONEOBJX,height,true,false);
		testTemp = map->IfCollision(wx-SPEED-ONEOBJX,wy,ONEOBJX,height,true,false,true);
		if(testTemp/E8==5 || temp/E8==5 && testTemp/E8!=5 && testTemp/E8!=0)	temp = testTemp;
		if(testTemp/E8==UnVisibleStone || temp/E8==UnVisibleStone && testTemp/E8!=UnVisibleStone && testTemp/E8!=0)	temp = testTemp;
		if(temp!=1&&temp!=2&&temp!=0&&temp/E8!=4){
			map->ClearObstacle(temp);
			SetNowTaken(temp);
			A_FLAG = false;
		}
	}
	temp = CollisionChipDale(4,SPEED,0);
	//移動
	if(!map->IfCollision(wx-SPEED,wy,SPEED,height,true) && !temp){

		SetWxWy(-SPEED,0,true);
		if(canMoveMapX && wx < limitX)
			map->MoveMap(4);
		return true;
	}
	else if(temp) {
		//FIRST 腳色完全貼近
		i=SPEED-1;while(i>=0 && CollisionChipDale(4,i,0))i--;
		SetWxWy(-i,0,true);
		//移動剩餘額度 SPEED-i

		//貼近之後推擠對方
		if(!getPartner()->ReturnHideComplete()&&!IfCollision(4,1))
		CollisionChipDale(4,1);
		//帶動螢幕
		if(canMoveMapX && wx < limitX)
			map->MoveMap(4,1);
		return true;
	}
	else {
		//撞到地圖障礙物修正
		i=0;while(!map->IfCollision(wx-i,wy,width,height,true))i++;
		wx -=(i!=0)*(i-1)+(i==0)*(-SPEED);
	}
	return false;
}
bool BasicInstructions::MoveRight(MapManage* map){
	int i,limitX = (MWIDTH + width)/2,temp,testTemp;
	int UnVisibleStone = OrderSize+MonsterOrderSize +0;
	TriggerObj(4);
	if(ToolCDC::ReturnStage()!=0)return false;	//防止已經存圖又移動
	//抓人
	if(Instance()->Partner!=NULL&&CollisionChipDale(8,2,0)&&!Instance()->ReturnIsTaken()&&Instance()->A_FLAG
		&&!Instance()->Partner->ReturnIsTaken()&&!Instance()->Partner->ReturnInvincible()){
		Object** CanThrow=ChipDale::CanThrow;
		
		for(int i=0;i<CanTakeNum;i++){
			if(CanThrow[i]==NULL){
				CanThrow[i]=new ChipDale_Taken(Instance());
				Instance()->NowTakeObj=CanThrow[i];
				Instance()->NowTaken=ChipDale_taken;
				Instance()->A_FLAG = false;
				break;
			}
		}
	}
	//拿東西
	if(A_FLAG && !SetNowTaken(-1)){
		temp = map->IfCollision(wx+width+SPEED,wy,ONEOBJX,height,true,false);
		testTemp = map->IfCollision(wx+width+SPEED,wy,ONEOBJX,height,true,false,true);
		if(testTemp/E8==5 || temp/E8==5 && testTemp/E8!=5 && testTemp/E8!=0)	temp = testTemp;
		if(testTemp/E8== UnVisibleStone|| temp/E8==UnVisibleStone && testTemp/E8!=UnVisibleStone && testTemp/E8!=0)	temp = testTemp;
		if(temp!=1&&temp!=2&&temp!=0&&temp/E8!=4){//箱子...
			map->ClearObstacle(temp);
			SetNowTaken(temp);
			A_FLAG = false;
		}
	}
	//移動
	temp = CollisionChipDale(8,SPEED,0);
	if(!map->IfCollision(wx+width,wy,SPEED,height,true) && !temp){
		SetWxWy(SPEED,0,true);
		if(canMoveMapX && wx > limitX)
			map->MoveMap(8);
		return true;
	}
	else if(temp){
		//FIRST 腳色完全貼近
		i=SPEED-1;while(i>=0 && CollisionChipDale(8,i,0))i--;
		SetWxWy(i,0,true);
		//移動剩餘額度 SPEED-i
		//貼近之後推擠對方
		if(!getPartner()->ReturnHideComplete()&&!IfCollision(8,1))
		CollisionChipDale(8,1);
		//帶動螢幕
		if(canMoveMapX && wx > limitX)
			map->MoveMap(8,1);
		return true;
	}
	else{
		i=0;while(!map->IfCollision(wx+i,wy,width,height,true))i++;
		wx +=(i!=0)*(i-1)+(i==0)*(-SPEED);
	}
	return false;
}
bool BasicInstructions::Jump()
{
	if(ToolCDC::ReturnStage()!=0)return false;	//防止已經存圖又移動
	if(time_jump < TIMEFORJUMP && !IsLessCollision){
		if(B_FLAG){
			UpSpeed = UP_VELOCITY - (time_jump-1)*GRAVITY -Reduce_UP_VELOCITY;
			time_jump++;
		}
		else{
			time_jump = TIMEFORJUMP;
			UpSpeed = UP_VELOCITY - (TIMEFORJUMP-1)*GRAVITY -Reduce_UP_VELOCITY;
		}
		return true;
	}
	return false;
}
int BasicInstructions::FallingDown(MapManage* map)
{
	int i=0,limitY = (MHEIGHT )/2- height;
	int If_Reset_State=false,tempFixY=0,tempFixX=0;
	if(ToolCDC::ReturnStage()!=0)return If_Reset_State;	//防止已經存圖又移動
	//暫時性修正　讓遊戲在未修正前也能判斷正常
	if(map->MoveMap(0)&1)tempFixY = Jump_Fix;
	if(map->MoveMap(0)&4)tempFixX = SPEED;
	if(map->MoveMap(0)&8)tempFixX = -SPEED;
	//落下過程的FLAG初始化 // 預設是不進行跳躍
	if(!IsLessCollision && !map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,1/*ONEOBJY*/)
		&&!CollisionChipDale(2,1,0)){
		UpSpeed -= GRAVITY;
		IsJump = true;
		If_Reset_State = true;
	}
	//如果下方為藍色地板以外的物件則不能下跳
	if(IsLessCollision && map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,1/*ONEOBJY*/,true )){
		IsLessCollision= false;
		IsJump=false;
		B_FLAG = false;
		UpSpeed=0;
	}
	//等到MoveRL修好了再啟用
	if(CollisionChipDale(2,0,0)){
		i=0;
		while(CollisionChipDale(2,1,0)){
			wy--;
			i++;/* 碰撞的修正 */
		}
	}
	///////////////////////////////
	//以下部分為操作速度的實作過程
	///////////////////////////////
	//吃花
	if(UpSpeed<=0)
		TriggerObj(1);
	else
		TriggerObj(2);
	if(IsLessCollision){
		//下跳穿越地板
		UpSpeed -= GRAVITY;
		wy -= UpSpeed;
		if(!map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,1))
			IsLessCollision= false;
		IsJump = true;
		If_Reset_State = true;
	}
	else if(UpSpeed<0 && map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,abs(UpSpeed))){
		//落地動作
		while(!map->IfCollision(wx+tempFixX,wy+height+i+tempFixY,width,1))i++;/* 碰撞的修正 */
		wy += i;
		UpSpeed = 0;
		time_jump = 0;
		If_Reset_State = true;
		IsJump = false;
		B_FLAG = false;
		ani_jump_count=0;
	}
	else if(UpSpeed<0 && CollisionChipDale(2,UpSpeed,0)){
		i=0;
		while(CollisionChipDale(2,i,0)!=1){
			i++;/* 碰撞的修正 */
		}
		wy += i-1;
		UpSpeed = 0;
		time_jump = 0;
		If_Reset_State = true;
		IsJump = false;
		B_FLAG = false;
		
		ani_jump_count=0;
	}
	else if(UpSpeed>0 && map->IfCollision(wx+tempFixX,wy+tempFixY-abs(UpSpeed),width,abs(UpSpeed),true)){
		/*向上跳過程遇到障礙物*/
		i=0;while(!map->IfCollision(wx+tempFixX,wy-i+tempFixY-1,width,1,true))i++;// 碰撞的修正 
		wy -= i;
		UpSpeed = 0;
		time_jump = TIMEFORJUMP;
	}
	else{//跳躍過程,持續呼叫
		//2P碰撞
		if(UpSpeed>0)CollisionChipDale(1,UpSpeed,1);
		wy -= UpSpeed;
	}
	/*跳躍帶動螢幕*/
	if(canMoveMapY && wy < limitY && UpSpeed>0 && !(map->MoveMap(0)&1)){
		map->MoveMap(1,SPEED,UpSpeed/SPEED*SPEED);
		Jump_Fix =UpSpeed/SPEED*SPEED;
	}
	return If_Reset_State;
}