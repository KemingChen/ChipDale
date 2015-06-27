class CFrame{
public:
	CFrame();
	void SetWxWy(int wx,int wy,bool shiftmode=false);
	virtual void FixXY(MapManage *);				// 修正螢幕位置
	virtual void ReFixXY(MapManage *);				// 反修正螢幕位置
	virtual void FixMapMove(int fixX,int fixY);
	void SetWidthHeight(int tempW,int tempH);
	virtual int IfCollision(int twx,int twy,int twidth,int theight);
	int IfCollision(int Direct,int passSpeed);		//與地圖做運算碰撞
	virtual void CollisionReact(int setDirect,CFrame *which){;}
	int ReturnWX(){return wx;}				// 回傳螢幕x座標
	int ReturnWY(){return wy;}				// 回傳螢幕y座標
	int ReturnWidth(){return width;}
	int ReturnHeight(){return height;}
	bool ReturnLR(){return LRflag;}
	static int Jump_Fix;
private:
protected:
	int wx,wy;								// 框框左上角的螢幕座標	window x,y
	int width,height;
	bool flag[6];							// 上下左右AB	 按鍵旗標
	bool LRflag;							// 0 左 1 右     標示方向
};