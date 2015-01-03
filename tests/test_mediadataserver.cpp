
#include<QFileInfo>
#include "test_mediadataserver.h"
#include "mockclient.h"

#include <QDebug>

void Test_MediaDataServer::test_library_date_reqest()
{
    MediaDataServer server(50003);
    MockClient client(50003);

    QVERIFY(client.waitForConnected(1000));
    char req[4] = { 0x2, 0x0, 0x1, 0x0 };
    QCOMPARE((int)client.write(req, 4), 4);
    QVERIFY(client.waitForBytesWritten(1000));

    char respBuf[8];
    memset(respBuf, '\0', 8);
    char respSizeBuf[2];
    QTest::qWait(500);
    qint64 readAmt = client.read(respSizeBuf, 2);
    QCOMPARE(readAmt, (qint64)2);
    QTest::qWait(500);
    quint16 respSize = 0;
    memcpy(&respSize, respSizeBuf, 2);
    QCOMPARE(respSize, (quint16)8);

    client.read(respBuf, 8);
    byteSwap<char>(respBuf, 8);
    qint64 resp = 0;
    memcpy(&resp, respBuf, 8);
    QFileInfo library("md_library.db");
    qint64 localMsecs = library.lastModified().toMSecsSinceEpoch();
    QCOMPARE(localMsecs, resp);
}

template <class T>
void byteSwap(T *data, const unsigned int len)
{
    T temp;
    for (unsigned int i = 0; i < len/2; i++) {
        temp = data[i];
        data[i] = data[len-1-i];
        data[len-1-i] = temp;
    }
}
