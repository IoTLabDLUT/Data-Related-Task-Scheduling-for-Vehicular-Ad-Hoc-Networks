#include<stdio.h>
#include<conio.h>
#include<malloc.h>
#include<stdlib.h>
#include <time.h>
#define MAX_VERTEX_NUM  60   //最大定点个数
#define CORE_NUM 4


typedef struct ArcNode
{     
	int    later_adjvex;    //弧度指向的顶点位置   
	int    pre_adjvex;   
	struct ArcNode  *next_later_arc;     //下一个同一节点发出的弧
	struct ArcNode *next_pre_arc;  //下一个指向同一节点的弧
	struct ArcNode *nextBus;
	int info;
	int start;
	int end;
	bool pre;
}ArcNode;

typedef struct VNode 
{     
	char data; //任务字母编号
	bool flag;//分配
	bool done;//数据传输
	int core_num; //所在内核
	int info; //计算量
	int start; //开始时间
	int end; //结束时间
	ArcNode   * first_later_arc;  //指向第一条顶点的弧
	ArcNode *first_pre_arc; //第一条指向该顶点的弧
	VNode *nextTask; //内核任务队列中，该任务的下一个任务
}VNode, AdjList[MAX_VERTEX_NUM];

typedef struct
{
	AdjList  ver;      //邻接表表头
	int  vexnum, arcnum;    //当前顶点数和弧数
}ALGraph; 

typedef struct Core
{
	VNode *firstTask;
	VNode *nowTask;
}Core,Num[CORE_NUM];

typedef struct Bus
{
	ArcNode *nowTask;
	ArcNode *firstTask;
};


int LocateVex(ALGraph G,char v)   //确定v在G中的位置
{int k=1;
do{
	if(G.ver[k++].data==v)  break;   
}while(k<=G.vexnum); //判断是否满足条件然后运行do
return(--k);//？
}

void Create(ALGraph &G,FILE *fp) 
{int i,j,k,inf;
char v1,v2;
fscanf(fp,"%d,%d",&G.vexnum,&G.arcnum); //去指定文件fp按照格式读数
fgetc(fp);//读空格
 for(i=1;i<=G.vexnum;i++)
 {fscanf(fp,"%c%d",&G.ver[i].data,&G.ver[i].info);//读ver数组
G.ver[i].first_later_arc=NULL;
G.ver[i].first_pre_arc=NULL;
G.ver[i].flag=false;
G.ver[i].nextTask=NULL;
G.ver[i].core_num=1;
G.ver[i].start=-1;
G.ver[i].end=-1;
G.ver[i].done=false;
 }
fgetc(fp);
for(k=1;k<=G.arcnum;++k)
 {ArcNode *p;
  fscanf(fp,"%c%c%d",&v1,&v2,&inf);//读v1v2
fgetc(fp);
i=LocateVex(G,v1);  //确定v1v2在G中位置
j=LocateVex(G,v2);
p=(ArcNode *)malloc(sizeof(ArcNode));//申请一个节点
p->later_adjvex=j;
p->pre_adjvex=i;
p->info=inf;
p->start=-1;
p->end=-1;
p->pre=false;
p->nextBus=NULL;

ArcNode *s; //遍历寻找p适合插入的位置，标记后一节点
ArcNode *pre; //标记前一节点
char sChar; 


if(G.ver[i].first_later_arc==NULL)  //从矩阵转为邻接表
{
	G.ver[i].first_later_arc=p;
	p->next_later_arc=NULL;
}
else
{
	s=G.ver[i].first_later_arc;
	sChar=G.ver[s->later_adjvex].data;
	if(G.ver[j].data < sChar)
	{
		p->next_later_arc=s;
		G.ver[i].first_later_arc=p;
	}
	else
	{
		pre = G.ver[i].first_later_arc;
		s = s->next_later_arc;
		while(s)
		{
			sChar = G.ver[s->later_adjvex].data;
			if(G.ver[j].data > sChar)
			{
				pre = pre->next_later_arc;
				s = s->next_later_arc;
			}
			else
				break;
		}
		p->next_later_arc = pre->next_later_arc;
		pre->next_later_arc = p;
	}
}



p->next_pre_arc=G.ver[j].first_pre_arc;
G.ver[j].first_pre_arc=p;
 }
}
//输出邻接表
void DispAdj(ALGraph G) 
{int i;
ArcNode *p;
for(i=1;i<=G.vexnum;i++)
{ p=G.ver[i].first_later_arc;
if(p!=NULL)  printf("%3d:",i);
while(p!=NULL)
{printf("%3d,%d",p->later_adjvex,p->info);
p=p->next_later_arc;
}
printf("\n");
}
for(i=1;i<=G.vexnum;i++)
{ p=G.ver[i].first_pre_arc;
if(p!=NULL)  printf("%3d:",i);
while(p!=NULL)
{printf("%3d,%d",p->pre_adjvex,p->info);
p=p->next_pre_arc;
}
printf("\n");
}
}

