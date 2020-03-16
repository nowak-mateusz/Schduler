#include "scheduler.h"
#include <algorithm>
#include <iostream>

namespace alg
{
    //! Funkcja sprawdza czy zadnie si� zako�czy�o
    bool isTaskSchedulable(const Scheduler::Task &t)
    {
        return t.cycles < t.time;
    }

    //! Konstruktor domy�ny
    Scheduler::Scheduler() = default;

    //! Destruktor domy�ny
    Scheduler::~Scheduler() = default;

    void Scheduler::schedule(uint endTime)
    {
        //Wywa�ane po ty tylko by nabi� dane w zadaiach
        nextTick(false);
        --mCurrentTime;

        mSystemOk = true;

        //G��wna p�tla symulacji
        for ( mCurrentTime = 0; mCurrentTime < endTime; mSystemOk &= nextTick())
        {
            // wyb�r zadania
            int taskId = getBestTaskId();

            // przypisanie zadania do ostanio wykonywanego zadania
            mLastTaskId = taskId;

            // zwi�kszamy ilo�� cykli dla zadania
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

        // zwi�kszamy licznika czasu
        mCurrentTime++;

        // sparwdzamy kolajne zadani czy nie przeroczy�y terminu
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
                    emit notifyDeadlineMiss(this, static_cast<int>(i)); // termin zosta� przekroczony
                t.cycles = t.time; 
                ok = false;
            }

            if (t.timeToEnd > 0) 
            {
                t.timeToEnd--;
            }
            else 
            {
                // zadanie si� zako�czy�o resetujemy dane
                t.cycles = 0;
                t.timeToEnd = t.period - 1;
                t.timeToDeadline = t.deadline - 1;
            }
        }

        return ok;
    }

    int Scheduler::getBestTaskId() const
    {
        // por�nujemy 2 zadania i wyszukujemy najmniejszy 
        // element w tablicy wed�ug kryteri�w
        auto t = std::min_element(tasks.begin(), tasks.end(),
            [=](const Task &a, const Task &b)
        {
            if (!isTaskSchedulable(a))
                // a jest zako�czone, sortuj po b
                return false;
            else if (!isTaskSchedulable(b))
                //  b jest zako�czone, sortuj po a 
                return true;
            else
                // oba zadania s� wykonywane, wybierz to z wi�kszym priorytetem
                return getTaskPriority(a, b);
        }
        );

        // je�eli wybrane zadanie nie mo�e by� zrealizowane zwr�� brak zadania (-1)
        if ( t == tasks.end() || !isTaskSchedulable(*t))
            return -1;
        else
            return std::distance(tasks.begin(), t);
    }
}
