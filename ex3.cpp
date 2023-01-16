// lin tibi 318232139
//koral serb 207972282

#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;
#define time_quantum 2

//process class
class process {
private:
	int arrival, burst;
public:
	//ctor
	process(int arrival, int burst) {
		this->arrival = arrival;
		this->burst = burst;
	}
	//gettes
	int getArrival() const { return arrival; }
	int getBurst() const { return burst; }
	//setters
	void setArrival(int const arrival) { this->arrival = arrival; }
	void setBurst(int const burst) { this->burst = burst; }
};

//comperator struct
struct CompareBurst {
	bool operator()(process* const& p1, process* const& p2) {
		return p1->getBurst() > p2->getBurst();
	}
};

float FCFS(vector<process> arr, int length); //FCFS algorithm
float LCLS_np(vector<process> arr, int length); //LCLS non preemptive algorithm
float LCLS_p(vector<process> arr, int length); //LCLS preemptive algorithm
float RR(vector<process> arr, int length); //RR algorithm
float SJF_p(vector<process> arr, int length); //SJF preemptive algorithm

int main(int argc, char* argv[]) {

	if (argc != 2)
		exit(-1);

	//read from file
	string line, arrival, burst;
	ifstream file(argv[1]);

	//number of processes
	getline(file, line);
	int length = stoi(line);

	//get processes from file
	vector<process> arr;
	for (int i = 0; i < length; i++) {
		getline(file, arrival, ',');
		getline(file, burst);
		arr.push_back(process(stoi(arrival), stoi(burst)));
	}

	//close file
	file.close();

	//sort processes by arrival time
	sort(arr.begin(), arr.end(), (process const& p1, process const& p2) {return p1.getArrival() < p2.getArrival(); });

	//NOTICE: all algorithms assume that the processes array is sorted by arrival time

	//run all algorithms
	cout << "FCFS: mean turnaround = " << FCFS(arr, length) << endl;

	cout << "LCFS (NP): mean turnaround = " << LCLS_np(arr, length) << endl;

	cout << "LCFS (P): mean turnaround = " << LCLS_p(arr, length) << endl;

	cout << "RR: mean turnaround = " << RR(arr, length) << endl;

	cout << "SJF: mean turnaround = " << SJF_p(arr, length) << endl;

	return 1;
}

//FCFS algorithm
float FCFS(vector<process> arr, int length) {
	int curr_time = 0, turnaround_sum = 0, my_turnaround_time = 0;
	for (int i = 0; i < length; i++) {
		if (curr_time < arr[i].getArrival())
			curr_time = arr[i].getArrival();
		curr_time += arr[i].getBurst();
		my_turnaround_time = curr_time - arr[i].getArrival();
		turnaround_sum += my_turnaround_time;
	}
	return (float)turnaround_sum / length;
}

//LCLS non preemptive algorithm
float LCLS_np(vector<process> arr, int length) {
	int curr_time = 0, turnaround_sum = 0, my_turnaround_time = 0;
	int i = 0;
	stack<process*> processes;
	process* pro = NULL;

	while (i < length || !processes.empty())
	{
		while (i < length && arr[i].getArrival() <= curr_time) { //add all arrived processes
			processes.push(&arr[i]);
			i++;
		}

		if (processes.empty())
			curr_time = arr[i].getArrival();

		else
		{
			pro = processes.top();
			processes.pop();
			curr_time += pro->getBurst();
			my_turnaround_time = curr_time-pro->getArrival();
			turnaround_sum += my_turnaround_time;
		}
	}
	return (float)turnaround_sum / length;
}

//LCLS preemptive algorithm
float LCLS_p(vector<process> arr, int length) {
	int curr_time = 0, turnaround_sum = 0, my_turnaround_time = 0;
	stack<process*> processes;
	process* pro = NULL;
	int i = 0;

	while (i < length || !processes.empty())
	{
		while (i < length && arr[i].getArrival() <= curr_time) { //add all arrived processes
			processes.push(&arr[i]);
			i++;
		}

		if (processes.empty())
			curr_time = arr[i].getArrival();

		else
		{
			pro = processes.top();
			processes.pop();

			if (pro->getBurst() <= 1) {//finish in this time quantom
				curr_time += pro->getBurst();
				my_turnaround_time = curr_time - pro->getArrival();
				turnaround_sum += my_turnaround_time;
			}
			else
			{
				curr_time += 1;
				pro->setBurst(pro->getBurst() - 1);
				processes.push(pro);
			}
		}
	}

	return (float)turnaround_sum / length;
}

//RR algorithm
float RR(vector<process> arr, int length) {
	int curr_time = 0, turnaround_sum = 0, my_turnaround_time = 0;

	int finish_counter = 0;
	int i = 0;
        for (i = 0; i < length; i++)
                if (arr[i].getBurst() == 0)
					finish_counter++;

	while(finish_counter < length) {
		if (i == length) //finished going over arr (all processes) came processes
			i = 0; //restart

		else if (arr[i].getArrival() > curr_time) //finished going over all came processes
			if(i <= finish_counter) //all processes untill arr[i] are finished
				curr_time = arr[i].getArrival();
			else
				i = 0; //restart

		else { //if(arr[i].getArrival() <= curr_time) //process came
			if (arr[i].getBurst() != 0) { //process came and didn't finish
				if (arr[i].getBurst() <= time_quantum) { //process finish in this time quantum
					curr_time += arr[i].getBurst();
					my_turnaround_time = curr_time - arr[i].getArrival();
					turnaround_sum += my_turnaround_time;
					arr[i].setBurst(0);
					finish_counter++;
				}
				else {
					curr_time += time_quantum;
					arr[i].setBurst(arr[i].getBurst() - time_quantum);
				}
			}
			i++;
		}
	}
        return (float)turnaround_sum / length;
}

//SJF preemptive algorithm
float SJF_p(vector<process> arr, int length) {
	int curr_time = 0, turnaround_sum = 0, my_turnaround_time = 0;
	priority_queue< process*, vector<process*>, CompareBurst > queue_bursts; //down going bursts time queue

	int i = 0;
	process* last_process;
	while (i < length || !queue_bursts.empty()) {
		while (i < length && arr[i].getArrival() <= curr_time) {//add all arrived processes
			queue_bursts.push(&arr[i]);
			i++;
		}

		if (queue_bursts.empty())
			curr_time = arr[i].getArrival();

		else {
			//get process with shortest burst 
			last_process = queue_bursts.top();
			queue_bursts.pop();

			//execute process with shortest burst
			if (last_process->getBurst() <= 1) { //finish in this time quantom
				curr_time += last_process->getBurst();
				my_turnaround_time = curr_time - last_process->getArrival();
				turnaround_sum += my_turnaround_time;
			}
			else {
				curr_time += 1;
				last_process->setBurst(last_process->getBurst() - 1);
				queue_bursts.push(last_process);
			}
		}
	}
	return (float)turnaround_sum / length;
}