void CoreAssign(int i,Core core[],int core_num,ALGraph &G)  //将任务i分配到内核core_num的任务队列中
{
	VNode *s;
	VNode *pre;
	if(core[core_num].firstTask)
	{
		s=core[core_num].firstTask;
		if(G.ver[i].data<s->data)
		{
			G.ver[i].nextTask=s;
			core[G.ver[i].core_num].firstTask=&G.ver[i];
		}
		else
		{
			while(s)
			{
				if(G.ver[i].data<s->data)
				{
					G.ver[i].nextTask=s;
					pre->nextTask=&G.ver[i];
					break;
				}
				pre=s;
				s=s->nextTask;
			}
			if(!s)
			{
				G.ver[i].nextTask=NULL;
				pre->nextTask=&G.ver[i];
			}
		}
	}
	else
	{
		core[core_num].firstTask=&G.ver[i];
	}

}

void TaskAssign(ALGraph &G,Core core[])    //对周期任务簇G进行内核分配
{
	int circle=0;
	for(int i=1;i<=G.vexnum;i++)
	{			
		ArcNode *p;
		ArcNode *max;
		if(!G.ver[i].flag)
		{
			p=G.ver[i].first_pre_arc;
			max=G.ver[i].first_pre_arc;
			if(G.ver[i].first_pre_arc!=NULL)//有前驱
			{

				if(p->next_pre_arc!=NULL)//有多个前驱
				{	

					while(p->next_pre_arc)
					{
						p=p->next_pre_arc;
						if(max->info<p->info)
							max=p;
					}

					G.ver[i].core_num=G.ver[max->pre_adjvex].core_num;
					CoreAssign(i,core,G.ver[i].core_num,G);

					continue;   //对于有多个前驱的任务，分配至关联性最大的内核，然后分配下一个任务
				}



				else //只有一个前驱
				{
					ArcNode *q;
					ArcNode *max1;
					q=G.ver[p->pre_adjvex].first_later_arc;
					max1=G.ver[p->pre_adjvex].first_later_arc;

					if(q->next_later_arc)//前驱有多个后继
					{ 

						while(q->next_later_arc)
						{
							q=q->next_later_arc;
							if(q->info>max1->info)
								max1=q;
						}
						if(max1->later_adjvex==i)
						{	
							G.ver[i].core_num=G.ver[p->pre_adjvex].core_num;
							CoreAssign(i,core,G.ver[i].core_num,G);

						}

						else//有一个前驱但不是前驱最大的后继
						{
							G.ver[i].core_num=circle;
							CoreAssign(i,core,circle,G);
							circle++;
							if(circle>=CORE_NUM)
								circle=0;

						}

					}
					else//前驱只有一个后继
					{
						G.ver[i].core_num=G.ver[p->pre_adjvex].core_num;
						CoreAssign(i,core,G.ver[i].core_num,G);

					}

				}

			}
			else//没有前驱
			{
				G.ver[i].core_num=circle;
				CoreAssign(i,core,circle,G);
				circle++;
				if(circle>=CORE_NUM)
					circle=0;

			}

		}
	}
}

void DisplayTask(Core core[])   //输出内核任务分配情况
{
	VNode *p;
	for(int i=0;i<CORE_NUM;i++)
	{
		if(core[i].firstTask)
		{
			printf("%d:",i);
			p=core[i].firstTask;
			while(p)
			{
				printf("%c,",p->data);
				p=p->nextTask;

			}
			printf("\n");
		}
	}
}

