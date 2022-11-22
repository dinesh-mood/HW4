char *FullCopy(char *s)
{
    int i, c;
    char *cpy;

    i = 0;
    cpy = malloc(sizeof(char) * strlen(s));
    while ((c = s[i++]) != '\0' && c != '\n')
        cpy[i] = c;
    cpy[i] = '\0';

    return cpy;
}
void *BeginCommand(void *arg)
{
    struct Job *jp;
    char **args;
    pid_t pid;

    jp = (struct Job *)arg;

    PRESENT++;
    jp->jobstatus = "working";
    jp->begin = Now();

    pid = fork();
    if (pid == 0)
    {
        int fd, fd2;
        if ((fd = open(jp->fnout, O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1)
            exit(0);
        else if ((fd2 = open(jp->fnerr, O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1)
            exit(0);

        dup2(fd, STDOUT_FILENO);
        dup2(fd2, STDERR_FILENO);

        char *cpy = malloc(sizeof(char) * (strlen(jp->cmd) + 1));
        strcpy(cpy, jp->cmd);

        char *arg;
        char **args = malloc(sizeof(char *));
        int i = 0;
        while ((arg = strtok(cpy, " \t")) != NULL)
        {
            args[i] = malloc(sizeof(char) * (strlen(arg) + 1));
            strcpy(args[i], arg);
            args = realloc(args, sizeof(char *) * (i++ + 1));
            cpy = NULL;
        }
        args[i] = NULL;

        execvp(args[0], args);
        exit(0);
    }
    else if (pid > 0)
    {
        waitpid(pid, &jp->ejobstatus, WUNTRACED);
        jp->jobstatus = "complete";
        jp->finish = Now();
    }
    else
    {
        exit(0);
    }

    PRESENT--;
    return 0;
}

struct Queue *JOBQUEUE;
int QUEUE;

void *BeginAllCommands(void *arg)
{
    struct Job *jp;

    PRESENT = 0;
    do
    {
        if (JOBQUEUE->count > 0 && PRESENT < QUEUE)
        {
            if ((JOBQUEUE == NULL) || (JOBQUEUE->count == 0))
                jp = (struct Job *)NULL;

            else
            {
                struct Job *j = JOBQUEUE->buffer[JOBQUEUE->begin];
                JOBQUEUE->begin = (JOBQUEUE->begin + 1) % JOBQUEUE->length;
                JOBQUEUE->count--;
                jp = j;
            }

            pthread_create(&jp->threadid, NULL, BeginCommand, jp);

            pthread_detach(jp->threadid);
        }
        sleep(1);
    }
    while (1);
    return NULL;
}
int GetLine(char *s, int n)
{
    int i;
    int c;
    for (i = 0; i < n - 1 && (c = getchar()) != '\n'; i++)
    {
        if (c == EOF)
            return -1;
        s[i] = c;
    }
    s[i] = '\0';
    return i;
}

char *DeleteSpace(char *s)
{
    int i;

    i = 0;
    while (s[i] == ' ')
        i++;

    return s + i;
}