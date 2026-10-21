#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setWindowTitle("Трекер задач: Дерево объектов");
    resize(500, 600);
    setObjectName("MainWindow");

    m_rootLayout = new QVBoxLayout(this);

    // 1. Заголовок
    m_title = new LogLabel("Управление задачами (демо)", this);
    m_rootLayout->addWidget(m_title.data());

    // 2. Поле ввода для задачи
    m_input = new LogLineEdit(this);
    m_input->setPlaceholderText("Введите текст задачи...");
    m_rootLayout->addWidget(m_input.data());

    // 3. Статус
    m_status = new LogLabel("Статус: окно создано", this);
    m_status->setWordWrap(true);
    m_rootLayout->addWidget(m_status.data());

    // 4. Панель задач
    m_taskListPanel = new LogPanel("Панель задач", this);
    m_rootLayout->addWidget(m_taskListPanel.data());

    m_panelLayout = new QVBoxLayout(m_taskListPanel.data());

    LogButton *staticTask = new LogButton("Задача 1 (статическая)", m_taskListPanel.data());
    m_panelLayout->addWidget(staticTask);

    m_movableBtn = new LogButton("Задача 2 (перемещаемая)", m_taskListPanel.data());
    m_panelLayout->addWidget(m_movableBtn.data());

    // 5. Кнопки управления
    m_moveButton = new LogButton("Переместить Задачу 2: панель <-> окно", this);
    connect(m_moveButton.data(), &QPushButton::clicked, this, &MainWindow::moveMovableWidget);
    m_rootLayout->addWidget(m_moveButton.data());

    m_deleteButton = new LogButton("Удалить панель задач (deleteLater)", this);
    connect(m_deleteButton.data(), &QPushButton::clicked, this, &MainWindow::deletePanel);
    m_rootLayout->addWidget(m_deleteButton.data());

    LogButton *detachBtn = new LogButton("Демо setParent(nullptr)", this);
    connect(detachBtn, &QPushButton::clicked, this, &MainWindow::demoDetaching);
    m_rootLayout->addWidget(detachBtn);

    m_rootLayout->addStretch();

    setStatus("Готово. Все объекты имеют родителей.");
}

void MainWindow::setStatus(const QString &text)
{
    if (!m_status.isNull()) m_status->setText(text);
}

void MainWindow::moveMovableWidget()
{
    if (m_movableBtn.isNull() || m_taskListPanel.isNull()) return;

    if (m_movableBtn->parentWidget() == m_taskListPanel.data()) {
        // Из панели в окно
        m_panelLayout->removeWidget(m_movableBtn.data());
        m_movableBtn->setParent(this);
        m_rootLayout->addWidget(m_movableBtn.data());
        m_movableBtn->setText("Задача 2 (в главном окне)");
        setStatus("setParent(this): панель больше не удалит эту задачу.");
    } else {
        // Из окна в панель
        m_rootLayout->removeWidget(m_movableBtn.data());
        m_movableBtn->setParent(m_taskListPanel.data());
        m_panelLayout->addWidget(m_movableBtn.data());
        m_movableBtn->setText("Задача 2 (в панели)");
        setStatus("Задача снова в панели.");
    }
    m_movableBtn->show();
}

void MainWindow::deletePanel()
{
    if (m_taskListPanel.isNull()) return;

    if (!m_movableBtn.isNull() && m_movableBtn->parentWidget() == m_taskListPanel.data()) {
        setStatus("Панель удаляется. Задача 2 умрет вместе с ней.");
    } else {
        setStatus("Панель удаляется. Задача 2 выживет (она в окне).");
    }

    m_deleteButton->setEnabled(false);
    m_moveButton->setEnabled(false);

    m_taskListPanel->deleteLater();
}

void MainWindow::demoDetaching()
{
    LogPanel *tempPanel = new LogPanel("Временная панель", this);

    LogLabel *taskA = new LogLabel("Задача А (остается)", tempPanel);
    LogLabel *taskB = new LogLabel("Задача Б (отвязывается)", tempPanel);

    qDebug() << "DEMO: Задач до detach:" << tempPanel->children().size();

    taskB->setParent(nullptr); // Отвязываем
    qDebug() << "DEMO: Задач после detach:" << tempPanel->children().size();

    tempPanel->deleteLater(); // Удаляем панель (Задача А умрет)

    taskB->deleteLater();     // Вручную удаляем отвязанную Задачу Б

    Q_UNUSED(taskA); // Убираем warning
    setStatus("Создана временная панель. Задача Б отвязана и удалена вручную.");
}