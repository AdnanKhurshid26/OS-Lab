#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <algorithm>
#include <limits.h>
using namespace std;

class Job
{
private:
    int jobId;             // job id
    int priority;          // priority of the job
    int arrivalTime;       // arrival time of the job
    vector<int> cpubursts; // cpu bursts of the job
    vector<int> iobursts;  // io bursts of the job
    int countcpu;          // count of cpu bursts
    int countio;           // count of io bursts
    int nextcpu;           // next cpu burst
    int nextio;            // next io burst
    int nextarrivaltime;   // next arrival time
    int totaltime;         // total time of the job
    int firtsarrivaltime; // first arrival time of the job
    int totalbursttime;  

public:
    Job()
    { // default constructor
        jobId = priority = arrivalTime = countcpu = countio = nextcpu = nextio = nextarrivaltime = totaltime = 0;
    }
    Job(vector<int> execProfile)
    { // paramaterized constructor with execution profile as the argument
        int n = execProfile.size();
        jobId = execProfile[0];
        priority = execProfile[1];
        nextarrivaltime = arrivalTime = execProfile[2];
        countcpu = countio = 0;
        totaltime = 0;
        totalbursttime =0;
        for (int i = 3; i < n; ++i)
        {
            if (i & 1)
            { // for cpu bursts
                cpubursts.push_back(execProfile[i]);
                totalbursttime += execProfile[i];
                countcpu++;
            }
            else
            { // for i/o bursts
                iobursts.push_back(execProfile[i]);
                countio++;
            }
            totaltime += execProfile[i];
        }
        nextcpu = nextio = 0;
        cout << "Job with ID read from execution profile: " << jobId << "\n"; // debug statement
    }
    // getters and setters
    //  getter functions
    int getJobId() { return jobId; }
    int getPriority() { return priority; }
    int getArrivalTime() { return arrivalTime; }
    int getcountcpu() { return countcpu; }
    int getcountio() { return countio; }
    int getNextcpu() { return nextcpu; }
    int getNextio() { return nextio; }
    int getCurrCPUTime() { return cpubursts[nextcpu]; }
    int getCurrIOTime() { return iobursts[nextio]; }
    int getn() { return nextarrivaltime; }
    int getTotalTime() { return totaltime; }
    int getnextarrivaltime() { return nextarrivaltime; }
    // setter functions
    void incNextcpu()
    {
        if (nextcpu < countcpu)
            ++nextcpu;
    }
    void incNextio()
    {
        if (nextio < countio)
            ++nextio;
    }
    void updateCPUTime(int dur)
    {
        cpubursts[nextcpu] -= dur;
    }
    void updateArrival(int dur)
    {
        if (cpubursts[nextcpu] == dur)
            nextarrivaltime = (cpubursts[nextio] + iobursts[nextio]);
        cpubursts[nextcpu] -= dur;
    }
    void setTotalTime(int t){totaltime = t;}
    int gettotaltime(){return totaltime;}
    int getFirstArrivalTime() { return firtsarrivaltime; }
    void setFirstArrivalTime(int t){firtsarrivaltime = t;}
    int getTotalBurstTime() { return totalbursttime; }
    // checker functions
    bool cpuLeft() { return nextcpu < countcpu; }
    bool ioLeft() { return nextio < countio; }
};

// comparator class for ordering jobs on the basis of arrival time
class FCFSComparator
{
public:
    bool operator()(Job &a, Job &b)
    {
        return a.getArrivalTime() < b.getArrivalTime();
    }
};

// abstract class for scheduler that will be used to implement different scheduling algorithms
class Scheduler
{

protected:
    vector<Job> jobs;    // vector of jobs
    int jobsleft;        // number of jobs left
    int totalwait;       // total wait time
    int totalturnaround; // total turnaround time
    float avgwait;       // average wait time
    float avgturnaround; // average turnaround time
public:
    // constructor gets a filename as an argument and parses file to make execution profiles
    Scheduler(string filename)
    {
        ifstream fin;
        fin.open(filename, ios::in);
        int num;
        totalturnaround = totalwait = jobsleft = 0;
        while (fin >> num)
        {
            if (num == -1)
            {
                break;
            }
            else
            { // new job starting
                vector<int> execProfile(1, num);
                fin >> num;
                if (num != -1)
                    execProfile.push_back(num);
                while (fin >> num)
                {
                    if (num == -1)
                    {
                        break;
                    }
                    else
                    {
                        execProfile.push_back(num);
                    }
                }
                Job J(execProfile);
                jobs.push_back(J);
                jobsleft++;
            }
        }
    }

    // pure virtual function to schedule processes following the scheduling algorithms
    virtual void schedule() = 0;

