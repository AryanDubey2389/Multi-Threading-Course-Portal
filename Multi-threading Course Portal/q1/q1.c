#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef long long int ll;

struct courses
{
    ll id;
    char name[50];
    ll signal_end;
    ll ta_id[1000];
    ll mentor;
    float interest;
    ll max_slot;
    ll lab_id;
    ll num_of_labs;
    ll ta_labid;
    ll complete;
    ll seats;
    ll got_ta;
};
struct students
{
    ll course_prior[10];
    ll id;
    float time_taken;
    float interest_prob;
    float calibre_quot;
    ll registered_courses[10];
    ll current_courses[10];
    ll withdrawn_courses[10];
    ll pref;
    ll filled;
};
struct labs
{
    ll id;
    ll num_students;
    ll num_ta;
    ll avail_ta;
    ll max_num_times_ta;
    ll ta_mentor_avail[1000];
    ll ta_tut_no[1000];
    char name[50];
};
#define Red_color       "\033[0;32m"
#define Blue_color      "\x1b[32m"
#define Green_color     "\033[0;31m"
#define Yellow_color    "\e[0;33m"
#define Purple_color    "\033[0;35m"
#define Cyan_color      "\x1b[36m"
pthread_t courses_thread[1000];
pthread_t stud_thread[1000];
pthread_t lab_thread[1000];
sem_t stud_semaphore1[1000], stud_semaphore2[1000];
pthread_mutex_t get_ta,select_stud_course,stud_pref[1000];
pthread_mutex_t avail_stud[100],decreamenter;
struct courses course[1000];
struct students student[1000];
struct labs lab[1000];
ll num_ta_lab = 0,total_avail_stud;
ll num_stud,num_labs,num_courses;
double prob;

