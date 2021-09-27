//
// Created by qihan on 2021/8/25.
//
// buffer 数量 2n，以两步为例考虑AOAO作为历史信息
// 可以多次运行取平均结果画曲线

#include <iostream>
#include <string>
#include <cstring>
#include "utils/utils.h"
#include "utils/sysadmin.h"
#include <ctime>

using namespace std;

#define NUM_RUN 100
#define Computer_NUM 3
#define OBS_NUM ((Computer_NUM + 1) * 3)
#define ACTION_NUM (Computer_NUM * 2 + 1)
#define MAX_EPISODE 2000
#define INIT_OBS 0
#define MAX_STEP_EPISODE 20
#define STEP_USE 2    // Computer_NUM:3  max_history_step = 2; Computer_NUM:6  max_history_step = ?
#define REAL_OBS_NUM (int(pow(OBS_NUM, STEP_USE)) * int(pow(ACTION_NUM, STEP_USE)))
#define BUFF_SIZE (STEP_USE * 2)

// this value using for Q(s,a)=R(s,a)+alpha * max{Q(s', a')}
// Q(s', a') is the all next step Q value
#define gamma 0.95
#define alpha 0.85

bool print_action = false;
bool print_Q = false;

const uint real_obs_num = max(REAL_OBS_NUM, OBS_NUM);
const uint buff_size = max(BUFF_SIZE, 1);

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

double get_action(double Q[][ACTION_NUM], uint obs)
{
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

void run_training(const SIMULATOR& env, double Q[][ACTION_NUM], double (&hist)[])
{
    //srand((unsigned)time(NULL));
    double episode_reward_mean = 0;
    cout << "[INFO] start training..." << endl;
    for (int i = 0; i < MAX_EPISODE; i++)
    {
        double r_hist[MAX_STEP_EPISODE];
        uint state = env.CreateStartState();
        cout << "------ [INFO] Episode: " << i + 1 << " ------" <<endl;
        bool terminal = false;
        double undiscountedReturn = 0.0;
        double discountedReturn = 0.0;

        uint observation = INIT_OBS;
        int step = 0;
        uint buffer[buff_size];
        if (STEP_USE == 0)
        {
            buffer[0] = observation;
        }
        uint obs_id;
        uint obs_id_new;
        uint observation_new;
        uint statenew;

        while (1)
        {
            uint action;
            int rand_ints = rand() % 100;
            if (step < STEP_USE)
            {
                action = rand() % ACTION_NUM;
                // action = 0;
                buffer[2 * step] = action;
            }
            else
            {
                if (step == STEP_USE)
                {
                    obs_id = seq2realob(buffer);
                }

                if (rand_ints >= 95)
                {
                    action = rand() % ACTION_NUM;
                }
                else
                {
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
                    buffer[2 * step + 1] = observation_new;
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
                cout << "-- Episode " << (i + 1) << " done! "
                                              "Total step: "
                                              << step + 1  <<
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
        // episode_reward_mean += undiscountedReturn;
        episode_reward_mean += compute_disc_R(r_hist, step);
        if ((i + 1) % (MAX_EPISODE / 20) == 0)
        {
            cout << "Writing !!!!!!!!!!!!!!!!!!!!!!!" << endl;
            hist[(i+1)/(MAX_EPISODE / 20) - 1] += episode_reward_mean / ((MAX_EPISODE / 20));
            //expresult<< episode_reward_mean / (MAX_EPISODE / 20) << endl;
            episode_reward_mean = 0;
        }
    }
    if (print_Q)
    {
        cout << "Q convergence matrix:" << endl;
        print_matrix(Q, REAL_OBS_NUM, ACTION_NUM);
    }
}

int main()
{

    std::cout << "Real_Obs_Num: " << real_obs_num << endl;
    std::cout << "Buffer_Size: " << buff_size << endl;
    double R_hist[20]={0};  //只记录其中的20个数据点

    for (int run_ = 0; run_ < NUM_RUN; run_++)
    {
        double Q[real_obs_num][ACTION_NUM];

        for (int i = 0; i < real_obs_num; i++)
        {
            for (int j = 0; j < ACTION_NUM; j++)
            {
                Q[i][j]=rand() % 200 - 200 ;
            }
        }

        srand((int)time(0));
        if (print_Q)
        {
            cout << "Q matrix:" << endl;
            print_matrix(Q, real_obs_num, ACTION_NUM);
        }

        SIMULATOR* env = 0;
        env = new SYS(Computer_NUM, gamma);

        cout << "Run: " << run_ << endl;
        run_training(*env, Q, R_hist);
    }

    std::ofstream expresult("result.txt");
    for (int re = 0; re < 20; re++)
    {
        expresult<< R_hist[re]/NUM_RUN << endl;
    }

    return 0;
}