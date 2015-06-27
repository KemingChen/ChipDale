/****************************************************************/
//AI_Instructions��@											*/
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
	//�ץ����U�ɥ��k�������ӪŦh�֤��P�_
	wx += LR_Space;
	width -= 2*LR_Space;
	//�̫᭱�O�o�n�^�_�쥻�]�w
	//���U�L�{��FLAG��l��
	if(IsJump || !map->IfCollision(wx+fixX,wy+height+fixY,width,1,false,NoIgnore_eat/*ONEOBJY*/)){
		UpSpeed -= GRAVITY;
		IsJump = true;
	}
	///////////////////////////////
	//�H�U�������ާ@�t�ת���@�L�{
	///////////////////////////////
	if(UpSpeed<0 && map->IfCollision(wx+fixX,wy+height+fixY,width,abs(UpSpeed),false,NoIgnore_eat)){
		//���a�ʧ@
		while(!map->IfCollision(wx+fixX,wy+height+i+fixY,width,1,false,NoIgnore_eat))i++;/* �I�����ץ� */
		wy += i;
		UpSpeed = 0;
		IsJump = false;
		IsLand=true;
	}
	else if(UpSpeed>0 && map->IfCollision(wx+fixX,wy+fixY-abs(UpSpeed),width,abs(UpSpeed),NoIgnore_2)){
		/*�V�W���L�{�J���ê��*/
		i=0;while(!map->IfCollision(wx+fixX,wy-i+fixY-1,width,1,NoIgnore_2))i++;// �I�����ץ� 
		wy -= i;
		UpSpeed = 0;
	}
	else{
		//���D�L�{
		wy -= UpSpeed;
	}
	if(map->IfCollision(wx+fixX,wy+height+fixY,width,1,false,NoIgnore_eat/*ONEOBJY*/)){
		IsLand=true;
		IsJump = false;
	}
	//�ץ����U�ɥ��k�������ӪŦh�֤��P�_ �^�_�쥻�]�w
	wx -= LR_Space;
	width += 2*LR_Space;
	return IsLand;
}
/****************************************************************/
//BasicInstructions��@											*/
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
	if(ToolCDC::ReturnStage()!=0)return false;	//����w�g�s�ϤS����
	//��H
	if(Instance()->Partner!=NULL&&CollisionChipDale(4,2,0)&&!Instance()->ReturnIsTaken()&&Instance()->A_FLAG
		&&!Instance()->Partner->ReturnIsTaken()&&!Instance()->Partner->ReturnInvincible()){
		Object** CanThrow=ChipDale::CanThrow;
		for(int i=0;i<CanTakeNum;i++){
			if(CanThrow[i]==NULL){
				//if(Partner->Alive)break; //���ѱ�����|���ݼv�ĪG
				CanThrow[i]=new ChipDale_Taken(Instance());
				Instance()->NowTakeObj=CanThrow[i];
				Instance()->NowTaken=ChipDale_taken;
				Instance()->A_FLAG = false;
				break;
			}
		}
	}
	//���F��
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
	//����
	if(!map->IfCollision(wx-SPEED,wy,SPEED,height,true) && !temp){

		SetWxWy(-SPEED,0,true);
		if(canMoveMapX && wx < limitX)
			map->MoveMap(4);
		return true;
	}
	else if(temp) {
		//FIRST �}�⧹���K��
		i=SPEED-1;while(i>=0 && CollisionChipDale(4,i,0))i--;
		SetWxWy(-i,0,true);
		//���ʳѾl�B�� SPEED-i

		//�K�񤧫�������
		if(!getPartner()->ReturnHideComplete()&&!IfCollision(4,1))
		CollisionChipDale(4,1);
		//�a�ʿù�
		if(canMoveMapX && wx < limitX)
			map->MoveMap(4,1);
		return true;
	}
	else {
		//����a�ϻ�ê���ץ�
		i=0;while(!map->IfCollision(wx-i,wy,width,height,true))i++;
		wx -=(i!=0)*(i-1)+(i==0)*(-SPEED);
	}
	return false;
}
bool BasicInstructions::MoveRight(MapManage* map){
	int i,limitX = (MWIDTH + width)/2,temp,testTemp;
	int UnVisibleStone = OrderSize+MonsterOrderSize +0;
	TriggerObj(4);
	if(ToolCDC::ReturnStage()!=0)return false;	//����w�g�s�ϤS����
	//��H
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
	//���F��
	if(A_FLAG && !SetNowTaken(-1)){
		temp = map->IfCollision(wx+width+SPEED,wy,ONEOBJX,height,true,false);
		testTemp = map->IfCollision(wx+width+SPEED,wy,ONEOBJX,height,true,false,true);
		if(testTemp/E8==5 || temp/E8==5 && testTemp/E8!=5 && testTemp/E8!=0)	temp = testTemp;
		if(testTemp/E8== UnVisibleStone|| temp/E8==UnVisibleStone && testTemp/E8!=UnVisibleStone && testTemp/E8!=0)	temp = testTemp;
		if(temp!=1&&temp!=2&&temp!=0&&temp/E8!=4){//�c�l...
			map->ClearObstacle(temp);
			SetNowTaken(temp);
			A_FLAG = false;
		}
	}
	//����
	temp = CollisionChipDale(8,SPEED,0);
	if(!map->IfCollision(wx+width,wy,SPEED,height,true) && !temp){
		SetWxWy(SPEED,0,true);
		if(canMoveMapX && wx > limitX)
			map->MoveMap(8);
		return true;
	}
	else if(temp){
		//FIRST �}�⧹���K��
		i=SPEED-1;while(i>=0 && CollisionChipDale(8,i,0))i--;
		SetWxWy(i,0,true);
		//���ʳѾl�B�� SPEED-i
		//�K�񤧫�������
		if(!getPartner()->ReturnHideComplete()&&!IfCollision(8,1))
		CollisionChipDale(8,1);
		//�a�ʿù�
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
	if(ToolCDC::ReturnStage()!=0)return false;	//����w�g�s�ϤS����
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
	if(ToolCDC::ReturnStage()!=0)return If_Reset_State;	//����w�g�s�ϤS����
	//�Ȯɩʭץ��@���C���b���ץ��e�]��P�_���`
	if(map->MoveMap(0)&1)tempFixY = Jump_Fix;
	if(map->MoveMap(0)&4)tempFixX = SPEED;
	if(map->MoveMap(0)&8)tempFixX = -SPEED;
	//���U�L�{��FLAG��l�� // �w�]�O���i����D
	if(!IsLessCollision && !map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,1/*ONEOBJY*/)
		&&!CollisionChipDale(2,1,0)){
		UpSpeed -= GRAVITY;
		IsJump = true;
		If_Reset_State = true;
	}
	//�p�G�U�謰�Ŧ�a�O�H�~������h����U��
	if(IsLessCollision && map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,1/*ONEOBJY*/,true )){
		IsLessCollision= false;
		IsJump=false;
		B_FLAG = false;
		UpSpeed=0;
	}
	//����MoveRL�צn�F�A�ҥ�
	if(CollisionChipDale(2,0,0)){
		i=0;
		while(CollisionChipDale(2,1,0)){
			wy--;
			i++;/* �I�����ץ� */
		}
	}
	///////////////////////////////
	//�H�U�������ާ@�t�ת���@�L�{
	///////////////////////////////
	//�Y��
	if(UpSpeed<=0)
		TriggerObj(1);
	else
		TriggerObj(2);
	if(IsLessCollision){
		//�U����V�a�O
		UpSpeed -= GRAVITY;
		wy -= UpSpeed;
		if(!map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,1))
			IsLessCollision= false;
		IsJump = true;
		If_Reset_State = true;
	}
	else if(UpSpeed<0 && map->IfCollision(wx+tempFixX,wy+height+tempFixY,width,abs(UpSpeed))){
		//���a�ʧ@
		while(!map->IfCollision(wx+tempFixX,wy+height+i+tempFixY,width,1))i++;/* �I�����ץ� */
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
			i++;/* �I�����ץ� */
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
		/*�V�W���L�{�J���ê��*/
		i=0;while(!map->IfCollision(wx+tempFixX,wy-i+tempFixY-1,width,1,true))i++;// �I�����ץ� 
		wy -= i;
		UpSpeed = 0;
		time_jump = TIMEFORJUMP;
	}
	else{//���D�L�{,����I�s
		//2P�I��
		if(UpSpeed>0)CollisionChipDale(1,UpSpeed,1);
		wy -= UpSpeed;
	}
	/*���D�a�ʿù�*/
	if(canMoveMapY && wy < limitY && UpSpeed>0 && !(map->MoveMap(0)&1)){
		map->MoveMap(1,SPEED,UpSpeed/SPEED*SPEED);
		Jump_Fix =UpSpeed/SPEED*SPEED;
	}
	return If_Reset_State;
}