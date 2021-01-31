#include<Windows.h>
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<conio.h>
#include<string>
#define CLS system("cls")
using namespace std;

char v[] = "（Build 0.4）"; //版本号


//单个方块数据的结构体模板
struct block
{
	string name;
	string dis;
	int yd;
	unsigned short item[5];
	unsigned short amount[5];
	bool can_th;
	bool can_dised;
	bool can_placed; //允许被其它方块覆盖掉 且 无掉落物
};

//单个物品数据的结构体模板
struct item
{
	char name[5];
	bool can_place = 0; //是否允许放置
	short place_block = 0; //放置所对应的方块
};

//随机生成范围随机数
int aich_fw(int m1, int m2, int seed) {
	if (m1 >= m2) {
		if (seed == -1) {
			srand((unsigned)time(NULL));
		}
		else {
			srand((unsigned)seed);
		}
		return (rand() % (m1 - m2 + 1) + m2);
	}
	else {
		return 0;
	}
}


//玩家数据类模板
class player {
public:
	string dis;
	int hp = 100;//生命值
	int x = 5;//X坐标
	int y = 5;//Y坐标
	unsigned short item_ch = 0;//手持的背包格数
	unsigned short turn = 0;//朝向(0上/1下/2左/3右)
	class bag {
	public:
		unsigned short type; //物品类型
		unsigned short amount; //物品数量
	} items[10] 
	{
		{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}
	};

};

//随机生成概率布尔值
int aich_gl(double gl, unsigned int seed) {
	double g = gl * 100;
	return (aich_fw(100, 1, seed) <= g);
}


//显示:行
class D1 {
public:
	//显示:列
	class {
	public:
		string d3;//显示:位
	} d2[10];
};

//世界:方块数据
unsigned short world_blocks[10][10];


