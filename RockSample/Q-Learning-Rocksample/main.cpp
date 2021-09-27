//
// Created by qihan on 2021/8/25.
//

#include <iostream>
#include <string>
#include <math.h>
#include <cstring>
#include "utils/environment.h"
#include <ctime>
#include <queue>
#include <fstream>

typedef unsigned int uint;

using namespace std;

#define NUM_RUN 100
#define ROCK_NUM 5
#define OBS_NUM 6
#define ACTION_NUM (ROCK_NUM + 5)
#define MAX_EPISODE 9000
#define MAX_STEP_EPISODE 30
#define STEP_USE 0    // Rock_num:5  max_history_step = 2; Rock_num:7  max_history_step = 2
#define REAL_OBS_NUM (int(pow(OBS_NUM, STEP_USE)) * int(pow(ACTION_NUM, STEP_USE)))
#define BUFF_SIZE (STEP_USE * 2)
//#define REAL_OBS_NUM 21600
// ROCK_NUM  OBS_NUM   STEP_USE  ACTION_NUM  REAL_OBS_NUM
//    5         6         2          10         219600
//    7         6         5          12

// this value using for Q(s,a)=R(s,a)+alpha * max{Q(s', a')}
// Q(s', a') is the all next step Q value
#define gamma 0.95
#define alpha 0.85

bool print_action = false;
bool print_Q = false;
//double Q[REAL_OBS_NUM][ACTION_NUM];

const uint real_obs_num = max(REAL_OBS_NUM, OBS_NUM);
const uint buff_size = max(STEP_USE * 2, 1);

double compute_disc_R(double (&r)[MAX_STEP_EPISODE], uint step)
{
    double R = 0;
    for (int i=step; i>=0; i--)
    {
        R = gamma * R + r[i];
    }
    return R;
}

void appand(uint (&a)[], uint obs, uint action)
{
    if (STEP_USE==0)
    {
        a[0] = obs;
    }
    else
    {
        for (int i = 0; i < buff_size - 2; i++)
        {
            a[i] = a[i+2];
        }
        a[-2] = action;
        a[-1] = obs;
    }
}

uint seq2realob(uint arr[])
{
    uint total = arr[buff_size - 1];
    if (STEP_USE == 0)
    {
        return total;
    }
    uint potential = 1;
    for (int i = buff_size - 2; i >= 0; i--)
    {
        if (i % 2 !=0)
        {
            potential *= ACTION_NUM;
        }
        else
        {
            potential *= OBS_NUM;
        }
        total += arr[i] * potential;
    }
    return total;
}

