# //ysp+

{
  'targets': [
    {
      # GN version: //components/ysp_login
      'target_name': 'ysp_sysinfo',
      'type': 'static_library',
      'dependencies': [
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
      ],
      'include_dirs': [
        '../..',
      ],
      'sources': [
        'ysp_login/mac/ysp_sysinfo.mm',
        'ysp_login/mac/ysp_sysinfo.h',
      ],
    'link_settings': {
      'libraries': [
        '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
      ],
    },
    },
  ],
}
