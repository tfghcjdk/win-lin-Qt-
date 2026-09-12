#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

// Initializes the board clock without blocking the UI. On the X6818 target it
// restores the system clock from the RTC and periodically synchronizes the
// system clock and RTC from verified NTP replies while networking is available.
class SystemManager {
public:
    static void initializeClock();
};

#endif
