/****************************************************************/
//CFrame¹ê§@													*/
//																*/
/****************************************************************/
int CFrame::Jump_Fix = 0;
CFrame::CFrame()
{
	wx=wy=0;
	for(int i=0;i<6;i++)
		flag[i]=false;
	LRflag=1;
}
void CFrame::SetWxWy(int setX,int setY,bool shiftmode)
{
	if(shiftmode){
		if((wx+width+setX)<=MWIDTH&&(wx+setX)>=0)	wx += setX;
		if((wy+setY)<MHEIGHT)				wy += setY;
	}
	else{
		if(setX>=0&&(width+setX)<MWIDTH)			wx = setX;
		if(setY<MHEIGHT)							wy = setY;
	}
}
void CFrame::SetWidthHeight(int tempW,int tempH)
{
	width = tempW;
	height = tempH;
}
void CFrame::FixXY(MapManage *map)
{
	int temp = map->MoveMap(0);
	if(temp&1)
		wy += Jump_Fix;
	if(temp&2)
		wy -= Jump_Fix;
	if(temp&4)
		wx += MapManage::LRMargin;
	if(temp&8)
		wx -= MapManage::LRMargin;
}
void CFrame::ReFixXY(MapManage *map)
{
	int temp = map->MoveMap(0);
	if(temp&1)
		wy -= Jump_Fix;
	if(temp&2)
		wy += SPEED;
	if(temp&4)
		wx -= MapManage::LRMargin;
	if(temp&8)
		wx += MapManage::LRMargin;
}
void CFrame::FixMapMove(int fixX,int fixY)
{
	wx -= fixX;
	wy -= fixY;
}
int CFrame::IfCollision(int twx,int twy,int twidth,int theight)
{
	//¥ª¤W 1
	int cx,cy;
	for(int i=1;i<=4;i++){
		switch(i){
			case 1:
				cx = twx;
				cy = twy;
				break;
			case 2:
				cx = twx+twidth;
				cy = twy;
				break;
			case 3:
				cx = twx+twidth;
				cy = twy+theight;
				break;
			case 4:
				cx = twx;
				cy = twy+theight;
				break;
		}
		if(wx<=cx			&& wx+width >= cx			&& wy<=cy			&& wy+height >= cy)		return i;
	}
	if(twx<=wx			&& twx+twidth >= wx			&& twy<=wy			&& twy+theight >= wy)		return 5;
	return 0;
}
int CFrame::IfCollision(int Direct,int passSpeed){
	MapManage *map = ChipDale::Maps;
	passSpeed = abs(passSpeed);
	switch(Direct){
		case 1: return map->IfCollision(wx,wy-passSpeed,width,passSpeed,true);
		case 4: 
			if(wx-passSpeed < 0) return 1;
			return map->IfCollision(wx-passSpeed,wy,passSpeed,height,true);
		case 8: 
			if(wx+width+passSpeed > MWIDTH) return 1;
			return map->IfCollision(wx+width,wy,passSpeed,height,true);
	}
	return 0;
}