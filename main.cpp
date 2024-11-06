#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include"tools.h"//���ù����ļ�
#include<mmsystem.h>//���ڲ�����Ч��1��
#include <math.h>
#include"vector2.h"
#define _CRT_SECURE_NO_WARNINGS 
#pragma comment(lib,"winmm.lib")//���ڲ�����Ч��2��
IMAGE imgBg;//��ʾ����ͼƬ0
IMAGE imgBar;
enum { WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT };//ö��
IMAGE imgCards[ZHI_WU_COUNT];//��ʾֲ�￨��
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];
int sunshine ;

#define WIN_WIDTH 900
#define WIN_HEIGHT 600
#define ZM_MAX   10
int curX, curY;//��ǰѡ��ֲ����ƶ������е�λ��
int curZhiWu;//0:û��ѡ�У�1��ѡ�е�һ��ֲ��
int index;
enum { GOING,WIN,FAIL };
int killCount;
int zmCount;//�Ѿ����ֵĽ�ʬ����
int  gameStatus;
//�ӵ�����������

struct bullet
{
	int x;
	int y;
	int speed;
	int row;
	bool used;
	bool blast;//�Ƿ�����ը
	int frameIndex;
};
IMAGE imgBulletNormal;
IMAGE imgBullBlast[4];
struct bullet bullets[30];
struct zm
{
	int x, y;
	int frameIndex;//��ʬ��֡
	bool used;
	int speed;
	int row;
	int blood;
	bool eating;//��ֲ��״̬
	bool dead;
};
struct zm zms[10];//ʮ��zm��Ľ�ʬ
IMAGE imgZM[22];
IMAGE imgZMDead[20];
IMAGE imgZMEat[21];
IMAGE imgZMStand[11];
struct sunshineBall
{
	int x, y;//Ʈ������е�����
	int frameIndex;//��ǰ��ͼƬ֡���
	int destY;//Ʈ��Ŀ��λ��y����
	bool used;//�Ƿ���ʹ��
    int timer;
	float xoff;
	float yoff;
	float t;//���������ߵ�ʱ���
	vector2 p1, p2, p3, p4;//��Դ��vertor2.h���Ǳ��������ߵ��ĸ�ʱ���
	vector2 pCur;//��ǰʱ���������λ��
	float speed;
	int status;
};
struct sunshineBall balls[10];//��������������
IMAGE imgSunshineBall[29];

enum{SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};
struct zhiwu//����structֲ������
{
	int type;//0:û�У�1����һ��ֲ��
	int frameIndex;//����֡�����
	bool catched ;//ֲ�ﱻץס���ԣ���״̬
	//int deadTime;//����������
	int blood;
	int timer;
	int x, y;
};
struct zhiwu map[3][9];//����struct zhiwu ���͵�����
bool fileExist(const char* name)//�ж��ļ��Ƿ����
{  
	FILE* fp = fopen(name, "r");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	else
	{
		return false;

	}

}

