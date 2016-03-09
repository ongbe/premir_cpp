#include "KohonenDemosTest.h"

namespace test {
    namespace kohonen_demos_test {

        typedef utils::RMatrix<models::NeuronInfo, float> OutCodes;

        void buildAndShowSammonMap(OutCodes *somTrainedMatrix, graphics::SammonMapChart<float> &sammonChart) {
            kohonen::SammonMap<float> sammonMap(somTrainedMatrix->getRowSize());
            kohonen::RandomGenerator *randomEngine = sammonMap.getRandomGenerator();
            // для теста псевдоинициализация
            randomEngine->setNextValue(1);
            sammonMap.initializeMap(somTrainedMatrix);

            for (size_t i = 0; i < 200; ++i) {
                sammonChart.removeDataSafely();
                sammonMap.doOneIteration();
                sammonChart.addSammonMapPoints(sammonMap.getMapPoints());
                sammonChart.drawOnWindow();
            }
        }

        void drawUMat(OutCodes *somTrainedMatrix,
                      graphics::UMatChart<float> &chart, size_t xdim, size_t ydim, size_t dim) {
            kohonen::umat::HexaUMat<float> umat(xdim, ydim, dim);
            umat.initializeMat(somTrainedMatrix);
            umat.buildUMatrix();
            umat.medianUMatrix();
//            umat.averageUMatrix();

            chart.removeDataSafely();
            chart.addHexaUMatPoints(umat.getUMatrix());
            chart.drawOnWindow();
        }

        void test_speech_signal() {
            size_t xdim = 68;
            size_t ydim = 68;
            size_t dim = 20;
            size_t teachSize = 80000;
            bool isScale = false;

            // инициализация потока чтения файла с данными
            file::CsvFileReader csvReader("../test/datafiles/kohonen/ex1.dat", ' ');
            KohonenDemo2CsvFileRowParser demoRowParser;
            file::stream::CsvFileStreamReader<DemoInRow, float> dataReader(&csvReader, &demoRowParser, dim, false);
            file::CsvFileSummary<DemoInRow, float> summary(&csvReader, &demoRowParser, dim);
            summary.collectSummary(0); // 0 - значит без ограничений
//            summary.getSummary()->print();

            kohonen::NetworkInitializer<DemoInRow, float, float> initializer(&dataReader, &summary);
            kohonen::RandomGenerator *randomEngine = initializer.getRandomGenerator();
            randomEngine->setNextValue(1);
            OutCodes *somCodesMatrix = initializer.lineInitialization(xdim, ydim, dim, isScale);

            kohonen::winner::EuclideanWinnerSearch<float, float> winnerSearcher;
            kohonen::alphafunc::LinearAlphaFunction<float> alphaFunc;
            kohonen::mapdist::HexaMapDistance<float> mapDist;

            kohonen::neighadap::GaussianNeighborAdaptation<float, float> gausAdap(&mapDist, xdim, ydim);
            kohonen::neighadap::BubbleNeighborAdaptation<float, float> neiAdap(&mapDist, xdim, ydim);
            kohonen::SomTrainer<DemoInRow, float, float> trainer(&alphaFunc, &winnerSearcher, &neiAdap, 0.002, 0.0,
                                                                 xdim, ydim);

            //
            graphics::PointChart qErrorChart(true, 710, 460);
            qErrorChart.setWindowTitle("Quantum error");
            graphics::ChartThread<bool> chartThread(&qErrorChart);

            size_t winnerSize = winnerSearcher.getWinnerSize();
            size_t colSize = somCodesMatrix->getColSize();


            double qerror = 0;
            int step = 10000;
//            int step2 = 6000;

//            graphics::SammonMapChart<float> sammonChart(xdim, 1200, 700);
//            sammonChart.setWindowTitle("Sammon Map");
//            graphics::ChartThread<bool> sammonChartThread(&sammonChart);
//            buildAndShowSammonMap(somCodesMatrix, sammonChart);

            graphics::UMatChart<float> umatChart(700, 700);
            umatChart.setWindowTitle("UMat");
            graphics::ChartThread<float> umchartThread(&umatChart);
            drawUMat(somCodesMatrix, umatChart, xdim, ydim, dim);

            for (size_t le = 0; le < teachSize; ++le) {
                models::DataSample<float> samples[colSize];
                DemoInRow rowData;
                bool hasInRow = dataReader.readNext(rowData, samples);
                if (!hasInRow) {
                    drawUMat(somCodesMatrix, umatChart, xdim, ydim, dim);

                    // значит достигли конца данных, начинаем читать с начала
                    // установить поток на начало
                    dataReader.rewindReader();
                    // читаем первую стоку данных
                    dataReader.readNext(rowData, samples);
                }

                // нормализация
                if (isScale) {
                    summary.scaleSamples(samples);
                }

//                std::cout << "le: " << le << " " << rowData.label << std::endl;
                kohonen::winner::WinnerInfo<float> winners[winnerSize];
                bool ok = trainer.trainingBySample(somCodesMatrix, samples, winners, teachSize, le);
                if (ok) {
//                    int cnt = le % step;
                    qerror += std::sqrt(winners[0].diff);
                    if (le % step==0 && le!=0) {
                        qerror = qerror / step;
                        qErrorChart.redrawNewPoint(le, qerror);
//                        drawUMat(somCodesMatrix, umatChart, xdim, ydim, dim);
                        qerror = 0;
//                        std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    }

//                    if (le % step2 == 0) {
//                        drawUMat(somCodesMatrix, umatChart, xdim, ydim, dim);
//                        buildAndShowSammonMap(somCodesMatrix, sammonChart);
//                    }

                }
            }

            graphics::UMatChart<float> umatChart2(2000, 2000);
            umatChart2.setWindowTitle("UMat2");
            graphics::ChartThread<float> umchartThread2(&umatChart2);
            drawUMat(somCodesMatrix, umatChart2, xdim, ydim, dim);
            umatChart2.saveImage("u-matrix-speech-final.png");

//            graphics::SammonMapChart<float> sammonChart2(xdim, 1200, 700);
//            sammonChart2.setWindowTitle("Sammon Map2");
//            graphics::ChartThread<bool> sammonChartThread2(&sammonChart2);
//            buildAndShowSammonMap(somCodesMatrix, sammonChart2);

            delete somCodesMatrix;
        }

        void kohonen_demos_test() {
            suite("KohonenDemos");
            mytest(speech_signal);
        }
    }
}


