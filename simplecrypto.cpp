#include "simplecrypto.h"

CKeyEditor::CKeyEditor(CSimpleCrypto *parent) : QDialog(parent){
    this->key = new QMap<QChar, QChar>();
    this->table = new QTableWidget(1, 2);
    this->oldItemValue = new QString();
    this->leStringKeyInput = new QLineEdit();
    this->pbKeyRegen = new QPushButton();
    this->pbKeyReset = new QPushButton();
    this->pbApplyStringKey = new QPushButton();
    this->pbDone = new QPushButton();
    this->alphabet = new QString();
    //Семя генерируется согласно расчету времени от начала эпохи
    this->rndSeed = static_cast<unsigned>(std::chrono::system_clock::now()
                                          .time_since_epoch().count());

    this->initWidgetBehavior();
    this->initWidgetLogic();
    this->initWidgetGUI();
}
CKeyEditor::~CKeyEditor(){
    delete this->key;
    delete this->alphabet;
    delete this->table;
    delete this->oldItemValue;
    delete this->leStringKeyInput;
    delete this->pbKeyRegen;
    delete this->pbKeyReset;
    delete this->pbApplyStringKey;
    delete this->pbDone;
}
void CKeyEditor::initWidgetGUI(){
    this->resize(500, 500);
    this->table->verticalHeader()->setVisible(false);
    this->table->horizontalHeader()->setVisible(false);

    this->setWindowTitle("Редактор ключа");
    this->pbDone->setText("Закрыть");
    this->pbApplyStringKey->setText("Применить ключ");
    this->pbKeyRegen->setText("Генерировать");
    this->pbKeyReset->setText("Сброс");

    QVBoxLayout *editorLayout = new QVBoxLayout(this);
    QHBoxLayout *keymanipLayout = new QHBoxLayout();
    keymanipLayout->addWidget(this->pbKeyRegen);
    keymanipLayout->addWidget(this->pbKeyReset);
    editorLayout->addLayout(keymanipLayout);
    editorLayout->addSpacing(15);
    editorLayout->addWidget(this->leStringKeyInput);
    editorLayout->addWidget(this->pbApplyStringKey);
    editorLayout->addSpacing(15);
    editorLayout->addWidget(this->table);
    editorLayout->addWidget(this->pbDone);
}
void CKeyEditor::initWidgetLogic(){
    this->setModal(true);
    this->table->setSortingEnabled(false);

    this->generateAlphabet();
    this->resetKey();
    this->generateTableTemplate();
    //Первоначальная генерация случайного ключа
//    this->regenKey();
}
void CKeyEditor::initWidgetBehavior(){
    CKeyEditor::connect(this->pbDone, &QPushButton::clicked, this, &CKeyEditor::done);
    CKeyEditor::connect(this->pbApplyStringKey, &QPushButton::clicked, [&](){
        this->setStringifyKey(this->leStringKeyInput->text());
        this->updateTableValues();
    });
    CKeyEditor::connect(this->table, &QTableWidget::itemDoubleClicked, [&](QTableWidgetItem *item){
        *this->oldItemValue = item->text();
        qDebug() << "OldItemValue=" << *this->oldItemValue;
    });
    CKeyEditor::connect(this->pbKeyRegen, &QPushButton::clicked, [&](){
        this->regenKey();
        this->update();
    });
    CKeyEditor::connect(this->pbKeyReset, &QPushButton::clicked, [&](){
        this->resetKey();
        this->update();
    });
}

