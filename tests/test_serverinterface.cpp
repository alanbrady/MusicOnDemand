#include <QtTest/QtTest>

class Test_ServerInterface : public QObject
{
    Q_OBJECT
public:
    Test_ServerInterface() {}
    ~Test_ServerInterface() {}

private slots:
    void test_fail() { QVERIFY(true); }
//    void test_blank() { QCOMPARE("foo", "bar"); }
};

QTEST_MAIN(Test_ServerInterface)
#include "test_serverinterface.moc"
