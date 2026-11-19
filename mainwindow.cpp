#include "mainwindow.h"
#include <QDateTime>   // НОВОЕ (ЛР3): метка времени для логов

// ============================================================
//  КОНСТРУКТОР / ДЕСТРУКТОР
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    // НОВОЕ (ЛР3): после построения интерфейса запускаем автосохранение.
    // ЗАЩИТА: порядок важен — setupUi() создаёт m_status, куда пишет
    // лямбда автосохранения. Если вызвать setupAutoSave() раньше,
    // первое срабатывание (через 60 с) обратится к nullptr.
    setupAutoSave();
}

MainWindow::~MainWindow()
{
    // НОВОЕ (ЛР3): явно останавливаем таймер перед уничтожением окна.
    // ЗАЩИТА: Qt и сам разорвёт соединение при удалении объекта-контекста
    // (this), но явный stop() — хорошая практика и показывает понимание.
    m_autoSaveTimer.stop();
    qDebug() << "[DESTROY] MainWindow";
}

// ============================================================
//  ПОСТРОЕНИЕ ИНТЕРФЕЙСА (ЛР2, без изменений)
// ============================================================
void MainWindow::setupUi()
{
    setWindowTitle("Трекер задач: Дерево объектов");
    resize(500, 600);
    setObjectName("MainWindow");

    m_rootLayout = new QVBoxLayout(this);

    m_title = new LogLabel("Управление задачами (демо)", this);
    m_rootLayout->addWidget(m_title.data());

    m_input = new LogLineEdit(this);
    m_input->setPlaceholderText("Введите текст задачи...");
    m_rootLayout->addWidget(m_input.data());

    m_status = new LogLabel("Статус: окно создано", this);
    m_status->setWordWrap(true);
    m_rootLayout->addWidget(m_status.data());

    // Панель задач — здесь живут «задачи» (ЛР2: дерево объектов).
    m_taskListPanel = new LogPanel("Панель задач", this);
    m_rootLayout->addWidget(m_taskListPanel.data());

    m_panelLayout = new QVBoxLayout(m_taskListPanel.data());

    LogButton *staticTask = new LogButton("Задача 1 (статическая)", m_taskListPanel.data());
    m_panelLayout->addWidget(staticTask);

    m_movableBtn = new LogButton("Задача 2 (перемещаемая)", m_taskListPanel.data());
    m_panelLayout->addWidget(m_movableBtn.data());

    // Кнопки управления (ЛР2).
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

    // ============================================================
    // НОВОЕ (ЛР3): ЛОГИРОВАНИЕ ДЕЙСТВИЙ ПОЛЬЗОВАТЕЛЯ
    // ============================================================
    //  это ЛЯМБДА-СЛОТЫ. Ключевые моменты:
    //  1. Захват [this] — лямбда получает доступ к методам MainWindow.
    //     Захват по значению ([=]) здесь тоже сработал бы, но [this]
    //     это стандарт для слотов класса и дешевле по копированию.
    //  2. Третий аргумент connect() — контекст (this). Он привязывает
    //     время жизни соединения к MainWindow. Если окно удалят,
    //     Qt автоматически разорвёт соединение — лямбда не вызовется
    //     с висячим указателем (защита от use-after-free).
    //  3. Антипаттерн [&] здесь был бы ОПАСЕН, если бы захватывали
    //     локальные переменные — после выхода из setupUi() они мертвы.
    connect(m_moveButton.data(), &QPushButton::clicked, this, [this]() {
        logUserAction("Пользователь нажал «Переместить задачу»");
    });
    connect(m_deleteButton.data(), &QPushButton::clicked, this, [this]() {
        logUserAction("Пользователь нажал «Удалить панель»");
    });
    connect(detachBtn, &QPushButton::clicked, this, [this]() {
        logUserAction("Пользователь нажал «Демо setParent(nullptr)»");
    });

    setStatus("Готово. Все объекты имеют родителей.");
}