//PUBLIC
void CKeyEditor::show(){
    this->update();
    return QDialog::show();
}
void CKeyEditor::update(){
    qDebug() << "Обновление полей CKeyEditor";
    this->updateStringifyKey();
    this->updateTableValues();
}
void CKeyEditor::resetKey(){
    qDebug() << "Ключ сброшен.";
    for (QChar &ch : *this->alphabet){
        this->key->insert(ch, ch);
    }
}
void CKeyEditor::regenKey(){
    qDebug() << "Генерация ключа.";
    QList<QChar> tmpValues = this->key->values();
    QList<QChar> tmpKeys = this->key->keys();
    //В Qt есть все, кроме того, что нужно
    //Shuffle посредством std::shuffle
    shuffle(tmpValues.begin(), tmpValues.end(), std::default_random_engine(this->rndSeed));

    QString dbgNewKey;
    for (int i = 0; i < tmpKeys.length(); i++) {
        dbgNewKey.append(tmpValues[i]);
        this->key->operator[](tmpKeys[i]) = tmpValues[i];
//        this->key->insert(tmpKeys[i], tmpValues[i]);
    }
    qDebug() << "Ключ: " << dbgNewKey;
}
void CKeyEditor::setKey(QMap<QChar, QChar> inputKey){
    *this->key = inputKey;
}
void CKeyEditor::setStringifyKey(QString stringifyKey){
    QMap<QChar, QChar>::iterator itPair;
    QString::iterator strKeyChar = stringifyKey.begin();
    for (itPair = this->key->begin();
            (itPair != this->key->end() && strKeyChar != stringifyKey.end());
            itPair++, strKeyChar++
        ){
        itPair.value() = *strKeyChar;
    }
}
QMap<QChar, QChar> CKeyEditor::getKey(){
    return *this->key;
}
QString CKeyEditor::getStringifyKey(){
    QString stringifyKey("");
    QMap<QChar, QChar>::iterator itPair;
    for (itPair = this->key->begin(); itPair != this->key->end(); itPair++){
        stringifyKey.append(itPair.value());
    }

    return stringifyKey;
}
void CKeyEditor::updateTableValues(){
    this->setTableItemChangedSignalEnable(false);
    QList<QChar> tmpValues = this->key->values();

    int columns = this->table->columnCount();
    for (int i = 0, k = 1; i < tmpValues.length(); i++) {
        for (int j = 0; j < columns; j++) {
            if (i+j >= tmpValues.length()){
                this->table->setItem(k, j, this->getDisabledTableItem(""));
                continue;
            }
            QTableWidgetItem *editableItem = new QTableWidgetItem(tmpValues[i+j]);
            editableItem->setTextAlignment(Qt::AlignCenter);
            this->table->setItem(k, j, editableItem);
        }
        i+= columns-1;
        k+= 2;
    }
    this->setTableItemChangedSignalEnable(true);
}
void CKeyEditor::updateStringifyKey(){
    this->leStringKeyInput->setText(this->getStringifyKey());
}
//PROTECTED
QTableWidgetItem *CKeyEditor::getDisabledTableItem(QString data){
    QTableWidgetItem *disabledItem = new QTableWidgetItem(data);
    disabledItem->setFlags(Qt::ItemIsEnabled);
    disabledItem->setTextAlignment(Qt::AlignCenter);
    return disabledItem;
}
void CKeyEditor::generateTableTemplate(){
    qDebug() << "Генерация таблицы";
    QList<QChar> tmpKeys = this->key->keys();
    int columns = 6;
    this->table->setColumnCount(columns);
    int rows = static_cast<int>(ceil(tmpKeys.length()/static_cast<double>(columns)));
    this->table->setRowCount(rows*2);
    qDebug() << "tmpKeys.length() = " << tmpKeys.length();

    QFont font;
    font.setBold(true);
    for (int i = 0, k = 0; i < tmpKeys.length(); i++) {
        for (int j = 0; j < columns; j++) {
            if (i+j >= tmpKeys.length()){
                this->table->setItem(k, j, this->getDisabledTableItem(""));
                continue;
            }
            QTableWidgetItem *headItem = this->getDisabledTableItem(tmpKeys[i+j]);
//            headItem->setBackgroundColor(QColor("lightGray"));
            headItem->setBackground(QBrush(QColor("black")));
            headItem->setFont(font);

            this->table->setItem(k, j, headItem);
        }
        i+= columns-1;
        k+= 2;
    }
}
void CKeyEditor::applyChangedItem(QTableWidgetItem *item){
    this->setTableItemChangedSignalEnable(false);
    item->setText(QString(item->text()[0]));
    qDebug() << "NewItemValue=" << item->text();
    this->insertItemKeyValue(item);

    //Стоит ли заменять значение в ячейки в которой такое же как тут?
    QList<QTableWidgetItem *> foundBrother = this->table->findItems(item->text(), Qt::MatchExactly);
    for (QTableWidgetItem *tmpItem : foundBrother){
        if (tmpItem != item && (tmpItem->row()%2 != 0)){
            tmpItem->setText(QString(oldItemValue->operator[](0)));
            this->insertItemKeyValue(tmpItem);
        }
    }
    this->updateStringifyKey();
    this->setTableItemChangedSignalEnable(true);
}
void CKeyEditor::insertItemKeyValue(QTableWidgetItem *item){
    QTableWidgetItem *headItem = this->table->item(item->row()-1, item->column());
    this->key->operator[](headItem->text()[0]) = item->text()[0];
}
void CKeyEditor::setTableItemChangedSignalEnable(bool flag){
    if(flag){
        CKeyEditor::connect(this->table, &QTableWidget::itemChanged, this, &CKeyEditor::applyChangedItem);
        return;
    }
    CKeyEditor::disconnect(this->table, &QTableWidget::itemChanged, this, &CKeyEditor::applyChangedItem);
}
void CKeyEditor::generateAlphabet(){
    //Было проблемно найти нужные коды unicode
    //Обошелся урезанным алфавитом
    *this->alphabet = "1234567890-= "\
                      "~`!@#$^&*()_+№"\
                      "QWERTYUIOP{}ASDFGHJKL:\"|ZXCVBNM<>?"\
                      "qwertyuiop[]asdfghjkl;'\\zxcvbnm,./"\
                      "ЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮЁ"\
                      "йцукенгшщзхъфывапролджэячсмитьбюё";
}
//void CKeyEditor::done(int result){
//    if (result){
//        this->saveKey();
//    }
//    return QDialog::done(result);
//}





