import 'package:flutter_test/flutter_test.dart';
import 'package:app_badge_control_flutter/app_badge_control_flutter.dart';
import 'package:app_badge_control_flutter/app_badge_control_flutter_platform_interface.dart';
import 'package:app_badge_control_flutter/app_badge_control_flutter_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockAppBadgeControlFlutterPlatform
    with MockPlatformInterfaceMixin
    implements AppBadgeControlFlutterPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  Future<bool> isAppBadgeSupported() {
    return Future.value(true);
  }

  @override
  Future<void> removeBadge() {
    return Future.value(null);
  }

  @override
  Future<void> updateBadgeCount(int count) {
    return Future.value(null);
  }
}

void main() {
  final AppBadgeControlFlutterPlatform initialPlatform =
      AppBadgeControlFlutterPlatform.instance;

  test('$MethodChannelAppBadgeControlFlutter is the default instance', () {
    expect(initialPlatform,
        isInstanceOf<MethodChannelAppBadgeControlFlutter>());
  });

  test('getPlatformVersion', () async {
    AppBadgeControlFlutter appBadgeControlFlutterPlugin =
        AppBadgeControlFlutter();
    MockAppBadgeControlFlutterPlatform fakePlatform =
        MockAppBadgeControlFlutterPlatform();
    AppBadgeControlFlutterPlatform.instance = fakePlatform;
    addTearDown(() => AppBadgeControlFlutterPlatform.instance = initialPlatform);

    expect(await appBadgeControlFlutterPlugin.getPlatformVersion(), '42');
  });
}
