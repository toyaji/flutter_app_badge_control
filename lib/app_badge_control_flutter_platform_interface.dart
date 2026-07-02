import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'app_badge_control_flutter_method_channel.dart';

abstract class AppBadgeControlFlutterPlatform extends PlatformInterface {
  /// Constructs an AppBadgeControlFlutterPlatform.
  AppBadgeControlFlutterPlatform() : super(token: _token);

  static final Object _token = Object();

  static AppBadgeControlFlutterPlatform _instance =
      MethodChannelAppBadgeControlFlutter();

  /// The default instance of [AppBadgeControlFlutterPlatform] to use.
  ///
  /// Defaults to [MethodChannelAppBadgeControlFlutter].
  static AppBadgeControlFlutterPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [AppBadgeControlFlutterPlatform] when
  /// they register themselves.
  static set instance(AppBadgeControlFlutterPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<void> updateBadgeCount(int count) {
    throw UnimplementedError(
        'updateBadgeCount(int count) has not been implemented.');
  }

  Future<void> removeBadge() {
    throw UnimplementedError('removeBadge() has not been implemented.');
  }

  Future<bool> isAppBadgeSupported() async {
    throw UnimplementedError('isAppBadgeSupported() has not been implemented.');
  }
}
