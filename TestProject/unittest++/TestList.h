#ifndef UNITTEST___TESTLIST_H__
#define UNITTEST___TESTLIST_H__


namespace UnitTest {

class Test;

class TestList
{
public:
    TestList();
    void Add (Test* test);

    Test* GetHead() const;

private:
    Test* m_head;
    Test* m_tail;
};


class ListAdder
{
public:
    ListAdder(TestList& list, Test* test);
};

}


#endif
