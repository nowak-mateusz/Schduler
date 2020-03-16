#pragma once

#include <vector>

#include <QObject>

namespace alg
{
    // skracam nazw� typu
    using uint = unsigned int;

    //! Harmonogram
    /*!
    Klasa abstrakcyjna harmonogramu.
    Zawiera metody i funkcje pozwalaj�ce na symulowanie dzia�ania alogrytm�w szeregowania
    zada�. 

    Podklasy musz� zaimplementowa� funcj� getTaskPriority().
    */
    class Scheduler: public QObject
    {
        Q_OBJECT
    public:
        //! Konstruktor
        Scheduler();

        //! Destruktor
        virtual ~Scheduler();

        //! Zadanie
        /*!
        Struktura danych reprezentuj�ca zadanie w harmonogramie
        */
        struct Task
        {
            Task() = default;
            Task(int index, int period, int time, int deadline, int prioryty)
                : index(index)
                , period(period)
                , time(time)
                , deadline(deadline)
                , prioryty(prioryty)
                , timeToDeadline(deadline)
            {}

            int index   {0};  //!< koleny indeks zadania 
            int period  {0};  //!< okres 
            int time    {0};  //!< czas wykonania zadania
            int deadline{0};  //!< termin
            int prioryty{0};  //!< priorytet

            int cycles         {0}; //!< liczba cykli trwania zadania
            int timeToDeadline {0}; //!< cykle pozosta�e do nst�pnego terminu
            int timeToEnd  {0}; //!< cykle pozosta�e do zako�czenia zadania
        };

        //! Metoda symuluj�ca dzia�anie harmonogramu
        /*!
        \param endTime czas zako�czenia symulacji
        */
        void schedule(uint endTime);

        //! Funkcja sprawdza czy mo�liwe jest zasymulowanie kolejnego cyklu
        /*!
        \param print czy wypisa� informacje na konsole
        */
        bool nextTick(bool print = true);

        //! Funkcja zawraca id zadania z tablicy (std::vector)
        /*!
        Wyb�r najlepszego zadania odbywa si� zgodnie z przyj�tym algorytmem
        */
        int getBestTaskId() const;

        //! Funkcja virtualna por�wnuj�ca dwa zadania i dokonuj�ca 
        //! wyboru najlepszego wed�ug zdefinowanego kryterium
        /*!
        \param a pierwsze zadanie do por�wnania
        \param b drugie zadanie do por�wnania
        */
        virtual bool getTaskPriority(const Task& a, const Task& b) const = 0;

        //! Funkcja zwraca akttualny czas symulacji
        uint currentTime() const { return mCurrentTime; }

        //! Funkcja zwraca ilo�� zada�
        int tasksSize() const { return static_cast<int>(tasks.size()); }

        std::vector<Task> tasks; //!< tablica zada�

    signals:
        void notifyTask(alg::Scheduler* scheduler, int taskId);
        void notifyDeadlineMiss(alg::Scheduler* scheduler, int taskId);

    private:
        uint mCurrentTime   {0};    //!< czas symulacji
        int mLastTaskId     {-1};   //!< id ostaniego wykonanego zadania. Warto�c -1 oznacza brak zadania
        bool mSystemOk      {true};  //!< flaga sprawdzaj�ca czy system dzia�a

    };

    //! DMS
    /*!
    Klasa algorytmu DMS.
    Przeci��a funkcj� getTaskPriority().
    */
    class DMSScheduler : public Scheduler
    {
        //! Przeci��ona funkcja por�wnuj�ca dwa zadania
        /*!
        Por�wnywane s� terminy i wybierany jest ten z kr�tszym terminem
        */
        bool getTaskPriority(const Scheduler::Task& a, const Scheduler::Task& b) const override
        {
            return a.deadline < b.deadline;
        }
    };

    //! LLF
    /*!
    Klasa algorytmu LLS.
    Przeci��a funkcj� getTaskPriority().
    */
    class LLSScheduler : public Scheduler
    {
        //! Przeci��ona funkcja por�wnuj�ca dwa zadania
        /*!
        Por�wnywane s� swobody czasowe i wybierane jest to zadanie z mniejsz� swobod�
        */
        bool getTaskPriority(const Scheduler::Task& a, const Scheduler::Task& b) const override
        {
            return laxity(a) < laxity(b);
        }

        //! Funkcja obliczaj�ca swobod� czasow� dla zadania
        int laxity(const Scheduler::Task& t) const
        {
            return t.deadline - (t.time - t.cycles);
        }
    };

    //! Prioryty
    /*!
    Klasa algorytmu piorytetowego.
    Przeci��a funkcj� getTaskPriority().
    */
    class PriorytyScheduler : public Scheduler
    {
        //! Przeci��ona funkcja por�wnuj�ca dwa zadania
        /*!
        Por�wnywane s� piorytety i wybierany jest te z wy�szym piorytetem (mniejsza liczba = wiekszy priorytet)
        */
        bool getTaskPriority(const Scheduler::Task& a, const Scheduler::Task& b) const override
        {
            return a.prioryty < b.prioryty;
        }
    };

}