// ============================================================
// НОВОЕ (ЛР3): НАСТРОЙКА АВТОСОХРАНЕНИЯ
// ============================================================
void MainWindow::setupAutoSave()
{
    // Интервал — 60 секунд (60000 мс). Артефакт лабораторной:
    // «автосохранение раз в 60 секунд».
    // ЗАЩИТА: interval() задаётся в МИЛЛИСЕКУНДАХ — это ключевой
    // момент QTimer. 60 * 1000 = 60000.
    m_autoSaveTimer.setInterval(60000);

    // Подключаем сигнал timeout() к лямбда-слоту.
    // ЗАЩИТА: QTimer не «тикает» сам по себе в реальном времени —
    // он регистрируется в цикле событий (event loop) и просит Qt
    // вызвать timeout(), когда управление вернётся в app.exec().
    // Без app.exec() таймер НИКОГДА не сработает.
    connect(&m_autoSaveTimer, &QTimer::timeout, this, [this]() {
        const QString timestamp = QDateTime::currentDateTime()
        .toString("hh:mm:ss");
        // Логируем «сохранение» в консоль.
        // ЗАЩИТА: в реальном проекте здесь было бы QFile/QSettings,
        // но по методичке ЛР3 артефакт — именно автосохранение через
        // QTimer с логированием. Файловый ввод-вывод — тема следующих лаб.
        qDebug() << "[AUTOSAVE]" << timestamp << "Автосохранение данных...";
        setStatus(QString("Автосохранение выполнено в %1").arg(timestamp));
    });

    // start() без аргументов запускает ПОВТОРЯЮЩИЙСЯ таймер
    // с текущим interval(). Если нужно один раз — был бы singleShot()
    // или start(ms).
    m_autoSaveTimer.start();

    qDebug() << "[AUTOSAVE] Таймер автосохранения запущен, интервал 60 с.";
}

// ============================================================
// НОВОЕ (ЛР3): ЛОГИРОВАНИЕ ДЕЙСТВИЙ
// ============================================================
void MainWindow::logUserAction(const QString &action)
{
    // Общий хелпер: пишет действие с меткой времени.
    // ЗАЩИТА: вызов qDebug() виден в панели «Вывод приложения»
    // (Application Output) в Qt Creator. На macOS отдельная консоль
    // не открывается — это нормально.
    const QString timestamp = QDateTime::currentDateTime()
                                  .toString("hh:mm:ss");
    qDebug() << "[USER ACTION]" << timestamp << action;
}

// ============================================================
//  ДАЛЬШЕ — КОД ИЗ ЛР2 (без изменений)
// ============================================================
void MainWindow::setStatus(const QString &text)
{
    // ЗАЩИТА: проверка isNull() — потому что m_status — это QPointer.
    // Если объект уже удалён, QPointer автоматически стал nullptr,
    // и обращение к нему без проверки упало бы.
    if (!m_status.isNull()) m_status->setText(text);
}

void MainWindow::moveMovableWidget()
{
    if (m_movableBtn.isNull() || m_taskListPanel.isNull()) return;

    if (m_movableBtn->parentWidget() == m_taskListPanel.data()) {
        // ЗАЩИТА (ЛР2): setParent() переносит объект из дерева панели
        // в дерево главного окна. Теперь удаление панели НЕ удалит кнопку.
        m_panelLayout->removeWidget(m_movableBtn.data());
        m_movableBtn->setParent(this);
        m_rootLayout->addWidget(m_movableBtn.data());
        m_movableBtn->setText("Задача 2 (в главном окне)");
        setStatus("setParent(this): панель больше не удалит эту задачу.");
    } else {
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

    // ЗАЩИТА (ЛР2): deleteLater() безопаснее delete, потому что
    // откладывает удаление до конца текущей итерации цикла событий.
    // Если удалить объект прямо внутри обработчика его же сигнала —
    // получим use-after-free.
    m_taskListPanel->deleteLater();
}

void MainWindow::demoDetaching()
{
    LogPanel *tempPanel = new LogPanel("Временная панель", this);

    LogLabel *taskA = new LogLabel("Задача А (остается)", tempPanel);
    LogLabel *taskB = new LogLabel("Задача Б (отвязывается)", tempPanel);

    qDebug() << "DEMO: Задач до detach:" << tempPanel->children().size();

    // ЗАЩИТА (ЛР2): setParent(nullptr) отвязывает объект от дерева.
    // Теперь он не удалится вместе с панелью — за него отвечаем мы.
    taskB->setParent(nullptr);
    qDebug() << "DEMO: Задач после detach:" << tempPanel->children().size();

    tempPanel->deleteLater(); // Удаляем панель (Задача А умрет)
    taskB->deleteLater();     // Вручную удаляем отвязанную Задачу Б

    Q_UNUSED(taskA);
    setStatus("Создана временная панель. Задача Б отвязана и удалена вручную.");
}