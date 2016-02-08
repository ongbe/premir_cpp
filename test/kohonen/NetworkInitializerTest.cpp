#include "NetworkInitializerTest.h"

namespace test {
    namespace kohonen_initializer {

        void readInitializer(file::CsvFileReader<char> *csvReader) {
            // skip first line
            csvReader->toEndLine();
        }

        bool isSkipSample(char *buffer, size_t bytesRead) {
            if (bytesRead == 0 || buffer[0] == 'x' || buffer[0] == 'X') {
                return true;
            } else {
                return false;
            }
        }

        utils::SMatrix<float> *read_codes_file(const char *filename,
                                               int skipLines) {
            file::CsvFileReader<char> reader(filename, ' ');
            utils::SMatrix<float> *somCodesMatrix =
                    new utils::SMatrix<float>(0, 5);
            // skip to lines
            for (int i = 0; i < skipLines; ++i) {
                reader.toEndLine();
            }

            while (reader.hasNext()) {
                float row[5];
                reader.read(row[0]);
                reader.read(row[1]);
                reader.read(row[2]);
                reader.read(row[3]);
                reader.read(row[4]);
                reader.toEndLine();
                if (!reader.isEmptyRead()) {
                    somCodesMatrix->pushRow(row);
                }
            }

            return somCodesMatrix;
        }

        utils::SMatrix<float> *read_some_initilized_codes() {
            return read_codes_file(
                    "../test/datafiles/kohonen/som_initialized.cod", 2);
        }

        void test_line_initialization() {
            // инициализация потока чтения файла с данными
            file::CsvFileReader<char> csvReader(
                    "../test/datafiles/kohonen/ex.dat", ' ');
            file::stream::CsvFileArrayStreamReader<float>
                    dataReader(&csvReader, readInitializer,
                               isSkipSample, 5, true);

            kohonen::
            NetworkInitializer<float, float> initializer(&dataReader);
            kohonen::RandomGenerator *randomEngine
                    = initializer.getRandomGenerator();
            // TODO: в тестах для генерации кодов нейронов таких же как и в
            // проверочном файле 'som_initialized.cod', необходимо задавать
            // начальное рандомное число setNextValue(1) т.к. файл
            // 'som_initialized.cod' был сгенерирован при начальном значении = 1
            randomEngine->setNextValue(1);
            //randomEngine->initGenerator();

            size_t xdim = 16;
            size_t ydim = 12;
            size_t dim = 5;

            utils::SMatrix<float> *resultsMatrix =
                    initializer.lineInitialization(xdim, ydim, dim);

            utils::SMatrix<float> *somCodesMatrix = read_some_initilized_codes();

            // данные матрицы должны быть практически идентичными
            assert(somCodesMatrix->equalsWithError(*resultsMatrix, 0.001));

            delete somCodesMatrix;
            delete resultsMatrix;
        }

        void test_eucw_bubble_hexa_16_12_som_training() {
            size_t xdim = 16;
            size_t ydim = 12;
            size_t dim = 5;

            // инициализация потока чтения файла с данными
            file::CsvFileReader<char> csvReader(
                    "../test/datafiles/kohonen/ex.dat", ' ');
            file::stream::CsvFileArrayStreamReader<float>
                    dataReader(&csvReader, readInitializer, isSkipSample, dim,
                               true);

            utils::SMatrix<float> *somCodesMatrix = read_some_initilized_codes();

            kohonen::winner::EuclideanWinnerSearch<float, float> winnerSearcher;
            kohonen::alphafunc::LinearAlphaFunction<float> alphaFunc;
            kohonen::mapdist::HexaMapDistance<float> mapDist;

            kohonen::neighadap::BubbleNeighborAdaptation<float, float>
                    neiAdap(&mapDist, xdim, ydim);
            kohonen::SomTrainer<float, float> trainer(&alphaFunc,
                                                      &winnerSearcher,
                                                      &neiAdap,
                                                      0.002, 3.0, xdim, ydim);

            trainer.training(somCodesMatrix, &dataReader, 10000);

            utils::SMatrix<float> *expectedCodesMatrix =
                    read_codes_file(
                            "../test/datafiles/kohonen/som_trained_10000_"
                                    "eucw_bubble_hexa_16_12.cod", 1);

            // данные матрицы должны быть практически идентичными
            assert(somCodesMatrix->equalsWithError(*expectedCodesMatrix,
                                                   0.001));

            kohonen::SomTrainer<float, float>::QuantumError qe =
                    trainer.quantizationError(somCodesMatrix, &dataReader);

//            std::cout.precision(std::numeric_limits<float >::digits);
//            std::cout << "sumWinnerDistance: " <<
//            qe.sumWinnerDistance / qe.samplesSize <<
//            ", num: " << qe.samplesSize << std::endl;

            assert_range(qe.sumWinnerDistance / qe.samplesSize, 4.7287, 0.0001);


            delete expectedCodesMatrix;
            delete somCodesMatrix;
        }