int isTaskDone(ALGraph &G,Core core[],int m,int time[])  //返回内核m任务队列中任务执行情况，所有任务执行完毕，返回0
{                                                        //                                 当前任务执行完毕，返回结束时间
	bool ready=true;  //ready用来标记当前任务的所有前驱边是否已执行完毕
	int start=time[m];
	int end=-1;
	if(core[m].nowTask)
	{
		if(core[m].nowTask->done)     //内核m当前任务已完成，指向下一个任务，若已无任务，则返回0
		{
			if(core[m].nowTask->nextTask)
				core[m].nowTask=core[m].nowTask->nextTask;
			else
				return 0;
		}
		if(core[m].nowTask->end>0)          //内核m当前任务已完成，返回完成时间
		{
			return core[m].nowTask->end;
		}
		else    //内核m目前任务未完成，开始目前任务的执行
		{
			if(core[m].nowTask->first_pre_arc!=NULL)
			{
				ArcNode *p;
				p=core[m].nowTask->first_pre_arc;
				while(p) //遍历当前任务所有的前驱边
				{
					if(core[m].nowTask->core_num==G.ver[p->pre_adjvex].core_num||p->pre) //如果当前任务与前驱任务在一个内核，或前驱边已完成
					{
						if(G.ver[p->pre_adjvex].end>0)   //如果本条边p已完成
						{
							if(start<G.ver[p->pre_adjvex].end) //如果当前内核时间小于本条边p完成时间，则更新当前内核时间
								start=G.ver[p->pre_adjvex].end;
						}
						else //如果本条边p未完成，则ready为假，当前任务还未准备完毕，退出循环
						{
							ready=false;
							break;
						}
					}
					else
					{
						if(p->end>0)
						{
							if(start<p->end)
								start=p->end;
						}
						else
						{
							ready=false;
							break;
						}
					}
					p=p->next_pre_arc;
				}
				if(ready)
				{
					core[m].nowTask->start=start;
					end=start+core[m].nowTask->info;
					core[m].nowTask->end=end;
				}
				return end;
			}
			else
			{
				core[m].nowTask->start=0;
				core[m].nowTask->end=core[m].nowTask->start+core[m].nowTask->info;
				end=core[m].nowTask->end;
				
				return end;
			}
		}
	}
	else
		return 0;
}

