#include "../Headers/Test_StringImplement.hpp"

#ifdef __linux__
TEST(Commons, StringImplement_obtainFirstNotSpaceCharLocation) {
    const unsigned char* string = (const unsigned char*)" 12   ";
    int index = Commons::StringImplement::obtainFirstNotSpaceCharLocation(string);
    ASSERT_EQ(1, index);

    index = Commons::StringImplement::obtainFirstNotSpaceCharLocation(string, 0x1);
    ASSERT_EQ(2, index);

    const unsigned char* string2 = (const unsigned char*)"     ";
    index = Commons::StringImplement::obtainFirstNotSpaceCharLocation(string2);
    ASSERT_EQ(-1, index);

    const unsigned char* string3 = nullptr;
    index = Commons::StringImplement::obtainFirstNotSpaceCharLocation(string3);
    ASSERT_EQ(-2, index);
}

TEST(Commons, StringImplement_trimLeftSpace) {
    unsigned char testString[] = " 12   ";
    int originalLength = (int)strlen((const char*)testString);
    unsigned char* ptr = testString;
    unsigned char** string = &ptr;
    int length = Commons::StringImplement::trimLeftSpace(string);
    ASSERT_EQ(originalLength - 1, length);
    ASSERT_EQ(originalLength - 1, (int)(strlen((const char*)(*string))));

    unsigned char testString2[] = "12345";
    originalLength = (int)strlen((const char*)testString2);
    ptr = testString2;
    string = &ptr;
    length = Commons::StringImplement::trimLeftSpace(string);
    ASSERT_EQ(originalLength, length);
    ASSERT_EQ(originalLength, (int)(strlen((const char*)(*string))));

    ptr = nullptr;
    string = &ptr;
    length = Commons::StringImplement::trimLeftSpace(string);
    ASSERT_EQ(-1, length);
}

TEST(Commons, StringImplement_trimRightSpace) {
    unsigned char testString[] = "12   ";
    int originalLength = (int)strlen((const char*)testString);
    unsigned char* ptr = testString;
    unsigned char** string = &ptr;
    int length = Commons::StringImplement::trimRightSpace(string);
    ASSERT_EQ(originalLength - 3, length);
    ASSERT_EQ(originalLength - 3, (int)(strlen((const char*)*string)));

    unsigned char testString2[] = "12345";
    originalLength = (int)strlen((const char*)testString2);
    ptr = testString2;
    string = &ptr;
    length = Commons::StringImplement::trimRightSpace(string);
    ASSERT_EQ(originalLength, length);
    ASSERT_EQ(originalLength, (int)(strlen((const char*)(*string))));

    ptr = nullptr;
    string = &ptr;
    length = Commons::StringImplement::trimRightSpace(string);
    ASSERT_EQ(-1, length);
}

TEST(Commons, StringImplement_mergeRedundantSpace) {
    unsigned char testString[] = "12  45 3";
    int originalLength = (int)strlen((const char*)testString);
    unsigned char* ptr = testString;
    unsigned char** string = &ptr;
    int length = Commons::StringImplement::mergeRedundantSpace(string);
    ASSERT_EQ(originalLength - 1, length);
    ASSERT_EQ(originalLength - 1, (int)(strlen((const char*)(*string))));
}
#endif