### How to run?
* First compile using gcc q1.c -lpthread.
* Then ./a.out and then give input.

### Threads Used
* Here I used two threads:
    * Person Thread
    * Goal Thread

### Colors Used:
   * Red
   * Blue
   * Green
   * Yellow 
   * Purple

### Working of the code
 - I made three structs one for spectators, one for zones and one for goals, and then used them by creating different variables from them as per the demand.
 - I also used many threads and also mutex threads for locking critical sections. I also used two semaphores here.
 - Thus the  process will run untill all the person have exited the simulation.

# Working of threads
### Person thread:
- Initially it sleeps as per the time mentioned in Input and then prints that which person i has reached the stadium. Then it goes into an infinite loop.Then I made two infinite loops and also two time dependent semaphores, initially person thread waits on a semaphore and then after getting signal it will either exit or goes to the next loop.
- And then in next loop Initially sleep for given time , and whenever a team scores then it will wake up from the sleep, and then I maintained a counter that keeps track of persons who are in stands as well as persons who are waiting at the ticket counter and then do the respective thing.

### Goal Thread:
- Initially it sleeps as per the time mentioned in Input and then if the probability > 0.5 then that particular teams scores a goal successfully otherwise they are unable to score a goal.
- Whenever a team scores a goal all the person thjat are waiting on the stands are signalled and then they are checked against the required threshold for opponent team goals.