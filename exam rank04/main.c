#include <stdio.h>
#include <string.h>
#include <stdlib.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <signal.h>
# include <fcntl.h>

typedef struct cmd
{
    char    **cmd;
    int cmdlen;
}               t_cmd;


int cmdlen(char **str)
{
    int i, j;

    i = j = 1;
    while(str[i] && strcmp(str[i], ";"))
    {
        if (!strcmp(str[i], "|"))
            j++;
        i++;
    }
    return (j);
}

t_cmd *addnewcmd(char **str, int i)
{
    int j, k;
    char **tmp;
    t_cmd *cmd;

    j = i;
    k = -1;
    while (str[j] && strcmp(str[j], "|") && strcmp(str[j], ";"))
        j++;
    cmd = malloc (sizeof(t_cmd*));
    tmp = malloc(sizeof(char**) * (j - i + 1));
    while (++k < j-i)
    {
        tmp[k] = str[k + i];
    }
    tmp[k] = NULL;
    cmd->cmd = tmp;
    return cmd;
}

t_cmd **getcmdlst(char **str, int i)
{
    t_cmd **cmd;
    int x, j, k;

    x = k = i = 0;
    j = 0;
    k = cmdlen(str);
    cmd = malloc(sizeof(t_cmd**) * (k+1));
    while (str[j] && strcmp(str[j], ";"))
    {
        if (!strcmp(str[j], "|"))
            j++;
        cmd[x] = addnewcmd(str, j);
        cmd[x]->cmdlen = k;
        x++;
        while (str[j] && strcmp(str[j], "|") && strcmp(str[j], ";"))
            j++;
    }
    cmd[x] = NULL;
    return (cmd);
}

void do_cd(t_cmd *cmd)
{
    char **str = cmd->cmd;

    if (!str[1] || str[2])
        exit(0);
    if (chdir(str[1]) < 0)
    {
        perror("chdir failed");
        exit(0);
    }
}

int mlp(t_cmd **cmd, char **env)
{
    int i, k;
    int fds[2];
    pid_t pid;
    int fd = 0;

    i = 0;
    k = cmd[0]->cmdlen;
    while (i < k)
    {
        pipe(fds);
        pid = fork();
        if (pid == 0)
        {
            if (i != 0)
            {
                dup2(fd, 0);
                close (fd);
            }
            if (i != k - 1)
                dup2(fds[1], 1);
            close(fds[0]);
            close(fds[1]);
            if (!strcmp(cmd[i]->cmd[0], "cd"))
                do_cd(cmd[i]);
            else
                execve(cmd[i]->cmd[0], cmd[i]->cmd, env);
            exit(0);
        }
        if(fd)
            close (fd);
        fd = fds[0];
        close(fds[1]);
        i++;
    }
    if (fd)
        close (fd);
    waitpid(pid, &i, 0);
    return 0;
}

void freecmd(t_cmd **cmd)
{
    int i;

    i = -1;
    while (cmd[++i])
    {
        free(cmd[i]->cmd);
        free (cmd[i]);
    }
    free (cmd);
}

void ff(int av, char **ac , char **env)
{
     int i, j;

    j = 1;
    t_cmd** cmd;
    while (ac[j])
    {
        if (!strcmp(ac[j], ";"))
            j++;
        
        cmd = getcmdlst(&ac[j] , 0);
        mlp(cmd, env);
        while (ac[j] && strcmp(ac[j], ";"))
            j++;
        freecmd(cmd);
    }
}
int main(int av, char **ac , char **env)
{
   ff(av,ac,env);
   while (1);
}