#ifndef TEST_KOHONEN_KOHONENDEMOTEST_H
#define TEST_KOHONEN_KOHONENDEMOTEST_H

#include <iostream>
#include <cmath>

#include "test.h"
#include "kohonen/NetworkInitializer.h"
#include "file/CsvFileReader.h"
#include "file/CsvFileRowParser.h"
#include "file/stream/CsvFileStreamReader.h"
#include "utils/SMatrix.h"
#include "utils/RMatrix.h"
#include "kohonen/RandomGenerator.h"
#include "kohonen/winner/EuclideanWinnerSearch.h"
#include "kohonen/alphafunc/InverseAlphaFunction.h"
#include "kohonen/alphafunc/LinearAlphaFunction.h"

#include "kohonen/neighadap/BubbleNeighborAdaptation.h"
#include "kohonen/neighadap/GaussianNeighborAdaptation.h"

#include "kohonen/mapdist/HexaMapDistance.h"
#include "kohonen/mapdist/RectMapDistance.h"

#include "kohonen/SomTrainer.h"
#include "kohonen/SammonMap.h"

#include "kohonen/umat/UMat.h"
#include "kohonen/umat/RectUMat.h"
#include "kohonen/umat/HexaUMat.h"

#include "graphics/ChartThread.h"
#include "graphics/SammonMapChart.h"
#include "graphics/PointChart.h"
#include "graphics/UMatChart.h"

namespace test {
    namespace kohonen_demos_test {

        struct DemoInRow {
//            bool value1;
            char label;
        };

        class KohonenDemo2CsvFileRowParser : public file::CsvFileRowParser<DemoInRow, float> {

            bool parseRow(DemoInRow &row, models::DataSample<float> *samples, file::CsvFileReader *csvReader) {
                size_t colSize = 20;
                for (size_t i = 0; i < colSize; ++i) {
                    readNextDataSample(samples[i], csvReader);
                }
                // get label
                char buffer[2];
                *buffer = '\0';
                size_t bytesRead = csvReader->read(buffer, 2);
                if (bytesRead == 0) {
                    row.label = '\0';
                } else {
                    row.label = buffer[0];
                }

                return true;
            }

            void initReadFile(file::CsvFileReader *csvReader) {
                // skip first line
                csvReader->toEndLine();
                csvReader->toEndLine();
            }

        private:

            size_t readNextDataSample(models::DataSample<float> &sample, file::CsvFileReader *csvReader) {
                char buffer[64];
                *buffer = '\0';
                size_t bytesRead = csvReader->read(buffer, 64);
                if (bytesRead == 0 || buffer[0] == 'x' || buffer[0] == 'X') {
                    sample.skipped = true;
                } else {
                    sample.skipped = false;
                    sample.value = std::atof(buffer);
                }
                return bytesRead;
            }

        };

        void kohonen_demos_test();
    }
}

#endif
