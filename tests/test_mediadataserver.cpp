#include <mediadataserver.h>

#include<QFileInfo>
#include "test_mediadataserver.h"
#include "mockclient.h"

void Test_MediaDataServer::test_library_date_reqest()
{
    MediaDataServer server(49999);
    MockClient client(49999);

    QVERIFY(client.waitForConnected(1000));
    char req[2] = { 0x0, 0x1 };
    client.write(req, 2);
    QVERIFY(client.waitForBytesWritten(1000));

    char respBuf[8];
    memset(respBuf, '\0', 8);
    char respSizeBuf[2];
    QTest::qWait(500);
//    QVERIFY(client.waitForReadyRead(1000));
    client.read(respSizeBuf, 2);
    quint16 respSize = 0;
    memcpy(&respSize, respSizeBuf, 2);
    QCOMPARE((int)respSize, 8);

    client.read(respBuf, 8);
    qint64 resp = 0;
    memcpy(&resp, respBuf, 8);
    QFileInfo library("md_library.db");
    QCOMPARE(library.lastModified().toMSecsSinceEpoch(), resp);
}
