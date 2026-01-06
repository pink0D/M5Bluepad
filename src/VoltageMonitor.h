//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _VOLTAGE_MONITOR_H_
#define _VOLTAGE_MONITOR_H_

namespace bluepadhub {

    class VoltageMonitor {

        public:
            VoltageMonitor() {
                
                setVoltageSampleCount(voltageSampleCount);
            };

            ~VoltageMonitor() {
                delete [] voltageSamples;
            };
            
            // returns battery status and updates if it's too old
            bool isLowBattery() {
                update();
                return lowBattery;
            };

            double getVoltage() {
                update();
                return voltage;
            };

            // sets cut-off value for a single lithium cell
            void setMinCellVoltage(double min_cell_voltage) {
                minCellVoltage = min_cell_voltage;
            };

            // sets how often battery voltage is measured
            void setVoltageUpdateInterval(int millis) {
                update_interval = 1000 * millis;
            };

            // sets number of ADC readings for every update
            void setVoltageReadCount(int count) {
                if (count > 0) {
                    voltageReadCount = count;
                }
            }
            // example: 
            //   update time = 500ms
            //   read count = 5
            //   sample count = 20
            // this will read 5x ADC values every 500ms and store total 20x values
            // so average battery voltage is calculated for a total interval of 2secs

            // sets total number of samples for average battery voltage calculation
            void setVoltageSampleCount(int size) {

                if (voltageSamples != nullptr) {
                    delete [] voltageSamples;
                }

                if (size > 0) {
                    voltageSampleCount = size;
                } else {
                    voltageSampleCount = 5;
                }

                voltageSamples = new double[voltageSampleCount];

                for (int i=0; i<voltageSampleCount; i++) {
                    voltageSamples[i] = 0;
                }
            };

            void printVoltage(bool _printVoltage) {
                printVoltageEnabled = _printVoltage;
            }
            
        protected:
            virtual double readVoltage() { return 0; };
            virtual void updateStatus() {};
            
        private:

            void update();
            bool isLowVoltage(double voltage);

            static double getAverageFromSamples(double *samples, int sampleCount);

            double voltage = 0;
            bool lowBattery = false;
            double minCellVoltage = 3.2;

            long time_next_update = 0;
            long update_interval = 500000;

            int voltageReadCount = 1;
            int voltageSampleCount = 5;
            double *voltageSamples = nullptr;
            int voltageIndex = 0;

            bool printVoltageEnabled = false;
    };

}

#endif