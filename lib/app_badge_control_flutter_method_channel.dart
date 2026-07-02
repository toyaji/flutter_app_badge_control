import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'app_badge_control_flutter_platform_interface.dart';

/// An implementation of [AppBadgeControlFlutterPlatform] that uses method channels.
class MethodChannelAppBadgeControlFlutter
    extends AppBadgeControlFlutterPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('flutter_app_badge_control');

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<void> updateBadgeCount(int count) async {
    return await methodChannel.invokeMethod<void>('updateBadgeCount', count);
  }

  @override
  Future<void> removeBadge() async {
    return await methodChannel.invokeMethod<void>('removeBadge');
  }

  @override
  Future<bool> isAppBadgeSupported() async {
    final isSupported =
        await methodChannel.invokeMethod<bool>('isAppBadgeSupported');
    return isSupported ?? false;
  }
}
