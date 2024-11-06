#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include"tools.h"//调用工具文件
#include<mmsystem.h>//用于播放音效（1）
#include <math.h>
#include"vector2.h"
#define _CRT_SECURE_NO_WARNINGS 
#pragma comment(lib,"winmm.lib")//用于播放音效（2）
IMAGE imgBg;//表示背景图片0
IMAGE imgBar;
enum { WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT };//枚举
IMAGE imgCards[ZHI_WU_COUNT];//表示植物卡牌
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];
int sunshine ;

#define WIN_WIDTH 900
#define WIN_HEIGHT 600
#define ZM_MAX   10
int curX, curY;//当前选中植物，在移动过程中的位置
int curZhiWu;//0:没有选中，1：选中第一个植物
int index;
enum { GOING,WIN,FAIL };
int killCount;
int zmCount;//已经出现的僵尸个数
int  gameStatus;
//子弹的数据类型

struct bullet
{
	int x;
	int y;
	int speed;
	int row;
	bool used;
	bool blast;//是否发生爆炸
	int frameIndex;
};
IMAGE imgBulletNormal;
IMAGE imgBullBlast[4];
struct bullet bullets[30];
struct zm
{
	int x, y;
	int frameIndex;//僵尸的帧
	bool used;
	int speed;
	int row;
	int blood;
	bool eating;//吃植物状态
	bool dead;
};
struct zm zms[10];//十个zm类的僵尸
IMAGE imgZM[22];
IMAGE imgZMDead[20];
IMAGE imgZMEat[21];
IMAGE imgZMStand[11];
struct sunshineBall
{
	int x, y;//飘落过程中的坐标
	int frameIndex;//当前的图片帧序号
	int destY;//飘落目标位置y坐标
	bool used;//是否在使用
    int timer;
	float xoff;
	float yoff;
	float t;//贝塞尔曲线的时间点
	vector2 p1, p2, p3, p4;//来源于vertor2.h，是贝塞尔曲线的四个时间点
	vector2 pCur;//当前时刻阳光球的位置
	float speed;
	int status;
};
struct sunshineBall balls[10];//包含阳光的阳光池
IMAGE imgSunshineBall[29];

