#ifndef DIALOGSKIN_H
#define DIALOGSKIN_H

#include <QDialog>
#include <QString>
namespace Ui {
class DialogSkin;
}

class DialogSkin : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSkin(QWidget *parent = nullptr);
    ~DialogSkin();

private:
    Ui::DialogSkin *ui;
public:
    QString getSkinName() const;
};

#endif // DIALOGSKIN_H
