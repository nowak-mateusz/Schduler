#include "ganttwidget.h"
#include <iostream>
#include <set>

#include <QtGui>

#include "scheduler.h"

// Funkcja szablonowa dokonuj¹ca konwersji do int
template < typename T>
int toInt( T a) { return static_cast<int>(a); }

GanttWidget::GanttWidget(QWidget *parent)
    : QWidget(parent)
{
    // ustawimy minimaln¹ szerokoœc widgetu
    setMinimumWidth(600);
}

QSize GanttWidget::getGanttSize() const
{
    // je¿li nie mamy harmonogramu zwracamy szerokoœæ widgetu
    if(!mSchedulerPtr)
        return size();

    // obliczmy szerokoœc dla pe³nego wykresu
    // szerokoœæ = czas_symulacji + 10 * 2 + 30
    // wysokoœæ  = iloœc_zadañ * 20 + 30
    return QSize( toInt((mSchedulerPtr->currentTime()+10)*timeWidth) +30, (toInt(mSchedulerPtr->tasks.size()) * 20) + 30);
}

void GanttWidget::paintEvent(QPaintEvent* )
{
    // tworzymy obiekt QPainter po którym bedziemy rysowaæ
    QPainter painter;
    painter.begin(this);

    // ustawimy skalê
    //painter.scale(scale, 1.0);
    // zmieniemy minimaln¹ szerokoœc widgetu by paski przesuwania siê dostosowa³y
    setMinimumSize(getGanttSize());
    // ustawimy typ renderowania
    painter.setRenderHint(QPainter::Antialiasing);

    // rysujemy bia³e t³o
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(rect());

    // wywo³ujemy metodê rusowania wykresu
    paint(&painter);
    
    painter.end();
}


void GanttWidget::wheelEvent(QWheelEvent * event)
{
    // ustawimy i ograniczmy skalê
    scale += (event->delta() / 480.0);
    scale = qMax(1.0, qMin(15.0, scale));
    timeWidth = scale*2;
    update();
}

void GanttWidget::paint(QPainter *painter)
{
    // sparawdzamy czy mamy do narysowania jakieœ prostok¹ty
    if (!mRectanglesArray)
    {
        return;
    }

    painter->setPen(QPen(Qt::black));

    // wypisujemy kolejne nazwy dla zadañ
    for (int i = 0; i< mSchedulerPtr->tasksSize(); ++i)
    {
        painter->drawText(5, (i * 20 + 15), "T" + QString::number(i + 1));
    }

    // 1. rysujamy uk³ad wspó³rzêdnych

    // obliczmy wysokoœæ lini pionowej
    int height = mSchedulerPtr->tasksSize() * 20 + 5;
    // rysujemy line pionow¹
    painter->drawLine(25, 0, 25, height);
    // rysujemy linie poziom¹
    painter->drawLine(25, height, toInt((mSchedulerPtr->currentTime()+10)*timeWidth) +30, height);

    // 2. rysujemy kolejne odzinki czasu

    // pêtla dla kolejnych próbek czasu co 10
    for (uint i = 0; i< mSchedulerPtr->currentTime()+10; i++)
    {
        // wyznaczamy x
        double x = 30 + i*timeWidth;

        if( (timeWidth < 5.0 && (i % 20 == 0))
        ||  (timeWidth >= 5.0 && (i % 10 == 0))
        ||  (timeWidth > 10.0 && (i % 5 == 0))
        ||  (timeWidth > 20.0 ))
        {
            // rysujemy pionow¹ szar¹ kreskowan¹ liniê
            painter->setPen(QPen(Qt::lightGray, 0, Qt::DashLine));
            painter->drawLine(QPointF(x, 0.0), QPointF(x, height));
            // piszemy kolejny licznik
            painter->setPen(QPen(Qt::black));
            QRectF rect(x-30, (height + 10 )-30, 60,60);
            painter->drawText(rect, Qt::AlignCenter, QString::number(i ) );
        }
    }
    
    // 3. rysujemy prostok¹ty dla zadañ

    painter->setPen(Qt::NoPen);

    double golden_ratio = 0.618033988749895;
    double h = 0;
    std::set<std::pair<int,double>> missX;

    // dla kolejnych zadañ
    for (int i = 0; i < mSchedulerPtr->tasksSize(); ++i)
    {
        h = golden_ratio * 360 / mSchedulerPtr->tasksSize() * (i+1);
        const  VecRec &vecRec = mRectanglesArray[i];

        // dla kolejnych prostok¹tów dla zadania
        for (const auto &rec : vecRec)
        {
            QRectF r = rec;
            r.setX( 30 + (r.x() *timeWidth) );
            r.setWidth(rec.width()*timeWidth);
            switch(rec.notifyType)
            {
                case NotifyType::TASK :
                {
                    // rysujemy prostok¹t o odpowiednim kolorze
                    painter->fillRect( r, QBrush(QColor::fromHsv(int(h), 245, 245, 255)));
                    break;
                }
                case NotifyType::MISS :
                {
                    //painter->fillRect( r, QBrush(QColor::fromHsv(int(h), 245, 245, 255)));
                    // przekroczenie terminu dodajemy do listy
                    missX.insert(std::make_pair(i,r.x()));
                    break;
                }
            }
        }
    }

    // 4. rysujemy znacznik rozpoczêcia zadania (strza³ka), koñca terminu (okr¹g), przekroczenia terminu (czerwone ko³o)

    //painter->setPen(QPen(Qt::black));

    // pêtla po kolejnych zadaniach
    for (int i = 0; i<mSchedulerPtr->tasksSize(); ++i)
    {
        // pobieramy zadanie
        const alg::Scheduler::Task &t = mSchedulerPtr->tasks[static_cast<size_t>(i)];

        // dla kolejnych próbek casu
        for (uint time = 0; time<mSchedulerPtr->currentTime(); time += static_cast<uint>(t.period))
        {
            // obliczmy x pocz¹tku zadania
            double x = toInt(time)*timeWidth + 30;
            // obliczamy x terminu zadania
            double x2 = x + t.deadline*timeWidth;
            // obliczamy y zadania
            int y = i * 20;

            // rysujemy strza³kê
             painter->setPen(QPen(Qt::black,2));
            painter->drawLine(QPointF(x, y), QPointF(x, y+20));
            painter->drawLine(QPointF(x - 5, y + 15), QPointF( x, y + 20));
            painter->drawLine(QPointF(x + 5, y + 15), QPointF(x, y + 20));

            // je¿eli x terminu zadania jest mniejszy od czasu zakoñczenia symulacji
            // to rysujemy znacznik terminu
            painter->setPen(QPen(Qt::black));
             painter->setBrush(QBrush(Qt::white));
            if( missX.find(std::make_pair(i,x2)) != missX.end() )
             {
                 //painter->setPen(QPen(Qt::red));
                 painter->setBrush(QBrush(Qt::red));
             }

            if( x2 < mSchedulerPtr->currentTime()*timeWidth )
                painter->drawEllipse(QPointF(x2, y + 20), 4.0, 4.0);
        }
    }

}

