#ifndef SIMPLECRYPTO_H
#define SIMPLECRYPTO_H

//QT штуки
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QChar>
#include <QGridLayout>
#include <QMap>
#include <QList>
#include <QPushButton>
#include <QDebug>
#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
//STD штуки
#include <random>
#include <chrono>

using std::shuffle;
class CSimpleCrypto;
class CKeyEditor;

class CKeyEditor : public QDialog {
    Q_OBJECT

    private:
        QString *alphabet;
        QMap<QChar, QChar> *key;
        QLineEdit *leStringKeyInput;
        QPushButton *pbDone, *pbApplyStringKey,
                    *pbKeyRegen, *pbKeyReset;
        QTableWidget *table;
        QString *oldItemValue;
        unsigned rndSeed;
    public:
        explicit CKeyEditor(CSimpleCrypto *parent = nullptr);
        ~CKeyEditor();
        void show();
        void update();
        void resetKey();
        void regenKey();
        void setKey(QMap<QChar, QChar>);
        void setStringifyKey(QString);
        QMap<QChar, QChar> getKey();
        QString getStringifyKey();
        void updateTableValues();
        void updateStringifyKey();
    protected:
        void initWidgetGUI();
        void initWidgetLogic();
        void initWidgetBehavior();

        QTableWidgetItem *getDisabledTableItem(QString);
        void generateTableTemplate();
        void applyChangedItem(QTableWidgetItem *);
        void insertItemKeyValue(QTableWidgetItem *);
        void setTableItemChangedSignalEnable(bool);
        void generateAlphabet();

    friend CSimpleCrypto;
};

class CSimpleCrypto : public QWidget {
    Q_OBJECT

    private:
        CKeyEditor *keyEditor; //friend
        QLineEdit *leInput, *leOutput, *leKey;
        QPushButton *pbEncrypt, *pbDecrypt, *pbKeyEdit;

    public:
        explicit CSimpleCrypto(QWidget *parent = nullptr);
        ~CSimpleCrypto();
        QString encrypt(QString, bool decrypt = false);
        QString decrypt(QString);
        //Описаны как анонимные функции
//        void internalEncrypt();
//        void internalDecrypt();
    protected:
        void initWidgetGUI();
        void initWidgetLogic();
        void initWidgetBehavior();
};

#endif // SIMPLECRYPTO_H
