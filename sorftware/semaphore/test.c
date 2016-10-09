/****************************************************
*semget好比于文件的open,信号量存在时打开,不存在时创建
*	semget(key, num, flags):
*	当所指的key没有信号量,且flags是IPC_CREAT时,系统为此自动创建 num 个信号量的信号量集合
*	成功返回信号集合标识符,失败返-1
*semop操作信号量
*	semop(semid, struct sembuf *sop, nsops):
*	semid是操作的信号量集合的id, nsops 要操作的信号量数量, struct sembuf 是每个信号量的操作内容
*	struct senbuf{
*		sem_num,	//某个信号量的编号
*		sem_op,		//由于信号量获取是-1操作,释放是+1操作,所以这个的值, 正值表示释放信号+1,负值表示获取信号量-1,
*		sem_flg,	//IPC_NOWAIT:对信号的操作不能满足时，semop()不会阻塞，并立即返回，同时设定错误信息。
*				//IPC_UNDO:程序结束时(不论正常或不正常)，保证信号值会被重设为semop()调用前的值。
*				//这样做的目的在于避免程序在异常情况下结束时未将锁定的资源解锁，造成该资源永远锁定。
*	}
*	一般操作多个信号的方式如下:
*	struct senbuf AA[]{
*		{2,1,IPC_NOWAIT},{3,1,IPC_NOWAIT},{4,1,IPC_NOWAIT}
*	}
*	semop(xxx,AA,3);
*
*初始使用信号量前还要检查信号量的值,使用semctl()
*	semctl(semid,0 GETVAL);
*
*现在主要出现了操作信号量时,permission denied 未解决,root下正常运作.
*
*
* perror(char *S):返回临近perror的一个函数执行后的状态,若这函数失败,返回其错误信息
*
*
*两个不同进程打开一个文件,各有两个不同的文件描述符A,B,指向文件,但是描述里的文件操作指针a,b是一致的,
*	表现为:某刻,A-a 指向文件第200个字节,B-b也指向第200个字节,
	       当B-b移到指向第20个字节,A-a也自动同步到第20个字节.
*
*
*
*****************************************************/

#include <sys/types.h>	//semget() semop()
#include <sys/ipc.h>	//semget() semop()
#include <sys/sem.h>	//semget() semop()
#include <sys/stat.h>	


#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define MES1	"first message from master\n\r"
#define MES2	"second message from master\n\r"

int main (void)
{
	int fd;
	key_t key;
	int semid;
	//struct sembuf sops[];			//错误 这样没有初始化的声明,不允许不填数组单元数量
	struct sembuf sops[2];
	int semval0,semval2;			//检查信号量的值
	int ret;				//检查信号量的操作函数状态.

	key = ftok("/home/kingders/smart210/sorftware/semaphore",1);	//创造一个键值,
									//所有通过IPc访问 项目1且指向这个semaphore文件夹里的内容 的进程,
									//都要使用这个key来操作.
	semid = semget(key,3,IPC_CREAT);				//创建了3个信号量的,信号量集合.
	printf("semid:%d\n\r",semid);

	//检查信号量初始值,一般要应为值1,(但是普通用户状态下,操作semctl permission denied)
	semval0 = semctl(semid,0,GETVAL);
	if(semval0 == -1)
		perror("semctl_getvalue");
	semval2 = semctl(semid,2,GETVAL);
	if(semval2 == -1)
		perror("semctl_getvalue");
	printf("semval0:%d,semval2:%d\n\r",semval0,semval2);


#if 1	
	fd = open("./board.txt",O_RDWR|O_APPEND);			//并不执行"没有时新建",所以自己先人为先建一个

	//填写两个信号量的控制方式 获取信号量集合的0和1两个信号量
	//结构体数组变量赋值可以这样
	//sops[0] =(struct sembuf){.sem_num = 2,
	//	   .sem_op = -1,
	//	   .sem_flg = IPC_NOWAIT};
	//sops[1] = (struct sembuf){0,-1,IPC_NOWAIT};
	//也可以这样
	sops[0] =(struct sembuf){.sem_num = 2,
		   .sem_op = -1};
	sops[1] = (struct sembuf){0,-1};
	//不可以这样	
	//sops = ((struct sembuf[2]){{2,-1},{0,-1}});

	ret = semop(semid,sops,2);			//这里操作获取两个信号量
	if(ret == -1)
		perror("semop_getsem");
	printf("ret:%d\n\r",ret);

	write(fd,MES1,sizeof(MES1)-1);		//与读写管道不一样,一个文件里不应该有'\0','\0'是系统自动为文件结尾添加,不用人为写入
						//所以sizeof(MES1)-1,因为sizeof自动算上'\0',write也会因为写入字节比实际字节多一个,
						//而最后把'\0'写入
	//sleep(10);
	//getchar();
	
	write(fd,MES2,sizeof(MES2)-1);

	//释放两个信号量
	sops[0].sem_op = 1;	
	sops[1].sem_op = 1;
	semop(semid,sops,2);
	if(ret == -1)
		perror("semop_freesem");
	close(fd); 

#endif

	return 0;
}





