void GanttWidget::notifyTask( alg::Scheduler* scheduler, int taskId)
{
    notify(scheduler, taskId, toInt(scheduler->currentTime()), NotifyType::TASK );
}

void GanttWidget::notifyDeadlineMiss(alg::Scheduler* scheduler, int taskId)
{
    notify(scheduler, taskId, toInt(scheduler->currentTime()), NotifyType::MISS);
}

void GanttWidget::clear()
{
    if(!mSchedulerPtr)
        return;

    // czyscimy tablice
    for (uint i = 0; i < mSchedulerPtr->tasks.size(); ++i)
    {
         mRectanglesArray[i].clear();
    }
    delete[]  mRectanglesArray;
    mRectanglesArray = nullptr;
}

void GanttWidget::notify(const alg::Scheduler* schedulerPtr, int taskId, int time, NotifyType notifyType)
{
    // przypisujemy harmonogram
    mSchedulerPtr = schedulerPtr;

    // sprawdzmy czy jest zadanie
    if (taskId == -1) 
        return;

    // jezeli nie mamy stworzonej tablicy to j¹ tworzymy
    if (!mRectanglesArray) 
    {
        mRectanglesArray = new VecRec[mSchedulerPtr->tasks.size()];
    }

    VecRec &vecRec = mRectanglesArray[taskId];

    // je¿eli talica jest pusta
    // lub typ powadamienia jest inny ni¿ poprzedni
    // lub punkt koñca ostaniego prostok¹ta jest inny ni¿ aktualny
    // to stwórz nowy prostok¹t
    if (vecRec.size() == 0 
    ||  vecRec.back().notifyType != notifyType 
    ||  toInt(vecRec.back().x() + vecRec.back().width()) != 30 + time)
    {
        GanttRect r;
        
        r.setX(time);
        r.setY(taskId * 20);
        r.setWidth(1);
        r.setHeight(20);
        r.notifyType = notifyType;

        vecRec.push_back(r);
    }
    // roszerz poprzedni prostok¹t
    else 
    {
        vecRec.back().setWidth(vecRec.back().width()+1);
    }

}




