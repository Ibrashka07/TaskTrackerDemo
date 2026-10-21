#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include "widgets.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void moveMovableWidget(); // Перемещение задачи
    void deletePanel();       // Удаление панели задач
    void demoDetaching();     // Отвязка

private:
    void setupUi();
    void setStatus(const QString &text);

    // QPointer безопасно обнуляется при удалении объекта
    QPointer<QVBoxLayout> m_rootLayout;
    QPointer<QVBoxLayout> m_panelLayout;

    QPointer<LogLabel> m_title;
    QPointer<LogLineEdit> m_input;
    QPointer<LogLabel> m_status;

    QPointer<LogPanel> m_taskListPanel; // Панель задач
    QPointer<LogButton> m_movableBtn;   // Перемещаемая задача
    QPointer<LogButton> m_moveButton;
    QPointer<LogButton> m_deleteButton;
};

#endif // MAINWINDOW_H