int Calculate(ALGraph &G,Core core[],Bus &bus)
{
	int time[CORE_NUM]={0};
	bool cmpl=false;
	while(!cmpl)
	{
		cmpl=true;
		for(int i=0;i<CORE_NUM;i++)
		{
			time[i]=isTaskDone(G,core,i,time);
			if(time[i]!=0)
				cmpl=false;
		}
		//printf("%d,%d,%d ",time[0],time[1],time[2]);
		if(!cmpl)
		{
			int m=0;
			int min;
			for(int i=0;i<CORE_NUM;i++)
			{
				if(time[i]>0)
				{
					min=time[i];
					m=i;
					break;
				}
			}
			for(int i=1;i<CORE_NUM;i++)
			{
				if(min>time[i]&&time[i]>0)
				{
					min=time[i];
					m=i;
				}
			}
			if(min>0)
			{
				ArcNode *p;
				p=NULL;
				if(core[m].nowTask->first_later_arc)
				{
					p=core[m].nowTask->first_later_arc;
				}
				while(p)
				{
					if(G.ver[p->pre_adjvex].core_num!=G.ver[p->later_adjvex].core_num)
					{
						if(bus.nowTask)
						{
							p->start=bus.nowTask->end>G.ver[p->pre_adjvex].end?bus.nowTask->end:G.ver[p->pre_adjvex].end;
							p->end=p->start+p->info;
							bus.nowTask->nextBus=p;
							bus.nowTask=p;
						}
						else
						{
							p->start=core[m].nowTask->end;
							p->end=p->start+p->info;
							bus.nowTask=p;
							bus.firstTask=p;
						}
					}
					p=p->next_later_arc;
				}
				core[m].nowTask->done=true;
			}
		}
	}
	int t=core[0].nowTask->end;
	for(int i=1;i<CORE_NUM;i++)
	{
		if(core[i].nowTask != NULL)
		{
			if(t<core[i].nowTask->end)
			t=core[i].nowTask->end;
		}
		

	}
	if(t<bus.nowTask->end)
		t=bus.nowTask->end;
	return t;
}
void SetPre(ALGraph &G,Bus &bus)
{
	int max=0;
	int time;
	int pretime;
	ArcNode *p;
	p=bus.firstTask;
	time=p->info;
	while(p&&p->nextBus)
	{
		if(p->nextBus->pre_adjvex==p->pre_adjvex&&p->end==p->nextBus->start)
		{
			time=time+p->nextBus->info;
			p=p->nextBus;
			if(time>max)
			{
				max=time;
				pretime=p->end;
			}
		}
		else
		{
			p=p->nextBus;
			time=p->info;
			if(time>max)
			{
				max=time;
				pretime=p->end;
			}
		}
	}
	p=bus.firstTask;
	while(p)
	{
		if(p->end<=pretime)
			p->pre=true;
		else
			break;
		p=p->nextBus;
	}
}
void ReSet(ALGraph &G,Bus &bus)
{
	for(int i=1;i<=G.vexnum;i++)
	{
		G.ver[i].done=false;
		G.ver[i].start=-1;
		G.ver[i].end=-1;
		ArcNode *p;
		p=G.ver[i].first_later_arc;
		while(p)
		{
			p->start=-1;
			p->end=-1;
			p->nextBus=NULL;
			p=p->next_later_arc;
		}
	}
	bus.firstTask=NULL;
	bus.nowTask=NULL;
}
void AddNode(ALGraph &G,int i)
{

	G.ver[i].data++;
}
bool CreateRandom(ALGraph &G, Core core[], Bus &bus, int length) //产生一个随机任务，并按照本文算法分配至相应内核
{
	int add=G.vexnum+1; //任务节点数+1
	int vInfo;

	int m;
	int n;
	char mChar;
	char nChar;
	int arcInfo;

	int allInfo = 0;

	FILE *fp;
	if((fp=fopen("e:\\DATA2.txt","r"))==NULL)
	{
		printf("This file is not exist!\n");
		exit(0);
	}

	fscanf(fp, "%d,%d,%d", &vInfo, &m, &n);
	G.ver[add].info=vInfo;
	G.ver[add].done=false;
	G.ver[add].start=-1;
	G.ver[add].end=-1;
	G.ver[add].flag=false;
	G.ver[add].nextTask=NULL;
	G.ver[add].first_later_arc=NULL;
	G.ver[add].first_pre_arc=NULL;

	char maxData='A';
	int coreInfo[CORE_NUM];
	for(int core_num=0; core_num<CORE_NUM; core_num++)
	{
		coreInfo[core_num] = 0;
	}

	for(int i=1; i<=m; i++)
	{
		ArcNode *p;
		fgetc(fp);
		fscanf(fp,"%c%d", &mChar, &arcInfo);//读mChar以及权值
		if(mChar > maxData)
		{
			maxData = mChar;
		}
		allInfo = allInfo + arcInfo;
		int mNmb=LocateVex(G,mChar);  //确定mChar在G中位置
		p=(ArcNode *)malloc(sizeof(ArcNode));//申请一个节点
		p->later_adjvex=add;
		p->pre_adjvex=mNmb;
		p->info=arcInfo;
		coreInfo[G.ver[mNmb].core_num] = coreInfo[G.ver[mNmb].core_num] + arcInfo;
		p->start=-1;
		p->end=-1;
		p->pre=false;
		p->nextBus=NULL;

		p->next_pre_arc=G.ver[add].first_pre_arc;
		G.ver[add].first_pre_arc=p;
		p->next_later_arc=G.ver[mNmb].first_later_arc;
		G.ver[mNmb].first_later_arc=p;
	}

	for( int j = 1; j <= n; j++ )
	{
		ArcNode *p;
		fgetc(fp);
		fscanf(fp,"%c%d", &nChar, &arcInfo);//读nChar以及权值
		allInfo = allInfo + arcInfo;
		
		int nNmb=LocateVex(G,nChar);  //确定nChar在G中位置
		p=(ArcNode *)malloc(sizeof(ArcNode));//申请一个节点
		p->later_adjvex=nNmb;
		p->pre_adjvex=add;
		p->info=arcInfo;
		coreInfo[G.ver[nNmb].core_num] = coreInfo[G.ver[nNmb].core_num] + arcInfo;
		p->start=-1;
		p->end=-1;
		p->pre=false;
		p->nextBus=NULL;

		p->next_pre_arc = G.ver[nNmb].first_pre_arc;
		G.ver[nNmb].first_pre_arc = p;
		p->next_later_arc = G.ver[add].first_later_arc;
		G.ver[add].first_later_arc = p;
	}
	fclose(fp);

	if(maxData == 'Z')
	{
		G.ver[add].data = 'a';
	}
	else
	{
		G.ver[add].data = maxData + 1;
	}
	
	for(int i = 1; i<=G.vexnum; i++)
	{
		if(G.ver[i].data >= G.ver[add].data)
		{
			if(G.ver[i].data == 'Z')
				G.ver[i].data = 'a';
			else
				G.ver[i].data++;
		}
	}


	int assignCore;
	assignCore = 0;
	int maxCore;
	maxCore= coreInfo[0];
	for(int i=0; i<CORE_NUM; i++)
	{
		if(maxCore < coreInfo[i])
		{
			maxCore = coreInfo[i];
			assignCore = i;
		}
	}
	printf("减少量%d",maxCore);
	printf("\n");

	int restInfo = allInfo - maxCore; 

	int fullBus = 0;
	ArcNode *busTask;
	busTask = bus.firstTask;
	while(busTask)
	{
		fullBus = fullBus + busTask->info;
		busTask = busTask ->nextBus;
	}

	int restBus = length - fullBus;


	G.ver[add].core_num = assignCore;
	G.vexnum++;
	CoreAssign(add,core,G.ver[add].core_num,G);

	if(restBus>=restInfo)
	return true;
}

