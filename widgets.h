#ifndef WIDGETS_H
#define WIDGETS_H

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

// 1. Метка (для заголовков и статуса)
class LogLabel : public QLabel {
public:
    explicit LogLabel(const QString &text, QWidget *parent = nullptr)
        : QLabel(text, parent)
    {
        qDebug() << "[CREATE] LogLabel" << text;
    }
    ~LogLabel() override {
        qDebug() << "[DESTROY] LogLabel" << text();
    }
};

// 2. Поле ввода
class LogLineEdit : public QLineEdit {
public:
    explicit LogLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
        qDebug() << "[CREATE] LogLineEdit";
    }
    ~LogLineEdit() override {
        qDebug() << "[DESTROY] LogLineEdit";
    }
};

// 3. Кнопка
class LogButton : public QPushButton {
public:
    explicit LogButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
        qDebug() << "[CREATE] LogButton" << text;
    }
    ~LogButton() override {
        qDebug() << "[DESTROY] LogButton" << text();
    }
};

// 4. Панель (наша "Панель задач")
class LogPanel : public QFrame {
public:
    explicit LogPanel(const QString &name, QWidget *parent = nullptr)
        : QFrame(parent)
    {
        setObjectName(name);
        setFrameShape(QFrame::StyledPanel); // Рамка для красоты
        qDebug() << "[CREATE] LogPanel" << name;
    }
    ~LogPanel() override {
        qDebug() << "[DESTROY] LogPanel" << objectName();
    }
};

#endif // WIDGETS_H