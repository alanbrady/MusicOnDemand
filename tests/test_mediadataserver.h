#ifndef TEST_MEDIADATASERVER
#define TEST_MEDIADATASERVER

#include <mediadataserver.h>
#include <QtTest/QtTest>


class Test_MediaDataServer : public QObject {
    Q_OBJECT
private slots:
    void test_library_date_reqest();
    void test_library_file_request();

};

template <class T>
void byteSwap(T* data, const unsigned int len);

#endif // TEST_MEDIADATASERVER

