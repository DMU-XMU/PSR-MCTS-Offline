//
// Created by qihan on 2021/8/25.
//

#include <iostream>
#include <string>
#include <string.h>
#include "utils/utils.h"
#include "utils/tiger.h"
#include <ctime>

using namespace std;

#define STATE_NUM 2
#define OBS_NUM 3              // 0 -- Nothing; 1/2 -- Tiger roars !
#define ACTION_NUM 3           // 0 -- Listening; 1/2 -- Pull the doors !
#define MAX_EPISODE 10000

// this value using for Q(s,a)=R(s,a)+alpha * max{Q(s', a')}
// Q(s', a') is the all next step Q value
#define gamma 0.95
#define alpha 0.85

bool print_action = false;
bool print_Q = false;
double Q[OBS_NUM][ACTION_NUM];


void print_matrix(double m[OBS_NUM][ACTION_NUM], int rows, int columns){
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            std::cout << m[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

double get_action(double Q[OBS_NUM][ACTION_NUM], uint obs){
    double temp_max = Q[obs][0];
    uint action = 0;
    for (uint i = 1; i < ACTION_NUM; ++i) {
        if (Q[obs][i] > temp_max){
            temp_max = Q[obs][i];
            action = i;
        }
    }
    return action;
}

void run_training(const SIMULATOR& env){
    //srand((unsigned)time(NULL));
    cout << "[INFO] start training..." << endl;
    for (int i = 0; i < MAX_EPISODE; ++i) {
        uint state = env.CreateStartState();
        cout << "------ [INFO] Episode: " << i + 1 << " ------" <<endl;
        bool terminal = false;
        double undiscountedReturn = 0.0;
        //double discountedReturn = 0.0;

        uint observation = 0;
        //discountedReturn += reward * gamma;
        int step = 0;

        while (1){
            uint action;
            int rand_ints = rand() % 100;
            if (rand_ints >= 95)
            {
                action = rand() % ACTION_NUM;
            }
            else
            {
                action = get_action(Q, observation);
            }
            uint observation_new;
            uint statenew;
            double reward;
            if (print_action)
            {
                cout << "-- step " << step << ": action: " << action << endl;
            }
            terminal = env.StepPOMDP(state, action, observation_new, statenew, reward);
            observation_new++;
            if (observation_new < 3)
            {
                uint maxq_next_action = get_action(Q, observation_new);
                double U = reward + gamma * Q[observation_new][maxq_next_action];
                double gap = alpha * (U - Q[observation][action]);
                Q[observation][action] += gap;
                state = statenew;
                observation = observation_new;
            }
            else
            {
                double U = reward;
                Q[observation][action] += alpha * (U - Q[observation][action]);
            }
            undiscountedReturn += reward;
            //discountedReturn += reward * gamma;
            if (terminal)
            {
                cout << "-- Episode " << i << " done! Reward(un_discount) = " << undiscountedReturn << std::endl;
                break;
            }
//            if (step >= 9)
//            {
//                break;
//            }
            step++;
        }
        if (print_Q)
        {
            cout << "-- updated Q matrix: " << endl;
            print_matrix(Q, OBS_NUM, ACTION_NUM);
        }

    }
}

int main() {

    for (int i = 0; i < OBS_NUM; ++i)
    {
        for (int j = 0; j < ACTION_NUM; ++j)
        {
            Q[i][j]=rand() % 200 - 100 ;
        }
    }

    srand((int)time(0));

    cout << "Q matrix:" << endl;
    print_matrix(Q, OBS_NUM, ACTION_NUM);

    SIMULATOR* env = 0;
    env = new Tiger(gamma);

    run_training(*env);
    cout << "Q convergence matrix:" << endl;
    print_matrix(Q, OBS_NUM, ACTION_NUM);

    return 0;
}