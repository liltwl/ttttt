/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otaouil <otaouil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/07 04:09:13 by otaouil           #+#    #+#             */
/*   Updated: 2021/12/15 03:55:35 by otaouil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	free_fork(t_all *data, t_philo *philo)
{
	t_fork	*fork;
	t_fork	*fork1;

	fork = get_fork(data, philo->id);
	if (philo->id < data->philo_num)
		fork1 = get_fork(data, philo->id + 1);
	else
		fork1 = get_fork(data, 1);
	pthread_mutex_unlock(&fork1->flock);
	pthread_mutex_unlock(&fork->flock);
	return ;
}

void	*ft_start(void *arg)
{
	t_philo	*philo;
	t_all	*data;

	philo = (t_philo *)arg;
	data = philo->data;
	philo->t0 = get_time_mls();
	philo->t_stop_eat = get_time_mls();
	while (data->i)
	{
		is_philo_dead(philo, data);
		ft_checkstatus(philo, data);
		if (data->i)
			ft_statu_up(philo, philo->data);
	}
	return (NULL);
}

void	ft_philo(t_all *data)
{
	t_philo	*philo;
	int		err;

	philo = data->philo;
	while (philo)
	{
		if (data->philo_num == 1)
		{
			philo->statu = 9;
			ft_print("has taken a fork", philo, 0, 1);
		}
		else if (philo->id % 2 != 0 && data->philo_num != philo->id)
			ft_philoinit(philo, data);
		else
			philo->statu = 0;
		philo = philo->next;
	}
	philo = data->philo;
	while (philo)
	{
		err = pthread_create(&philo->trd_id, NULL, &ft_start, philo);
		philo = philo->next;
		usleep(5);
	}
}

void	*is_philodead(void *data1)
{
	t_philo	*phi;
	t_all	*data;

	data = (t_all *)data1;
	while (ft_chekph(data))
	{
		phi = data->philo;
		while (phi)
		{
			if (phi && (get_time_mls() - phi->t_stop_eat)
				>= data->t_to_die)
			{
				ft_print("died", phi, get_time_mls(), 1);
				data->i = 0;
				pthread_mutex_unlock(&data->lk);
				return (NULL);
			}
			phi = phi->next;
		}
		usleep(30);
	}
	return (NULL);
}

int	main(int ac, char **av)
{
	t_all			data1;
	pthread_t		trd_id;

	trd_id = 0;
	if (ft_init(ac, av, &data1) == -1)
		return (0);
	ft_addlst(&data1);
	ft_philo(&data1);
	pthread_create(&trd_id, NULL, &is_philodead, &data1);
	pthread_mutex_lock(&data1.lk);
	ft_freeall(&data1);
}
