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
            VoltageMonitor() {};

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

            void setVoltageUpdateInterval(int millis) {
                update_interval = 1000 * millis;
            };

            void printVoltage(bool _printVoltage) {
                printVoltageEnabled = _printVoltage;
            }
            
        protected:
            virtual double readVoltage() { return 0; };
            
        private:

            void update();
            bool isLowVoltage(double voltage);

            double voltage = 0;
            bool lowBattery = false;
            double minCellVoltage = 3.4;

            long time_next_update = 0;
            long update_interval = 500000;

            bool printVoltageEnabled = false;
    };

}

#endif