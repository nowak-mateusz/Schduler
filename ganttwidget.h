#pragma once

#include <vector>

#include <QWidget>

// deklaracja zapowiadaja�a klasy alg::Scheduler
namespace alg
{
    class Scheduler;
}

//! GanttWidget
/*!
Klasa reprezentuje widget wykresu Gantt'a.
Ob�uguje ona rysowanie wykresu i skalowanie wykresu na podstawie
przekazanych danych
*/
class GanttWidget : public QWidget
{
    Q_OBJECT

public:
    //! Konstruktor
    GanttWidget(QWidget *parent = nullptr);

    //! Metoda rysuj�ca wykres Gantt'a
    void paint(QPainter *painter);

public slots:
    //! Slot ob�uguj�cy dodanie zadnia
    void notifyTask(alg::Scheduler* scheduler, int taskId);
    //! Slot ob�uguj�cy przekroczenie terminu
    void notifyDeadlineMiss(alg::Scheduler* scheduler, int taskId);
    //! Slot ob�uguj�cy czyszczeie danych
    void clear();

protected:
    //! Przeci��ony event rysowania widgetu
    void paintEvent(QPaintEvent *event);
    //! Przeci��ony event ob�ugi k�ka myszki
    void wheelEvent(QWheelEvent *event);

private:
    //! Typty powiadomie�
    enum class NotifyType
    {
        TASK, //!< Zadanie
        MISS  //!< Przekroczenie terminu
    };

    //! Struktura reprezentuj�c prostok�t. Dziediczy po QRectF i dodaje pole typu powidomienia.
    struct GanttRect : public QRectF
    {
        NotifyType notifyType;
    };

    //! Metoda ob�uguje odpowiednie powiadamoenia. Dodaje kolenjne prostok�ty dla odpowiednich zada�.
    void notify(const alg::Scheduler* scheduler, int taskId, int time, NotifyType notifyType);
    //! Funkcja zwraca rozmiar wykresu Gantt'a
    QSize getGanttSize() const;

    const alg::Scheduler* mSchedulerPtr {nullptr}; //!< wska�nik na harmonogram

    // skracacmy nazw�
    using VecRec = std::vector<GanttRect>;
    VecRec* mRectanglesArray {nullptr}; //!< tablica 2D prostok�t�w
    double scale {1.0}; //!< wsp�czynnik skali wykresu
    double timeWidth {2.0}; //!< szeroko�� jednostki czasu na wykrsie
};
