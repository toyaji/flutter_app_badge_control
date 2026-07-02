// In order to *not* need this ignore, consider extracting the "web" version
// of your plugin as a separate package, instead of inlining it in the same
// package as the core of your plugin.
// ignore: avoid_web_libraries_in_flutter

import 'dart:js_interop';
import 'package:flutter_web_plugins/flutter_web_plugins.dart';
import 'package:web/web.dart' as web;

import 'app_badge_control_flutter_platform_interface.dart';

@JS('navigator')
external JSObject get _navigator;

@JS('navigator.setAppBadge')
external JSPromise? _setAppBadge(JSNumber count);

@JS('navigator.clearAppBadge')
external JSPromise? _clearAppBadge();

@JS('Reflect.has')
external bool _reflectHas(JSObject target, JSString propertyKey);

/// A web implementation of the AppBadgeControlFlutterPlatform of the AppBadgeControlFlutter plugin.
class AppBadgeControlFlutterWeb extends AppBadgeControlFlutterPlatform {
  /// Constructs a AppBadgeControlFlutterWeb
  AppBadgeControlFlutterWeb();

  static void registerWith(Registrar registrar) {
    AppBadgeControlFlutterPlatform.instance = AppBadgeControlFlutterWeb();
  }

  /// Returns a [String] containing the version of the platform.
  @override
  Future<String?> getPlatformVersion() async {
    final version = web.window.navigator.userAgent;
    return version;
  }

  bool get isSupported {
    try {
      return _reflectHas(_navigator, 'setAppBadge'.toJS);
    } catch (e) {
      return false;
    }
  }

  @override
  Future<void> updateBadgeCount(int count) async {
    if (isSupported) {
      try {
        final promise = _setAppBadge(count.toJS);
        if (promise != null) {
          await promise.toDart;
        }
      } catch (e) {
        // Ignore API failures
      }
    }
  }

  @override
  Future<void> removeBadge() async {
    if (isSupported) {
      try {
        final promise = _clearAppBadge();
        if (promise != null) {
          await promise.toDart;
        }
      } catch (e) {
        // Ignore
      }
    }
  }

  @override
  Future<bool> isAppBadgeSupported() async {
    return isSupported;
  }
}
