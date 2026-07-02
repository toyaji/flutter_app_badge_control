import 'app_badge_control_flutter_platform_interface.dart';

class AppBadgeControlFlutter {
  Future<String?> getPlatformVersion() {
    return AppBadgeControlFlutterPlatform.instance.getPlatformVersion();
  }

  static Future<void> updateBadgeCount(int count) {
    return AppBadgeControlFlutterPlatform.instance.updateBadgeCount(count);
  }

  static Future<void> removeBadge() {
    return AppBadgeControlFlutterPlatform.instance.removeBadge();
  }

  static Future<bool> isAppBadgeSupported() {
    return AppBadgeControlFlutterPlatform.instance.isAppBadgeSupported();
  }
}
