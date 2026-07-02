#include "include/app_badge_control_flutter/app_badge_control_flutter_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "app_badge_control_flutter_plugin.h"

void AppBadgeControlFlutterPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  app_badge_control_flutter::AppBadgeControlFlutterPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
