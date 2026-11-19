#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QTimer>          // НОВОЕ (ЛР3): таймер автосохранения
#include "widgets.h"

class MainWindow : public QWidget
{
    Q_OBJECT
    //  Q_OBJECT обязателен, потому что в классе есть слоты
    // и мы используем connect() с сигналами. Без него MOC не сгенерирует
    // метаинформацию, и все connect() упадут с "undefined reference to vtable".

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void moveMovableWidget(); // Перемещение задачи (ЛР2)
    void deletePanel();       // Удаление панели задач (ЛР2)
    void demoDetaching();     // Отвязка (ЛР2)

private:
    void setupUi();
    void setStatus(const QString &text);

    // НОВОЕ (ЛР3): автосохранение и логирование
    void setupAutoSave();                      // настройка таймера
    void logUserAction(const QString &action); // логирование действий

    // QPointer вместо сырого указателя — защита от use-after-free.
    // Если объект удалён, QPointer автоматически станет nullptr.
    // Это прямая связь с ЛР2 (дерево объектов и безопасное удаление).
    QPointer<QVBoxLayout> m_rootLayout;
    QPointer<QVBoxLayout> m_panelLayout;

    QPointer<LogLabel>   m_title;
    QPointer<LogLineEdit> m_input;
    QPointer<LogLabel>   m_status;

    QPointer<LogPanel>   m_taskListPanel;
    QPointer<LogButton>  m_movableBtn;
    QPointer<LogButton>  m_moveButton;
    QPointer<LogButton>  m_deleteButton;

    // НОВОЕ (ЛР3): таймер автосохранения.
    // ЗАЩИТА: объект-поле, а не указатель — таймер живёт столько же,
    // сколько MainWindow, и уничтожается вместе с ним.
    QTimer m_autoSaveTimer;
};

#endif // MAINWINDOW_H