void gameInit()
{
	//���ر���ͼ
	//���ַ����ĳɡ����ֽ��ַ�����
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	killCount = 0;
	zmCount = 0;
	gameStatus = GOING;
	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));
	//��ʼ��ֲ�￨��
	char name[64];
	for (int i = 0; i < ZHI_WU_COUNT;i++)
	{
		//����ֲ���ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		//����ֲ�￨��
		loadimage(&imgCards[i], name);


		for (int j = 0; j < 20; j++)
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i,j + 1);
			if (fileExist(name))
			{
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else
			{
				break;
			}
		}
		
	}
	curZhiWu= 0;
	sunshine = 50;//��ʼ������ֵ//////////////////////////////////////////////////////////

	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++)
	{
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		
		loadimage(&imgSunshineBall[i], name);
		
	}
	srand(time(NULL));
	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	//��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");//������������
	f.lfQuality = ANTIALIASED_QUALITY;//�����Ч��
	settextstyle(&f);//��������
	setbkmode(TRANSPARENT);//���屳����Ϊ͸��
	setcolor(BLACK);
      

	//��ʼ����ʬ����
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}
	//��ʼ���ӵ�����
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));
	//��ʼ���㶹�ӵ���֡ͼƬ����
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++)
	{
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res/bullets/bullet_blast.png",
			imgBullBlast[3].getwidth()*k, 
			imgBullBlast[3].getheight()*k,true);//�ȱ�������СͼƬ����ͼƬ֡��

	}
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);

	}
	for (int i = 0; i < 21; i++) 
	{
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
	}

	// ��ʬվ�����ݳ�ʼ��
	for (int i = 0; i < 11; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZMStand[i], name);
	}


}
void drawZM()
{
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++)
	{
		if (zms[i].used)
		{
			IMAGE* img = NULL;
			//IMAGE* img = (zms[i].dead) ? imgZMDead : imgZM;
			if (zms[i].dead)
			{
				img = imgZMDead;
			}
			else if(zms[i].eating)
			{
				img = imgZMEat;
			}
			else
			{
				img = imgZM;
			}
			img += zms[i].frameIndex;
			putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
		}
	}
}
void drawSunshines()
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		//if (balls[i].used || balls[i].xoff)
		if(balls[i].used)
		{
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}
	//��ʾ����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);//��������ת�����ַ�
	outtextxy(280, 67, scoreText);//��ʾ������

}
void drawCard()
{
for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		int x = 338 + i * 65;
		int y = 6;
		putimage(x,y,&imgCards[i]);
	}
}
void drawZhiWu()
{
//ʵ����ֲֲ��
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				/*int x = 256 + j * 81;
				int y = 179 + i * 102+14;*/
				int zhiWuType = map[i][j].type-1;
				int index= map[i][j].frameIndex;
				
				//putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);
			}
		}
	}
	//��Ⱦ�϶���ֲ��
	if (curZhiWu > 0)
	{
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
}
void drawBullets()
{
//��Ⱦ�ӵ�
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++)
	{
		if (bullets[i].used==false)
		{
			continue;
		}
		if (bullets[i].blast)
		{
			IMAGE *img=&imgBullBlast[bullets[i].frameIndex];
			putimagePNG(bullets[i].x, bullets[i].y, img);
		}
		else 
		{
			putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
        }
	}
}
void updateWindow()
{
	BeginBatchDraw();//˫������������˸
	putimage(0,0,&imgBg);//ǰ��λ��������//���뱳��ͼ
	putimagePNG(250,0,&imgBar);//����ֲ����
	drawCard();
	drawZhiWu();
	//��Ⱦ������
	drawSunshines();
	drawZM();//���ƽ�ʬ
	drawBullets();
	EndBatchDraw();//˫������������˸
}
void collectSunshine(ExMessage* msg)
{ 
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++)
	{
		if (balls[i].used)
		{
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;
			if (msg->x > balls[i].pCur.x && msg->x < (balls[i].pCur.x + w) &&
				msg->y > balls[i].pCur.y && msg->y < (balls[i].pCur.y + h))
			{
				//balls[i].used = false;
				balls[i].status = SUNSHINE_COLLECT;
				//sunshine += 25;
				//mciSendString("play res/sunshine.mp3",0,0,0);
				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
				//����������ƫ����
				/*float destY = 0;
				float destX = 262;
				float angle = atan((y - destY) / (x - destX));
				balls[i].xoff = 60* cos(angle);
				balls[i].yoff = 60 * sin(angle);*/

				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(262,0);
				balls[i].t = 0;
				float off = 16;//ÿ���ƶ�������
				float distance = dis(balls[i].p1 - balls[i].p4);
				balls[i].speed = 1.0 / (distance / off);
				break;
            }

			
		}
	}
}
void userClick()
{
	ExMessage msg;//������һ����Ϊ msg �� ExMessage ���͵ı�����
	static int status = 0;
	int price[2] = { 100,50 };
	
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)//�����
		{
			if (msg.x > 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y < 96)
			{
				int index = (msg.x - 338) / 65;
				status = 1;
				curZhiWu = index + 1;//ֲ�￨��λ�����
				
			}
			else
			{
				
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE&&status==1)//����ƶ�
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status == 1)//���̧��
		{		
			if (msg.x > 256 && msg.y < 489 && msg.y>179)
			{
				
				
					
				 
						
					int row = (msg.y - 179) / 102;
					int col = (msg.x - 256) / 81;
					if (map[row][col].type == 0)
					{
						if (price[curZhiWu - 1] <= sunshine)
						{

							sunshine -= price[curZhiWu - 1];
							map[row][col].type = curZhiWu;
							map[row][col].frameIndex = 0;
							/*int x = 256 + j * 81;
						int y = 179 + i * 102+14;*/
							map[row][col].x = 256 + col * 81;
							map[row][col].y = 179 + row * 102 + 14;
							
						}
					}
				

				
				
			}
			curZhiWu = 0;
			status = 0;
		}
	}


}
void createSunshine()
{
	static int count=0;
	static int fre = 250;
	count++;
	
	if(count>=fre)//ʹ�����м��������
	{
		fre = 150+ rand() % 100;
		count = 0;
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for ( i = 0; i < ballMax && balls[i].used; i++);//�ж��������Ƿ�ʹ��
		if (i >= ballMax)
		{
				return;
		}
		balls[i].used = true;
		balls[i].frameIndex = 0;
		/*balls[i].x = 220 + rand() % (900 - 260);
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 3)* 90;*/
		balls[i].timer = 0;
		/*balls[i].xoff = 0;
		balls[i].yoff = 0;*/
		balls[i].status =SUNSHINE_DOWN ;
		balls[i].t = 0;
		balls[i].p1 = vector2(220 + rand() % (900 - 260),60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 3) * 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
     }
	//���տ���������
	/*static int count2 = 0;
	static int fre2 = 0;
	count2++;
	if(count2>=fre2)
	{*/
		/*fre2 = 0+ rand() % 1;
		count2 = 0;*/
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (map[i][j].type == XIANG_RI_KUI + 1)
				{
					map[i][j].timer++;
					if (map[i][j].timer > 200)
					{
						map[i][j].timer = 0;
						int k=0;
						for ( k = 0; k < ballMax && balls[k].used; k++);

						if (k>=ballMax)return;
						balls[k].used = true;
					
						balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
						int w = (rand() % 51 + 30) * (rand() % 2 ? 1 : -1);
						balls[k].p4 = vector2(map[i][j].x + w,
							map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight() - imgSunshineBall[0].getheight());
						balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 80);
						balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 100);
						balls[k].status = SUNSHINE_PRODUCT;
						balls[k].speed = 0.06;
						balls[k].t = 0;
					}
				}
			}
		}
	//}

}
void updateSunshine()
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if(balls[i].used)
		{
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;//��ֹ���
			if (balls[i].status == SUNSHINE_DOWN)
			{
				/*struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1)
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}*/
				balls[i].t += balls[i].speed;
				balls[i].pCur = balls[i].p1 + balls[i].t * (balls[i].p4 - balls[i].p1);
				if (balls[i].t >= 1)
				{
					balls[i].status= SUNSHINE_GROUND;
					balls[i].timer = 0;
				}
			}
	        else if (balls[i].status == SUNSHINE_GROUND)
			{
				balls[i].timer++;
				if (balls[i].timer > 100)
				{
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT)
			{
				balls[i].t += balls[i].speed;
				balls[i].pCur = balls[i].p1 + balls[i].t * (balls[i].p4 - balls[i].p1);
				if (balls[i].t > 1)
				{
					balls[i].used = false;
					sunshine += 25;
				}

			}
			else if (balls[i].status == SUNSHINE_PRODUCT)
			{
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t >= 1)
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
        	//balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;//��ֹ���
			//if(balls[i].timer == 0)
			//{
			//	balls[i].y += 2;
			//}
             //    if (balls[i].y >= balls[i].destY)
			//{
			//	
			//	balls[i].timer++;
			//	if (balls[i].timer > 100)//����������ͣ��
			//	{
			//		balls[i].used = false;
			//	}
			//	
			//}
			
		}
		/*else if (balls[i].xoff)
		{
			float destY = 0;
			float destX = 262;
			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
			balls[i].xoff = 60 * cos(angle);
			balls[i].yoff = 60 * sin(angle);


			balls[i].x -= balls[i].xoff;
			balls[i].y -= balls[i].yoff;
			if (balls[i].y < 0 || balls[i].x < 262)
			{
				balls[i].xoff = 0;
				balls[i].yoff = 0;
				sunshine += 25;
			}
		}*/
	}
}
void createZM()/////////////////////////////////
{
	if (zmCount >= ZM_MAX)
	{
		return;
	}
	static int zmFre = 200;
	static int count = 0;
	count++;
	if (count > zmFre)
	{
		count = 0;
		zmFre = 200+ rand() % 100;
		int i = 0;
		int zmMax = sizeof(zms) / sizeof(zms[0]);
		for (i = 0; i < zmMax && zms[i].used; i++);//��ѭ��//ֱ��δʹ�õĽ�ʬλ��ֹͣ


		if (i < zmMax)
		{
			memset(&zms[i], 0, sizeof(zms[i]));
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 3;
			zms[i].y = 172 + (1 + zms[i].row) * 100;
			zms[i].speed = 1;
			zms[i].blood = 200;
			//zms[i].dead = false;����Ŀ�����д���д����updateZM�����У�ʵ����д������Ҳ����
			zmCount++;
		}
	}
}

