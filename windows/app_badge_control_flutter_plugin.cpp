#include "app_badge_control_flutter_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <string>

namespace app_badge_control_flutter {

namespace {
// Windows' own notification/alert red (#E81123).
constexpr BYTE kBadgeColorR = 0xE8;
constexpr BYTE kBadgeColorG = 0x11;
constexpr BYTE kBadgeColorB = 0x23;
}  // namespace

// static
void AppBadgeControlFlutterPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "flutter_app_badge_control",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<AppBadgeControlFlutterPlugin>(registrar);

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

AppBadgeControlFlutterPlugin::AppBadgeControlFlutterPlugin(flutter::PluginRegistrarWindows *registrar)
    : registrar_(registrar) {
  Gdiplus::GdiplusStartupInput gdiplus_startup_input;
  Gdiplus::GdiplusStartup(&gdiplus_token_, &gdiplus_startup_input, NULL);
  InitializeTaskbarList();
}

AppBadgeControlFlutterPlugin::~AppBadgeControlFlutterPlugin() {
  CleanupIcon();
  if (taskbar_list_) {
    taskbar_list_->Release();
    taskbar_list_ = nullptr;
  }
  if (gdiplus_token_) {
    Gdiplus::GdiplusShutdown(gdiplus_token_);
    gdiplus_token_ = 0;
  }
}

void AppBadgeControlFlutterPlugin::InitializeTaskbarList() {
  HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&taskbar_list_));
  if (SUCCEEDED(hr)) {
    hr = taskbar_list_->HrInit();
    if (FAILED(hr)) {
      taskbar_list_->Release();
      taskbar_list_ = nullptr;
    }
  }
}

HWND AppBadgeControlFlutterPlugin::GetTopLevelWindow() {
  HWND view_hwnd =
      registrar_->GetView() ? registrar_->GetView()->GetNativeWindow() : nullptr;
  if (!view_hwnd) {
    return nullptr;
  }
  return GetAncestor(view_hwnd, GA_ROOT);
}

void AppBadgeControlFlutterPlugin::CleanupIcon() {
  if (current_overlay_icon_) {
    DestroyIcon(current_overlay_icon_);
    current_overlay_icon_ = nullptr;
  }
}

HICON AppBadgeControlFlutterPlugin::CreateBadgeIcon(int count) {
  using namespace Gdiplus;

  // Overlay icons are shown at the shell's small-icon size; drawing at that
  // native size (instead of a hardcoded 16px) keeps the badge crisp at the
  // user's current DPI setting.
  int size = GetSystemMetrics(SM_CXSMICON);
  if (size <= 0) {
    size = 16;
  }

  // A 32bpp ARGB bitmap gives the icon a real per-pixel alpha channel, so the
  // circle's edges are anti-aliased instead of the hard, stair-stepped edges
  // produced by a legacy 1-bit transparency mask.
  Bitmap bitmap(size, size, PixelFormat32bppARGB);
  Graphics graphics(&bitmap);
  graphics.SetSmoothingMode(SmoothingModeAntiAlias);
  graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
  graphics.Clear(Color(0, 0, 0, 0));

  Color circleColor(255, kBadgeColorR, kBadgeColorG, kBadgeColorB);
  SolidBrush circleBrush(circleColor);
  // Inset by half a pixel on each side: a bounding box flush with the bitmap
  // edges leaves no room for GDI+'s anti-aliasing to blend evenly, which
  // shows up as the circle looking clipped on the right/bottom edge.
  graphics.FillEllipse(&circleBrush, 0.5f, 0.5f, static_cast<REAL>(size) - 1.0f,
                        static_cast<REAL>(size) - 1.0f);

  // Taskbar overlay icons conventionally show only one or two glyphs, so clamp
  // 3+ digit counts to "99+" to keep them legible in the small icon.
  std::wstring countStr = (count > 99) ? L"99+" : std::to_wstring(count);

  REAL fontSize = static_cast<REAL>(size) *
      (countStr.length() > 2 ? 0.42f : (countStr.length() > 1 ? 0.50f : 0.62f));
  FontFamily fontFamily(L"Segoe UI");
  Font font(&fontFamily, fontSize, FontStyleBold, UnitPixel);
  SolidBrush textBrush(Color(255, 255, 255, 255));

  StringFormat format;
  format.SetAlignment(StringAlignmentCenter);
  format.SetLineAlignment(StringAlignmentCenter);
  RectF layoutRect(0.0f, 0.0f, static_cast<REAL>(size), static_cast<REAL>(size));
  graphics.DrawString(countStr.c_str(), -1, &font, layoutRect, &format, &textBrush);

  HICON hIcon = nullptr;
  bitmap.GetHICON(&hIcon);
  return hIcon;
}

void AppBadgeControlFlutterPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_call.method_name().compare("updateBadgeCount") == 0) {
    int count = 0;
    const auto* arguments = method_call.arguments();
    if (arguments) {
      if (std::holds_alternative<int32_t>(*arguments)) {
        count = std::get<int32_t>(*arguments);
      } else if (std::holds_alternative<int64_t>(*arguments)) {
        count = static_cast<int>(std::get<int64_t>(*arguments));
      } else if (std::holds_alternative<std::string>(*arguments)) {
        try {
          count = std::stoi(std::get<std::string>(*arguments));
        } catch (...) {
          result->Error("INVALID_ARGUMENT", "Count string is not a valid integer");
          return;
        }
      } else {
        result->Error("INVALID_ARGUMENT", "Count must be an integer");
        return;
      }
    }

    HWND hwnd = GetTopLevelWindow();
    if (taskbar_list_ && hwnd) {
      CleanupIcon();
      if (count <= 0) {
        // Convention: 0 (or less) means "no badge", matching iOS/Web/macOS.
        taskbar_list_->SetOverlayIcon(hwnd, NULL, NULL);
      } else {
        current_overlay_icon_ = CreateBadgeIcon(count);
        taskbar_list_->SetOverlayIcon(hwnd, current_overlay_icon_, L"Notifications");
      }
      result->Success();
    } else {
      result->Error("UNAVAILABLE", "Taskbar interface or Window Handle is unavailable");
    }
  } else if (method_call.method_name().compare("removeBadge") == 0) {
    HWND hwnd = GetTopLevelWindow();
    if (taskbar_list_ && hwnd) {
      CleanupIcon();
      taskbar_list_->SetOverlayIcon(hwnd, NULL, NULL);
      result->Success();
    } else {
      result->Error("UNAVAILABLE", "Taskbar interface or Window Handle is unavailable");
    }
  } else if (method_call.method_name().compare("isAppBadgeSupported") == 0) {
    HWND hwnd = GetTopLevelWindow();
    result->Success(flutter::EncodableValue(taskbar_list_ != nullptr && hwnd != nullptr));
  } else {
    result->NotImplemented();
  }
}

}  // namespace app_badge_control_flutter
