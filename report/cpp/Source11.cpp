class CFrame{
public:
	CFrame();
	void SetWxWy(int wx,int wy,bool shiftmode=false);
	virtual void FixXY(MapManage *);				// �ץ��ù���m
	virtual void ReFixXY(MapManage *);				// �ϭץ��ù���m
	virtual void FixMapMove(int fixX,int fixY);
	void SetWidthHeight(int tempW,int tempH);
	virtual int IfCollision(int twx,int twy,int twidth,int theight);
	int IfCollision(int Direct,int passSpeed);		//�P�a�ϰ��B��I��
	virtual void CollisionReact(int setDirect,CFrame *which){;}
	int ReturnWX(){return wx;}				// �^�ǿù�x�y��
	int ReturnWY(){return wy;}				// �^�ǿù�y�y��
	int ReturnWidth(){return width;}
	int ReturnHeight(){return height;}
	bool ReturnLR(){return LRflag;}
	static int Jump_Fix;
private:
protected:
	int wx,wy;								// �خإ��W�����ù��y��	window x,y
	int width,height;
	bool flag[6];							// �W�U���kAB	 ����X��
	bool LRflag;							// 0 �� 1 �k     �Хܤ�V
};