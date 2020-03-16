#include "mainwindow.h"
#include <iostream> 
#include <random>

#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //przygotowujemy elemty graficzne okna
    ui->setupUi(this);
    // ustawiomy tytu³ okna
    setWindowTitle("Symulacja");
    // dodajemy anazwy algorytmów
    ui->comboBox->addItem("DMS");
    ui->comboBox->addItem("LLF");
    ui->comboBox->addItem("Priorytetowy");

    ui->spinBox->setValue(500);
    // ³¹czymy sygna³y i sloty
    connect(ui->pushButton, SIGNAL(clicked()), SLOT(runSimulation()));
    connect(ui->pushButton_2, SIGNAL(clicked()), SLOT(randomTasks()));
    connect(ui->pushButton_3,SIGNAL(clicked()), SLOT(insertTask()));
    connect(ui->pushButton_4,SIGNAL(clicked()), SLOT(deleteTask()));

    // wstawimy przyk³adowe zadania
    alg::Scheduler::Task t0(0, 80, 30, 80, 0);
    alg::Scheduler::Task t1(1, 120, 60, 120, 0);

    insertTask(&t0);
    insertTask(&t1);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mScheduler;
}

alg::Scheduler* MainWindow::createScheduler()
{
    // w zale¿noœci od wybranego algorytmu tworzymy odpowiedni harmonogram
    if (ui->comboBox->currentText() == "DMS")
    {
        return new alg::DMSScheduler;
    }
    else if( ui->comboBox->currentText() == "LLF")
    {
        return new alg::LLSScheduler;
    }
    else if(ui->comboBox->currentText() == "Priorytetowy")
    {
        return new alg::PriorytyScheduler;
    }

    return nullptr;
}

void MainWindow::runSimulation()
{
    // czyscimy widget wykresu Gantt'a
    ui->widget->clear();
    // usuwamy poprzedni harmonogram
    delete mScheduler;

    // tworzywy nowy harmonogram
    mScheduler = createScheduler();
    if (!mScheduler)
        return;

    // ³¹czymy sygan³y i sloty pomiêdzy harmonogramem a widgetem wykresu Gantt'a
    connect(mScheduler, SIGNAL(notifyTask(alg::Scheduler*, int)), ui->widget, SLOT(notifyTask(alg::Scheduler*, int)));
    connect(mScheduler, SIGNAL(notifyDeadlineMiss(alg::Scheduler*, int)), ui->widget, SLOT(notifyDeadlineMiss(alg::Scheduler*, int)));

    // parsujemy tabelkê z zadanaimi i wpisujemy zadania do harmonogramu
    updateTask(mScheduler);

    // pobieramy czas symulacji
    uint time = static_cast<uint>(ui->spinBox->value());

    // uruchamimy symulacjê o oktreœlon¹ iloœæ czasu symulacji
    mScheduler->schedule(time);

    // od³aczamy wszyskie sygna³y po³¹czone z widgetu wykresu Gantt'a
    disconnect(ui->widget);

    // aktualizujmy okno
    update();
    // hack by pojawi³ siê wykres zmienimy rozmiar okna
    resize(width() + 1, height());
    resize(width() - 1, height());
}

void MainWindow::updateTask( alg::Scheduler* scheduler)
{
    // sparwdzamy czy zosta³ przekazany harmonogram
    if(!scheduler)
        return;

    // czyœcimy tablicê zadañ
    scheduler->tasks.clear();

    // parsujemy kolejne wiersze tabelii z zadanimai
    for(int row = 0; row < ui->tableWidget->rowCount() ; row++)
    {
        // uzyskujemy odpowiednie widgety z kolejnych kolumn tabeli dla danego wiersza
        QLineEdit *timeWidget = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(row,1));
        QLineEdit *periodWidget = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(row,2));
        QLineEdit *deadlineWidget =  dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(row,3));
        QLineEdit *priorytyWidget = dynamic_cast<QLineEdit*>(ui->tableWidget->cellWidget(row,4));

        // dane dla zadania
        int index {row};   // koleny indeks zadania
        int time {1};      // okres
        int period {1};    // czas wykonania zadania
        int deadline {1};  // termin
        int prioryty {0};  // priorytet

        // je¿eli uda³o siê uzyskaæ odpowiednie widgety aktualizujemy dane
        if(timeWidget)
            time = timeWidget->text().toInt();
        if(periodWidget)
            period = periodWidget->text().toInt();
        if(deadlineWidget)
            deadline = deadlineWidget->text().toInt();
        if(priorytyWidget)
            prioryty = priorytyWidget->text().toInt();

        // tworzymy obiekt zadania
        alg::Scheduler::Task t(index, period, time, deadline,  prioryty);

        // wstawimy zadanie do tablicy zadañ w harmonogramie
        mScheduler->tasks.push_back(t );
    }
}

