#include "id_generator.h"

#include "gtest/gtest.h"

namespace whisper {
namespace testing {

TEST(TimestampIDGeneratorTest, UniqueIDTest) {
  int64_t id1 = TimestampIDGenerator::GenerateId();
  int64_t id2 = TimestampIDGenerator::GenerateId();
  EXPECT_NE(id1, id2);
}

}  // namespace testing
}  // namespace whisper

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}