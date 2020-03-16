#include "scheduler.h"
#include <algorithm>
#include <iostream>

namespace alg
{
    //! Funkcja sprawdza czy zadnie siê zakoñczy³o
    bool isTaskSchedulable(const Scheduler::Task &t)
    {
        return t.cycles < t.time;
    }

    //! Konstruktor domyœny
    Scheduler::Scheduler() = default;

    //! Destruktor domyœny
    Scheduler::~Scheduler() = default;

    void Scheduler::schedule(uint endTime)
    {
        //Wywa³ane po ty tylko by nabiæ dane w zadaiach
        nextTick(false);
        --mCurrentTime;

        mSystemOk = true;

        //G³ówna pêtla symulacji
        for ( mCurrentTime = 0; mCurrentTime < endTime; mSystemOk &= nextTick())
        {
            // wybór zadania
            int taskId = getBestTaskId();

            // przypisanie zadania do ostanio wykonywanego zadania
            mLastTaskId = taskId;

            // zwiêkszamy iloœæ cykli dla zadania
            if (taskId != -1) 
            {
                tasks[static_cast<size_t>(taskId)].cycles++;
            }

            // wykonujemy zadanie
            emit notifyTask(this, taskId);
        }
    }

    bool Scheduler::nextTick(bool print)
    {
        bool ok = true;

        // zwi¹kszamy licznika czasu
        mCurrentTime++;

        // sparwdzamy kolajne zadani czy nie przeroczy³y terminu
        for (std::size_t i = 0; i< tasks.size(); ++i) 
        {
            Task &t = tasks[i];

            if (t.timeToDeadline > 0)
            {
                t.timeToDeadline--;
            }
            else if(t.cycles != t.time) 
            {

                if (print)
                    emit notifyDeadlineMiss(this, static_cast<int>(i)); // termin zosta³ przekroczony
                t.cycles = t.time; 
                ok = false;
            }

            if (t.timeToEnd > 0) 
            {
                t.timeToEnd--;
            }
            else 
            {
                // zadanie siê zakoñczy³o resetujemy dane
                t.cycles = 0;
                t.timeToEnd = t.period - 1;
                t.timeToDeadline = t.deadline - 1;
            }
        }

        return ok;
    }

    int Scheduler::getBestTaskId() const
    {
        // porónujemy 2 zadania i wyszukujemy najmniejszy 
        // element w tablicy wed³ug kryteriów
        auto t = std::min_element(tasks.begin(), tasks.end(),
            [=](const Task &a, const Task &b)
        {
            if (!isTaskSchedulable(a))
                // a jest zakoñczone, sortuj po b
                return false;
            else if (!isTaskSchedulable(b))
                //  b jest zakoñczone, sortuj po a 
                return true;
            else
                // oba zadania s¹ wykonywane, wybierz to z wiêkszym priorytetem
                return getTaskPriority(a, b);
        }
        );

        // je¿eli wybrane zadanie nie mo¿e byæ zrealizowane zwróæ brak zadania (-1)
        if ( t == tasks.end() || !isTaskSchedulable(*t))
            return -1;
        else
            return std::distance(tasks.begin(), t);
    }
}