int main() {
	
	char stch = 0; //选择操作
	bool stop = 1; //重复主菜单
	bool dis_ch = 1; //主显示更新
	char null = NULL;
	char last_input = 0; //主游戏: 上一次按键
	unsigned int aitemp = 0; //随机生成: 缓存变量
	bool aistop = 0; //随机生成: 循环
	unsigned int seed = (unsigned)time(NULL);

	//主游戏: 时间信息类
	class game_time {
	public:
		unsigned long all = 0; //主游戏时间周期
		unsigned long obj[5] = { 0 }; //主游戏单项目时间周期
	}gat;

	unsigned short wbjg = 0;
	unsigned short block_broke = 0; //方块破坏计数器
	unsigned short block_broke_x = 0; //被破坏方块x坐标
	unsigned short block_broke_y = 0; //被破坏方块y坐标
	unsigned short ai_amount = 0;
	short block_yd = -1; //方块剩余硬度
	short block_broke_type = 0; //当前破坏的方块类型
	bool game_stop = 1; //重复主游戏循环
	bool gamedis_stop = 1; //重复主游戏主显示循环
	bool ai_stop = 1;//重复随机生成世界循环
	//DWORD time1; //时间刻1
	//DWORD time2; //时间刻2
	int input = 0; //主游戏: 本次按键
	string tips = " "; //主游戏显示: 提示
	int lastai = 0; //上一次随机生成方块结果

	//主游戏显示类: 右侧状态栏
	class state { 
	public:
		string dis = " "; //显示的字符串
		bool on = 0; //是否启用
		short type = 0; //类型(1是破坏方块)
	}; 

	while (stop == 1) {
		stch = NULL;
		printf("欢迎来到 S T %s ，请输入要选择的选项的序号!\n\n", v);

		printf("1. 开始游戏\n"
			"2. 空位\n"
			"3. 新手教程(必学)\n"
			"4. 进入设置\n"
			"5. 关于\n\n\n"



			"请输入: "
		);

		stch = _getch();

		if (stch == '1') {
			CLS;

			//重置主游戏: 时间类
			gat.all = 0;
			for (int i = 0; i < 5; i++) {
				gat.obj[i] = 0;
			}


			//创建并初始化玩家类
			class player pl;
			pl.hp = 100;
			pl.x = 5;
			pl.y = 5;
			pl.turn = 0;
			pl.item_ch = 0;
			pl.dis = "Q ";
			for (int i = 0; i < 10; i++) {
				pl.items[i].type = 0;
				pl.items[i].amount = 0;
			}

			//定义所有方块(可显示的)
			struct block blocks[5]{
				{"未知", "  ", -1, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 1, 1, 1},
				{"树", "树", 20, {1, 0, 0, 0, 0}, {3, 0, 0, 0, 0}, 0, 0, 0},
				{"木块", "■", 10, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, 0, 0, 0},
				{"未知", "  ", -1, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 1, 1, 0},
				{"未知", "  ", -1, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 1, 1, 0}
			};

			//定义所有物品(可获得的)
			struct item items[5]{
				{NULL, 0, 0},
				{"木材", 1, 2},
				{},
				{},
				{}
			};

			//初始化世界:方块
			for (int i1 = 0; i1 < 10; i1++) {
				for (int i2 = 0; i2 < 10; i2++) {
					world_blocks[i1][i2] = 0U;
				}
			}

			//世界:方块随机化
			ai_stop = 1;
			//time2 = GetTickCount64();
			//Sleep(aich_fw(50, 1, -1));
			while (ai_stop) {
				ai_amount = 0;
				for (int i1 = 0; i1 < 10; i1++) {
					for (int i2 = 0; i2 < 10; i2++) {
						/*wbjg = aich_fw(10, 1, seed);
						//printf("%d\n", wbjg);
						Sleep(aich_fw(9, 1, wbjg));
						time1 = GetTickCount64();
						if (wbjg <= 1) wbjg = 1;
						else wbjg = 0;
						aitemp = seed;

						seed = time1 - time2;
						world_blocks[i1][i2] = wbjg;*/

						wbjg = aich_fw(100, 1, (int)(GetTickCount64() % (ULONGLONG)667 + 1));
						if (wbjg <= 20) {
							wbjg = 1;
						}
						else wbjg = 0;
						Sleep(aich_fw(9, 1, aich_fw(1000, 1, wbjg)));
						if (wbjg == lastai && aich_gl(0.8, (int)(GetTickCount64() % (ULONGLONG)23))) wbjg = 0;
						if (wbjg == 1) {
							ai_amount += 1;
						}
						world_blocks[i1][i2] = wbjg;
						lastai = wbjg;

					}
				}
				if (ai_amount >= 10 && ai_amount <= 20) break;

			}
			world_blocks[pl.y][pl.x] = 0;

			//创建并定义显示类
			class D1 d1[10];
			for (int i1 = 0; i1 < 10; i1++) {
				for (int i2 = 0; i2 < 10; i2++) {
					d1[i1].d2[i2].d3 = "  ";
				}
			}

			//创建并定义 主游戏显示类: 右侧状态栏
			class state states[10];


			//重置变量
			gamedis_stop = 1;
			game_stop = 1;
			last_input = 0;
			block_yd = -1;
			block_broke = 0;
			block_broke_type = 0;
			block_broke_x = 0; 
			block_broke_y = 0; 
			for (int i = 0; i < 10; i++) {
				states[i].dis = " ";
				states[i].on = 0;
			}

			//重置提示
			tips = "按下\"WASD\"或\"↑↓←→\"以移动！";

			//主游戏循环------------------------------------------------------------------------------------------------------------------------
			while (game_stop == 1) {
				gat.all+= 1; //主游戏: 时间周期(计时器)+1
				
				for (int i1 = 0; i1 < 10; i1++) {
					for (int i2 = 0; i2 < 10; i2++) {
						d1[i1].d2[i2].d3 = blocks[world_blocks[i1][i2]].dis;
					}
				}



				//刷新主显示
				if (gamedis_stop) {

					//玩家显示
					d1[pl.y].d2[pl.x].d3 = "Q ";

					//方向标
					switch (pl.turn)
					{
					case 0:if ((pl.y - 1 >= 0) && (blocks[world_blocks[pl.y - 1][pl.x]].can_dised == 1)) d1[pl.y - 1].d2[pl.x].d3 = "↑"; break;
					case 1:if ((pl.y + 1 <= 9) && (blocks[world_blocks[pl.y + 1][pl.x]].can_dised == 1)) d1[pl.y + 1].d2[pl.x].d3 = "↓"; break;
					case 2:if ((pl.x - 1 >= 0) && (blocks[world_blocks[pl.y][pl.x - 1]].can_dised == 1)) d1[pl.y].d2[pl.x - 1].d3 = "←"; break;
					case 3:if ((pl.x + 1 <= 9) && (blocks[world_blocks[pl.y][pl.x + 1]].can_dised == 1)) d1[pl.y].d2[pl.x + 1].d3 = "→"; break;
					}

					CLS;
					

					cout << "S T Game" << v << endl << endl
						<< "提示: " << tips << endl << endl
						<< d1[0].d2[0].d3 << d1[0].d2[1].d3 << d1[0].d2[2].d3 << d1[0].d2[3].d3 << d1[0].d2[4].d3 << d1[0].d2[5].d3 << d1[0].d2[6].d3 << d1[0].d2[7].d3 << d1[0].d2[8].d3 << d1[0].d2[9].d3 << " " << states[0].dis << endl
						<< d1[1].d2[0].d3 << d1[1].d2[1].d3 << d1[1].d2[2].d3 << d1[1].d2[3].d3 << d1[1].d2[4].d3 << d1[1].d2[5].d3 << d1[1].d2[6].d3 << d1[1].d2[7].d3 << d1[1].d2[8].d3 << d1[1].d2[9].d3 << " " << states[1].dis << endl
						<< d1[2].d2[0].d3 << d1[2].d2[1].d3 << d1[2].d2[2].d3 << d1[2].d2[3].d3 << d1[2].d2[4].d3 << d1[2].d2[5].d3 << d1[2].d2[6].d3 << d1[2].d2[7].d3 << d1[2].d2[8].d3 << d1[2].d2[9].d3 << " " << states[2].dis << endl
						<< d1[3].d2[0].d3 << d1[3].d2[1].d3 << d1[3].d2[2].d3 << d1[3].d2[3].d3 << d1[3].d2[4].d3 << d1[3].d2[5].d3 << d1[3].d2[6].d3 << d1[3].d2[7].d3 << d1[3].d2[8].d3 << d1[3].d2[9].d3 << " " << states[3].dis << endl
						<< d1[4].d2[0].d3 << d1[4].d2[1].d3 << d1[4].d2[2].d3 << d1[4].d2[3].d3 << d1[4].d2[4].d3 << d1[4].d2[5].d3 << d1[4].d2[6].d3 << d1[4].d2[7].d3 << d1[4].d2[8].d3 << d1[4].d2[9].d3 << " " << states[4].dis << endl
						<< d1[5].d2[0].d3 << d1[5].d2[1].d3 << d1[5].d2[2].d3 << d1[5].d2[3].d3 << d1[5].d2[4].d3 << d1[5].d2[5].d3 << d1[5].d2[6].d3 << d1[5].d2[7].d3 << d1[5].d2[8].d3 << d1[5].d2[9].d3 << " " << states[5].dis << endl
						<< d1[6].d2[0].d3 << d1[6].d2[1].d3 << d1[6].d2[2].d3 << d1[6].d2[3].d3 << d1[6].d2[4].d3 << d1[6].d2[5].d3 << d1[6].d2[6].d3 << d1[6].d2[7].d3 << d1[6].d2[8].d3 << d1[6].d2[9].d3 << " " << states[6].dis << endl
						<< d1[7].d2[0].d3 << d1[7].d2[1].d3 << d1[7].d2[2].d3 << d1[7].d2[3].d3 << d1[7].d2[4].d3 << d1[7].d2[5].d3 << d1[7].d2[6].d3 << d1[7].d2[7].d3 << d1[7].d2[8].d3 << d1[7].d2[9].d3 << " " << states[7].dis << endl
						<< d1[8].d2[0].d3 << d1[8].d2[1].d3 << d1[8].d2[2].d3 << d1[8].d2[3].d3 << d1[8].d2[4].d3 << d1[8].d2[5].d3 << d1[8].d2[6].d3 << d1[8].d2[7].d3 << d1[8].d2[8].d3 << d1[8].d2[9].d3 << " " << states[8].dis << endl
						<< d1[9].d2[0].d3 << d1[9].d2[1].d3 << d1[9].d2[2].d3 << d1[9].d2[3].d3 << d1[9].d2[4].d3 << d1[9].d2[5].d3 << d1[9].d2[6].d3 << d1[9].d2[7].d3 << d1[9].d2[8].d3 << d1[9].d2[9].d3 << " " << states[9].dis << endl
						<< endl << "背包:" << items[pl.items[0].type].name << " * " << pl.items[0].amount;
					
					
				}

				if (gamedis_stop == 0) Sleep(80);

				//重置提示
				tips = "按下\"WASD\"或\"↑↓←→\"以移动！";

				//停止主显示循环 并 重置循环变量
				gamedis_stop = 0;

				//获取按键(无需暂停)
				if (_kbhit()) {
					input = _getch();
				}
				else {
					input = 0;
				}

				//按键判断
				if (input == 72) {
					if (pl.turn != 0) {
						pl.turn = 0;
					}
					else {
						if ((pl.y - 1 >= 0) && (blocks[world_blocks[pl.y - 1][pl.x]].can_th == 1)) pl.y -= 1;
					}
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 'w' || input == 'W') {
					pl.turn = 0;
					if ((pl.y - 1 >= 0) && (blocks[world_blocks[pl.y - 1][pl.x]].can_th == 1)) pl.y -= 1;
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 80) {
					if (pl.turn != 1) {
						pl.turn = 1;
					}
					else {
						if ((pl.y + 1 <= 9) && (blocks[world_blocks[pl.y + 1][pl.x]].can_th == 1)) pl.y += 1;
					}
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 's' || input == 'S') {
					pl.turn = 1;
					if ((pl.y + 1 <= 9) && (blocks[world_blocks[pl.y + 1][pl.x]].can_th == 1)) pl.y += 1;
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 75) {
					if (pl.turn != 2) {
						pl.turn = 2;
					}
					else {
						if ((pl.x - 1 >= 0) && (blocks[world_blocks[pl.y][pl.x - 1]].can_th == 1)) pl.x -= 1;
					}
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 'a' || input == 'A') {
					pl.turn = 2;
					if ((pl.x - 1 >= 0) && (blocks[world_blocks[pl.y][pl.x - 1]].can_th == 1)) pl.x -= 1;
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 77) {
					if (pl.turn != 3) {
						pl.turn = 3;
					}
					else {
						if ((pl.x + 1 <= 9) && (blocks[world_blocks[pl.y][pl.x + 1]].can_th == 1)) pl.x += 1;
					}
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 'd' || input == 'D') {
					pl.turn = 3;
					if ((pl.x + 1 <= 9) && (blocks[world_blocks[pl.y][pl.x + 1]].can_th == 1)) pl.x += 1;
					gamedis_stop = 1;
					block_broke = 0U;
				}
				else if (input == 27) {
					game_stop = 0;
					block_broke = 0U;
				}
				else if (input == 'j' || input == 'J') {

					switch (pl.turn) //破坏方块判断
					{
					case 0:
						if (block_broke == 0 && pl.y - 1 >= 0) {
							block_yd = blocks[world_blocks[pl.y - 1][pl.x]].yd;
							block_broke_type = world_blocks[pl.y - 1][pl.x];
							block_broke_x = pl.x;
							block_broke_y = pl.y - 1;
							gamedis_stop = 1;
						}
						if (pl.y - 1 >= 0) { 
							block_broke += 1; 
							gamedis_stop = 1;
						}
						break;

					case 1:
						if (block_broke == 0 && pl.y + 1 <= 9) {
							block_yd = blocks[world_blocks[pl.y + 1][pl.x]].yd;
							block_broke_type = world_blocks[pl.y + 1][pl.x];
							block_broke_x = pl.x;
							block_broke_y = pl.y + 1;
							gamedis_stop = 1;
						}
						if (pl.y + 1 <= 9) { 
							block_broke += 1; 
							gamedis_stop = 1;
						}
						break;

					case 2:
						if (block_broke == 0 && pl.x - 1 >= 0) {
							block_yd = blocks[world_blocks[pl.y][pl.x - 1]].yd;
							block_broke_type = world_blocks[pl.y][pl.x - 1];
							block_broke_x = pl.x - 1;
							block_broke_y = pl.y;
							gamedis_stop = 1;
						}
						if (pl.x - 1 >= 0) { 
							block_broke += 1; 
							gamedis_stop = 1;
						}
						break;

					case 3:
						if (block_broke == 0 && pl.x + 1 <= 9) {
							block_yd = blocks[world_blocks[pl.y][pl.x + 1]].yd;
							block_broke_type = world_blocks[pl.y][pl.x + 1];
							block_broke_x = pl.x + 1;
							block_broke_y = pl.y;
							gamedis_stop = 1;
						}
						if (pl.x + 1 <= 9) { 
							block_broke += 1; 
							gamedis_stop = 1;
						}
						break;
					} //破坏方块判断
				}
				else if (input == 'k' || input == 'K') {
					switch (pl.turn) {
					case 0:if (items[pl.items[pl.item_ch].type].can_place && pl.items[pl.item_ch].amount >= 1 && blocks[world_blocks[pl.y - 1][pl.x]].can_placed) {
						world_blocks[pl.y - 1][pl.x] = items[pl.items[pl.item_ch].type].place_block;
						pl.items[pl.item_ch].amount -= 1;
						gamedis_stop = 1;
						if (pl.items[pl.item_ch].amount == 0) {
							pl.items[pl.item_ch].type = 0;
						}
					}
						break;
					case 1:if (items[pl.items[pl.item_ch].type].can_place && pl.items[pl.item_ch].amount >= 1 && blocks[world_blocks[pl.y + 1][pl.x]].can_placed) {
						world_blocks[pl.y + 1][pl.x] = items[pl.items[pl.item_ch].type].place_block;
						pl.items[pl.item_ch].amount -= 1;
						gamedis_stop = 1;
						if (pl.items[pl.item_ch].amount == 0) {
							pl.items[pl.item_ch].type = 0;
						}
					}
						  break;
					case 2:if (items[pl.items[pl.item_ch].type].can_place && pl.items[pl.item_ch].amount >= 1 && blocks[world_blocks[pl.y][pl.x - 1]].can_placed) {
						world_blocks[pl.y][pl.x - 1] = items[pl.items[pl.item_ch].type].place_block;
						pl.items[pl.item_ch].amount -= 1;
						gamedis_stop = 1;
						if (pl.items[pl.item_ch].amount == 0) {
							pl.items[pl.item_ch].type = 0;
						}
					}
						  break;
					case 3:if (items[pl.items[pl.item_ch].type].can_place && pl.items[pl.item_ch].amount >= 1 && blocks[world_blocks[pl.y][pl.x + 1]].can_placed) {
						world_blocks[pl.y][pl.x + 1] = items[pl.items[pl.item_ch].type].place_block;
						pl.items[pl.item_ch].amount -= 1;
						gamedis_stop = 1;
						if (pl.items[pl.item_ch].amount == 0) {
							pl.items[pl.item_ch].type = 0;
						}
					}
						  break;
					}
				}//按键判断

				//更新状态栏: 破坏方块
				if (block_broke > 0 && block_yd >= 0) {
					for (int i = 0; i < 10; i++) {
						if (states[i].on == 0 || states[i].type == 1) {

							string s1 = "正在破坏: ", s2 = blocks[block_broke_type].name;
							states[i].dis = s1 + s2;
							states[i].on = 1;
							states[i].type = 1;
							gamedis_stop = 1;
							break;
						}
					}
				}

				//破坏方块的提示
				switch (pl.turn) {
				case 0:if (blocks[world_blocks[pl.y - 1][pl.x]].yd >= 0) { tips = "按住并持续\"J\"键以挖掘面前的方块！"; gamedis_stop = 1; }
					  else if (block_broke < block_yd) {
					for (int i = 0; i < 5; i++) {
						if (states[i].type == 1) {
							gamedis_stop = 1;
							block_broke = 0;
							gat.obj[1] = gat.all + 15;
							states[i].dis = "破坏方块失败";
							block_yd = -1;
							break;
						}
					}
				}
					  break;
				case 1:if (blocks[world_blocks[pl.y + 1][pl.x]].yd >= 0) { tips = "按住并持续\"J\"键以挖掘面前的方块！"; gamedis_stop = 1; } 
					  else if (block_broke < block_yd) {
					for (int i = 0; i < 5; i++) {
						if (states[i].type == 1) {
							gamedis_stop = 1;
							block_broke = 0;
							gat.obj[1] = gat.all + 15;
							states[i].dis = "破坏方块失败";
							block_yd = -1;
							break;
						}
					}
				}
					  break;
				case 2:if (blocks[world_blocks[pl.y][pl.x - 1]].yd >= 0) { tips = "按住并持续\"J\"键以挖掘面前的方块！"; gamedis_stop = 1; }
					  else if (block_broke < block_yd) {
					for (int i = 0; i < 5; i++) {
						if (states[i].type == 1) {
							gamedis_stop = 1;
							block_broke = 0;
							gat.obj[1] = gat.all + 15;
							states[i].dis = "破坏方块失败";
							block_yd = -1;
							break;
						}
					}
				}
					  break;
				case 3:if (blocks[world_blocks[pl.y][pl.x + 1]].yd >= 0) { tips = "按住并持续\"J\"键以挖掘面前的方块！"; gamedis_stop = 1; } 
					  else if (block_broke < block_yd) {
					for (int i = 0; i < 5; i++) {
						if (states[i].type == 1) {
							gamedis_stop = 1;
							block_broke = 0;
							gat.obj[1] = gat.all + 15;
							states[i].dis = "破坏方块失败";
							block_yd = -1;
							break;
						}
					}
				}
					  break;
				}

				//成功破坏方块判断
				if (block_broke == block_yd) {
					bool stop = 1;
					for (int i = 0; i < 5; i++) {
						if (pl.items[i].type == blocks[block_broke_type].item[0]) {
							pl.items[i].amount += blocks[block_broke_type].amount[0];
							world_blocks[block_broke_y][block_broke_x] = 0;
							gamedis_stop = 1;
							block_broke = 0;
							gat.obj[1] = gat.all + 15;
							states[i].dis = "破坏方块成功";

							break;

						}
						else if (pl.items[i].type == 0) {
							pl.items[i].type = blocks[block_broke_type].item[0];
							pl.items[i].amount = blocks[block_broke_type].amount[0];
							world_blocks[block_broke_y][block_broke_x] = 0;
							gamedis_stop = 1;
							block_broke = 0;
							gat.obj[1] = gat.all + 15;
							states[i].dis = "破坏方块成功";

							break;
						}
					}

				}

				//破坏方块进度
				if(block_broke != 0 && block_yd >= 0 && block_broke < block_yd){
					for (int i = 0; i < 10; i++) {
						if (states[i].type == 1) {
							string add = "(进度:  %)";
							int jd = ((double)block_broke / (double)block_yd * 100);
							add[5] = ((jd - jd % 10) / 10 + 48);
							add[6] = (jd % 10 + 48);
							states[i].dis += add;
							gamedis_stop = 1;
							break;
						}
					}
				}

				//检测时间周期 并 条件触发项目
				for (int i = 0; i < 5; i++)  //状态栏延迟清除检测
				{
					if (gat.obj[i] == gat.all) {
						for (int i1 = 0; i1 < 10; i1++) {
							if (states[i1].type == i) {
								states[i1].dis = " ";
								states[i1].on = 0;
								states[i1].type = 0;
								gamedis_stop = 1;
							}
						}
					}
				}

				last_input = input;
				
				
			} //主游戏循环


		}
		else if (stch == '2') {
			int a = 0;
			while (true) {
				CLS;
				a = _getch();
				printf("%d", a);
				Sleep(2000);
			}
		}
		else if (stch == '3') {
			printf("这是3");
		}
		else if (stch == '4') {
			printf("这是4");
		}
		else if (stch == '5') {
			CLS;
			printf("S T 版本: %s\n"
				"制作:QQ1851445251\n\n", v);
			system("pause");
		}
		else {
			printf("请正确输入！");
			stch = 0;
			Sleep(1000);
			CLS;
			continue;
		}

		Sleep(100);
		CLS;

		
	}

	return 0;
}



