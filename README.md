#  pc 

* creats n ampunt of producers x amount of consumers whith y amount of tasks and z amount of total tasks that can be worked on at one time by the producers  



## building

* Type make into the command line while you are in the same directory that the file is in and the make file will be created with the same name as the c file.

## files

* eventbuf.h
* eventbuf.c 
* pc.c 

## data

* there are 3 semaphores that lock.limit  either the producers, the consumers, and acsess to the buffer 



## functions

*in main you create n amount of producers that each creates a thread and x amount of consumers treads .

* producers() creates the events 

* consumers() processes those events ( this also has a qiting time that auto closes all tasks if no more tasks are being produced)

## notes

* was harder than i thought used in class exesersize and got help form beej and shane 
#   P r o j e c t - 3 - P r o d u c e r s - a n d - C o n s u m e r s  
 