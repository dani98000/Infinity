#include <stdlib.h>/* EXIT_FAILURE */
#include <stdio.h> /* printf */
#include <sys/sem.h> /*sembuf*/
#include <signal.h> /* sigaction */
#include <unistd.h> /* sleep */
#include <pthread.h>/*threads*/
#include <assert.h>
#include <errno.h>

#include "watchdog.h"
#include "scheduler.h"

#define WD_PATH "./wd"
#define UNUSED(X) (void)(X)

union semun
{
    int val;  
    struct semid_ds *buf;  
    unsigned short *array;  
    struct seminfo *__buf;  
};

int g_wd_pid = 0;
int g_sem_id = 0;
volatile sig_atomic_t g_got_signal = 0;
volatile sig_atomic_t g_should_stop = 0;

static int GetSemVal(int sid, int semnum);
static char **MakeArgv(char *argv[], int argc, int proj_id);
static long HeartBeat(void *params);
static long CheckWatchDog(void *params);
static void *SchedulerRun(void *scheduler);
static void RunThreads(scd_t *scheduler);
static int SemInit(int proj_id, char **argv);
static void sigusr1_handler(int signum);
static void sigusr2_handler(int signum);
static void SignalsInit();
static void ReviveWD(char *argv[]);

int WDStart(int argc, char *argv[], int proj_id)
{
	key_t sem_key;
    struct sembuf wd_ready = {0,-1,0};
    struct sembuf app_ready = {1,1,0};
	char **new_argv = NULL;
	scd_t *scheduler = ScdCreate();

	struct sigaction action1;
	struct sigaction action2;

    action1.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &action1, NULL);

    action2.sa_handler = sigusr2_handler;
    sigaction(SIGUSR2, &action2, NULL);

	/*SignalsInit();*/
	g_sem_id = SemInit(proj_id, argv);

	new_argv = MakeArgv(argv, argc, proj_id);

    if(0 == GetSemVal(g_sem_id, 0))
    {
    	if ((g_wd_pid = fork()) < 0) 
    	{ 
        	perror("fork"); 
       		exit(EXIT_FAILURE); 
    	}
    	if(g_wd_pid == 0)
    	{
    		if(-1 == execv(new_argv[0], new_argv))
    		{
    			perror("exec");
    			exit(EXIT_FAILURE);
    		}
    	}
    }
    else
    {
    	g_wd_pid = getppid();
    }

    ScdAdd(scheduler, 1, HeartBeat, NULL);
	ScdAdd(scheduler, 1, CheckWatchDog, new_argv);
    RunThreads(scheduler);

    if (semop(g_sem_id, &app_ready, 1) == -1) 
    {
        perror("semop(app_ready)");
       	return 0;
    }

    if(GetSemVal(g_sem_id, 0) == 1)
    {
    	if (semop(g_sem_id, &wd_ready, 1) == -1) 
    	{
       		perror("semop(wd_ready)");
       		return 0;
    	}
    }
}

static long HeartBeat(void *params)
{	
	UNUSED(params);
	
	if(!g_should_stop)
	{
		printf("%d\n", g_wd_pid);
		kill(g_wd_pid, SIGUSR1);
	}

	return 0;			
}

static long CheckWatchDog(void *params)
{	
	UNUSED(params);

	static int lives = 3;
	if(lives == 0 && !g_got_signal)
	{
		ReviveWD((char **)params);
		printf("revive watchdog\n");
		lives = 3;
	}
	else if(g_got_signal)
	{
		lives = 3;
		g_got_signal = 0;
	}
	else
	{
		--lives;
	}

	return 0;			
}

static void ReviveWD(char *argv[])
{
	struct sembuf wd_ready = {0,-1,0};
	struct sembuf app_ready = {1,1,0};

	if (semop(g_sem_id, &wd_ready, 1) == -1) 
    {
  		perror("semop(wd_ready)");
    	return;
    }

	g_wd_pid = fork();
	if(g_wd_pid == 0)
	{
		execv(argv[0], argv);
	}

	if (semop(g_sem_id, &app_ready, 1) == -1) 
    {
  		perror("semop(wd_ready)");
    	return;
    }

    wd_ready.sem_op = -1;
    if (semop(g_sem_id, &wd_ready, 1) == -1) 
    {
  		perror("semop(wd_ready)");
    	return;
    }
}

static void RunThreads(scd_t *scheduler)
{
	pthread_t thread;

    if (0 != pthread_create(&thread, NULL, SchedulerRun, scheduler))
    {
        printf("failed to create thread.\n");
        exit(EXIT_FAILURE);
    }
    
	if (pthread_detach(thread) != 0) 
	{
		perror("pthread_detach() error");
		exit(EXIT_FAILURE);
	}
}

static void *SchedulerRun(void *scheduler)
{
	ScdRun((scd_t *)scheduler);
}

static int GetSemVal(int g_sem_id, int semnum)
{
	union semun arg;    
    return semctl(g_sem_id, semnum, GETVAL, arg);
}

static void SemDestroy(int g_sem_id)
{
	if(-1 == semctl(g_sem_id, 0, IPC_RMID))
	{
		perror("failed to desroy semaphore");
		exit(EXIT_FAILURE);
	}
}

static char **MakeArgv(char *argv[], int argc, int proj_id)
{
	int i = 0;
	char *num_buffer = (char *)malloc(sizeof(*num_buffer) * 8);
	char **buffer = (char **)malloc((argc + 3) * sizeof(*buffer));
	if(NULL == buffer)
	{
		return WD_E_MEM;
	}

	sprintf(num_buffer, "%d", proj_id);
	buffer[0] = WD_PATH;
	buffer[1] = num_buffer;
	
	i = 2;
	while(argv[i - 2] != NULL)
	{
		buffer[i] = argv[i - 2];
		++i;
	}
	buffer[i] = NULL;


	return buffer;
}

static int SemInit(int proj_id, char **argv)
{
	int sem_key = 0;
	int g_sem_id = 0;

	printf("%s\n", argv[0]);
	printf("%d\n",proj_id);

	if ((sem_key = ftok(argv[0], proj_id)) == (key_t) -1) 
    {
        perror("IPC error: ftok"); 
        exit(EXIT_FAILURE);
    }

    g_sem_id = semget(sem_key, 2, IPC_CREAT | IPC_EXCL | 0600);
    if (errno == EEXIST)
    {
    	g_sem_id = semget(sem_key, 2, 0600);
    }

    return g_sem_id;
}

static void SignalsInit()
{
    struct sigaction action;
    sigset_t block_mask;
  	action.sa_mask = block_mask;

  	sigfillset(&block_mask);

    action.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &action, NULL);

    action.sa_handler = sigusr2_handler;
    sigaction(SIGUSR2, &action, NULL);
}

static void sigusr1_handler(int signum)
{
	UNUSED(signum);
	printf("I am the watch |puppy| and i got the signal\n");
	g_got_signal = 1;
}

static void sigusr2_handler(int signum)
{
	UNUSED(signum);

	g_got_signal = 1;
}

int main(int argc, char *argv[])
{
	WDStart(argc, argv, 50);
	static int counter = 0;

	while(1)
	{
		printf("%d\n", ++counter);
		sleep(1);
	}

	return 0;
}