void print_matrix(double m[][ACTION_NUM], int rows, int columns){
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            std::cout << m[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

uint get_action(double m[][ACTION_NUM], uint obs, Environment *e)
{
    double temp_max = m[obs][0];
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
        temp_max = m[obs][1];
    }
    for (int i = start; i < ACTION_NUM; i++)
    {
        if (m[obs][i] > temp_max and e->CheckAction(i))
        {
            temp_max = m[obs][i];
            action = i;
        }
    }
    return action;
}

uint random_action(Environment *e)
{
    uint action;
    uint action_bf[ACTION_NUM];
    int legal_num = 0;
    for (int a = 0; a < ACTION_NUM; a++)
    {
        if (e->CheckAction(a))
        {
            action_bf[legal_num] = a;
            legal_num ++;
        }
    }
    action = action_bf[rand() % legal_num];
    return action;
}

void run_training(Environment *e, double Q[][ACTION_NUM], double (&hist)[])
{
    //srand((unsigned)time(NULL));
    cout << "[INFO] start training..." << endl;
    double episode_reward_mean = 0;
    for (int i = 0; i < MAX_EPISODE; i++)
    {
        double r_hist[MAX_STEP_EPISODE];
        e->initRun();
        cout << "------ [INFO] Episode: " << i + 1 << " ------" <<endl;
        bool terminal = false;
        double undiscountedReturn = 0.0;
        double discountedReturn = 0.0;
        //observation = e->getCurrentObservation();
        int step = 0;
        uint buffer[buff_size];
        if (STEP_USE == 0)
        {
            buffer[0] = 0;
        }
        uint obs_id;
        uint obs_id_new;
        Symbol* observation_new;

        while (1)
        {
            uint action;
            int rand_ints = rand() % 100;
            if (step < STEP_USE)
            {
                action = random_action(e);
                buffer[2 * step] = action;
            }
            else
            {
                if (step == 0)
                {
                    obs_id = 0;
                }
                else if (step == STEP_USE)
                {
                    obs_id = seq2realob(buffer);
                }

                if (rand_ints >= 95)
                {
                    action = random_action(e);
                }
                else
                {
                    action = get_action(Q, obs_id, e);
                }
            }
            double reward;
            if (print_action)
            {
                cout << "-- step " << step << ": action: " << action << endl;
            }
            e->moveRocksample(action);
            reward = e->Reward;
            terminal = e->inTerminalState;
            r_hist[step] = reward;
            if (step < (MAX_STEP_EPISODE - 1) and !terminal)
            {
                observation_new = e->getCurrentObservation();
                if (step < STEP_USE)
                {
                    buffer[2 * step + 1] = observation_new->id;
                }
                else
                {
                    appand(buffer, observation_new->id, action);
                    obs_id_new = seq2realob(buffer);
                    uint maxq_next_action = get_action(Q, obs_id_new, e);
                    double U = reward + gamma * Q[obs_id_new][maxq_next_action];
                    double gap = alpha * (U - Q[obs_id][action]);
                    Q[obs_id][action] += gap;
                    obs_id = obs_id_new;
                }
            }
            else
            {
                if (step >= STEP_USE)
                {
                    double U = reward;
                    Q[obs_id][action] += alpha * (U - Q[obs_id][action]);
                }
            }
            undiscountedReturn += reward;

            if (terminal or step >= MAX_STEP_EPISODE - 1)
            {
                cout << "-- Episode " << i + 1 << " done! "
                                              "Total step:"
                                              << step + 1 <<
                                              "; Reward(un_discount) = " << undiscountedReturn << std::endl;
                break;
            }
            step++;
        }
        if (print_Q)
        {
            cout << "-- updated Q matrix: " << endl;
            print_matrix(Q, REAL_OBS_NUM, ACTION_NUM);
        }
        //episode_reward_mean += undiscountedReturn;
        episode_reward_mean += compute_disc_R(r_hist, step);
        if ((i+1) % 1000 == 0)
        {
            cout << "Writing !!!!!!!!!!!!!!!!!!!!!!!" << endl;
            hist[(i+1) / 1000 - 1] += episode_reward_mean / 1000;
            //expresult<< episode_reward_mean / 1000 << endl;
            episode_reward_mean = 0;
        }

    }
    if (print_Q)
    {
        cout << "Q convergence matrix:" << endl;
        print_matrix(Q, REAL_OBS_NUM, ACTION_NUM);
    }
}

int main() {

    std::cout << "Real_Obs_Num: " << real_obs_num << endl;
    std::cout << "Buffer_Size: " << buff_size << endl;
    double R_hist[9]={0};  //9

    for (int run_ = 0; run_ < NUM_RUN; run_++)
    {
        double Q[real_obs_num][ACTION_NUM];

        for (int i = 0; i < real_obs_num; i++)
        {
            for (int j = 0; j < ACTION_NUM; j++)
            {
                Q[i][j]=rand() % 20;
            }
        }

        srand((int)time(0));
        if (print_Q)
        {
            cout << "Q matrix:" << endl;
            print_matrix(Q, real_obs_num, ACTION_NUM);
        }


        //    SIMULATOR* env = 0;
        //    env = new SYS(Computer_NUM, gamma);
        Environment* env = new Environment(5, ROCK_NUM);

        //    cout << env->nrActions() << "actionnum" << endl;
        //    cout << env->nrObservs() << "obsnum" << endl;
        //    cout << env->nrStates() << "statenum" << endl;

        run_training(env, Q, R_hist);
    }
    std::ofstream expresult("result.txt");
    for (int re = 0; re < 9; re++)
    {
        expresult<< R_hist[re]/NUM_RUN << endl;
    }
    return 0;
}