#ifndef FLUTTER_PLUGIN_APP_BADGE_CONTROL_FLUTTER_PLUGIN_H_
#define FLUTTER_PLUGIN_APP_BADGE_CONTROL_FLUTTER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include <windows.h>
#include <shobjidl.h>
// GDI+ headers use std::min/std::max, which collide with the min/max macros
// windows.h defines unless those macros are cleared first.
#undef min
#undef max
#include <gdiplus.h>

namespace app_badge_control_flutter {

class AppBadgeControlFlutterPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  AppBadgeControlFlutterPlugin(flutter::PluginRegistrarWindows *registrar);

  virtual ~AppBadgeControlFlutterPlugin();

  // Disallow copy and assign.
  AppBadgeControlFlutterPlugin(const AppBadgeControlFlutterPlugin&) = delete;
  AppBadgeControlFlutterPlugin& operator=(const AppBadgeControlFlutterPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

 private:
  flutter::PluginRegistrarWindows *registrar_;
  ITaskbarList3* taskbar_list_ = nullptr;
  HICON current_overlay_icon_ = nullptr;
  ULONG_PTR gdiplus_token_ = 0;

  void InitializeTaskbarList();
  HICON CreateBadgeIcon(int count);
  void CleanupIcon();
  // ITaskbarList3::SetOverlayIcon requires the top-level window handle that
  // owns the taskbar button; the Flutter view's HWND is a child of it.
  HWND GetTopLevelWindow();
};

}  // namespace app_badge_control_flutter

#endif  // FLUTTER_PLUGIN_APP_BADGE_CONTROL_FLUTTER_PLUGIN_H_
