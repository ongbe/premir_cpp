#include "CHashSetTest.h"

namespace test {
    namespace utils_cHashSet {

        void test_hash_set() {


            ml::LevensteinDistance<int> ld;
            const wchar_t *str1 = L"моя руссская строка";
            const wchar_t *str2 = L"моя руrсская стро21";
            int d1 = ld.wld(str1, str2, wcslen(str1), wcslen(str2));
            int d2 = ld.wldOptimazed(str1, str2, wcslen(str1), wcslen(str2));
            std::cout << "d1: " << d1 << std::endl;
            std::cout << "d2: " << d2 << std::endl;


//            const wchar_t * str = L"моя руссская строка";
//            wprintf(L"%ls\n", str);
//            wprintf(L"Первая буква Большая!\n");
//            for (size_t i = 0; i<1000; ++i) {
////                wprintf(&str[i]);
////                wprintf(L"Привет\0\0");
////                wprintf (L"str[%d]: %lc\n", i, str[i]);
////                std::wcout << str[i] << std::endl;// <<std::wstring(L"Привет");
//                std::cout << "[" << i << "]: " << i % 64 << std::endl;
//            }

            utils::hash::StringHash<char *> shash;
            utils::CHashSet<char*> s1(&shash);
            char ch1[] = "test";
            size_t h = s1.hash(ch1);
//            std::cout << "hash: " << h << std::endl;

            utils::hash::FloatHash<double> hash2(1.2, 1.6);
            utils::CHashSet<double> s2(&hash2);
            size_t h2 = s2.hash(1.22222);
//            std::cout << "hash2: " << h2 << std::endl;
            size_t h3 = s2.hash(1.233);
//            std::cout << "hash2: " << h3 << std::endl;
        }

        void test_number_hash_charts() {
            graphics::PointChart numberHashChart(false, 710, 460);
            numberHashChart.setWindowTitle("Number hash");
            graphics::ChartThread<bool> numberHashChartThread(&numberHashChart);

            utils::hash::NumberHash<double> nflash;
//            utils::CHashSet<double> numSet(&nflash);
            for (double i = -100; i < 100; i = i + 0.1) {
                size_t h = nflash.hashCode(i);
                numberHashChart.redrawNewPoint(0, i, h);
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
        }

        void test_float_hash_charts() {
            graphics::PointChart floatHashChart(false, 710, 460);
            floatHashChart.setWindowTitle("Float hash");
            graphics::ChartThread<bool> floatHashChartThread(&floatHashChart);

            utils::hash::FloatHash<double> fhash(1.2, 1.6);
            utils::CHashSet<double> floatSet(&fhash);
            for (double i = 1.2; i < 1.6; i = i + 0.0001) {
                size_t h = floatSet.hash(i);
                floatHashChart.redrawNewPoint(0, i, h);
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
        }

        void cHashSet_test() {
            suite("CHashSetTest");
            mytest(hash_set);
//            mytest(number_hash_charts);
//            mytest(float_hash_charts);
        }
    }
}



