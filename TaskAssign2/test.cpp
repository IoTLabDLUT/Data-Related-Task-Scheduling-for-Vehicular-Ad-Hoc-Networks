#include<stdio.h>
#include<conio.h>
#include<malloc.h>
#include<stdlib.h>
#include <time.h>
#define MAX_VERTEX_NUM  60   //��󶨵����
#define CORE_NUM 4


typedef struct ArcNode
{     
	int    later_adjvex;    //����ָ��Ķ���λ��   
	int    pre_adjvex;   
	struct ArcNode  *next_later_arc;     //��һ��ͬһ�ڵ㷢���Ļ�
	struct ArcNode *next_pre_arc;  //��һ��ָ��ͬһ�ڵ�Ļ�
	struct ArcNode *nextBus;
	int info;
	int start;
	int end;
	bool pre;
}ArcNode;

typedef struct VNode 
{     
	char data; //������ĸ���
	bool flag;//����
	bool done;//���ݴ���
	int core_num; //�����ں�
	int info; //������
	int start; //��ʼʱ��
	int end; //����ʱ��
	ArcNode   * first_later_arc;  //ָ���һ������Ļ�
	ArcNode *first_pre_arc; //��һ��ָ��ö���Ļ�
	VNode *nextTask; //�ں���������У����������һ������
}VNode, AdjList[MAX_VERTEX_NUM];

