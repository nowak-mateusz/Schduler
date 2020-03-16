#pragma once

#include <vector>

#include <QWidget>

// deklaracja zapowiadajaæa klasy alg::Scheduler
namespace alg
{
    class Scheduler;
}

//! GanttWidget
/*!
Klasa reprezentuje widget wykresu Gantt'a.
Ob³uguje ona rysowanie wykresu i skalowanie wykresu na podstawie
przekazanych danych
*/
class GanttWidget : public QWidget
{
    Q_OBJECT

public:
    //! Konstruktor
    GanttWidget(QWidget *parent = nullptr);

    //! Metoda rysuj¹ca wykres Gantt'a
    void paint(QPainter *painter);

public slots:
    //! Slot ob³uguj¹cy dodanie zadnia
    void notifyTask(alg::Scheduler* scheduler, int taskId);
    //! Slot ob³uguj¹cy przekroczenie terminu
    void notifyDeadlineMiss(alg::Scheduler* scheduler, int taskId);
    //! Slot ob³uguj¹cy czyszczeie danych
    void clear();

protected:
    //! Przeci¹¿ony event rysowania widgetu
    void paintEvent(QPaintEvent *event);
    //! Przeci¹¿ony event ob³ugi kó³ka myszki
    void wheelEvent(QWheelEvent *event);

private:
    //! Typty powiadomieñ
    enum class NotifyType
    {
        TASK, //!< Zadanie
        MISS  //!< Przekroczenie terminu
    };

    //! Struktura reprezentuj¹c prostok¹t. Dziediczy po QRectF i dodaje pole typu powidomienia.
    struct GanttRect : public QRectF
    {
        NotifyType notifyType;
    };

    //! Metoda ob³uguje odpowiednie powiadamoenia. Dodaje kolenjne prostok¹ty dla odpowiednich zadañ.
    void notify(const alg::Scheduler* scheduler, int taskId, int time, NotifyType notifyType);
    //! Funkcja zwraca rozmiar wykresu Gantt'a
    QSize getGanttSize() const;

    const alg::Scheduler* mSchedulerPtr {nullptr}; //!< wskaŸnik na harmonogram

    // skracacmy nazwê
    using VecRec = std::vector<GanttRect>;
    VecRec* mRectanglesArray {nullptr}; //!< tablica 2D prostok¹tów
    double scale {1.0}; //!< wspó³czynnik skali wykresu
    double timeWidth {2.0}; //!< szerokoœæ jednostki czasu na wykrsie
};
