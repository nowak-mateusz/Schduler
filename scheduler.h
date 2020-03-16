#pragma once

#include <vector>

#include <QObject>

namespace alg
{
    // skracam nazwê typu
    using uint = unsigned int;

    //! Harmonogram
    /*!
    Klasa abstrakcyjna harmonogramu.
    Zawiera metody i funkcje pozwalaj¹ce na symulowanie dzia³ania alogrytmów szeregowania
    zadañ. 

    Podklasy musz¹ zaimplementowaæ funcjê getTaskPriority().
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
        Struktura danych reprezentuj¹ca zadanie w harmonogramie
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
            int timeToDeadline {0}; //!< cykle pozosta³e do nstêpnego terminu
            int timeToEnd  {0}; //!< cykle pozosta³e do zakoñczenia zadania
        };

        //! Metoda symuluj¹ca dzia³anie harmonogramu
        /*!
        \param endTime czas zakoñczenia symulacji
        */
        void schedule(uint endTime);

        //! Funkcja sprawdza czy mo¿liwe jest zasymulowanie kolejnego cyklu
        /*!
        \param print czy wypisaæ informacje na konsole
        */
        bool nextTick(bool print = true);

        //! Funkcja zawraca id zadania z tablicy (std::vector)
        /*!
        Wybór najlepszego zadania odbywa siê zgodnie z przyjêtym algorytmem
        */
        int getBestTaskId() const;

        //! Funkcja virtualna porównuj¹ca dwa zadania i dokonuj¹ca 
        //! wyboru najlepszego wed³ug zdefinowanego kryterium
        /*!
        \param a pierwsze zadanie do porównania
        \param b drugie zadanie do porównania
        */
        virtual bool getTaskPriority(const Task& a, const Task& b) const = 0;

        //! Funkcja zwraca akttualny czas symulacji
        uint currentTime() const { return mCurrentTime; }

        //! Funkcja zwraca iloœæ zadañ
        int tasksSize() const { return static_cast<int>(tasks.size()); }

        std::vector<Task> tasks; //!< tablica zadañ

    signals:
        void notifyTask(alg::Scheduler* scheduler, int taskId);
        void notifyDeadlineMiss(alg::Scheduler* scheduler, int taskId);

    private:
        uint mCurrentTime   {0};    //!< czas symulacji
        int mLastTaskId     {-1};   //!< id ostaniego wykonanego zadania. Wartoœc -1 oznacza brak zadania
        bool mSystemOk      {true};  //!< flaga sprawdzaj¹ca czy system dzia³a

    };

    //! DMS
    /*!
    Klasa algorytmu DMS.
    Przeci¹¿a funkcjê getTaskPriority().
    */
    class DMSScheduler : public Scheduler
    {
        //! Przeci¹¿ona funkcja porównuj¹ca dwa zadania
        /*!
        Porównywane s¹ terminy i wybierany jest ten z krótszym terminem
        */
        bool getTaskPriority(const Scheduler::Task& a, const Scheduler::Task& b) const override
        {
            return a.deadline < b.deadline;
        }
    };

    //! LLF
    /*!
    Klasa algorytmu LLS.
    Przeci¹¿a funkcjê getTaskPriority().
    */
    class LLSScheduler : public Scheduler
    {
        //! Przeci¹¿ona funkcja porównuj¹ca dwa zadania
        /*!
        Porównywane s¹ swobody czasowe i wybierane jest to zadanie z mniejsz¹ swobod¹
        */
        bool getTaskPriority(const Scheduler::Task& a, const Scheduler::Task& b) const override
        {
            return laxity(a) < laxity(b);
        }

        //! Funkcja obliczaj¹ca swobodê czasow¹ dla zadania
        int laxity(const Scheduler::Task& t) const
        {
            return t.deadline - (t.time - t.cycles);
        }
    };

    //! Prioryty
    /*!
    Klasa algorytmu piorytetowego.
    Przeci¹¿a funkcjê getTaskPriority().
    */
    class PriorytyScheduler : public Scheduler
    {
        //! Przeci¹¿ona funkcja porównuj¹ca dwa zadania
        /*!
        Porównywane s¹ piorytety i wybierany jest te z wy¿szym piorytetem (mniejsza liczba = wiekszy priorytet)
        */
        bool getTaskPriority(const Scheduler::Task& a, const Scheduler::Task& b) const override
        {
            return a.prioryty < b.prioryty;
        }
    };

}