        void test_visible_som_training() {
            size_t xdim = 26;
            size_t ydim = 12;
            size_t dim = 5;

            // инициализация потока чтения файла с данными
            file::CsvFileReader<char> csvReader(
                    "../test/datafiles/kohonen/ex.dat", ' ');
            file::stream::CsvFileArrayStreamReader<float>
                    dataReader(&csvReader, readInitializer, isSkipSample, dim,
                               true);

//            utils::SMatrix<float> *somCodesMatrix = read_some_initilized_codes();

            kohonen::
            NetworkInitializer<float, float> initializer(&dataReader);
            kohonen::RandomGenerator *randomEngine
                    = initializer.getRandomGenerator();
            randomEngine->setNextValue(1);
            utils::SMatrix<float> *somCodesMatrix =
                    initializer.lineInitialization(xdim, ydim, dim);

            kohonen::winner::EuclideanWinnerSearch<float, float> winnerSearcher;
            kohonen::alphafunc::LinearAlphaFunction<float> alphaFunc;
            kohonen::mapdist::HexaMapDistance<float> mapDist;

            kohonen::neighadap::BubbleNeighborAdaptation<float, float>
                    neiAdap(&mapDist, xdim, ydim);
            kohonen::SomTrainer<float, float> trainer(&alphaFunc,
                                                      &winnerSearcher,
                                                      &neiAdap,
                                                      0.002, 3.0, xdim, ydim);

            //
            graphics::ChartThread qErrorChart(710, 460);
            qErrorChart.getChart().setWindowTitle("Quantum error");

            size_t teachSize = 4000000;
            size_t winnerSize = winnerSearcher.getWinnerSize();
            size_t colSize = somCodesMatrix->getColSize();

            double qerror = 0;
            int step = 10000;

            for (size_t le = 0; le < teachSize; ++le) {
                models::DataSample<float> inRow[colSize];
                bool hasInRow = dataReader.readNext(inRow, colSize);
                if (!hasInRow) {
                    // значит достигли конца данных, начинаем читать с начала
                    // установить поток на начало
                    dataReader.rewindReader();
                    // читаем первую стоку данных
                    dataReader.readNext(inRow, colSize);
                }

                kohonen::winner::WinnerInfo<float> winners[winnerSize];
                bool ok = trainer.trainingBySample(somCodesMatrix, inRow,
                                           winners, teachSize, le);
                if (ok) {
                    int cnt = le%step;
                    qerror+=std::sqrt(winners[0].diff);
                    if (cnt==0 && le!=0) {
                        qerror = qerror / step;
                        qErrorChart.getChart().redrawNewPoints(le, qerror);
                        qerror = 0;
                        std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    }

                }
            }

            // проверка
//            utils::SMatrix<float> *expectedCodesMatrix =
//                    read_codes_file(
//                            "../test/datafiles/kohonen/som_trained_10000_"
//                                    "eucw_bubble_hexa_16_12.cod", 1);
//            assert(somCodesMatrix->equalsWithError(*expectedCodesMatrix,
//                                                   0.001));

//            delete expectedCodesMatrix;
            delete somCodesMatrix;
        }

        void test_eucw_gaussian_rect_16_12_som_training() {
            size_t xdim = 16;
            size_t ydim = 12;
            size_t dim = 5;

            // инициализация потока чтения файла с данными
            file::CsvFileReader<char> csvReader(
                    "../test/datafiles/kohonen/ex.dat", ' ');
            file::stream::CsvFileArrayStreamReader<float>
                    dataReader(&csvReader, readInitializer,
                               isSkipSample, dim, true);

            utils::SMatrix<float> *somCodesMatrix = read_some_initilized_codes();

            kohonen::winner::EuclideanWinnerSearch<float, float> winnerSearcher;
            kohonen::alphafunc::LinearAlphaFunction<float> alphaFunc;
            kohonen::mapdist::RectMapDistance<float> mapDist;

            kohonen::neighadap::GaussianNeighborAdaptation<float, float>
                    neiAdap(&mapDist, xdim, ydim);
            kohonen::SomTrainer<float, float> trainer(&alphaFunc,
                                                      &winnerSearcher,
                                                      &neiAdap,
                                                      0.002, 3, xdim, ydim);

            trainer.training(somCodesMatrix, &dataReader, 10000);

            utils::SMatrix<float> *expectedCodesMatrix =
                    read_codes_file(
                            "../test/datafiles/kohonen/som_trained_10000_"
                                    "eucw_gaussian_rect_16_12.cod", 1);

            // данные матрицы должны быть практически идентичными
            assert(somCodesMatrix->equalsWithError(*expectedCodesMatrix,
                                                   0.001));

            delete expectedCodesMatrix;
            delete somCodesMatrix;
        }

        void network_initializer_test() {
            suite("NetworkInitializer_test");
            mytest(line_initialization);
            mytest(eucw_bubble_hexa_16_12_som_training);
            mytest(eucw_gaussian_rect_16_12_som_training);
            mytest(visible_som_training);
        }
    }
}



