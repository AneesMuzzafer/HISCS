# HISCS
1. Home and Inverter Supply Control System (HISCS) is an electronic device that monitors the current readings of various electrical loads connected to it and based on a configurable priority, schedule, maximum allowed current and live control through an Android application, it switches the loads ON or OFF for maximum, efficient and as-required utilization of the available current. configurable priority, sch
2. The device makes use of two MPUs(both ARM based) to achieve its functionality.
3. The first MPU called as "Main Driver" deals with current sensing, wireless routing of data to and from users, live decision making, and control of Electrical loads.
4. The other MPU called as "Display Driver" is responsible for displaying the priority assigned, device status and sechedule set on the OLED.
5. The presence of a separate Display Driver(which works like a GPU) greatly enhances the speed of the system so that extremely fast decision making can be achieved.
