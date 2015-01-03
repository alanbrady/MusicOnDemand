#ifndef TEST_MEDIADATASERVER
#define TEST_MEDIADATASERVER

#include <mediadataserver.h>
#include <QtTest/QtTest>


class Test_MediaDataServer : public QObject {
    Q_OBJECT
private slots:
    void test_library_date_reqest();
};

class MockMediaDataServer : public MediaDataServer {
public:
    MockMediaDataServer(quint16 port) : MediaDataServer(port) {}
};

template <class T>
void byteSwap(T* data, const unsigned int len);

#endif // TEST_MEDIADATASERVER

