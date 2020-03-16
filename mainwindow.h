#pragma once

#include <QMainWindow>
#include "scheduler.h"

// deklaracja zapowiadaja�a klasy Ui::MainWindow
namespace Ui
{
class MainWindow;
}

//! MainWindow
/*!
Klasa okienka.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //! Konstruktor
    explicit MainWindow(QWidget *parent = nullptr);
    //! Destruktor
    ~MainWindow();

private:
    //!Funckcja tworzy odpowiedni harmonogram w zale�no�ci od wybranego algorytmu
    alg::Scheduler* createScheduler();
    //!Metoda parsuje tabelk� zada� i dodaje zadania do harmonogramu
    void updateTask( alg::Scheduler* scheduler);

    Ui::MainWindow* ui; //!< Wska�nik na klas� zwieraj�c� elemty graficzne okienka
    alg::Scheduler* mScheduler { nullptr };  //!< Wska�ni na aktualnie wykorzystywany harmonogram

private slots:
    //! Metoda wywo�uj�ca symulacj�
    void runSimulation();
    //! Metoda lozuje zadania
    void randomTasks();
    //! Metoda dodaj� zadanie do tabeli
    void insertTask();
    //! Metoda dodaj� zadanie do tabeli w oparciu o wska�nik do zadania
    void insertTask(alg::Scheduler::Task*);
    //! Metoda usuwaj�ca ostatnie zadanie w tabeli
    void deleteTask();
};
