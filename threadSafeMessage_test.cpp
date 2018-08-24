#include "threadSafeMessage.hpp"
#include "catch.hpp"

TEST_CASE("test threadedSafeMessage queue", "[threadedSafeMessageQueue]")
{
	{
		ThreadSafeMessageQueue<int> myQueue;

		myQueue.push(1);
		myQueue.push(2);

		int temp;
		bool didPop = myQueue.try_pop(temp);
		REQUIRE(didPop == true);
		REQUIRE(temp == 1);

		int temp2;
		myQueue.wait_and_pop(temp2);
		REQUIRE(temp2 == 2);
		REQUIRE(myQueue.empty() == true);
	}

}

