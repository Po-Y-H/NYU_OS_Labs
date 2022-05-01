#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <queue>
#include <list>

#include "Event.h"

class Scheduler
{
public:
    Scheduler() {}
    virtual void addProcess(Process* proc) = 0;
    virtual Process* getNextProcess() = 0;
    virtual bool isEmpty() = 0;
    virtual bool testPreempt(Process* p, int curtime) = 0; // false but for ．E・    
};

class SchedulerFCFS : public Scheduler
{
public:
    SchedulerFCFS();
    void addProcess(Process* proc);
    Process* getNextProcess();
    bool isEmpty();
    bool testPreempt(Process* p, int currtime); // false but for ．E・    
    ~SchedulerFCFS();

    queue<Process*> readyQueue;
};

class SchedulerLCFS : public Scheduler
{
public:
    SchedulerLCFS();
    void addProcess(Process* proc);
    Process* getNextProcess();
    bool isEmpty();
    bool testPreempt(Process* p, int currtime); // false but for ．E・    
    ~SchedulerLCFS();

    list<Process*> readyQueue;
};

class SchedulerSRTF : public Scheduler
{
public:
    SchedulerSRTF();
    void addProcess(Process* proc);
    Process* getNextProcess();
    bool isEmpty();
    bool testPreempt(Process* p, int currtime); // false but for ．E・    
    ~SchedulerSRTF();

    list<Process*> readyQueue;
};

class SchedulerRR : public Scheduler
{
public:
    SchedulerRR();
    void addProcess(Process* proc);
    Process* getNextProcess();
    bool isEmpty();
    bool testPreempt(Process* p, int currtime); // false but for ．E・    
    ~SchedulerRR();

    queue<Process*> readyQueue;
};

class SchedulerPRIO : public Scheduler
{
public:
    SchedulerPRIO();
    SchedulerPRIO(int maxPrio);
    void addProcess(Process* proc);
    Process* getNextProcess();
    bool isEmpty();
    bool isEmpty(vector<list<Process*>>* q);
    bool testPreempt(Process* p, int currtime); // false but for ．E・    
    void printQueue();
    ~SchedulerPRIO();

    int maxprio = 4;
    vector<list<Process*>>* activeQueue;
    vector<list<Process*>>* expiredQueue;

};

class SchedulerPREPRIO : public Scheduler
{
public:
    SchedulerPREPRIO();
    SchedulerPREPRIO(int maxPrio);
    void addProcess(Process* proc);
    Process* getNextProcess();
    bool isEmpty();
    bool isEmpty(vector<list<Process*>>* q);
    bool testPreempt(Process* p, int currtime); // false but for ．E・    
    void printQueue();
    ~SchedulerPREPRIO();

    int maxprio = 4;
    vector<list<Process*>>* activeQueue;
    vector<list<Process*>>* expiredQueue;
};
#endif