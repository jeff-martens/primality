#include <iostream>
#include <string>
#include <list>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <netinet/in.h>

#include "primer.hpp"

using namespace CppUnit;
using namespace std;

//-----------------------------------------------------------------------------

class TestPrimer : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestPrimer);
    CPPUNIT_TEST(TestLowerBounds);
    CPPUNIT_TEST(TestUpperBounds);
    CPPUNIT_TEST(TestParallel);
    CPPUNIT_TEST(TestPrimeCount);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void TestLowerBounds(void);
    void TestUpperBounds(void);
    void TestParallel(void);
    void TestPrimeCount(void);

private:
    Primer *m_uut;	// Unit under test
};

//-----------------------------------------------------------------------------

void TestPrimer::setUp(void)
{
    m_uut = new Primer();
}

void TestPrimer::tearDown(void)
{
    delete m_uut;
}

//-----------------------------------------------------------------------------
void
TestPrimer::TestLowerBounds(void)
{
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(0, 0, 1));
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(1, 0, 1));
    CPPUNIT_ASSERT(true == m_uut->CheckSlice(2, 0, 1));
    CPPUNIT_ASSERT(true == m_uut->CheckSlice(3, 0, 1));
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(4, 0, 1));
    CPPUNIT_ASSERT(true == m_uut->CheckSlice(5, 0, 1));
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(6, 0, 1));
    CPPUNIT_ASSERT(true == m_uut->CheckSlice(7, 0, 1));
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(8, 0, 1));
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(9, 0, 1));
    CPPUNIT_ASSERT(false == m_uut->CheckSlice(10, 0, 1));
    CPPUNIT_ASSERT(true == m_uut->CheckSlice(11, 0, 1));
}

void
TestPrimer::TestUpperBounds(void)
{
	const uint64_t biggest_prime = UINT64_MAX - 58;

	CPPUNIT_ASSERT(false == m_uut->CheckSlice(UINT64_MAX, 0, 1));
	CPPUNIT_ASSERT(false == m_uut->CheckSlice(biggest_prime - 1, 0, 1));
	CPPUNIT_ASSERT(true == m_uut->CheckSlice(biggest_prime, 0, 1));

	for (auto i = biggest_prime + 1; i < UINT64_MAX; ++i)
		CPPUNIT_ASSERT(false == m_uut->CheckSlice(i, 0, 1));
}

void
TestPrimer::TestParallel(void)
{
	// Test 2 slices
	for (auto i = 0; i < 100000; ++i )
		CPPUNIT_ASSERT(m_uut->CheckSlice(i, 0, 1) == (m_uut->CheckSlice(i, 0, 2) &&  m_uut->CheckSlice(i, 1, 2)));
	for (auto i = UINT64_MAX; i > UINT64_MAX - 100; --i )
		CPPUNIT_ASSERT(m_uut->CheckSlice(i, 0, 1) == (m_uut->CheckSlice(i, 0, 2) &&  m_uut->CheckSlice(i, 1, 2)));

	// Test 4 slices
	for (auto i = 0; i < 100000; ++i )
	{
		CPPUNIT_ASSERT(m_uut->CheckSlice(i, 0, 1)
				== (m_uut->CheckSlice(i, 0, 4) &&  m_uut->CheckSlice(i, 1, 4) && m_uut->CheckSlice(i, 2, 4) &&  m_uut->CheckSlice(i, 3, 4)));
	}
	for (auto i = UINT64_MAX; i > UINT64_MAX - 100; --i )
	{
		CPPUNIT_ASSERT(m_uut->CheckSlice(i, 0, 1)
				== (m_uut->CheckSlice(i, 0, 4) &&  m_uut->CheckSlice(i, 1, 4) && m_uut->CheckSlice(i, 2, 4) &&  m_uut->CheckSlice(i, 3, 4)));
	}
}

void
TestPrimer::TestPrimeCount(void)
{
	int count = 0;
	for (auto i = 0; i < 1000000; ++i)
	{
		if (m_uut->CheckSlice(i, 0, 1))
			++count;
	}
	CPPUNIT_ASSERT(count == 78498);
}

//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( TestPrimer );

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();

    // Output XML for Jenkins CPPunit plugin
    ofstream xmlFileOut("cppTestPrimer.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