void updateZM()
{
	int zmMax = sizeof(zms) / sizeof(zms[0]);
	static int count = 0;
	count++;
	if(count>2)
	{
		count = 0;
		
		//���½�ʬλ��
		for (int i = 0; i < zmMax; i++)
		{
			if (zms[i].used)
			{
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 130)
				{
					gameStatus = FAIL;

					//MessageBox(NULL, "OVER", "OVER", 0);//���Ż�
					//exit(0);//���Ż�
				}
			}
		}
		
	}
	static int count2 = 0;
	count2++;
	if(count2>1)
	{
		count2 = 0;
		for (int i = 0; i < zmMax; i++)
		{
			
			if (zms[i].used)
			{
				if (zms[i].dead)
				{
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20)
					{
						zms[i].used = false;
						//zms[i].dead = false;//  //��������״̬(�ý�ʬһֱ����)
						killCount++;
						if (killCount == ZM_MAX)
						{
							gameStatus = WIN;
						}
					}
				}
				else if (zms[i].eating)
				{
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;

				}
				else
				{
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 22;
				}
			}
		}
	}
}
void shoot()
{
	int lines[3] = { 0 };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	int dangerX = WIN_WIDTH - imgZM[0].getwidth()/2;
	for (int i = 0; i < zmCount; i++)
	{
		if (zms[i].used && zms[i].x < dangerX)
		{
			lines[zms[i].row] = 1;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type == WAN_DOU + 1 && lines[i])//WAN_DOU��ö���еĵ�һ������0����һ����Ϊ��֮ǰд��userClick�����д洢�����ͼ���1
			{
				static int count = 0;
				count++;
				if (count > 30)
				{
					count = 0;
					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);

					if (k < bulletMax)
					{
						bullets[k].used = true;
						bullets[k].row = i;
						bullets[k].speed = 8;
						//int x = 256 + j * 81;//��i��j��ֲ���x���꣬��updateWindow��������
						//int y = 179 + i * 102 + 14;//��i��j��ֲ���y���ꣻ
						int zwX = 256 + j * 81;
						int zwY = 179 + i * 102 + 14;
						bullets[k].x = zwX +imgZhiWu[map[i][j].type - 1][0]->getwidth() - 10;
						//imgZhiWu[map[i][j].type-1][0]->getwidth()ָ����ֲ���λ�ÿ�ȣ�
						bullets[k].y = zwY + 5;
						bullets[k].blast = false;
						bullets[k].frameIndex = 0;
					}

				}
			}
		}
	}
}
void updateBullets()
{
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++)
	{
		if (bullets[i].used)
		{
			bullets[i].x +=bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH)
			{
				bullets[i].used = false;
			}
			//��ʵ�ֵ���ײ���
			if (bullets[i].blast)
			{
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex > 3)
				{
					bullets[i].used = false;
				}
			}
		}
	}
}
void checkBullet2Zm()
{
	int bulletCount = sizeof(bullets) / sizeof(bullets[0]);
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < bulletCount; i++)
	{
		if (bullets[i].used == false && bullets[i].blast)
		{
			continue;
		}
		for (int k = 0; k < zmCount; k++)
		{
			if (zms[k].used == false)
			{
				continue;
			}
			int x1 = zms[k].x + 80;
			int x2 = zms[k].x + 110;
			int x = bullets[i].x;
			if (zms[k].dead == false && bullets[i].row == zms[k].row && x > x1 && x < x2)
			{
				zms[k].blood -= 5;
				bullets[i].blast = true;
				bullets[i].speed = 0;
				if (zms[k].blood <= 0)
				{
					zms[k].dead = true;
					zms[k].speed = 0;
					zms[k].frameIndex = 0;
				}
				break;
			}
		}
	}
}
void checkZm2ZhiWu()
{
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++)
	{
		if (!zms[i].used||zms[i].dead)//////////////
		{
			continue;
		}
		int row = zms[i].row;
		for (int k = 0; k < 9; k++)
		{
			if (map[row][k].type == 0)
			{
				continue;
			}
			//int x = 256 + j * 81;
			int zhiWuX= 256 + k * 81;
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;
			if (x3 > x1 && x3 < x2)
			{
				if (map[row][k].catched)
				{
					map[row][k].blood-=2;
					if (!map[row][k].blood ) 
					{
						map[row][k].blood =100;
						map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = 1;
						map[row][k].catched = false;///////////////////////////////
					}
				}
				else
				{
					map[row][k].catched = true;
					map[row][k].blood = 100;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}
			}
		}
	}
}

