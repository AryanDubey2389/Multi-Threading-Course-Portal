#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
typedef long long int ll;
struct zones
{
    ll capacity;
    ll num_of_people;  
};
struct spectators
{
    ll reach_time;
    ll p;
    char supporting;
    ll zone;
    char zone_repr;
    char name[1000];
    ll num_goals;
};  
struct goals
{
    ll goals_hit;
    char team;
    float prob;
    ll wait_time;
};
#define Red_color       "\033[0;32m"
#define Blue_color      "\x1b[32m"
#define Green_color     "\033[0;31m"
#define Yellow_color    "\e[0;33m"
#define Purple_color    "\033[0;35m"

struct zones zone[10]; 
struct spectators person[1000]; 
struct goals goal[1000]; 
pthread_mutex_t zone_mutex[5];
pthread_t person_thread[1000];
pthread_t goal_thread[1000];
sem_t zone_sem[5];
sem_t wait_sem1, wait_sem2;
ll h_zone_cap, a_zone_cap, n_zone_cap;
ll num_group, spectating_time,num_people;
ll num_goal_chances;
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_fella = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stands_mutex = PTHREAD_MUTEX_INITIALIZER;
ll waiting_fella = 0;
ll pop_inside_stands = 0;
ll itr = 0,num,total = 0,team_a_goals = 0,team_b_goals;
void *func_person(void * arg)   
{
    ll indx = *(ll*)arg;
    ll sleep_time = person[indx].reach_time;
    sleep(sleep_time);   
    struct timespec ts1;
    printf(Yellow_color"%s has reached the stadium\n",person[indx].name);
    if (clock_gettime(CLOCK_REALTIME, &ts1) == -1)
        return NULL;
    ts1.tv_sec += person[indx].p;
    ll s;
    ll i=0,j=0;
    char supporter = person[indx].supporting;
    for(;;)
    {
        pthread_mutex_lock(&mutex_fella); 
        if(supporter=='H' || supporter == 'N')
        {
            pthread_mutex_lock(&zone_mutex[0]);
            ll curr_occupancy = zone[0].num_of_people;
            ll limit = zone[0].capacity;
            if(curr_occupancy<=limit-1)
            {
                person[indx].zone_repr = 'H';
                person[indx].zone=0;
                zone[0].num_of_people+=1;
                pthread_mutex_unlock(&zone_mutex[0]);
                pthread_mutex_unlock(&mutex_fella);
                break;
            }
            else
            {
                pthread_mutex_unlock(&zone_mutex[0]);
            }
            pthread_mutex_lock(&zone_mutex[2]);
            curr_occupancy = zone[2].num_of_people;
            limit = zone[2].capacity;
            if(curr_occupancy<=limit-1)
            {
                person[indx].zone_repr = 'N';
                person[indx].zone=2;
                zone[2].num_of_people+=1;
                pthread_mutex_unlock(&zone_mutex[2]);
                pthread_mutex_unlock(&mutex_fella);
                break;
            }
            else
            {
                pthread_mutex_unlock(&zone_mutex[2]);
            }
        }
        if(supporter=='N')
        {
            pthread_mutex_lock(&zone_mutex[1]);
            ll curr_occupancy = zone[1].num_of_people;
            ll limit = zone[1].capacity;
            if(curr_occupancy<=limit-1)
            {
                person[indx].zone=1;
                person[indx].zone_repr = 'A';
                zone[1].num_of_people+=1;
                pthread_mutex_unlock(&zone_mutex[1]);
                pthread_mutex_unlock(&mutex_fella);
                break;
            }
            else
            {
                pthread_mutex_unlock(&zone_mutex[1]);
            }
        }
        else if(supporter == 'A')
        {
            pthread_mutex_lock(&zone_mutex[1]);
            ll curr_occupancy = zone[1].num_of_people;
            ll limit = zone[1].capacity;
            if(curr_occupancy<=limit-1)
            {
                person[indx].zone_repr = 'A';
                person[indx].zone=1;
                zone[1].num_of_people+=1;
                pthread_mutex_unlock(&zone_mutex[1]);
                pthread_mutex_unlock(&mutex_fella);
                break;
            }
            else
            {
                pthread_mutex_unlock(&zone_mutex[1]);
            }
        }
        waiting_fella+=1;
        pthread_mutex_unlock(&mutex_fella);
        s = sem_timedwait(&wait_sem1, &ts1);
        while (s  == -1 && errno == EINTR)
        {
            s = sem_timedwait(&wait_sem1, &ts1);
            continue;
        }
        if (s == -1 && errno == ETIMEDOUT)
        {
            printf(Purple_color"%s could not get a seat\n",person[indx].name);
            pthread_mutex_lock(&mutex_fella);
            waiting_fella-=1;
            pthread_mutex_unlock(&mutex_fella);
            printf(Red_color"%s is leaving for dinner \n",person[indx].name);
            return NULL;
        } 
        else
        {
            if(supporter=='H' || supporter == 'N')
            {
                pthread_mutex_lock(&zone_mutex[0]);
                ll curr_occupancy = zone[0].num_of_people;
                ll limit = zone[0].capacity;
                if(curr_occupancy<=limit-1)
                {
                    person[indx].zone_repr = 'H';
                    person[indx].zone=0;
                    zone[0].num_of_people+=1;
                    pthread_mutex_unlock(&zone_mutex[0]);
                    break;
                }
                else
                {
                    pthread_mutex_unlock(&zone_mutex[0]);
                }
                pthread_mutex_lock(&zone_mutex[2]);
                curr_occupancy = zone[2].num_of_people;
                limit = zone[2].capacity;
                if(curr_occupancy<=limit-1)
                {
                    person[indx].zone_repr = 'N';
                    person[indx].zone=2;
                    zone[2].num_of_people+=1;
                    pthread_mutex_unlock(&zone_mutex[2]);
                    break;
                }
                else
                {
                    pthread_mutex_unlock(&zone_mutex[2]);
                }
            }
            if(supporter=='N')
            {
                pthread_mutex_lock(&zone_mutex[1]);
                ll curr_occupancy = zone[1].num_of_people;
                ll limit = zone[1].capacity;
                if(zone[1].num_of_people<zone[1].capacity)
                {
                    person[indx].zone_repr = 'A';
                    person[indx].zone=1;
                    zone[1].num_of_people+=1;
                    pthread_mutex_unlock(&zone_mutex[1]);
                    break;
                }
                else
                {
                    pthread_mutex_unlock(&zone_mutex[1]);
                }
            }
            else if(supporter == 'A')
            {
                pthread_mutex_lock(&zone_mutex[1]);
                ll curr_occupancy = zone[1].num_of_people;
                ll limit = zone[1].capacity;
                if(curr_occupancy<=limit-1)
                {
                    person[indx].zone_repr = 'A';
                    person[indx].zone=1;
                    zone[1].num_of_people+=1;
                    pthread_mutex_unlock(&zone_mutex[1]);
                    break;
                }
                else
                {
                    pthread_mutex_unlock(&zone_mutex[1]);
                }
            }
            continue;
        }
    }
    struct timespec ts2;
    printf(Purple_color"%s got a seat in zone %c\n",person[indx].name,person[indx].zone_repr);
    pthread_mutex_lock(&stands_mutex);
    pop_inside_stands+=1;
    pthread_mutex_unlock(&stands_mutex);
    if (clock_gettime(CLOCK_REALTIME, &ts2) == -1)
        return NULL;
    ll s1;
    ts2.tv_sec += spectating_time;
    for(;;)
    {
        ll gola = -1;
        if(supporter=='H') 
            gola = team_a_goals;
        if(supporter=='A')
            gola = team_b_goals;
        pthread_mutex_lock(&mutex_a);
        if(gola>person[indx].num_goals+1)
        {
            printf(Red_color"%s is leaving due to bad performance of his team \n",person[indx].name);
            pthread_mutex_unlock(&mutex_a);
            pthread_mutex_lock(&stands_mutex);
            pop_inside_stands-=1;
            pthread_mutex_unlock(&stands_mutex);
            pthread_mutex_lock(&mutex_fella);
            for(i=0;i<waiting_fella;++i)
                sem_post(&wait_sem1);
            pthread_mutex_unlock(&mutex_fella);
            printf(Red_color"%s is leaving for dinner \n",person[indx].name);
            return NULL;
        }
        else
        {
            pthread_mutex_unlock(&mutex_a);
        }
        s1 = sem_timedwait(&wait_sem2, &ts2);
        while (s1== -1 && errno == EINTR)
        { 
            s1 = sem_timedwait(&wait_sem2, &ts2);
            continue;
        }
        if(s1 == -1 && errno == ETIMEDOUT)
        {
            pthread_mutex_lock(&zone_mutex[person[indx].zone]);
            zone[person[indx].zone].num_of_people-=1;
            pthread_mutex_unlock(&zone_mutex[person[indx].zone]);
            printf(Red_color"%s watched the match for %lld seconds and is leaving \n",person[indx].name,spectating_time);
            pthread_mutex_lock(&stands_mutex);
            pop_inside_stands-=1;
            pthread_mutex_unlock(&stands_mutex);
            pthread_mutex_lock(&mutex_fella);
            for(i=0;i<waiting_fella;++i) 
                sem_post(&wait_sem1);
            pthread_mutex_unlock(&mutex_fella);
            printf(Red_color"%s is leaving for dinner\n",person[indx].name);
            return NULL;
        }
        else
        {
            ll gola=-1;
            if(supporter=='H')
                gola = team_a_goals;
            else if(supporter=='A')
                gola = team_b_goals;
            if(gola>person[indx].num_goals+1)
            {
                printf(Red_color"%s is leaving due to bad performance of his team \n",person[indx].name);
                pthread_mutex_lock(&stands_mutex);
                pop_inside_stands-=1;
                pthread_mutex_unlock(&stands_mutex);
                pthread_mutex_lock(&mutex_fella);
                for(i=0;i<waiting_fella;++i)
                    sem_post(&wait_sem1);
                pthread_mutex_unlock(&mutex_fella);
                printf(Red_color"%s is leaving for dinner \n",person[indx].name);
                return NULL;   
            }
        }
    }
}
void* func_goal(void* arg)
{
    ll indx = *(ll*)arg;
    ll sleep_time = goal[indx].wait_time;
    sleep(sleep_time);
    ll i = 0, j = 0;
    char team_char = goal[indx].team;
    if(goal[indx].prob>0.5) 
    {
        ll gola;
        if(team_char=='A')
        { 
            ++team_a_goals;
            gola = team_a_goals;
        }
        else
        {
            ++team_b_goals;
            gola = team_b_goals;
        }
        pthread_mutex_lock(&mutex_a);
        pthread_mutex_lock(&stands_mutex);
        printf(Green_color"Team %c has scored their %lld goal\n",goal[indx].team,gola);
        for(i=0;i<pop_inside_stands;++i)
            sem_post(&wait_sem2); 
        pthread_mutex_unlock(&stands_mutex);
        pthread_mutex_unlock(&mutex_a);
    }
    else
    {
        ll gola;
        if(team_char=='A')
            gola = team_a_goals;
        else
            gola = team_b_goals;
        pthread_mutex_lock(&mutex_a);
        printf(Green_color"Team %c missed the chance to score their %lld goal\n",goal[indx].team,gola+1);
        pthread_mutex_unlock(&mutex_a);
    }
}
int main()
{
    scanf("%lld %lld %lld",&h_zone_cap,&a_zone_cap,&n_zone_cap);
    ll i=0,j=0;
    for(i = 0; i < 3; ++i)
        zone[i].num_of_people = 0;
    scanf("%lld",&spectating_time);
    scanf("%lld",&num_group);
    sem_init(&wait_sem1, 0, 0);
    sem_init(&wait_sem2, 0, 0);
    for(i = 0; i < 3; ++i)
        pthread_mutex_init(&zone_mutex[i], NULL);
    zone[0].capacity = h_zone_cap;
    zone[1].capacity = a_zone_cap;
    zone[2].capacity = n_zone_cap;
    for(i=0;i<num_group;++i)
    {
        scanf("%lld",&num);
        for(j=0;j<num;++j,++itr)
        {
            scanf("%s %c ",person[itr].name,&person[itr].supporting);
            scanf("%lld %lld %lld",&person[itr].reach_time,&person[itr].p,&person[itr].num_goals);
        }
    }
    scanf("%lld",&num_goal_chances);
    for(i=0;i<num_goal_chances;++i)
    {
        goal[i].goals_hit=0;
        scanf(" %c %lld %f",&goal[i].team,&goal[i].wait_time,&goal[i].prob);
    }
    sleep(7);
    for(i=0;i<itr;++i)
    {
        ll* a = malloc(sizeof(ll));
        *a = i;
        pthread_create(&person_thread[i],NULL,func_person,a);
    }
    for(i=0;i<num_goal_chances;++i)
    {
        ll* a = malloc(sizeof(ll));
        *a = i;
        pthread_create(&goal_thread[i],NULL,func_goal,a);
    }
    for(i=0;i<itr;++i)
        pthread_join(person_thread[i],NULL);
    for(i=0;i<num_goal_chances;++i)
        pthread_join(goal_thread[i],NULL);
    sem_destroy(&wait_sem1);
    sem_destroy(&wait_sem2);
}