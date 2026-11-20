#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QTimer>          // НОВОЕ (ЛР3): таймер автосохранения
#include <QFormLayout>     // НОВОЕ (ЛР4): адаптивная форма
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

protected:
    // НОВОЕ (ЛР4): логирование изменения размера окна
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void moveMovableWidget(); // Перемещение задачи (ЛР2)
    void deletePanel();       // Удаление панели задач (ЛР2)
    void demoDetaching();     // Отвязка (ЛР2)

    // НОВОЕ (ЛР4): слоты для кнопок нижней панели
    void onAddTask();
    void onDeleteTask();
    void onSave();
    void onSettings();

    // НОВОЕ (ЛР4): слоты режимов
    void setCompactMode();
    void setComfortableMode();
    void dumpSizePolicies();

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

    // НОВОЕ (ЛР4): Форма для добавления задачи
    QPointer<QFormLayout> m_addTaskForm;
    QPointer<LogLineEdit> m_taskNameInput;
    QPointer<LogLineEdit> m_taskPriorityInput;

    QPointer<LogLabel>   m_title;
    QPointer<LogLineEdit> m_input; // Старое поле (ЛР2)
    QPointer<LogLabel>   m_status;

    QPointer<LogPanel>   m_taskListPanel;
    QPointer<LogButton>  m_movableBtn;
    QPointer<LogButton>  m_moveButton;
    QPointer<LogButton>  m_deleteButton;

    // НОВОЕ (ЛР4): Кнопки нижней панели
    QPointer<LogButton>  m_addBtn;
    QPointer<LogButton>  m_deleteBtn;
    QPointer<LogButton>  m_saveBtn;
    QPointer<LogButton>  m_settingsBtn;

    // НОВОЕ (ЛР4): Кнопки режимов
    QPointer<LogButton>  m_compactBtn;
    QPointer<LogButton>  m_comfortBtn;
    QPointer<LogButton>  m_dumpBtn;

    // НОВОЕ (ЛР3): таймер автосохранения.
    //объект-поле, а не указатель — таймер живёт столько же,
    // сколько MainWindow, и уничтожается вместе с ним.
    QTimer m_autoSaveTimer;

    // НОВОЕ (ЛР4): список полей для дампа политик
    QList<QWidget*> m_trackedFields;
};

#endif // MAINWINDOW_H