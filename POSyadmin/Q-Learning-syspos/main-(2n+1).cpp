//
// Created by qihan on 2021/8/25.
//

#include <iostream>
#include <string>
#include <cstring>
#include "utils/utils.h"
#include "utils/sysadmin.h"
#include <ctime>

// buffer 数量 2n+1，以两步为例考虑OAOAO作为历史信息
using namespace std;

#define Computer_NUM 3
#define OBS_NUM ((Computer_NUM + 1) * 3)
#define ACTION_NUM (Computer_NUM * 2 + 1)
#define MAX_EPISODE 120
#define INIT_OBS 0
#define MAX_STEP_EPISODE 20
#define STEP_USE 1    // Computer_NUM:3  max_history_step = 1; Computer_NUM:6  max_history_step = ?
#define REAL_OBS_NUM (int(pow(OBS_NUM, STEP_USE+1)) * int(pow(ACTION_NUM, STEP_USE)))
#define BUFF_SIZE (STEP_USE * 2 + 1)

// this value using for Q(s,a)=R(s,a)+alpha * max{Q(s', a')}
// Q(s', a') is the all next step Q value
#define gamma 0.95
#define alpha 0.85

bool print_action = false;
bool print_Q = false;

double compute_disc_R(double (&r)[MAX_STEP_EPISODE], uint step)
{
    double R = 0;
    for (int i=step; i>=0; i--)
    {
        R = gamma * R + r[i];
    }
    return R;
}

void appand(uint (&a)[BUFF_SIZE], uint obs, uint action)
{
    if (STEP_USE==0)
    {
        a[0] = obs;
    }
    else
    {
        for (int i = 0; i < BUFF_SIZE - 2; i++)
        {
            a[i] = a[i+2];
        }
        a[-2] = action;
        a[-1] = obs;
    }
}

uint seq2realob(uint arr[BUFF_SIZE])
{
    uint total = arr[BUFF_SIZE - 1];
    if (BUFF_SIZE == 1)
    {
        return total;
    }
    uint potential = 1;
    for (int i = BUFF_SIZE - 2; i >= 0; i--)
    {
        if (i % 2 ==0)
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

double get_action(double Q[][ACTION_NUM], uint obs){
    double temp_max = Q[obs][0];
    uint action = 0;
    for (uint i = 1; i < ACTION_NUM; i++) {
        if (Q[obs][i] > temp_max){
            temp_max = Q[obs][i];
            action = i;
        }
    }
    return action;
}

void run_training(const SIMULATOR& env, double Q[][ACTION_NUM])
{
    //srand((unsigned)time(NULL));
    std::ofstream expresult("result.txt");
    double episode_reward_mean = 0;
    cout << "[INFO] start training..." << endl;
    for (int i = 0; i < MAX_EPISODE; i++)
    {
        double r_hist[MAX_STEP_EPISODE];
        uint state = env.CreateStartState();
        cout << "------ [INFO] Episode: " << i << " ------" <<endl;
        bool terminal = false;
        double undiscountedReturn = 0.0;

        uint observation = INIT_OBS;
        int step = 0;
        uint buffer[BUFF_SIZE];
        buffer[0] = observation;
        uint obs_id;
        uint obs_id_new;
        uint observation_new;
        uint statenew;

        while (1){
            uint action;
            int rand_ints = rand() % 100;
            if (step < STEP_USE)
            {
                action = rand() % ACTION_NUM;
                // action = 0;
                buffer[2 * step + 1] = action;
            }
            else if (rand_ints >= 95)
            {
                action = rand() % ACTION_NUM;
            }
            else
            {
                if (step==0)
                {
                    obs_id = observation;
                    action = get_action(Q, obs_id);
                }
                else
                {
                    if (step == STEP_USE)
                    {
                        obs_id = seq2realob(buffer);
                    }
                    action = get_action(Q, obs_id);
                }
            }
            double reward;
            if (print_action)
            {
                cout << "-- step " << step << ": action: " << action << endl;
            }
            env.StepPOMDP(state, action, observation_new, statenew, reward);
            r_hist[step] = reward;
            if (step < MAX_STEP_EPISODE - 1)
            {
                if (step < STEP_USE)
                {
                    buffer[2 * (step + 1)] = observation_new;
                }
                else
                {
                    appand(buffer, observation_new, action);
                    obs_id_new = seq2realob(buffer);
                    uint maxq_next_action = get_action(Q, obs_id_new);
                    double U = reward + gamma * Q[obs_id_new][maxq_next_action];
                    double gap = alpha * (U - Q[obs_id][action]);
                    Q[obs_id][action] += gap;
                    state = statenew;
                    observation = observation_new;
                    obs_id = obs_id_new;
                }

            }
            else
            {
                double U = reward;
                Q[obs_id][action] += alpha * (U - Q[obs_id][action]);
            }
            undiscountedReturn += reward;

            if (step >= MAX_STEP_EPISODE - 1)
            {
                cout << "-- Episode " << i << " done! "
                                              "Total step: "
                                              << step <<
                                              "; Reward(un_discount) = " << undiscountedReturn << std::endl;
                break;
            }
            step++;
        }
        if (print_Q)
        {
            cout << "-- updated Q matrix: " << endl;
            print_matrix(Q, OBS_NUM, ACTION_NUM);
        }
        episode_reward_mean += undiscountedReturn;
        // episode_reward_mean += compute_disc_R(r_hist, step);
        if ((i+1) % 6 == 0)
        {
            expresult<< episode_reward_mean / 6 << endl;
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
    std::cout << REAL_OBS_NUM << endl;
    std::cout << BUFF_SIZE << endl;
    double Q[REAL_OBS_NUM][ACTION_NUM];

    for (int i = 0; i < REAL_OBS_NUM; ++i)
    {
        for (int j = 0; j < ACTION_NUM; ++j)
        {
            Q[i][j]=rand() % 200 - 200 ;
        }
    }

    srand((int)time(0));
    if (print_Q)
    {
        cout << "Q matrix:" << endl;
        print_matrix(Q, REAL_OBS_NUM, ACTION_NUM);
    }

    SIMULATOR* env = 0;
    env = new SYS(Computer_NUM, gamma);

    run_training(*env, Q);

    return 0;
}