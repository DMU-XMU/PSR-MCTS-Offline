//
// Created by qihan on 2021/8/25.
//

#include <iostream>
#include <string>
#include <cstring>
#include "utils/environment.h"
#include <ctime>
typedef unsigned int uint;

using namespace std;

#define ROCK_NUM 5
#define OBS_NUM 6
#define ACTION_NUM (ROCK_NUM + 5)
#define MAX_EPISODE 1000
#define MAX_STEP_EPISODE 30

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

uint get_action(double Q[OBS_NUM][ACTION_NUM], uint obs, Environment *e)
{
    double temp_max = Q[obs][0];
    uint action;
    int start;
    if (e->CheckAction(0))
    {
        action = 0;
        start = 1;
    }
    else
    {
        action = 1;
        start = 2;
    }
    for (int i = start; i < ACTION_NUM; ++i)
    {
        if (Q[obs][i] > temp_max and e->CheckAction(i))
        {
            temp_max = Q[obs][i];
            action = i;
        }
    }
    return action;
}

void run_training(Environment *e)
{
    //srand((unsigned)time(NULL));
    cout << "[INFO] start training..." << endl;
    for (int i = 0; i < MAX_EPISODE; ++i)
    {
        e->initRun();
        cout << "------ [INFO] Episode: " << i << " ------" <<endl;
        bool terminal = false;
        double undiscountedReturn = 0.0;
        //double discountedReturn = 0.0;
        Symbol* observation;
        observation = e->getCurrentObservation();
        //discountedReturn += reward * gamma;
        int step = 0;

        while (1){
            uint action;
            int rand_ints = rand() % 100;
            if (rand_ints >= 95)
            {
                bool legal_mat[ACTION_NUM] = {false};
                uint action_bf[ACTION_NUM];
                int legal_num = 0;
                for (int a = 0; a < ACTION_NUM; ++a)
                {
                    if (e->CheckAction(a))
                    {
                        legal_mat[a] = true;
                        action_bf[legal_num] = a;
                        legal_num ++;
                    }
                }
                action = action_bf[rand() % legal_num];
            }
            else
            {
                action = get_action(Q, observation->id, e);
            }
            Symbol* observation_new;
            double reward;
            if (print_action)
            {
                cout << "-- step " << step << ": action: " << action << endl;
            }
            e->moveRocksample(action);
            reward = e->Reward;
            terminal = e->inTerminalState;
            if (step < MAX_STEP_EPISODE - 1)
            {
                observation_new = e->getCurrentObservation();
                uint maxq_next_action = get_action(Q, observation_new->id, e);
                double U = reward + gamma * Q[observation_new->id][maxq_next_action];
                double gap = alpha * (U - Q[observation->id][action]);
                Q[observation->id][action] += gap;
                observation = observation_new;
            }
            else
            {
                double U = reward;
                Q[observation->id][action] += alpha * (U - Q[observation->id][action]);
            }
            undiscountedReturn += reward;
            //discountedReturn += reward * gamma;
            if (terminal)
            {
                cout << "-- Episode " << i << " done! Reward(un_discount) = " << undiscountedReturn << std::endl;
                break;
            }
            if (step >= MAX_STEP_EPISODE - 1)
            {
                cout << "-- Episode " << i << " done! Reward(un_discount) = " << undiscountedReturn << std::endl;
                break;
            }
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
            Q[i][j]=rand() % 200 - 200 ;
        }
    }

    srand((int)time(0));

    cout << "Q matrix:" << endl;
    print_matrix(Q, OBS_NUM, ACTION_NUM);

//    SIMULATOR* env = 0;
//    env = new SYS(Computer_NUM, gamma);
    Environment* env = new Environment(5,ROCK_NUM);

//    cout << env->nrActions() << "actionnum" << endl;
//    cout << env->nrObservs() << "obsnum" << endl;
//    cout << env->nrStates() << "statenum" << endl;

    run_training(env);
    cout << "Q convergence matrix:" << endl;
    print_matrix(Q, OBS_NUM, ACTION_NUM);

    return 0;
}