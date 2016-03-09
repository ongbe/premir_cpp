#ifndef SRC_FILE_CSVFILESUMMARY_H
#define SRC_FILE_CSVFILESUMMARY_H

#include <cstdio>
#include <cmath>
#include <limits>

#include "file/CsvFileReader.h"
#include "file/CsvFileRowParser.h"
#include "file/stream/CsvFileStreamReader.h"
#include "utils/CArrayList.h"
#include "models/models.h"

namespace file {

    template<typename Row, typename Num>
    class CsvFileSummary {
    public:

        CsvFileSummary(file::CsvFileReader *_csvReader,
                       file::CsvFileRowParser<Row, Num> *_rowParser, size_t colSize) :
                csvReader(_csvReader), rowParser(_rowParser) {
            summary = new utils::CArrayList<models::ColSummary<Num>>(colSize, 1, colSize);
        }

        ~CsvFileSummary() {
            delete summary;
            summary = nullptr;
        }

        /**
         * Если rowsLimit > 0 тогда статистика будет посчитана только для этого количества строк.
         */
        void collectSummary(size_t rowsLimit) {
            file::stream::CsvFileStreamReader<Row, Num> reader(csvReader, rowParser);
            size_t colSize = summary->size();
            Num min = std::numeric_limits<Num>::min();
            Num max = std::numeric_limits<Num>::max();
            // initialization
            for (size_t i = 0; i < colSize; ++i) {
                models::ColSummary<Num> &colSummary = (*summary)[i];
                colSummary.count = 0;
                colSummary.sum = 0;
                colSummary.average = 0;
                colSummary.scaledAverage = 0;
                colSummary.max = min; // init max by min value
                colSummary.min = max; // init mim by max value
            }

            Row row;
            models::DataSample<Num> samples[colSize];
            size_t rowIndex = 0;
            while (reader.readNext(row, samples) && (rowsLimit == 0 || (rowIndex < rowsLimit))) {
                for (size_t i = 0; i < colSize; ++i) {
                    models::ColSummary<Num> &colSummary = (*summary)[i];
                    models::DataSample<Num> &sample = samples[i];
                    if (!sample.skipped) {
                        colSummary.count++;
                        colSummary.sum += sample.value;
                        if (sample.value > colSummary.max) {
                            colSummary.max = sample.value;
                        }
                        if (sample.value < colSummary.min) {
                            colSummary.min = sample.value;
                        }
                    }
                }
                rowIndex++;
            }

            // вычислим среднее
            for (size_t i = 0; i < colSize; ++i) {
                models::ColSummary<Num> &colSummary = (*summary)[i];
                colSummary.average = colSummary.sum / colSummary.count;
                colSummary.scaledAverage = (colSummary.average - colSummary.min) / (colSummary.max - colSummary.min);
            }
        }

        utils::CArrayList<models::ColSummary<Num>> *getSummary() {
            return summary;
        }

        /**
         * val = (val - min) / (max - min)
         */
        void scaleSamples(models::DataSample<Num> *samples) {
            size_t colSize = summary->size();
            for (size_t i = 0; i < colSize; ++i) {
                models::ColSummary<Num> &colSummary = (*summary)[i];
                samples[i].value = (samples[i].value - colSummary.min) / (colSummary.max - colSummary.min);
                if (samples[i].value > 1) {
                    samples[i].value = 1;
                }
                if (samples[i].value < 0) {
                    samples[i].value = 0;
                }
            }
        }


    private:
        file::CsvFileReader *csvReader;
        file::CsvFileRowParser<Row, Num> *rowParser;
        utils::CArrayList<models::ColSummary<Num>> *summary;
    };

}


#endif