typedef struct
{
	AdjList  ver;      //�ڽӱ��ͷ
	int  vexnum, arcnum;    //��ǰ�������ͻ���
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


int LocateVex(ALGraph G,char v)   //ȷ��v��G�е�λ��
{int k=1;
do{
	if(G.ver[k++].data==v)  break;   
}while(k<=G.vexnum); //�ж��Ƿ���������Ȼ������do
return(--k);//��
}

void Create(ALGraph &G,FILE *fp) 
{int i,j,k,inf;
char v1,v2;
fscanf(fp,"%d,%d",&G.vexnum,&G.arcnum); //ȥָ���ļ�fp���ո�ʽ����
fgetc(fp);//���ո�
 for(i=1;i<=G.vexnum;i++)
 {fscanf(fp,"%c%d",&G.ver[i].data,&G.ver[i].info);//��ver����
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
  fscanf(fp,"%c%c%d",&v1,&v2,&inf);//��v1v2
fgetc(fp);
i=LocateVex(G,v1);  //ȷ��v1v2��G��λ��
j=LocateVex(G,v2);
p=(ArcNode *)malloc(sizeof(ArcNode));//����һ���ڵ�
p->later_adjvex=j;
p->pre_adjvex=i;
p->info=inf;
p->start=-1;
p->end=-1;
p->pre=false;
p->nextBus=NULL;

ArcNode *s; //����Ѱ��p�ʺϲ����λ�ã���Ǻ�һ�ڵ�
ArcNode *pre; //���ǰһ�ڵ�
char sChar; 


if(G.ver[i].first_later_arc==NULL)  //�Ӿ���תΪ�ڽӱ�
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
//����ڽӱ�
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

void CoreAssign(int i,Core core[],int core_num,ALGraph &G)  //������i���䵽�ں�core_num�����������
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

void TaskAssign(ALGraph &G,Core core[])    //�����������G�����ں˷���
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
			if(G.ver[i].first_pre_arc!=NULL)//��ǰ��
			{

				if(p->next_pre_arc!=NULL)//�ж��ǰ��
				{	

					while(p->next_pre_arc)
					{
						p=p->next_pre_arc;
						if(max->info<p->info)
							max=p;
					}

					G.ver[i].core_num=G.ver[max->pre_adjvex].core_num;
					CoreAssign(i,core,G.ver[i].core_num,G);

					continue;   //�����ж��ǰ�������񣬷����������������ںˣ�Ȼ�������һ������
				}



				else //ֻ��һ��ǰ��
				{
					ArcNode *q;
					ArcNode *max1;
					q=G.ver[p->pre_adjvex].first_later_arc;
					max1=G.ver[p->pre_adjvex].first_later_arc;

					if(q->next_later_arc)//ǰ���ж�����
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

						else//��һ��ǰ��������ǰ�����ĺ��
						{
							G.ver[i].core_num=circle;
							CoreAssign(i,core,circle,G);
							circle++;
							if(circle>=CORE_NUM)
								circle=0;

						}

					}
					else//ǰ��ֻ��һ�����
					{
						G.ver[i].core_num=G.ver[p->pre_adjvex].core_num;
						CoreAssign(i,core,G.ver[i].core_num,G);

					}

				}

			}
			else//û��ǰ��
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

void DisplayTask(Core core[])   //����ں�����������
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

int isTaskDone(ALGraph &G,Core core[],int m,int time[])  //�����ں�m�������������ִ���������������ִ����ϣ�����0
{                                                        //                                 ��ǰ����ִ����ϣ����ؽ���ʱ��
	bool ready=true;  //ready������ǵ�ǰ���������ǰ�����Ƿ���ִ�����
	int start=time[m];
	int end=-1;
	if(core[m].nowTask)
	{
		if(core[m].nowTask->done)     //�ں�m��ǰ��������ɣ�ָ����һ�����������������򷵻�0
		{
			if(core[m].nowTask->nextTask)
				core[m].nowTask=core[m].nowTask->nextTask;
			else
				return 0;
		}
		if(core[m].nowTask->end>0)          //�ں�m��ǰ��������ɣ��������ʱ��
		{
			return core[m].nowTask->end;
		}
		else    //�ں�mĿǰ����δ��ɣ���ʼĿǰ�����ִ��
		{
			if(core[m].nowTask->first_pre_arc!=NULL)
			{
				ArcNode *p;
				p=core[m].nowTask->first_pre_arc;
				while(p) //������ǰ�������е�ǰ����
				{
					if(core[m].nowTask->core_num==G.ver[p->pre_adjvex].core_num||p->pre) //�����ǰ������ǰ��������һ���ںˣ���ǰ���������
					{
						if(G.ver[p->pre_adjvex].end>0)   //���������p�����
						{
							if(start<G.ver[p->pre_adjvex].end) //�����ǰ�ں�ʱ��С�ڱ�����p���ʱ�䣬����µ�ǰ�ں�ʱ��
								start=G.ver[p->pre_adjvex].end;
						}
						else //���������pδ��ɣ���readyΪ�٣���ǰ����δ׼����ϣ��˳�ѭ��
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
bool CreateRandom(ALGraph &G, Core core[], Bus &bus, int length) //����һ��������񣬲����ձ����㷨��������Ӧ�ں�
{
	int add=G.vexnum+1; //����ڵ���+1
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
		fscanf(fp,"%c%d", &mChar, &arcInfo);//��mChar�Լ�Ȩֵ
		if(mChar > maxData)
		{
			maxData = mChar;
		}
		allInfo = allInfo + arcInfo;
		int mNmb=LocateVex(G,mChar);  //ȷ��mChar��G��λ��
		p=(ArcNode *)malloc(sizeof(ArcNode));//����һ���ڵ�
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
		fscanf(fp,"%c%d", &nChar, &arcInfo);//��nChar�Լ�Ȩֵ
		allInfo = allInfo + arcInfo;
		
		int nNmb=LocateVex(G,nChar);  //ȷ��nChar��G��λ��
		p=(ArcNode *)malloc(sizeof(ArcNode));//����һ���ڵ�
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
	printf("������%d",maxCore);
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

void ReAssignRandom(ALGraph &G,Core core[]) //���ｫ����������������·���(���ؾ���)�����ڱȽ�ʵ����
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

void ReAssignRandom2(ALGraph &G,Core core[], int cyclic) //���ｫ����������������·���(ָ���ں�cyclic)�����ڱȽ�ʵ����
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