CSimpleCrypto::CSimpleCrypto(QWidget *parent) : QWidget(parent){
//Возникли трудности при конвертации uchar в QChar из-за unicode
//    for (int i = 32; i <= 255; i++){
//        this->alphabet.append(QChar(static_cast<unsigned char>(i)));
//    }
    this->keyEditor = new CKeyEditor();
    this->pbEncrypt = new QPushButton();
    this->pbDecrypt = new QPushButton();
    this->pbKeyEdit = new QPushButton();
    this->leOutput = new QLineEdit();
    this->leInput = new QLineEdit();

    this->initWidgetBehavior();
    this->initWidgetLogic();
    this->initWidgetGUI();
}
CSimpleCrypto::~CSimpleCrypto(){
    delete this->keyEditor;
    delete this->leInput;
    delete this->leOutput;
    delete this->pbEncrypt;
    delete this->pbDecrypt;
    delete this->pbKeyEdit;
}
void CSimpleCrypto::initWidgetGUI(){
    this->setWindowTitle("Шифр простой замены");
    this->pbEncrypt->setText("Зашифровать");
    this->pbDecrypt->setText("Расшифровать");
    this->pbKeyEdit->setText("Изменить");

    QGridLayout *gridMain = new QGridLayout(this);

    QLabel *lblInput = new QLabel("Откр. текст:");
    gridMain->addWidget(lblInput, 1, 1);
    gridMain->addWidget(this->leInput, 1, 2);
    gridMain->addWidget(this->pbEncrypt, 1, 3, 1, 2);

    QLabel *lblKey = new QLabel("Ключ:");
    gridMain->addWidget(lblKey, 2, 1);
    gridMain->addWidget(pbKeyEdit, 2, 2, 1, 3);

    QLabel *lblOutput = new QLabel("Шифротекст:");
    gridMain->addWidget(lblOutput, 3, 1);
    gridMain->addWidget(this->leOutput, 3, 2);
    gridMain->addWidget(this->pbDecrypt, 3, 3, 1, 2);
}
void CSimpleCrypto::initWidgetLogic(){
    //?
}
void CSimpleCrypto::initWidgetBehavior(){
    CSimpleCrypto::connect(this->pbEncrypt, &QPushButton::clicked, [&]() {
        QString tmpText = this->leInput->text();
        if (!tmpText.isEmpty()){
            this->leOutput->setText(this->encrypt(tmpText));
            qDebug() << "Зашифр.:" << tmpText << "->" << this->leOutput->text();
        }
    });
    CSimpleCrypto::connect(this->pbDecrypt, &QPushButton::clicked, [&]() {
        QString tmpText = this->leOutput->text();
        if (!tmpText.isEmpty()){
            this->leInput->setText(this->decrypt(tmpText));
            qDebug() << "Расшифр.:" << tmpText << "->" << this->leInput->text();
        }
    });
    CSimpleCrypto::connect(this->pbKeyEdit, &QPushButton::clicked, [&]() {
        this->keyEditor->show();
    });
}

//PUBLIC
QString CSimpleCrypto::encrypt(QString inputString, bool decrypt){
    QString outputString{};
    for (QChar &ch : inputString) {
        if (!decrypt){
            ch = this->keyEditor->key->operator[](ch);
        }
        else {
            //Ключ ключа)
            ch = this->keyEditor->key->key(ch);
        }
        outputString.append(ch);
    }
    return outputString;
}
QString CSimpleCrypto::decrypt(QString outputString){
    return encrypt(outputString, true);
}