    // to show results
    void showAnalysis()
    {
        avgwait = totalwait * 1.0 / jobs.size();
        avgturnaround = totalturnaround * 1.0 / jobs.size();
        cout << "\nAverage Turnaround Time = " << avgturnaround << "\n";
        cout << "Average Waiting Time = " << avgwait << "\n";
    }
};

// FCFS class inherits from Scheduler class

class FCFS : public Scheduler
{
private:
    queue<Job> ready_q;                        // ready queue for cpu
    unordered_map<int, vector<Job>> waiting_q; // block queue for i/o operations
    vector<pair<int, int>> ganttChart;         // to store the scheduling steps
public:
    FCFS(string filename) : Scheduler(filename)
    {
        sort(jobs.begin(), jobs.end(), FCFSComparator());
    }

    virtual void schedule()
    {
        int n = jobs.size();
        int index = 0;
        for (int i = 0; i < n; ++i)
        { // push all the processes initially into the block queue
            waiting_q[jobs[i].getArrivalTime()].push_back(jobs[i]);
            totalturnaround += jobs[i].getTotalTime();
            // cout << jobs[i].getJobId() << " -> " << jobs[i].getArrivalTime() << "\n";
        }
        int timeline = 0;
        bool cpuEmpty = true;
        Job currentJob;
        int nextTerminate = INT_MAX;
        while (jobsleft > 0)
        {
            // process coming from block queue
            for (Job j : waiting_q[timeline])
            {
                ready_q.push(j);
                currentJob.setFirstArrivalTime(timeline);
            }
            // if the current time is the end of cpu burst of a job
            if (timeline == nextTerminate)
            {
                // free the cpu
                cpuEmpty = true;
                // if the job terminates, reduce remaining job count
                if (currentJob.cpuLeft() == false)
                {   
                    currentJob.setTotalTime(timeline);
                    --jobsleft;
                    
                }
            }

            // if the cpu is free
            if (cpuEmpty)
            {
                // and there are jobs on the ready queue
                if (!ready_q.empty())
                {
                    // pick the first job from the ready queue
                    currentJob = ready_q.front();
                    ready_q.pop();
                    // compute the time of end  of its cpu burst
                    nextTerminate = timeline + currentJob.getCurrCPUTime();
                    // store the profile for the current job in gantt chart
                    ganttChart.push_back(make_pair(currentJob.getJobId(), timeline));
                    // occupy the cpu
                    cpuEmpty = false;
                    // update its next arrival time
                    currentJob.updateArrival(currentJob.getCurrCPUTime());
                    // update its cpu index
                    currentJob.incNextcpu();
                    // if io is left
                    if (currentJob.ioLeft())
                    {
                        // resume io
                        currentJob.incNextio();
                        // add the job to the appropriate index of the waiting queue
                        if (currentJob.cpuLeft())
                        {
                            waiting_q[timeline + currentJob.getnextarrivaltime()].push_back(currentJob);
                        }
                    }
                }
            }
            // cout << timeline << " " << ready_q.size() << "\n";
            totalturnaround += ready_q.size();
            totalwait +=ready_q.size();
            ++timeline;
        }
        cout << "\nFCFS Gantt Chart --> \n";
        for (pair<int, int> t : ganttChart)
        {
            cout << "Job " << t.first << " @ Time " << t.second << "| ";
        }
    }

    void showAnalysis()
    {
        cout << "\n\nFCFS Scheduling Algorithm Statistics: \n";
        Scheduler::showAnalysis();
    }
};

// class to run the simulation
class Runner
{
    string filename;

public:
    Runner(string f)
    {
        filename = f;
    }
    void filegenerator()
    {
        int sz = 20;
        string filename = "jobprofiles_random.txt";
        ofstream fout;
        fout.open(filename);
        srand ( time(NULL) );
        for (int i = 1; i <= sz; ++i)
        {
            fout << i << " ";
            int arrival = rand() % 100;
            fout << arrival << " ";
            int priority = rand() % 17;
            fout << priority << " ";
            int burstsz = 1 + rand() % 13;
            for (int j = 0; j < burstsz; ++j)
            {
                // cpu
                int exp = rand() % 7;
                fout << (1 << exp) << " ";
                // i/o
                exp = rand() % 7;
                fout << (1 << exp) << " ";
            }
            int last = rand() % 2;
            if (last)
            {
                int exp = rand() % 7;
                fout << (1 << exp) << " ";
            }
            fout << "-1 ";
        }
    }
    void run()
    {
        cout << "Generate random file: (y/n) ";
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y')
        {
            filename = "jobprofiles_random.txt";
            filegenerator();
        }
        FCFS F(filename);
        F.schedule();
        F.showAnalysis();
    }
};

signed main()
{
    Runner R("jobprofiles.txt");
    R.run();
    return 0;
}