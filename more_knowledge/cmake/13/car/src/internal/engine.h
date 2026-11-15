#pragma once
#include <iostream>

// =========================================================================
// ĐÂY LÀ HEADER NỘI BỘ (PRIVATE)
// Chỉ có các file trong car/src được phép dùng.
// Ứng dụng bên ngoài (App) tuyệt đối KHÔNG ĐƯỢC PHÉP nhìn thấy file này!
// =========================================================================
namespace internal {
    void inject_fuel(int amount);
    void ignite_spark_plug();
}