void* func_student(void* arg)
{
    ll stud_id = *(ll*)arg;
    ll wait_time = student[stud_id].time_taken;
    sleep(wait_time);
    pthread_mutex_lock(&avail_stud[stud_id]);
    printf(Purple_color"Student %lld has filled in preference for course registration\n",stud_id);
    student[stud_id].filled = 1;
    pthread_mutex_unlock(&avail_stud[stud_id]);
    for(;;)
    {
        ll prior_course = student[stud_id].course_prior[student[stud_id].pref];
        sem_wait(&stud_semaphore1[prior_course]);
        pthread_mutex_lock(&avail_stud[stud_id]);
        student[stud_id].filled = 0;
        pthread_mutex_unlock(&avail_stud[stud_id]);
        pthread_mutex_lock(&stud_pref[prior_course]);
        if(!course[prior_course].complete)
            printf(Red_color"Student %lld has been allocated a seat in course %s\n",stud_id,course[prior_course].name);
        course[prior_course].seats-=1;
        pthread_mutex_unlock(&stud_pref[prior_course]);
        sem_wait(&stud_semaphore2[prior_course]);
        prob = student[stud_id].calibre_quot*course[prior_course].interest;
        pthread_mutex_lock(&stud_pref[prior_course]);
        course[prior_course].seats+=1;
        pthread_mutex_unlock(&stud_pref[prior_course]);
        double is_complete = course[prior_course].complete;
        if((prob > 0.5 )&&(is_complete == 0))
        {
            printf(Blue_color"Student %lld has selected the course %s permanently\n",stud_id,course[prior_course].name);
            pthread_mutex_lock(&decreamenter);
            total_avail_stud-=1;
            if(total_avail_stud==0)   
                exit(0);
            pthread_mutex_unlock(&decreamenter);
            return NULL;
        }
        else if(prob <= 0.5 || (is_complete !=0))
        {
            student[stud_id].pref+=1;
            if(course[prior_course].complete==0)
                printf(Yellow_color"Student %lld has withdrawn from course %s\n",stud_id,course[prior_course].name);
            if(student[stud_id].pref==3)
            {
                printf(Green_color"Student %lld could not get any of his preferred course\n",stud_id);
                pthread_mutex_lock(&decreamenter);
                total_avail_stud-=1;
                if(total_avail_stud<=0)
                    exit(0);
                pthread_mutex_unlock(&decreamenter);
                return NULL;
            }
            pthread_mutex_lock(&avail_stud[stud_id]);
            student[stud_id].filled=1;
            pthread_mutex_unlock(&avail_stud[stud_id]);
            printf(Cyan_color"Student %lld has changed current preference from %s (priority %lld) to %s (priority %lld)\n",stud_id,course[prior_course-1].name,student[stud_id].pref-1,course[prior_course].name,student[stud_id].pref);            
        }
    }
}
ll min(ll a ,ll b)
{
    if(a<=b)
    {
        return a;
    }
    return b;
}
void *func_course(void * arg) 
{
    ll stud_id = *(ll*)arg;
    ll ta_no,lab_no;
    sleep(3);
    ll num_cors_std=0;
    for(;;)
    {
        ll i = 0,j = 0;
        num_cors_std=0;
        for(i=0;i<num_stud;++i)
        {
            pthread_mutex_lock(&avail_stud[i]);
            if(!student[i].filled)
            {
                pthread_mutex_unlock(&avail_stud[i]);
                continue;
            }
            else if(student[i].course_prior[student[i].pref]==course[stud_id].id)
            {
                num_cors_std+=1;
                pthread_mutex_unlock(&avail_stud[i]);
            }
            else
            {
                pthread_mutex_unlock(&avail_stud[i]);
            }
        }
        if(num_cors_std==0)
            continue;
        for(i=0;i<num_labs;++i) 
        {
            if(course[stud_id].got_ta==1)
                break;
            for(j=0;j<lab[i].num_ta;++j)
            {
                if(course[stud_id].got_ta==1)
                    break;
                pthread_mutex_lock(&get_ta);
                if(lab[i].ta_mentor_avail[j]==1)
                {
                    lab_no = i;
                    lab[i].ta_mentor_avail[j]=0;
                    lab[i].ta_tut_no[j]+=1;
                    ta_no = j;
                    char str[10];
                    if(lab[i].ta_tut_no[j] == 1)
                    {
                        str[0] = 's';
                        str[1] = 't';
                    }
                    else if(lab[i].ta_tut_no[j] == 2)
                    {
                        str[0] = 'n';
                        str[1] = 'd';
                    }
                    else if(lab[i].ta_tut_no[j] == 3)
                    {
                        str[0] = 'r';
                        str[1] = 'd';
                    }
                    else
                    {
                        str[0] = 't';
                        str[1] = 'h';
                    }
                    printf(Green_color"TA %lld from lab %s has been allocated to course %s for his %lld%s TA ship\n",j,lab[i].name,course[stud_id].name,lab[i].ta_tut_no[j],str);
                    printf(Yellow_color"Course %s has been allocated %lld seats\n",course[stud_id].name,course[stud_id].seats);
                    course[stud_id].got_ta = 1;
                    course[stud_id].seats = (rand()%(course[stud_id].max_slot-1))+1;
                }
                pthread_mutex_unlock(&get_ta);
            }
            if(course[stud_id].got_ta==1)
                break;
        }
        if(course[stud_id].got_ta==1) 
        {
            ll total_allocated_seats = course[stud_id].seats;
            num_cors_std = min(course[stud_id].seats,num_cors_std);
            course[stud_id].seats=num_cors_std;
            ll temp = course[stud_id].seats;
            for(i=0;i<num_cors_std;++i)
                sem_post(&stud_semaphore1[course[stud_id].id]);
            printf(Green_color"Tutorial has started for course %s has started with %lld slots filled out of %lld\n",course[stud_id].name,num_cors_std,total_allocated_seats);
            sleep(8);
            printf(Purple_color"TA %lld from lab %s has completed the tutorial for course %s\n",ta_no,lab[lab_no].name,course[stud_id].name);
            while(course[stud_id].seats);
            pthread_mutex_lock(&get_ta);
            ll constraint = lab[lab_no].max_num_times_ta;
            ll tut_nun = lab[lab_no].ta_tut_no[ta_no];
            if(tut_nun==constraint)
            {
                lab[lab_no].avail_ta-=1;
                if(lab[lab_no].avail_ta==0)
                    printf(Purple_color"Lab %s no longer has students available for TA ship\n",lab[lab_no].name);        
                num_ta_lab-=1;
                pthread_mutex_unlock(&get_ta);
            }
            else
            {
                lab[lab_no].ta_mentor_avail[ta_no]=1;
                pthread_mutex_unlock(&get_ta);
            }
            course[stud_id].got_ta = 0;
            for(i=0;i<num_cors_std;++i)
                sem_post(&stud_semaphore2[course[stud_id].id]);
            while(course[stud_id].seats!=temp);
        }
        else
        {
            if((num_ta_lab==0)&&(num_cors_std!=0))
            {
                for(i=0;i<num_cors_std;++i)
                    sem_post(&stud_semaphore1[course[stud_id].id]);
                course[stud_id].complete = 1;
                course[stud_id].seats=num_cors_std;
                
                while(course[stud_id].seats);
                ll temp = num_cors_std;
                for(i=0;i<num_cors_std;++i)
                    sem_post(&stud_semaphore2[course[stud_id].id]);
                while(course[stud_id].seats!=temp);
            }
        }
    }
}
int main()
{
    ll i = 0,j=0;
    pthread_t courses_thread[1000];
    pthread_t stud_thread[1000];
    pthread_t lab_thread[1000];
    scanf("%lld%lld%lld",&num_stud,&num_labs,&num_courses);
    ll total_avail_stud = num_stud;
    for(i = 0; i < num_courses; ++i)
    {
        course[i].id = i;
        course[i].complete = 0;
        scanf("%s%f%lld%lld",course[i].name,&course[i].interest,&course[i].max_slot,&course[i].num_of_labs);
        for(j = 0; j < course[i].num_of_labs; ++j)
            scanf("%lld",&course[i].ta_labid);
        sem_init(&stud_semaphore1[i],0,0);
        sem_init(&stud_semaphore2[i],0,0);
    }
    for(i = 0; i < num_stud; ++i)
    {
        pthread_mutex_init(&stud_pref[i], NULL);
        student[i].pref = 0;
        scanf("%f",&student[i].calibre_quot);
        scanf("%lld",&student[i].course_prior[0]);
        scanf("%lld",&student[i].course_prior[1]);
        scanf("%lld",&student[i].course_prior[2]);
        scanf("%f",&student[i].time_taken);
        pthread_mutex_init(&avail_stud[i], NULL);
    }
    for(i = 0; i < num_labs; ++i)
    {
        scanf("%s%lld%lld",lab[i].name,&lab[i].num_ta,&lab[i].max_num_times_ta);
        num_ta_lab += lab[i].num_ta;
        lab[i].avail_ta = lab[i].num_ta;

        for(j=0;j<lab[i].num_ta;++j)
        {
            lab[i].ta_mentor_avail[j]=1;
            lab[i].ta_tut_no[j]=0;
        }
    }
    for(i=0;i<num_stud;++i)
    {
        ll* a = malloc(sizeof(ll));
        *a = i;
        pthread_create(&stud_thread[i],NULL,&func_student,a); 
    }
    for(int i=0;i<num_courses;++i)
    {
        ll* a = malloc(sizeof(ll));
        *a = i;
        pthread_create(&courses_thread[i],NULL,&func_course,a); 
    }
    
    for(i=0;i<num_stud;++i)
    {
        pthread_join(stud_thread[i],NULL);   
    }
    for(i=0;i<num_courses;++i)
    {
        pthread_join(courses_thread[i],NULL);
    }
    for(i = 0; i < num_courses; ++i)
    {
        sem_destroy(&stud_semaphore1[i]);
        sem_destroy(&stud_semaphore2[i]);
        pthread_mutex_destroy(&avail_stud[i]);
    }
}