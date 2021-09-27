//
// Created by qihan on 2021/8/25.
//

#include <iostream>
#include <string>
#include <string.h>
#include "utils/utils.h"
#include "utils/tiger.h"
#include <ctime>

// buffer 数量 2n+1，以两步为例考虑OAOAO作为历史信息
using namespace std;

#define STATE_NUM 2
#define INIT_OBS 0
#define OBS_NUM 3              // 0 -- Nothing; 1/2 -- Tiger roars !
#define ACTION_NUM 3           // 0 -- Listening; 1/2 -- Pull the doors !
#define MAX_EPISODE 400
#define MAX_STEP_EPISODE 20
#define STEP_USE 5   // max_history_step = 5
#define REAL_OBS_NUM (int(pow(OBS_NUM - 1, STEP_USE)) * int(pow(ACTION_NUM, STEP_USE)) * OBS_NUM)
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
    total = total - 1;
    uint potential = 1;
    for (int i = BUFF_SIZE - 2; i > 0; i--)
    {
        if (i % 2 ==0)
        {
            potential *= ACTION_NUM;
            total += (arr[i] - 1) * potential;
        }
        else
        {
            potential *= (OBS_NUM - 1);
            total += arr[i] * potential;
        }
    }
    potential *= ACTION_NUM;
    total += arr[0] * potential;
    return total;
}

void print_matrix(double m[][ACTION_NUM], int rows, int columns){
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            std::cout << m[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

double get_action(double m[][ACTION_NUM], uint obs)
{
    double temp_max = m[obs][0];
    uint action = 0;
    for (uint i = 1; i < ACTION_NUM; ++i) {
        if (m[obs][i] > temp_max){
            temp_max = m[obs][i];
            action = i;
        }
    }
    return action;
}

void run_training(const SIMULATOR& env, double Q[][ACTION_NUM])
{
    std::ofstream expresult("result.txt");
    //srand((unsigned)time(NULL));
    cout << "[INFO] start training..." << endl;
    double episode_reward_mean = 0;
    for (int i = 0; i < MAX_EPISODE; i++)
    {
        double r_hist[MAX_STEP_EPISODE];
        uint state = env.CreateStartState();
        cout << "------ [INFO] Episode: " << i + 1 << " ------" <<endl;
        bool terminal = false;
        double undiscountedReturn = 0.0;
        //double discountedReturn = 0.0;

        uint observation = INIT_OBS;
        //discountedReturn += reward * gamma;
        int step = 0;
        uint buffer[BUFF_SIZE];
        buffer[0] = observation;
        uint observation_new;
        uint statenew;
        uint obs_id;
        uint obs_id_new;

        while (1){
            uint action;
            int rand_ints = rand() % 100;
            if (step < STEP_USE)
            {
                action = rand() % ACTION_NUM;
                // action = 0;
                buffer[2 * step + 1] = action;
            }
            else if (rand_ints >= 99)
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
            terminal = env.StepPOMDP(state, action, observation_new, statenew, reward);
            observation_new++;
            r_hist[step] = reward;
            if (observation_new < 3 and step < (MAX_STEP_EPISODE - 1))
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
                if (step >= STEP_USE)
                {
                    double U = reward;
                    Q[obs_id][action] += alpha * (U - Q[obs_id][action]);
                }
            }
            undiscountedReturn += reward;
            //discountedReturn += reward * gamma;
            if (terminal or step >= MAX_STEP_EPISODE - 1)
            {
                cout << "-- Episode " << i << " done! "
                                              "Total step: "
                                              << step <<
                                              "; Reward(un_discount) = " << undiscountedReturn << std::endl;
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
            print_matrix(Q, REAL_OBS_NUM, ACTION_NUM);
        }
        episode_reward_mean += undiscountedReturn;
        // episode_reward_mean += compute_disc_R(r_hist, step);
        if ((i+1) % 20 == 0)
        {
            expresult<< episode_reward_mean/20 << endl;
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

    for (int i = 0; i < REAL_OBS_NUM; i++)
    {
        for (int j = 0; j < ACTION_NUM; j++)
        {
            Q[i][j]=rand() % 200 - 100 ;
        }
    }

    srand((int)time(0));

    if (print_Q)
    {
        cout << "Q matrix:" << endl;
        print_matrix(Q, REAL_OBS_NUM, ACTION_NUM);
    }

    SIMULATOR* env = 0;
    env = new Tiger(gamma);

    run_training(*env, Q);

    return 0;
}