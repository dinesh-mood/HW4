char *StringCopy(char *s)
{
    int i;
    char c;
    char *cpy;

    i = 0;
    cpy = malloc(sizeof(char) * strlen(s));
    c = s[i];
    
    while (c != '\0')
    {
        cpy[i] = c;
        i++;
        c = s[i];
    }
    cpy[i] = '\0';

    return cpy;
}

struct Queue
{
    int length;
    struct Job **buffer;
    int begin;
    int finish;
    int count;
};

char *Now()
{
    time_t tim = time(NULL);
    return FullCopy(ctime(&tim));
}

int main(int argc, char *argv[])
{
    char *fnerr;
    pthread_t threadid;

    QUEUE = atoi(argv[1]);

    JOBQUEUE = malloc(sizeof(struct Queue));
    JOBQUEUE->length = 128;
    JOBQUEUE->buffer = malloc(sizeof(struct Job *) * 128);
    JOBQUEUE->begin = 0;
    JOBQUEUE->finish = 0;
    JOBQUEUE->count = 0;

    pthread_create(&threadid, NULL, BeginAllCommands, NULL);

    int i;
    char line[512];
    char *kw;
    char *cmd;

    i = 0;
    while (printf("Enter Command> ") && GetLine(line, 512) != -1)
    {
        if ((kw = strtok(StringCopy(line), " ")) != NULL)
        {
            if (strcmp(kw, "submit") == 0)
            {
                if (i >= 512)
                    printf("Full job history; restart the program to add more\n");
                else if (JOBQUEUE->count >= JOBQUEUE->length)
                    printf("Job struct Queue full; retry when more jobs have finished\n");
                else
                {
                    cmd = DeleteSpace(strstr(line, "submit") + 6);

                    struct Job j;
                    j.jobid = i;
                    j.cmd = StringCopy(cmd);
                    j.jobstatus = "waiting";
                    j.ejobstatus = -1;
                    j.begin = j.finish = NULL;
                    sprintf(j.fnout, "%d.out", j.jobid);
                    sprintf(j.fnerr, "%d.err", j.jobid);
                    FULLJOBS[i] = j;

                    if ((JOBQUEUE != NULL) && (JOBQUEUE->count != JOBQUEUE->length))
                    {
                        JOBQUEUE->buffer[JOBQUEUE->finish % JOBQUEUE->length] = FULLJOBS + i;
                        JOBQUEUE->finish = (JOBQUEUE->finish + 1) % JOBQUEUE->length;
                        JOBQUEUE->count++;
                    }

                    printf("Struct Job %d was added to the struct Job struct Queue.\n", i++);
                }
            }
            else if (strcmp(kw, "showjobs") == 0 || strcmp(kw, "submithistory") == 0)
            {
                int ix;
                if (FULLJOBS != NULL && i != 0)
                {
                    if (strcmp(kw, "showjobs") == 0)
                    {
                        printf("jobid\tcommand\t\tstatusus\n");
                        for (ix = 0; ix < i; ix++)
                        {
                            if (strcmp(FULLJOBS[ix].jobstatus, "complete") != 0)
                                printf("%d\t%s\t%s\n",
                                    FULLJOBS[ix].jobid,
                                    FULLJOBS[ix].cmd,
                                    FULLJOBS[ix].jobstatus);
                        }
                    }
                    else if (strcmp(kw, "submithistory") == 0)
                    {
                        printf("Job ID\tCommand\t\tstarttime\tendtime\tstatusus\n");
                        for (ix = 0; ix < i; ix++)
                        {
                            if (strcmp(FULLJOBS[ix].jobstatus, "complete") == 0)
                                printf("%d\t%s\t%s\t%s\t%s\n",
                                    FULLJOBS[ix].jobid,
                                    FULLJOBS[ix].cmd,
                                    FULLJOBS[ix].begin,
                                    FULLJOBS[ix].finish,
                                    "Success");
                        }
                    }
                }
            }
            else if (strcmp(kw, "exit") == 0)
                break;
        }
    }
    kill(0, SIGINT);

    return 0;
}
