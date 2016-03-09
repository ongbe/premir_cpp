#include "HashMapArrayTest.h"

namespace test {
    namespace utils_hashMapArray {

        void test_push_value() {
            utils::hash::CharHash shash;
            utils::HashMapArray<char, int> a(&shash, 6, 10);

            a.pushValue(0, 'K', 4);
            assert(a.getValue(0, 'K') != nullptr);
            assert((*a.getValue(0, 'K')) == 4);

            a.pushValue(35, 'A', 7);
            assert(a.getValue(35, 'K') == nullptr);
            assert((*a.getValue(35, 'A')) == 7);

            a.pushValue(0, 'K', 5);
            assert((*a.getValue(0, 'K')) == 5);

            a.pushValue(0, 'P', 55);
            assert((*a.getValue(0, 'K')) == 5);
            assert((*a.getValue(0, 'P')) == 55);
            assert(a.getValue(0, 'M') == nullptr);

            utils::hash::StringHash<char*> strHash;
            utils::HashMapArray<char*, int> b(&strHash, 6, 10);

            char mystr[] = "my string";
            b.pushValue(0, mystr, 4);
            assert(b.getValue(0, mystr) != nullptr);
            assert((*b.getValue(0, mystr)) == 4);

            b.pushValue(0, mystr, 42);
            assert((*b.getValue(0, mystr)) == 42);
        }

        void test_push_or_update_value() {
            utils::hash::StringHash<char*> strHash;
            utils::HashMapArray<char*, MyValue> b(&strHash, 6, 10);
            MyValueUpdater updater;
            MyValue v1 = {1, 11};
            char mykey[] = "my key1";
            b.pushValue(0, mykey, v1, &updater);
            MyValue v2 = {1, 22};
            b.pushValue(0, mykey, v2, &updater);
            MyValue v3 = {1, 33};
            b.pushValue(0, mykey, v3, &updater);
            assert((*b.getValue(0, mykey)).count == 3);
            assert((*b.getValue(0, mykey)).value2 == 11);

            char mykey2[] = "my key2";
            b.pushValue(0, mykey2, v1, &updater);
            assert((*b.getValue(0, mykey)).count == 3);
            assert((*b.getValue(0, mykey2)).count == 1);
        }

        void hashMapArray_test() {
            suite("HashMapArray");
            mytest(push_value);
            mytest(push_or_update_value);
        }
    }
}


