#pragma once

#include <QMainWindow>
#include "scheduler.h"

// deklaracja zapowiadajaæa klasy Ui::MainWindow
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
    //!Funckcja tworzy odpowiedni harmonogram w zale¿noœci od wybranego algorytmu
    alg::Scheduler* createScheduler();
    //!Metoda parsuje tabelkê zadañ i dodaje zadania do harmonogramu
    void updateTask( alg::Scheduler* scheduler);

    Ui::MainWindow* ui; //!< Wska¿nik na klasê zwieraj¹c¹ elemty graficzne okienka
    alg::Scheduler* mScheduler { nullptr };  //!< Wska¿ni na aktualnie wykorzystywany harmonogram

private slots:
    //! Metoda wywo³uj¹ca symulacjê
    void runSimulation();
    //! Metoda lozuje zadania
    void randomTasks();
    //! Metoda dodaj¹ zadanie do tabeli
    void insertTask();
    //! Metoda dodaj¹ zadanie do tabeli w oparciu o wska¿nik do zadania
    void insertTask(alg::Scheduler::Task*);
    //! Metoda usuwaj¹ca ostatnie zadanie w tabeli
    void deleteTask();
};