void collisionCheck()
{
	checkBullet2Zm();//����ӵ��Խ�ʬ��ײ
	checkZm2ZhiWu();//��⽩ʬ��ֲ����ײ
	
}
void updateZhiWu()
{
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				
				map[i][j].frameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index]==NULL)
				{
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}
void updateGame()
{
	updateZhiWu();
	createSunshine();//��������
	updateSunshine();//��������״̬
	createZM();
	updateZM();
	shoot();//���ͣ��������㶹�ӵ�
	updateBullets();//�����㶹�ӵ�
	collisionCheck();//ʵ����ײ���
	

}
void startUI()//�����˵�
{
	IMAGE imgBg;//�ֲ�����
	IMAGE imgMenu1;
	IMAGE imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;
	while (1)
	{
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(474, 75, flag ? &imgMenu2:&imgMenu1);
		


		ExMessage msg;//�ֲ�����
		if (peekmessage(&msg))
		{
			if (msg.message == WM_MOUSEMOVE && msg.x > 474 && msg.x < 474 + 300 && msg.y>75 && msg.y < 75 + 140)
			{
				flag = 1;
			}
			if (msg.message == WM_MOUSEMOVE  &&(msg.y < 75 && msg.y>0 || msg.y < 600 && msg.y > 75 + 140) ||(msg.x > 0 && msg.x < 474 || msg.x >474 + 300 && msg.x < 900) )
			{
				flag = 0;

			}
			
			else if (msg.message == WM_LBUTTONUP && msg.x > 474 && msg.x < 474 + 300 && msg.y>75 && msg.y < 75 + 140)
			{
				mciSendString("play res/audio/crazydaveshort1.mp3", 0, 0, 0);

				Sleep(1000);
				EndBatchDraw();
				break;
			}
		}
		EndBatchDraw();
	}
}
void viewScene()
{
	int xMin = WIN_WIDTH - imgBg.getwidth();//900-1400=-500
	vector2 points[9] = 
	{
		{500,80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340}
	};
	int index[9] ;
	for (int i = 0; i < 9; i++)
	{
		index[i] = rand() % 11;
	}
	int count = 0;
	for (int x = 0; x >= xMin; x -= 2)
	{
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		count++;
		
			for (int k = 0; k < 9; k++)
			{
				if (count >= 5)
				{
					index[k] = (index[k] + 1) % 11;
				}
				putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
				
			}
			if (count >= 5)
			{
				count = 0;
			}
		
	
		EndBatchDraw();
		Sleep(5);
	}
	//ͣ��һ������
	for (int i = 0; i < 100; i++)
	{
		BeginBatchDraw();
		putimage(xMin, 0, &imgBg);
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x , points[k].y, &imgZMStand[index[k]]);
			
				index[k] = (index[k] + 1) % 11;
			
		}
		EndBatchDraw();
		Sleep(30);
	}
	for (int x = xMin; x <= 0; x+=2)
	{
		BeginBatchDraw();
        putimagePNG(x,0,&imgBg);
		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
			if (count >= 5)
			{
				index[k] = (index[k] + 1) % 11;
			}
			
		}
		if (count >= 5)
		{
			count = 0;
		}
		EndBatchDraw();
		Sleep(5);
	}
}
void barsDown()
{
	int height = imgBar.getheight();
	for (int h = -height; h <= 0; h++)
	{
		BeginBatchDraw();
		
		//putimage(0, 0, &imgBg);

		putimagePNG(250, h, &imgBar);
		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
        int x = 338 + i * 65;
		int y = 6+h;
		putimage(x, y, &imgCards[i]);
		}
		
		EndBatchDraw();
		Sleep(10);
	}
}
bool checkOver()
{
	int ret = false;
	
	if (gameStatus == WIN)
	{
		
		loadimage(0, "res/gameWin.png");
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(2000);
		ret = true;
	}
	else if (gameStatus == FAIL)
	{
		
		
		loadimage(0, "res/gameFail.png");
		mciSendString("play res/lose.mp3", 0, 0, 0);
		Sleep(2000);
		ret = true;
	}
	return ret;
}
int main()
{
	gameInit();//��Ϸ��ʼ��
	startUI();
	viewScene();//�鿴����������ת����
	barsDown();
	
	mciSendString("play res/audio/2.75.mp3", 0, 0, 0);
	int timer = 0;
	int flag = true;
	while (1)
	{
	    userClick();
		timer += getDelay();
		if (timer > 50)
		{
			flag = true;
			timer = 0;
		}
		if (flag)
		{
			flag = false;
			updateWindow();
            updateGame();
			if (checkOver())
			{

				break;
			}
		}
	}
	system("pause");
	return 0;
}