#pragma once

// =========================================================================
// ĐÂY LÀ THƯ VIỆN HEADER-ONLY (INTERFACE)
// Không hề có bất kỳ file .cpp nào để biên dịch.
// Bản thân nó không cần compile, chỉ phục vụ cho người khác link tới nó!
// =========================================================================
namespace FastMath {
    inline double calculate_fuel_efficiency(double km, double liters) {
        if (liters <= 0.0) return 0.0;
        return km / liters;
    }
}
