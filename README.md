# app_badge_control_flutter

`app_badge_control_flutter` is a Flutter plugin for managing app icon badges. It allows you to update or remove the badge count on both iOS and Android devices.

## Why this fork exists

This package is a fork of [`flutter_app_badge_control`](https://github.com/funseek/flutter_app_badge_control) by Funseek, republished under a new name and maintained independently. The original repository was forked and republished because:

- **The upstream repository is no longer maintained.** No commits, releases, or reviews have landed there in a long time.
- **It doesn't support the Swift Package Manager (SPM).** iOS projects that have migrated off CocoaPods can't consume it. This fork adds SPM support while keeping CocoaPods working.
- **Issues and pull requests go unanswered.** Bug reports and contributions have no path to being reviewed or merged upstream.

Since publishing changes under the original name isn't possible, this fork is published to pub.dev as a new, independent package: `app_badge_control_flutter`. Ongoing maintenance, bug fixes, and feature requests will happen here.

## Migrating from `flutter_app_badge_control`

Migration only requires a rename — the plugin's behavior and method signatures are unchanged.

1. Update your dependency in `pubspec.yaml`:

   ```yaml
   dependencies:
     # remove
     # flutter_app_badge_control: ^0.0.3
     # add
     app_badge_control_flutter: ^0.1.0
   ```

2. Update your imports:

   ```dart
   // before
   import 'package:flutter_app_badge_control/flutter_app_badge_control.dart';
   // after
   import 'package:app_badge_control_flutter/app_badge_control_flutter.dart';
   ```

3. Rename the class you call, `FlutterAppBadgeControl` → `AppBadgeControlFlutter`. Every method name and its signature stayed the same:

   | Old (`flutter_app_badge_control`)              | New (`app_badge_control_flutter`)             |
   | ----------------------------------------------- | ---------------------------------------------- |
   | `FlutterAppBadgeControl.updateBadgeCount(count)` | `AppBadgeControlFlutter.updateBadgeCount(count)` |
   | `FlutterAppBadgeControl.removeBadge()`           | `AppBadgeControlFlutter.removeBadge()`           |
   | `FlutterAppBadgeControl.isAppBadgeSupported()`   | `AppBadgeControlFlutter.isAppBadgeSupported()`   |

No native (Android/iOS) configuration changes are required — the native method channel and platform code are unchanged.

## Overview

This plugin provides an easy way to control the badge count of your app's icon. It is compatible with iOS, Android, macOS, Windows, and Web. You can update the badge count, remove the badge, or check if badge support is available on the device.

This plugin is inspired by and based on the [flutter_app_badger](https://github.com/g123k/flutter_app_badger) plugin.

## Platform Behavior

| Platform | Support Status | Implementation Details |
| :--- | :--- | :--- |
| **iOS** | Supported | Uses native iOS user notification settings. Requires permission request. |
| **Android** | Supported | Tied to system notifications. Android lacks a direct, standalone badge API, so the plugin posts a silent, ongoing notification using `NotificationCompat.Builder.setNumber` to display the badge. |
| **macOS** | Supported | Updates the Dock icon badge using `NSApp.dockTile.badgeLabel`. |
| **Windows** | Supported | Implemented via **Taskbar Overlay Icons** using Win32 `ITaskbarList3::SetOverlayIcon`. Displays a red circle with the badge count overlaying the app's taskbar icon. |
| **Web** | Partial | Uses the Web Badging API (`navigator.setAppBadge`). Requires a **secure context** (HTTPS or `localhost`) and a **Chromium-based browser** (Chrome/Edge); Firefox and Safari are not supported. Mostly visible when the app is installed as a Progressive Web App (PWA). |
| **Linux** | Unsupported | No native or standardized desktop-wide badge API exists across Linux desktop environments (GNOME, KDE, XFCE, etc.). |

## Getting Started

### iOS

In order to use this plugin on iOS, you need to request permission for notifications.
For more information about requesting notification permissions, please refer to the [Apple Developer Documentation](https://developer.apple.com/documentation/usernotifications/asking_permission_to_use_notifications).

### Android

Android does not provide a native, direct API to update or clear the badge count on the app icon. Instead, since Android 8.0 (Oreo), badge dots/numbers on launcher icons are tied to **active notifications** in the notification drawer.

Because of this system design, the plugin implements badge control as follows:
* **`updateBadgeCount(count)`**: Posts a silent, ongoing notification in a dedicated notification channel (`flutter_app_badge_control_channel`) with `.setNumber(count)`.
* **`removeBadge()`**: Cancels this specific notification, which removes the badge dot/number.

**Limitations:**
1. **Launcher Dependent:** The appearance of the badge (whether it is a number or a simple dot) depends entirely on the device's launcher (e.g., Pixel Launcher displays a dot, while Samsung One UI can show numbers). Some custom launchers or custom ROMs may not support badges at all.
2. **Notification Present:** The badge count is only visible as long as the silent, ongoing notification remains in the notification drawer.

For more information about notification channels on Android, please visit the [Android Developer Documentation](https://developer.android.com/develop/ui/views/notifications/channels).

### macOS

No additional setup or permissions are required. The plugin updates the Dock tile icon badge natively using `NSApp.dockTile.badgeLabel`.

### Windows

No additional setup or permissions are required. Since standard Win32 desktop apps do not support native icon badges, the plugin displays the badge count as a **Taskbar Overlay Icon** on the application's taskbar window.

### Web

The Web Badging API (`navigator.setAppBadge`) relies on your app being installed as a **Progressive Web App (PWA)** and having **Notification Permissions**.

> **Requirements & limitations:**
> - **Secure context required.** The API is only exposed over **HTTPS** (or `localhost`). When served over plain HTTP, `navigator.setAppBadge` is undefined and `isAppBadgeSupported()` silently returns `false`.
> - **Chromium-only.** The Badging API is currently supported by **Chrome and Edge (Chromium)** only. **Firefox and Safari do not support it**, so `isAppBadgeSupported()` returns `false` there.
> - **Windows/Edge count updates are unreliable.** On a PWA installed via Edge on Windows, `updateBadgeCount()` reliably turns the taskbar overlay badge on (first non-zero value) and off (`removeBadge()`), but subsequent calls with a different count while the badge is already showing may not visually update the overlay, even though the JS call resolves successfully with no error. This has been confirmed to be a limitation in Chromium's own Windows taskbar overlay rendering, not something this plugin's code controls (the plugin only forwards the exact count to `navigator.setAppBadge()`).

1. **Request Notification Permission**: You must request user permission before the badge can be shown on the dock/taskbar icon. Here is an example implementation using `dart:js_interop` (Dart 3.x compatible):

```dart
import 'package:flutter/foundation.dart' show kIsWeb;
import 'dart:js_interop';

@JS('Notification.requestPermission')
external JSPromise? _jsRequestPermission();

Future<void> requestWebNotificationPermission() async {
  if (kIsWeb) {
    try {
      final promise = _jsRequestPermission();
      if (promise != null) {
        final result = await promise.toDart;
        final permissionStr = (result as JSString).toDart;
        debugPrint('Notification Permission status: $permissionStr');
      }
    } catch (e) {
      debugPrint('Failed to request permission: $e');
    }
  }
}
```

2. **Run as PWA**: The application must be installed as a Progressive Web App (PWA) (via Chrome's/Edge's "Install" icon). The badge will be rendered on the standalone PWA's app launcher/dock icon rather than standard browser tabs.

### Linux

This plugin does not support Linux desktop environments. Linux has many different desktop environments (GNOME, KDE Plasma, XFCE, Cinnamon, etc.), none of which provide a standardized or unified D-Bus API for displaying app icon badge overlays on launcher/dock icons. Historically, some Ubuntu configurations supported the `Unity` launcher API, but since Unity has been deprecated, there is currently no universal way to implement badge support on Linux.

## Usage

To use the plugin, add the following import to your Dart file:

```dart
import 'package:app_badge_control_flutter/app_badge_control_flutter.dart';
```

### Check if App Badge is Supported

```dart
AppBadgeControlFlutter.isAppBadgeSupported().then((value) {
  debugPrint("isAppBadgeSupported: $value");
});
```

### Remove Badge

```dart
AppBadgeControlFlutter.removeBadge();
```

### Update Badge Count

```dart
AppBadgeControlFlutter.updateBadgeCount(1);
```
**Note:** On Android, this will post a silent, ongoing notification to display the badge count. The visual representation (number vs. dot) varies depending on the launcher.

## Troubleshooting

### Web (PWA) Badge Not Showing Up
Web Badging API relies heavily on browser permissions and operating system integration. If the badge doesn't appear on the PWA icon in macOS Dock:
1. **Enable Notification Permissions**: Click the icon (lock or settings slider) on the left of the address bar inside your installed PWA window, and ensure **Notifications** are set to **Allow**.
2. **Enable macOS System Settings**: Go to **System Settings** -> **Notifications** -> Find your PWA app in the list -> ensure **Badge app icon** is toggled **On**.
3. **Standalone Mode Required**: The app icon badge will not be displayed on normal browser tabs. The web app must be installed and running as a Progressive Web App (PWA).

### Windows/Edge Badge Count Stuck After First Update
If the taskbar overlay badge appears once (e.g. shows "1") but never changes on later `updateBadgeCount()` calls, even though those calls succeed without throwing:
* This is a known limitation of Chromium's Windows taskbar overlay implementation, not a bug in this plugin. Turning the badge on/off (`removeBadge()` then a fresh non-zero count) works reliably; updating the count while the badge is already visible does not reliably repaint the overlay.
* There is no workaround available from the web/JS side today, since the plugin already forwards the exact count to `navigator.setAppBadge()` on every call.

### MissingPluginException on Web
If you encounter `MissingPluginException` while calling badge control APIs on Web:
* Web plugin registrations are dynamically resolved during the initial build. If you have modified dependencies or platforms in `pubspec.yaml`, you **must restart** the `flutter run` process completely. Simply hot reloading or hot restarting will not apply the web plugin register hook.
