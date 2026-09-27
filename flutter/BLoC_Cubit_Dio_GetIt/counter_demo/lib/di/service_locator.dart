import 'package:get_it/get_it.dart';
import '../cubit/counter_cubit.dart';

// GETIT: container quản lý dependency — giống singleton toàn cục
// Thay vì tạo object mới mỗi lần, GetIt cung cấp cùng 1 instance ở mọi nơi

final getIt = GetIt.instance;

void setupLocator() {
  // registerSingleton: tạo 1 lần, dùng mãi (như singleton)
  getIt.registerSingleton<CounterCubit>(CounterCubit());

  // Nếu muốn tạo mới mỗi lần gọi: dùng registerFactory
  // getIt.registerFactory<CounterCubit>(() => CounterCubit());
}