void ReAssignRandom(ALGraph &G,Core core[]) //这里将产生的随机任务重新分配(负载均衡)，用于比较实验结果
{
	VNode *pre;
	pre=NULL;
	VNode *random;
	random=core[G.ver[G.vexnum].core_num].firstTask;
	while(random)
	{
		if(G.ver[G.vexnum].data==random->data)
		{
			if(!pre)
			{
				core[G.ver[G.vexnum].core_num].firstTask=random->nextTask;
			}
			else
			{
				pre->nextTask=random->nextTask;
			}
		}
		pre=random;
		random=random->nextTask;
	}
	G.ver[G.vexnum].nextTask=NULL;
	
	int full[CORE_NUM];
	for(int i= 0; i<CORE_NUM; i++)
	{
		full[i]=0;
	}
	for(int i=0; i<CORE_NUM; i++)
	{
		VNode *fullTask;
		fullTask= core[i].firstTask;
		while(fullTask)
		{
			full[i]=full[i] + fullTask->info;
			fullTask = fullTask -> nextTask;
		}
	}
	int minFull = full[0];
	int assignNum= 0;
	for(int i=0; i<CORE_NUM; i++)
	{
		if(full[i] < minFull)
		{
			minFull = full[i];
			assignNum=i;
		}
	}
	G.ver[G.vexnum].core_num = assignNum;
	CoreAssign(G.vexnum,core,G.ver[G.vexnum].core_num,G);
}

void ReAssignRandom2(ALGraph &G,Core core[], int cyclic) //这里将产生的随机任务重新分配(指定内核cyclic)，用于比较实验结果
{
	VNode *pre;
	pre=NULL;
	VNode *random;
	random=core[G.ver[G.vexnum].core_num].firstTask;
	while(random)
	{
		if(G.ver[G.vexnum].data==random->data)
		{
			if(!pre)
			{
				core[G.ver[G.vexnum].core_num].firstTask=random->nextTask;
			}
			else
			{
				pre->nextTask=random->nextTask;
			}
		}
		pre=random;
		random=random->nextTask;
	}
	G.ver[G.vexnum].nextTask=NULL;
	G.ver[G.vexnum].core_num = cyclic;
	CoreAssign(G.vexnum,core,G.ver[G.vexnum].core_num,G);

}

void SetRandomPre(ALGraph &G, Core core[])
{
	ArcNode *pre;
	ArcNode *later;
	pre = G.ver[G.vexnum].first_pre_arc;
	while(pre)
	{
		pre->pre = true;
		pre = pre->next_pre_arc;
	}
	later = G.ver[G.vexnum].first_later_arc;
	while(later)
	{
		later->pre = true;
		later = later->next_later_arc;
	}
}