enum{SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};
struct zhiwu//定义struct植物类型
{
	int type;//0:没有，1：第一个植物
	int frameIndex;//序列帧的序号
	bool catched ;//植物被抓住（吃）的状态
	//int deadTime;//死亡计数器
	int blood;
	int timer;
	int x, y;
};
struct zhiwu map[3][9];//定义struct zhiwu 类型的数组
bool fileExist(const char* name)//判断文件是否存在
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
	//加载背景图
	//把字符集改成“多字节字符集”
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	killCount = 0;
	zmCount = 0;
	gameStatus = GOING;
	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));
	//初始化植物卡牌
	char name[64];
	for (int i = 0; i < ZHI_WU_COUNT;i++)
	{
		//生成植物文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		//加载植物卡牌
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
	sunshine = 50;//初始化阳光值//////////////////////////////////////////////////////////

	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++)
	{
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		
		loadimage(&imgSunshineBall[i], name);
		
	}
	srand(time(NULL));
	//创建游戏窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");//设置字体类型
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
	settextstyle(&f);//设置字体
	setbkmode(TRANSPARENT);//字体背景设为透明
	setcolor(BLACK);
      

	//初始化僵尸数据
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}
	//初始化子弹数据
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));
	//初始化豌豆子弹的帧图片数组
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++)
	{
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res/bullets/bullet_blast.png",
			imgBullBlast[3].getwidth()*k, 
			imgBullBlast[3].getheight()*k,true);//等比例放缩小图片放在图片帧中

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

	// 僵尸站立数据初始化
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
	//显示阳光值
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);//把阳光数转换成字符
	outtextxy(280, 67, scoreText);//显示阳光数

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
//实现种植植物
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
	//渲染拖动的植物
	if (curZhiWu > 0)
	{
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
}
void drawBullets()
{
//渲染子弹
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
	BeginBatchDraw();//双缓冲避免出现闪烁
	putimage(0,0,&imgBg);//前两位代表坐标//放入背景图
	putimagePNG(250,0,&imgBar);//放入植物栏
	drawCard();
	drawZhiWu();
	//渲染阳光球
	drawSunshines();
	drawZM();//绘制僵尸
	drawBullets();
	EndBatchDraw();//双缓冲避免出现闪烁
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
				//设置阳光球偏移量
				/*float destY = 0;
				float destX = 262;
				float angle = atan((y - destY) / (x - destX));
				balls[i].xoff = 60* cos(angle);
				balls[i].yoff = 60 * sin(angle);*/

				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(262,0);
				balls[i].t = 0;
				float off = 16;//每次移动的像素
				float distance = dis(balls[i].p1 - balls[i].p4);
				balls[i].speed = 1.0 / (distance / off);
				break;
            }

			
		}
	}
}
void userClick()
{
	ExMessage msg;//定义了一个名为 msg 的 ExMessage 类型的变量。
	static int status = 0;
	int price[2] = { 100,50 };
	
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)//鼠标点击
		{
			if (msg.x > 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y < 96)
			{
				int index = (msg.x - 338) / 65;
				status = 1;
				curZhiWu = index + 1;//植物卡牌位置序号
				
			}
			else
			{
				
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE&&status==1)//鼠标移动
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status == 1)//鼠标抬起
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
	
	if(count>=fre)//使阳光有间隔的生成
	{
		fre = 150+ rand() % 100;
		count = 0;
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for ( i = 0; i < ballMax && balls[i].used; i++);//判断阳光球是否被使用
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
	//向日葵生产阳光
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
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;//防止溢出
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
        	//balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;//防止溢出
			//if(balls[i].timer == 0)
			//{
			//	balls[i].y += 2;
			//}
             //    if (balls[i].y >= balls[i].destY)
			//{
			//	
			//	balls[i].timer++;
			//	if (balls[i].timer > 100)//用于让阳光停留
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
		for (i = 0; i < zmMax && zms[i].used; i++);//空循环//直到未使用的僵尸位置停止


		if (i < zmMax)
		{
			memset(&zms[i], 0, sizeof(zms[i]));
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 3;
			zms[i].y = 172 + (1 + zms[i].row) * 100;
			zms[i].speed = 1;
			zms[i].blood = 200;
			//zms[i].dead = false;本项目把这行代码写在了updateZM函数中，实际上写在这里也可以
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
		
		//更新僵尸位置
		for (int i = 0; i < zmMax; i++)
		{
			if (zms[i].used)
			{
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 130)
				{
					gameStatus = FAIL;

					//MessageBox(NULL, "OVER", "OVER", 0);//待优化
					//exit(0);//待优化
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
						//zms[i].dead = false;//  //重置死亡状态(让僵尸一直出来)
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
			if (map[i][j].type == WAN_DOU + 1 && lines[i])//WAN_DOU是枚举中的第一个代表0；加一是因为在之前写的userClick代码中存储的类型加了1
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
						//int x = 256 + j * 81;//在i行j列植物的x坐标，见updateWindow（）函数
						//int y = 179 + i * 102 + 14;//在i行j列植物的y坐标；
						int zwX = 256 + j * 81;
						int zwY = 179 + i * 102 + 14;
						bullets[k].x = zwX +imgZhiWu[map[i][j].type - 1][0]->getwidth() - 10;
						//imgZhiWu[map[i][j].type-1][0]->getwidth()指的是植物的位置宽度；
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
			//待实现的碰撞检测
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
	checkBullet2Zm();//检测子弹对僵尸碰撞
	checkZm2ZhiWu();//检测僵尸对植物碰撞
	
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
	createSunshine();//创建阳光
	updateSunshine();//更新阳光状态
	createZM();
	updateZM();
	shoot();//发送（创建）豌豆子弹
	updateBullets();//更新豌豆子弹
	collisionCheck();//实现碰撞检测
	

}
void startUI()//启动菜单
{
	IMAGE imgBg;//局部变量
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
		


		ExMessage msg;//局部变量
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
	//停留一秒左右
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
	gameInit();//游戏初始化
	startUI();
	viewScene();//查看场景（场景转场）
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