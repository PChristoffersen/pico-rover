#pragma once

#include <fbus2/telemetry.h>

namespace Radio {

    using Telemetry = FBus2::Telemetry;

    class TelemetryProvider {
        protected:
            friend class Receiver;
            virtual Telemetry get_next_telemetry() = 0;
    };

}