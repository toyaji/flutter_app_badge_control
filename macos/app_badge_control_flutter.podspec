#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint app_badge_control_flutter.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'app_badge_control_flutter'
  s.version          = '0.1.0'
  s.summary          = 'Flutter plugin for managing app icon badges.'
  s.description      = <<-DESC
Flutter plugin for managing app icon badges on macOS via the Dock tile badge.
                       DESC
  s.homepage         = 'https://github.com/toyaji/app_badge_control_flutter'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'toyaji' => 'happytoday83@naver.com' }

  s.source           = { :path => '.' }
  s.source_files = 'app_badge_control_flutter/Sources/app_badge_control_flutter/**/*'

  # If your plugin requires a privacy manifest, for example if it collects user
  # data, update the PrivacyInfo.xcprivacy file to describe your plugin's
  # privacy impact, and then uncomment this line. For more information,
  # see https://developer.apple.com/documentation/bundleresources/privacy_manifest_files
  # s.resource_bundles = {'app_badge_control_flutter_privacy' => ['app_badge_control_flutter/Sources/app_badge_control_flutter/PrivacyInfo.xcprivacy']}

  s.dependency 'FlutterMacOS'

  s.platform = :osx, '10.11'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
