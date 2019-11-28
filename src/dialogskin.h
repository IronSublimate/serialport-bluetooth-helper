#ifndef DIALOGSKIN_H
#define DIALOGSKIN_H

#include <QDialog>
#include <QString>
#include <QArrayData>

class QTranslator;

namespace Ui {
class DialogSkin;
}

class DialogSkin : public QDialog
{
    Q_OBJECT
public:
    struct Settings
    {
        int skinIndex = -1;
        int languageIndex = -1;
    };
public:
    explicit DialogSkin(QWidget *parent = nullptr);
    ~DialogSkin();

private:
    Ui::DialogSkin *ui;
    Settings settings;
    QTranslator* translator;
public:
    QString getSkinName() const;
    Settings getSettings() const;
protected:
    void accept();
    void changeEvent(QEvent* event);
private:
    void updateSettings();
    void updateTranslation();
};

#endif // DIALOGSKIN_H
