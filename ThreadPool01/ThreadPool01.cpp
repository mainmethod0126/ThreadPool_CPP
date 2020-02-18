// ThreadPool01.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <queue>
#include <thread>
#include <Windows.h>
#include <vector>
#include <functional>
#include <mutex>

bool IsThreading;
std::mutex m_Mutex;
std::queue<std::function<void(int)>> m_Jobs;

void Job(int nNum);
unsigned __stdcall WorkThread(void* arg);

int main()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    std::vector<HANDLE> vecThreads;

    int nCPUs = sysInfo.dwNumberOfProcessors;

    for (int i = 0; i < nCPUs; i++)
    {
        int* pArg = new int(i);

        unsigned unThread;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, pArg, 0, &unThread);

        vecThreads.emplace_back(hThread);
    }

    IsThreading = true;

    int nInputJob = 0;

    do {
        std::cout << "Insert JOB : ";
        std::cin >> nInputJob;
        std::cout << std::endl;

        for (int i = 0; i < nInputJob; i++)
        {
            m_Mutex.lock();
            m_Jobs.push(Job);
            m_Mutex.unlock();
        }

    } while (true);
} 

unsigned __stdcall WorkThread(void* arg)
{
    int nNum = *((int*)arg);
    delete arg;

    while (true)
    {
        if (!m_Jobs.empty() && IsThreading)
        {
            m_Mutex.lock();
            
            if (m_Jobs.empty())
            {
                m_Mutex.unlock(); 
                continue;
            }

            std::function<void(int)> CurrentJob = m_Jobs.front();
            m_Jobs.pop();

            m_Mutex.unlock();

            CurrentJob(nNum);
        }
    }
}

void Job(int nNum)
{
    printf("Thread : %d \n", nNum);
}

