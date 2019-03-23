#ifndef DMSTRSRV_H
#define DMSTRSRV_H

#include <Cutelyst/Application>

using namespace Cutelyst;

class dmstrsrv : public Application
{
    Q_OBJECT
    CUTELYST_APPLICATION(IID "dmstrsrv")
public:
    Q_INVOKABLE explicit dmstrsrv(QObject *parent = nullptr);
    ~dmstrsrv();

    bool init();
};

#endif //DMSTRSRV_H

