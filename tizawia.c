

typedef struct s_env
{
	char			*name;
	char			*str;
    struct s_env *next;
}				t_env;

t_env		*ft_lstfind(t_env *lst, char *name)
{
	while (lst)
	{
		if (!ft_cmp(name, lst->name))
			return (lst);
		else
			lst = lst->next;
	}
	return (NULL);
}

t_env	*ft_lstnewenv(char *name, char *str)
{
	t_env	*elt;

	if (!(elt = (t_env *)malloc(sizeof(* elt))))
		return (NULL);
	elt->name = ft_strdup(name);
	elt->str = ft_strdup(str);
	elt->next = NULL;
	return (elt);
}

t_env	*ft_lastenv(t_env *lst)
{
	while (lst)
	{
		if (!lst->next)
			return (lst);
		lst = lst->next;
	}
	return (lst);
}

void	ft_addenv_back(t_env **alst, t_env *new)
{
	t_env	*last;

	if (alst)
	{
		if (*alst)
		{
			last = ft_lastenv(*alst);
			last->next = new;
		}
		else
			*alst = new;
	}
}

t_env	*ft_addevnlist(t_env **l, char **evn)
{
    char    **str;
    int     i;
    t_env  *tmp;

	i = 0;
    str	= ft_split(evn[i], '=');
	ft_addenv_back(l, ft_lstnewenv(str[0], str[1]));
    str = ft_free_split(str);
    tmp = *l;
	while (evn[++i])
	{
		str	= ft_split(evn[i], '=');
		ft_addenv_back(l, ft_lstnewenv(str[0], str[1]));
		str = ft_free_split(str);
	}
	return (tmp);
}



void	change_oldpwd(t_data *data)
{
	char	*str;
	t_env	*oldpwd;

	oldpwd = ft_lstfind(data->env, "OLDPWD");
	str = oldpwd->str;
	free (str);
	if (!(str = getcwd(NULL, 0)))
		return ;
	oldpwd->str = ft_strdup(str);
	free (str);
}

void	change_pwd(t_data *data)
{
	char	*str;
	t_env	*pwd;

	pwd = ft_lstfind(data->env, "PWD");
	str = pwd->str;
	free (str);
	if (!(str = getcwd(NULL, 0)))
		return ;
	pwd->str = ft_strdup(str);
	free (str);
}


int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char	*str1;
	unsigned char	*str2;

	str1 = (unsigned char*)s1;
	str2 = (unsigned char*)s2;
	while (n--)
	{
		if (!*str1 && *str2)
			return (*str1 - *str2);
		if (*str1 && !*str2)
			return (*str1 - *str2);
		if (*str1 != *str2)
			return (*str1 - *str2);
		str1++;
		str2++;
	}
	if (strlen(s1) > strlen(s2))
		return 1;
	else if (strlen(s1) < strlen(s2))
		return -1;
	return (0);
}

t_env	*ft_smllst(t_env *env)
{
	char	*str;
	t_env	*tmp;

	str = env->name;
	tmp = env;
	env = env->next;
	while (env)
	{
		if (ft_memcmp(str, env->name, strlen(str) + 1) < 0)
		{
			tmp = env;
			str = env->name;
		}
		env = env->next;
	}
	return (tmp);
}

t_env	*ft_getbiglst(t_env	*env, t_env *big)
{
	t_env	*tmp;
	char	*c;
	char	*p;
	int		j;

	tmp = NULL;
	p = NULL;
	if (big)
		p = strdup(big->name);
	c = strdup("~~~~~~~~");
	while (env)
	{
		if ((ft_memcmp(c, env->name, strlen(c) + 1) > 0 && !p)||
			((p && ft_memcmp(p, env->name, strlen(env->name) + 1) < 0)
			&& ft_memcmp(c, env->name, strlen(env->name) + 1) > 0))
		{
			tmp = env;
			free (c);
			c = strdup(env->name);
		}
		env = env->next;
	}
	free (p);
	free (c);
	return (tmp);
}