void MainWindow::randomTasks()
{
    // czyœcimy tabele
    ui->tableWidget->setRowCount(0);

    // inicjalizujemy generatory liczb psudolosowych

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disTaskCount(1, 9);
    std::uniform_int_distribution<> disTime(1, 100);
    std::uniform_int_distribution<> disPrioryty(0, 9);

    // losujemy liczbê zadañ
    int taskCount = disTaskCount(gen);

    // dla kolejnych zadañ losujemy parametry zdania
    for(int i = 0; i < taskCount; i++)
    {
        int time = disTime(gen);
        int prioryty = disPrioryty(gen);

        std::uniform_int_distribution<> disPeriod(time+10, time+50);
        int period = disPeriod(gen);

        alg::Scheduler::Task t(i, period, time, period, prioryty );

        insertTask(&t);
    }

}

void MainWindow::insertTask()
{
    insertTask(nullptr);
}

void MainWindow::insertTask(alg::Scheduler::Task* task)
{
    // pobieramy liczbê wiery w tabelii
    int row = ui->tableWidget->rowCount();

    // dodajemy kolejny wiersz do tabeli
    ui->tableWidget->insertRow(row);

    // tworzymy obiekt komórki tabelii reprezentuj¹cy kolejny indeks zadania
    QTableWidgetItem *indexItem = new QTableWidgetItem("T" + QString::number(row+1));
    // ustawimy flagê by nie mo¿na by³o go zmieniæ
    indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEditable);
    // wstawimy utworzony obiekt do pierwszej kolumny
    ui->tableWidget->setItem(row,0, indexItem);

    // tworzymy widget edycji
    QLineEdit *time = new QLineEdit;
    // ustawimy by nie mia³ ramki
    time->setFrame(false);
    // tworzymy i ustawiamy widgetowy walidator zakresu danych
    time->setValidator( new QIntValidator(1, 1000,  this) );
    // sparwdzamy czy przekazywany jest wskanik do zadania je¿eli tak to wstawiamy dane z niego
    if(task)
        time->setText( QString::number(task->time));
    else
        time->setText( "1");
    // dodajemy widget to odpowiedniej komórki
    ui->tableWidget->setCellWidget ( row, 1,  time);

    // w dalszej i robimy podobnie dodj¹c widgety do odpowiednich komórek

    QLineEdit *period = new QLineEdit;
    period->setFrame(false);
    period->setValidator( new QIntValidator(1, 1000,  this) );
    if(task)
        period->setText( QString::number(task->period));
    else
        period->setText( "1");

    ui->tableWidget->setCellWidget ( row, 2,  period);

    QLineEdit *deadline= new QLineEdit;
    deadline->setFrame(false);
    deadline->setValidator( new QIntValidator(1, 1000,  this) );
    if(task)
        deadline->setText( QString::number(task->deadline));
    else
        deadline->setText( "1");
    ui->tableWidget->setCellWidget ( row, 3,  deadline);

    QLineEdit *prioryty= new QLineEdit;
    prioryty->setFrame(false);
    prioryty->setValidator( new QIntValidator(0, 100,  this) );
    if(task)
        prioryty->setText( QString::number(task->prioryty));
    else
        prioryty->setText( "1");
    ui->tableWidget->setCellWidget ( row, 4,  prioryty);

}

void MainWindow::deleteTask()
{
    ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
}
