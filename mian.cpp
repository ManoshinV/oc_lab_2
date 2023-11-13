#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <cmath>
using namespace std;

int step;
vector<string>dt;
vector<pair<double, int>>res;

struct pthread_args {
    int begin;
    int end;
};

void* Solve(void* arg) {
    struct pthread_args* args = ((struct pthread_args*)arg);
    int begin = args->begin, end = args->end;
    double sum = 0;
    for (int i = begin; i < end; ++i) {
        double wal = 0;
        for (int j = 0; j < 8; ++j) {
            if(dt[i][j] <= '9') 
                wal += (dt[i][j] - '0') * pow (16, 7 - j);
            else 
                wal += (dt[i][j] - '0' - 7) * pow (16, 7 - j);        
        }
        sum += wal;
    }
    res[begin/step] = { sum, end - begin };
    pthread_exit(0);
    return 0;
}

int main(int argc, char* argv[]) {

    string f_name;
    int memo = 0, flow = 0;
    for (int i = 0; i < static_cast <std::string> (argv[1]).size(); ++i)
        f_name += argv[1][i];
    for (int i = 0; i < static_cast <string> (argv[2]).size(); ++i)
        memo *= 10, memo += (argv[2][i] - '0');
    for (int i = 0; i < static_cast <string> (argv[3]).size(); ++i)
        flow *= 10, flow += (argv[3][i] - '0');
    int start_flow = flow;
    flow = min(flow, memo);
    ifstream fl(f_name);
    dt.resize(memo);
    res.resize(flow);

    double ans = 0;
    int kol = 0;

    struct timespec start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);
    bool end = 0;

    while (end == 0) {
        int sz = 0;
        while (sz < memo) {
            if ((fl >> dt[sz])) {
                ++sz;
            }
            else {
                end = 1;
                break;
            }
        }
        if (sz == 0) break;
        if (sz < memo) {
            memo = sz;
            flow = min(flow, memo);
            dt.resize(memo);
            res.resize(flow);
        }
        step = memo / flow;

        pthread_t tid[flow];

        struct pthread_args* args_pth = (struct pthread_args*)malloc(flow * sizeof(struct pthread_args));

        for (int i = 0; i < flow; ++i) {
            args_pth[i].begin = i * step;
            args_pth[i].end = (i == flow - 1) ? memo : (i + 1) * step;
        }
        for (int i = 0; i < flow; ++i)
            pthread_create(&tid[i], NULL, Solve, (void*)&args_pth[i]);
        for (int i = 0; i < flow; ++i)
            pthread_join(tid[i], NULL);
        for (auto [x, y] : res) {
            ans += x;
            kol += y;
        }
    }
    ans /= kol;
    clock_gettime(CLOCK_MONOTONIC, &stop);
    long long elapsed_time = (stop.tv_sec - start.tv_sec) * 1e12 + (stop.tv_nsec - start.tv_nsec);
    cout << start_flow << ' ' << elapsed_time << endl;
    cout << "ans = " << ans << endl;

    return 0;
}