void main()
{ ALGraph G;
  FILE *fp;
  Core core[CORE_NUM];
  for(int i=0;i<CORE_NUM;i++)
	  core[i].firstTask=NULL;
//printf("Create a Graph\n");
if((fp=fopen("e:\\DATA.txt","r"))==NULL)
		{ printf("This file is not exist!\n");
	exit(0);
		   }
Create(G,fp);
//printf("The graph is already created!Now display it!\n");
//DispAdj(G);
fclose(fp);

TaskAssign(G,core);

DisplayTask(core);
//printf("%c",core[0].firstTask->data);

for(int i=0;i<CORE_NUM;i++)
	core[i].nowTask=core[i].firstTask;

int t;
Bus bus;
bus.nowTask=NULL;
t=Calculate(G,core,bus);
printf("%d\n",t);
printf("\n");

SetPre(G,bus);

//ArcNode *p;
//p=bus.firstTask;
//while(p)
//{
//	if(p->pre)
	//	printf("T");
//	p=p->nextBus;
//}

ReSet(G,bus);
for(int j=0;j<CORE_NUM;j++)
	core[j].nowTask=core[j].firstTask;
t=Calculate(G,core,bus);
printf("%d",t);
printf("\n");

for(int i=0;i<CORE_NUM;i++)
	core[i].nowTask=core[i].firstTask;
bool setPre = CreateRandom(G,core, bus, t);
int myCoreNum = G.ver[G.vexnum].core_num;
ReSet(G,bus);

DisplayTask(core);
printf("%c\n", G.ver[G.vexnum].data);
printf("%d,%d,%d",G.ver[G.vexnum].first_pre_arc->info,G.ver[G.vexnum].info,G.ver[G.vexnum].first_later_arc->info);
printf("\n");
printf("%d,2,%d",G.ver[G.vexnum].first_pre_arc->pre_adjvex,G.ver[G.vexnum].first_later_arc->later_adjvex);
printf("\n");

t=Calculate(G,core,bus);
VNode *tmp;
for(int i = 0; i < CORE_NUM; i++)
{
	tmp = core[i].firstTask;
	int load = 0;
	while(tmp)
	{
		load += tmp->info;
		tmp = tmp->nextTask;
	}
	printf("%d", load);
	printf("\n");
}
printf("%d\n", G.ver[G.vexnum].core_num);
printf("\n");

ReSet(G,bus);
ReAssignRandom(G,core);
for(int i=0;i<CORE_NUM;i++)
	core[i].nowTask=core[i].firstTask;
t=Calculate(G,core,bus);
for(int i = 0; i < CORE_NUM; i++)
{
	tmp = core[i].firstTask;
	int load = 0;
	while(tmp)
	{
		load += tmp->info;
		tmp = tmp->nextTask;
	}
	printf("%d", load);
	printf("\n");
}
printf("%d\n", G.ver[G.vexnum].core_num);
printf("\n");


ReSet(G,bus);
int cyclic=3;
ReAssignRandom2(G,core,cyclic);
for(int i=0;i<CORE_NUM;i++)
	core[i].nowTask=core[i].firstTask;
t=Calculate(G,core,bus);
for(int i = 0; i < CORE_NUM; i++)
{
	tmp = core[i].firstTask;
	int load = 0;
	while(tmp)
	{
		load += tmp->info;
		tmp = tmp->nextTask;
	}
	printf("%d", load);
	printf("\n");
}
printf("%d\n", G.ver[G.vexnum].core_num);
printf("\n");

if(setPre)
{
	SetRandomPre(G,core);
	ReSet(G,bus);
	ReAssignRandom2(G,core,myCoreNum);
	for(int i=0;i<CORE_NUM;i++)
		core[i].nowTask=core[i].firstTask;
	t=Calculate(G,core,bus);
}

for(int i = 0; i < CORE_NUM; i++)
{
	tmp = core[i].firstTask;
	int load = 0;
	while(tmp)
	{
		load += tmp->info;
		tmp = tmp->nextTask;
	}
	printf("%d", load);
	printf("\n");
}
printf("%d\n", G.ver[G.vexnum].core_num);

system("pause");
}