void	ft_printsortlst(t_data	*data)
{
	int		i;
	t_env	*tmp;

	tmp = NULL;
	while (1)
	{
		tmp = ft_getbiglst(data->env, tmp);
		write(1, "declare -x ", 11);
		write(1, tmp->name, strlen(tmp->name));
		write(1, "=\"", 2);
		write(1, tmp->str, strlen(tmp->str));
		write(1, "\"\n", 2);
		if (!ft_cmp(tmp->name, ft_smllst(data->env)->name))
			break ;
	}
}

int	ft_findc(char *str, char c)
{
	int		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (0);
}

void	addnewenv(char	**p, t_data *data)
{
	t_env	*env;
	t_env	*tmp;

	env = ft_lstfind(data->env, p[0]);
	if (env)
	{
		free (env->str);
		if (p[1])
			env->str = strdup(p[1]);
		else
			env->str = strdup("");
	}
	else
	{
		if (p[1])
			env = ft_lstnewenv(p[0], p[1]);
		else
			env = ft_lstnewenv(p[0], "");
		tmp = ft_lastenv(data->env);
		ft_addenv_back(&data->env, env);
	}
}

char	**mycatstr(char *str, int i)
{
	char	**p;
	char	*tmp;

	p = malloc(sizeof(char **) * 3);
	if (i)
	{
		tmp = malloc(i + 2);
		if (str[i - 1] == '+')
			ft_strlcpy(tmp, str, i);
		else
			ft_strlcpy(tmp, str, i + 1);
		p[0] = tmp;
		tmp = malloc(strlen(&str[i] + 2));
		ft_strlcpy(tmp, &str[i + 1], strlen(&str[i]));
		p[1] = tmp;
		p[2] = NULL;
	}
	else
	{
		tmp = malloc(i + 2);
		ft_strcpy(tmp, str);
		p[0] = tmp;
		p[1] = NULL;
	}
	return (p);
}

void	export_join(t_data *data, char **p)
{
	char	*str;
	char	*tmp;
	t_env	*env;

	env = ft_lstfind(data->env, p[0]);
	if (env)
	{
		if (p[1])
			str = strdup(p[1]);
		else
			str = strdup("");
		tmp = ft_strjoin(env->str, str);
		free (env->str);
		free (str);
		env->str = tmp;
	}
	else
		addnewenv(p, data);
}

void	ft_lstupdate(t_data *data, char **str)
{
	int		i;
	char	**p;
	int		j;

	i = 0;
	while (str[++i])
	{
		j = ft_findc(str[i], '=');
		p = mycatstr(str[i], j);
		if (!ft_isalpha(str[i][0]) && str[i][0] != '_')
			data->exitstatu = 1;
		else
		{
			if (ft_strchr(p[0], '+'))
			{
				data->exitstatu = 1;
				return ;
			}
			if (str[i][j - 1] == '+')
				export_join(data, p);
			else
				addnewenv(p, data);
		}
		ft_free_split(p);
	}
}

void	do_export(t_data *data, t_cmd *cmd)
{
	int		i;
	char	**str;

	str = cmd->str;
	i = 0;
	if (!str[1])
		ft_printsortlst(data);
	else
		ft_lstupdate(data, str);
}





void	ft_deletlst(char *name, t_data *data)
{
	t_env *lst;
	t_env	*tmp;

	lst = data->env;
	tmp = NULL;
	while (lst)
	{
		if (!strncmp(name, lst->name, strlen(lst->name) + 1))
			break ;
		tmp = lst;
		lst = lst->next;
	}
	if (!lst)
		return ;
	if (tmp)
	{
		tmp->next = lst->next;
		free (lst->str);
		free (lst->name);
		free (lst);
	}
	else
	{
		data->env = lst->next;
		free (lst->str);
		free (lst->name);
		free (lst);
	}
}

 void	do_unset(t_data *data, t_cmd *cmd)
 {
	int		i;
	char	**str;

	i = 0;
	str = cmd->str;
	if (!str[1])
		return ;
	while (str[++i])
	{
		ft_deletlst(str[i],data);
	}
	data->exitstatu = 0;
 }

 