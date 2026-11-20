#include "mainwindow.h"
#include <QDateTime>   // НОВОЕ (ЛР3): метка времени для логов
#include <QDebug>
#include <QResizeEvent> // НОВОЕ (ЛР4)
#include <QSizePolicy>  // НОВОЕ (ЛР4)
#include <QHBoxLayout>
#include <QIntValidator>

// ============================================================
//  КОНСТРУКТОР / ДЕСТРУКТОР
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    // НОВОЕ (ЛР3): после построения интерфейса запускаем автосохранение.
    //  порядок важен — setupUi() создаёт m_status, куда пишет
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
//  НОВОЕ (ЛР4): ЛОГИРОВАНИЕ РАЗМЕРА ОКНА
// ============================================================
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    qDebug() << "[RESIZE] MainWindow ->"
             << event->size().width() << "x" << event->size().height();
}

// ============================================================
//  ПОСТРОЕНИЕ ИНТЕРФЕЙСА (ЛР2, ЛР3, ЛР4)
// ============================================================
void MainWindow::setupUi()
{
    setWindowTitle("Трекер задач: ЛР4 (Адаптивность)");
    resize(700, 600); // Чуть шире для новых элементов
    setObjectName("MainWindow");

    // НОВОЕ (ЛР4): Корневой Layout с отступами
    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(16, 16, 16, 16);
    m_rootLayout->setSpacing(10);

    m_title = new LogLabel("Управление задачами (демо)", this);
    m_rootLayout->addWidget(m_title.data());

    // ============================================================
    // НОВОЕ (ЛР4): АДАПТИВНАЯ ФОРМА ДОБАВЛЕНИЯ ЗАДАЧИ
    // ============================================================
    //  Обнуляем отступы, чтобы не было двойных отступов с корневым Layout.
    m_addTaskForm = new QFormLayout();
    m_addTaskForm->setContentsMargins(0, 0, 0, 0);
    //  Позволяем форме тянуть поля (иначе на macOS они застынут).
    m_addTaskForm->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Поле "Название" (тянется вширь)
    m_taskNameInput = new LogLineEdit(this);
    m_taskNameInput->setPlaceholderText("Введите текст задачи...");
    m_taskNameInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_addTaskForm->addRow("Название:", m_taskNameInput.data());

    // Поле "Приоритет" (Fixed + пружина)
    m_taskPriorityInput = new LogLineEdit(this);
    m_taskPriorityInput->setPlaceholderText("1-5");
    m_taskPriorityInput->setValidator(new QIntValidator(1, 5, m_taskPriorityInput.data()));
    m_taskPriorityInput->setMinimumWidth(50);
    m_taskPriorityInput->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *priorityRow = new QHBoxLayout();
    priorityRow->setContentsMargins(0, 0, 0, 0);
    priorityRow->addWidget(m_taskPriorityInput.data());
    priorityRow->addStretch(); // ЗАЩИТА: пружина оставляет поле коротким
    m_addTaskForm->addRow("Приоритет:", priorityRow);

    m_rootLayout->addLayout(m_addTaskForm.data());

    // ============================================================
    //  ПАНЕЛЬ ЗАДАЧ (ЛР2 + Адаптивность ЛР4)
    // ============================================================
    m_taskListPanel = new LogPanel("Панель задач", this);
    // Expanding по обеим осям — панель забирает всё свободное место.
    m_taskListPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_rootLayout->addWidget(m_taskListPanel.data(), 1); // stretch = 1

    m_panelLayout = new QVBoxLayout(m_taskListPanel.data());
    m_panelLayout->setContentsMargins(8, 8, 8, 8);

    LogButton *staticTask = new LogButton("Задача 1 (статическая)", m_taskListPanel.data());
    m_panelLayout->addWidget(staticTask);

    m_movableBtn = new LogButton("Задача 2 (перемещаемая)", m_taskListPanel.data());
    m_panelLayout->addWidget(m_movableBtn.data());
    m_panelLayout->addStretch(); // Задачи прижаты к верху

    // ============================================================
    //  НОВОЕ (ЛР4): НИЖНЯЯ ПАНЕЛЬ КНОПОК
    // ============================================================
    auto *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 0, 0, 0);

    m_addBtn      = new LogButton("Добавить", this);
    m_deleteBtn   = new LogButton("Удалить", this);
    m_saveBtn     = new LogButton("Сохранить", this);
    m_settingsBtn = new LogButton("Настройки", this);

    //  Кнопки фиксированы, чтобы не растягивались на пол-экрана.
    m_addBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_deleteBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_saveBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_settingsBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    btnRow->addWidget(m_addBtn.data());
    btnRow->addWidget(m_deleteBtn.data());
    btnRow->addWidget(m_saveBtn.data());
    //  Пружина раздвигает кнопки: левые — влево, "Настройки" — вправо.
    btnRow->addStretch();
    btnRow->addWidget(m_settingsBtn.data());
    m_rootLayout->addLayout(btnRow);

    // ============================================================
    //  НОВОЕ (ЛР4): ПАНЕЛЬ РЕЖИМОВ
    // ============================================================
    auto *modeRow = new QHBoxLayout();
    modeRow->setContentsMargins(0, 0, 0, 0);
    m_compactBtn = new LogButton("Компактный", this);
    m_comfortBtn = new LogButton("Удобный", this);
    m_dumpBtn    = new LogButton("Дамп политик", this);

    modeRow->addWidget(m_compactBtn.data());
    modeRow->addWidget(m_comfortBtn.data());
    modeRow->addStretch();
    modeRow->addWidget(m_dumpBtn.data());
    m_rootLayout->addLayout(modeRow);

    m_status = new LogLabel("Статус: окно создано", this);
    m_status->setWordWrap(true);
    m_rootLayout->addWidget(m_status.data());

    // ============================================================
    //  ПОДКЛЮЧЕНИЕ СИГНАЛОВ
    // ============================================================
    // ЛР2: Перемещение и удаление
    connect(m_moveButton.data(), &QPushButton::clicked, this, &MainWindow::moveMovableWidget);
    connect(m_deleteButton.data(), &QPushButton::clicked, this, &MainWindow::deletePanel);

    // ЛР4: Кнопки нижней панели
    connect(m_addBtn.data(), &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(m_deleteBtn.data(), &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(m_saveBtn.data(), &QPushButton::clicked, this, &MainWindow::onSave);
    connect(m_settingsBtn.data(), &QPushButton::clicked, this, &MainWindow::onSettings);

    // ЛР4: Режимы
    connect(m_compactBtn.data(), &QPushButton::clicked, this, &MainWindow::setCompactMode);
    connect(m_comfortBtn.data(), &QPushButton::clicked, this, &MainWindow::setComfortableMode);
    connect(m_dumpBtn.data(), &QPushButton::clicked, this, &MainWindow::dumpSizePolicies);

    // ============================================================
    // НОВОЕ (ЛР3): ЛОГИРОВАНИЕ ДЕЙСТВИЙ ПОЛЬЗОВАТЕЛЯ
    // ============================================================
    //  это ЛЯМБДА-СЛОТЫ. Ключевые моменты:
    //  1. Захват [this] — лямбда получает доступ к методам MainWindow.
    //  2. Третий аргумент connect() — контекст (this). Защита от use-after-free.
    //  3. Антипаттерн [&] здесь был бы ОПАСЕН (локальные переменные мертвы).
    connect(m_addBtn.data(), &QPushButton::clicked, this, [this]() {
        logUserAction("Пользователь нажал «Добавить»");
    });
    connect(m_deleteBtn.data(), &QPushButton::clicked, this, [this]() {
        logUserAction("Пользователь нажал «Удалить»");
    });

    // Список для дампа (ЛР4)
    m_trackedFields = {
        m_taskNameInput.data(), m_taskPriorityInput.data(),
        m_taskListPanel.data(), m_addBtn.data(), m_deleteBtn.data()
    };

    setStatus("Готово. Все объекты имеют родителей.");
}

// ============================================================
//  НОВОЕ (ЛР4): СЛОТЫ РЕЖИМОВ И ДАМПА
// ============================================================
void MainWindow::setCompactMode()
{
    m_rootLayout->setContentsMargins(4, 4, 4, 4);
    m_rootLayout->setSpacing(4);
    m_rootLayout->invalidate();
    setStatus("Компактный режим.");
}

void MainWindow::setComfortableMode()
{
    m_rootLayout->setContentsMargins(16, 16, 16, 16);
    m_rootLayout->setSpacing(10);
    m_rootLayout->invalidate();
    setStatus("Удобный режим.");
}

void MainWindow::dumpSizePolicies()
{
    qDebug() << "===== DUMP SIZE POLICIES =====";
    for (const QWidget *w : std::as_const(m_trackedFields)) {
        if (!w) continue;
        qDebug() << w->metaObject()->className()
                 << " H=" << int(w->sizePolicy().horizontalPolicy())
                 << " V=" << int(w->sizePolicy().verticalPolicy());
    }
}

// ============================================================
//  НОВОЕ (ЛР4): СЛОТЫ КНОПОК НИЖНЕЙ ПАНЕЛИ
// ============================================================
void MainWindow::onAddTask()
{
    if (m_taskNameInput->text().isEmpty()) {
        setStatus("Ошибка: введите название задачи!");
        return;
    }
    qDebug() << "[ADD] Задача:" << m_taskNameInput->text()
             << " Приоритет:" << m_taskPriorityInput->text();
    setStatus("Задача добавлена (симуляция).");
    m_taskNameInput->clear();
}

void MainWindow::onDeleteTask()
{
    qDebug() << "[DELETE] Удаление выбранной задачи (симуляция).";
    setStatus("Задача удалена (симуляция).");
}

void MainWindow::onSave()
{
    qDebug() << "[SAVE] Ручное сохранение данных...";
    setStatus("Данные сохранены.");
}

void MainWindow::onSettings()
{
    qDebug() << "[SETTINGS] Открытие настроек (симуляция).";
    setStatus("Открытие настроек...");
}

// ============================================================
// НОВОЕ (ЛР3): НАСТРОЙКА АВТОСОХРАНЕНИЯ
// ============================================================
void MainWindow::setupAutoSave()
{
    // Интервал — 60 секунд (60000 мс).
    // ЗАЩИТА: interval() задаётся в МИЛЛИСЕКУНДАХ. 60 * 1000 = 60000.
    m_autoSaveTimer.setInterval(60000);

    // Подключаем сигнал timeout() к лямбда-слоту.
    // ЗАЩИТА: Без app.exec() таймер НИКОГДА не сработает.
    connect(&m_autoSaveTimer, &QTimer::timeout, this, [this]() {
        const QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        // Логируем «сохранение» в консоль.
        qDebug() << "[AUTOSAVE]" << timestamp << "Автосохранение данных...";
        setStatus(QString("Автосохранение выполнено в %1").arg(timestamp));
    });

    m_autoSaveTimer.start();
    qDebug() << "[AUTOSAVE] Таймер автосохранения запущен, интервал 60 с.";
}

// ============================================================
// НОВОЕ (ЛР3): ЛОГИРОВАНИЕ ДЕЙСТВИЙ
// ============================================================
void MainWindow::logUserAction(const QString &action)
{
    const QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug() << "[USER ACTION]" << timestamp << action;
}

// ============================================================
//  ДАЛЬШЕ — КОД ИЗ ЛР2 (без изменений)
// ============================================================
void MainWindow::setStatus(const QString &text)
{
    if (!m_status.isNull()) m_status->setText(text);
}

void MainWindow::moveMovableWidget()
{
    if (m_movableBtn.isNull() || m_taskListPanel.isNull()) return;

    if (m_movableBtn->parentWidget() == m_taskListPanel.data()) {
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
    m_taskListPanel->deleteLater();
}

void MainWindow::demoDetaching()
{
    LogPanel *tempPanel = new LogPanel("Временная панель", this);
    LogLabel *taskA = new LogLabel("Задача А (остается)", tempPanel);
    LogLabel *taskB = new LogLabel("Задача Б (отвязывается)", tempPanel);

    qDebug() << "DEMO: Задач до detach:" << tempPanel->children().size();
    taskB->setParent(nullptr);
    qDebug() << "DEMO: Задач после detach:" << tempPanel->children().size();

    tempPanel->deleteLater();
    taskB->deleteLater();

    Q_UNUSED(taskA);
    setStatus("Создана временная панель. Задача Б отвязана и удалена вручную.");
}