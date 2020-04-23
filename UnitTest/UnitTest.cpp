#include "CppUnitTest.h"
#include "../AimBot/Point.h"
#include "../AimBot/Point.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(TestMethod1)
		{
			Assert::AreEqual(Point{ 0, 100, 0 }.distance(Point{ 0, 2, 0 }), 98.0f, 0.001f);
		}
	};
}
