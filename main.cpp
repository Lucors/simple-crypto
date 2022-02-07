#include <QApplication>
#include "simplecrypto.h"

int main(int argc, char* argv[]){
   QApplication app(argc, argv);

   CSimpleCrypto *crypto = new CSimpleCrypto();
   crypto->show();

   return app.exec();
}
