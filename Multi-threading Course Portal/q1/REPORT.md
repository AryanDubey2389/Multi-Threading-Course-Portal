### How to run?
 - First compile using gcc q1.c -lpthread
 - Then ./a.out and then give input

### Threads Used:
  * Here i used two threads:
    * Students thread
    * Course thread

### Colors Used:
   * Red
   * Blue
   * Green
   * Yellow 
   * Purple
   * Cyan 

### Working of the code:
 - I made three structs one for student, one for labs and one for courses, and   then used them by creating different variables from them as per the demand.
 - I also used many threads and also mutex threads for locking some important vairables. I also used semaphores which represents a particular course and students that are enrolled in that particular course.
 - The code runs till every student is done with their seat allocation.
 
# Working of threads:
### Student Thread
- Initially it sleeps as per the time mentioned in Input and then prints that which student has filled up the preference. Then it goes into an infinite loop.
- In the loop I used two arrays of semaphore that controls student entity after they filled their preference ,it waits until the students until the seats for their respective has been allocated and then they leave as it recieves the signal when the seats are allocated. After this they wait wait for the second semaphore(which is tutorial).
- After tutorial gets over based on the probability course of the student gets decieded if he/she not wants to take that course then he/she can change his/her preference.

### Course Thread
 - Initially i checked number of students filled a particular course as their first preference and number of students that have still that course as their first prefernce. Then I loop through lab array and then I check available TA mentors for tutorial. And then I select TA and then it allocates a random number of seats.
 - After the seats is being allocated then the course signals the mutex required number of times such that students that are waiting on the first semaphore gets cleared.
 - Then i made them to sleep for 8 seconds. Now after after tutorial is finished then all the students that are waiting are then singanlled and then they goes to infinite loop till all the students allocated a seat.
